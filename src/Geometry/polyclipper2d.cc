//---------------------------------Spheral++----------------------------------//
// Clip a faceted volume (box, polygon, or polyhedron) by a plane in place.
//
// We use the convention that any portion of the faceted volume "below" the 
// plane is clipped, i.e., only the portion of the faceted volume "above" the 
// plane
//    plane.compare(point) <= 0
// is retained.
//
// The algorithms herein are roughly based on the approach outlined in 
// Powell, D., & Abel, T. (2015). An exact general remeshing scheme applied to 
// physically conservative voxelization. Journal of Computational Physics, 297, 340–356.
// though I think not exactly the same.
//
// Created by J. Michael Owen, Tue Nov 28 10:00:51 PST 2017
//----------------------------------------------------------------------------//

#include "polyclipper.hh"
#include "Utilities/DBC.hh"
#include "Utilities/Timer.hh"

#include <list>
#include <map>
#include <iostream>
#include <iterator>
#include <algorithm>

// Declare the timers.
extern Timer TIME_PC2d_convertto;
extern Timer TIME_PC2d_convertfrom;
extern Timer TIME_PC2d_copy;
extern Timer TIME_PC2d_moments;
extern Timer TIME_PC2d_clip;
extern Timer TIME_PC2d_planes;
extern Timer TIME_PC2d_checkverts;
extern Timer TIME_PC2d_insertverts;
extern Timer TIME_PC2d_hanging;
extern Timer TIME_PC2d_compress;

namespace PolyClipper {

using namespace std;

namespace {    // anonymous methods

//------------------------------------------------------------------------------
// Return the sign of the argument determined as follows:
//   
//    x > 0 -> sgn0(x) =  1
//    x = 0 -> sgn0(x) =  0
//    x < 0 -> sgn0(x) = -1
//------------------------------------------------------------------------------
inline
double
sgn0(const double x) {
  return (x > 0.0 ?  1.0 :
          x < 0.0 ? -1.0 :
          0.0);
}

//------------------------------------------------------------------------------
// Compare a plane and point (our built-in plane one has some issues).
//------------------------------------------------------------------------------
inline
int compare(const Spheral::Dim<2>::Vector& planePoint,
            const Spheral::Dim<2>::Vector& planeNormal,
            const Spheral::Dim<2>::Vector& point) {
  return sgn0(planeNormal.dot(point - planePoint));
}

//------------------------------------------------------------------------------
// Intersect a line-segment with a plane.
//------------------------------------------------------------------------------
inline
Spheral::Dim<2>::Vector
segmentPlaneIntersection(const Spheral::Dim<2>::Vector& a,       // line-segment begin
                         const Spheral::Dim<2>::Vector& b,       // line-segment end
                         const Spheral::Dim<2>::Vector& p,       // point in plane
                         const Spheral::Dim<2>::Vector& phat) {  // plane unit normal

  const auto ab = b - a;
  const auto abhat = ab.unitVector();
  CHECK2(std::abs(abhat.dot(phat)) > 0.0, (abhat.dot(phat)) << " " << a << " " << b << " " << abhat << " " << phat);
  const auto s = (p - a).dot(phat)/(abhat.dot(phat));
  CHECK(s >= 0.0 and s <= ab.magnitude());
  const auto result = a + s*abhat;
  // CHECK2(fuzzyEqual((result - p).dot(phat), 0.0, 1.0e-10),
  //        a << " " << b << " " << s << " " << result << " " << (result - p).dot(phat));
  return result;
}

//------------------------------------------------------------------------------
// Return a nicely formatted string representing the polygon.
//------------------------------------------------------------------------------
std::string
polygon2string(const Polygon& poly) {
  ostringstream s;
  s << "[";

  // Numbers of vertices.
  const auto nverts = poly.size();
  const auto nactive = count_if(poly.begin(), poly.end(),
                                [](const Vertex2d& x) { return x.comp >= 0; });
  set<const Vertex2d*> usedVertices;

  // Go until we hit all the active vertices.
  while (usedVertices.size() < nactive) {
    s << "[";

    // Look for the first active unused vertex.
    auto vstart = &(poly.front());
    auto k = 0;
    while (k++ < nverts and
           (vstart->comp < 0 or usedVertices.find(vstart) != usedVertices.end())) vstart = vstart->neighbors.second;
    CHECK(k < nverts);
    auto vnext = vstart;

    // Read out this loop.
    auto force = true;
    while (force or vnext != vstart) {
      s << " " << vnext->position;
      force = false;
      usedVertices.insert(vnext);
      vnext = vnext->neighbors.second;
    }
    s << "]";
  }
  s << "]";
  return s.str();
}

}              // anonymous methods

//------------------------------------------------------------------------------
// Convert Spheral::GeomPolygon -> PolyClipper::Polygon.
//------------------------------------------------------------------------------
void convertToPolygon(Polygon& polygon,
                      const Spheral::Dim<2>::FacetedVolume& Spheral_polygon) {
  TIME_PC2d_convertto.start();

  // Useful types.
  typedef Spheral::Dim<2>::FacetedVolume FacetedVolume;
  typedef Spheral::Dim<2>::Vector Vector;

  // Convert the input polygon to a loop of Vertex's.
  const auto& vertPositions = Spheral_polygon.vertices();
  const auto& facets = Spheral_polygon.facets();
  const auto  nverts0 = vertPositions.size();
  vector<Vertex2d*> id2vert(nverts0, NULL);
  int v1, v2, ivert1, ivert2;
  for (const auto& facet: facets) {
    v1 = facet.ipoint1();
    v2 = facet.ipoint2();
    CHECK(v1 < nverts0);
    CHECK(v2 < nverts0);
    if (id2vert[v1] == NULL) {
      polygon.push_back(Vertex2d(vertPositions[v1]));
      id2vert[v1] = &polygon.back();
    }
    if (id2vert[v2] == NULL) {
      polygon.push_back(Vertex2d(vertPositions[v2]));
      id2vert[v2] = &polygon.back();
    }
    id2vert[v1]->neighbors.second = id2vert[v2];
    id2vert[v2]->neighbors.first =  id2vert[v1];
  }
  CHECK(polygon.size() == nverts0);
  TIME_PC2d_convertto.stop();
}

//------------------------------------------------------------------------------
// Convert PolyClipper::Polygon -> Spheral::GeomPolygon.
// Note: this method assumes the polygon has had all inactive vertices removed.
//------------------------------------------------------------------------------
void convertFromPolygon(Spheral::Dim<2>::FacetedVolume& Spheral_polygon,
                        const Polygon& polygon) {
  TIME_PC2d_convertfrom.start();

  // Useful types.
  typedef Spheral::Dim<2>::FacetedVolume FacetedVolume;
  typedef Spheral::Dim<2>::Vector Vector;

  if (polygon.empty()) {

    Spheral_polygon = FacetedVolume();

  } else {

    const auto nverts = polygon.size();
    CHECK(nverts >= 3);
    vector<Vector> coords(nverts);
    vector<vector<unsigned>> facets(nverts, vector<unsigned>(2));
    auto vptr = &polygon.front();
    for (int k = 0; k < nverts; ++k) {
      coords[k] = vptr->position;
      facets[k][0] = k;
      facets[k][1] = (k + 1) % nverts;
      vptr = vptr->neighbors.second;
    }
    CHECK(vptr == &polygon.front());
    Spheral_polygon = FacetedVolume(coords, facets);
  }
  TIME_PC2d_convertfrom.stop();
}

//------------------------------------------------------------------------------
// Copy a PolyClipper::Polygon.
//------------------------------------------------------------------------------
void copyPolygon(Polygon& polygon,
                 const Polygon& polygon0) {
  TIME_PC2d_copy.start();
  polygon.clear();
  if (not polygon0.empty()) {
    std::map<const Vertex2d*, Vertex2d*> ptrMap;
    for (auto& v: polygon0) {
      polygon.push_back(v);
      ptrMap[&v] = &polygon.back();
    }
    for (auto& v: polygon) {
      v.neighbors.first  = ptrMap[v.neighbors.first];
      v.neighbors.second = ptrMap[v.neighbors.second];
    }
  }
  TIME_PC2d_copy.stop();
}

//------------------------------------------------------------------------------
// Compute the zeroth and first moment of a Polygon.
//------------------------------------------------------------------------------
void moments(double& zerothMoment, Spheral::Dim<2>::Vector& firstMoment,
             const Polygon& polygon) {
  TIME_PC2d_moments.start();

  // Useful types.
  typedef Spheral::Dim<2>::Vector Vector;

  // Clear the result for accumulation.
  zerothMoment = 0.0;
  firstMoment = Vector::zero;

  // Walk the polygon, and add up our results triangle by triangle.
  if (not polygon.empty()) {
    const auto nverts = polygon.size();
    auto vfirst = &polygon.front();
    auto vptr = vfirst->neighbors.second;
    Vertex2d* vnext;
    for (auto k = 0; k < nverts; ++k) {
      vnext = vptr->neighbors.second;
      const auto triA = (vptr->position - vfirst->position).cross(vnext->position - vfirst->position).z();
      zerothMoment += triA;
      firstMoment += triA * (vptr->position + vnext->position);
      vptr = vnext;
    }
    CHECK(zerothMoment != 0.0);
    firstMoment = firstMoment/(3.0*zerothMoment) + vfirst->position;
    zerothMoment *= 0.5;
  }
  TIME_PC2d_moments.stop();
}

//------------------------------------------------------------------------------
// Clip a polygon by planes.
//------------------------------------------------------------------------------
void clipPolygon(Polygon& polygon,
                 const std::vector<Spheral::GeomPlane<Spheral::Dim<2>>>& planes) {
  TIME_PC2d_clip.start();

  // Useful types.
  typedef Spheral::Dim<2>::Vector Vector;

  // Loop over the planes.
  TIME_PC2d_planes.start();
  auto kplane = 0;
  const auto nplanes = planes.size();
  while (kplane < nplanes and not polygon.empty()) {
    const auto& plane = planes[kplane++];
    const auto& p0 = plane.point();
    const auto& phat = plane.normal();
    // cerr << "Clip plane: " << p0 << " " << phat << endl;

    // Check the current set of vertices against this plane.
    TIME_PC2d_checkverts.start();
    auto above = true;
    auto below = true;
    for (auto& v: polygon) {
      v.comp = compare(p0, phat, v.position);
      if (v.comp >= 0) {
        below = false;
      } else if (v.comp == -1) {
        above = false;
      }
    }
    CHECK(not (above and below));
    TIME_PC2d_checkverts.stop();

    // Did we get a simple case?
    if (below) {
      // The polygon is entirely below the clip plane, and is therefore entirely removed.
      // No need to check any more clipping planes -- we're done.
      polygon.clear();

    } else if (not above) {

      // This plane passes through the polygon.
      // Insert any new vertices.
      TIME_PC2d_insertverts.start();
      vector<Vertex2d*> hangingVertices;
      Vertex2d *vprev, *vnext;
      for (auto& v: polygon) {
        std::tie(vprev, vnext) = v.neighbors;

        if ((v.comp)*(vnext->comp) == -1) {
          // This pair straddles the plane and creates a new vertex.
          polygon.push_back(Vertex2d(segmentPlaneIntersection(v.position,
                                                              vnext->position,
                                                              p0,
                                                              phat),
                                     2));         // 2 indicates new vertex
          polygon.back().neighbors.first = &v;
          polygon.back().neighbors.second = vnext;
          v.neighbors.second = &polygon.back();
          vnext->neighbors.first = &polygon.back();
          hangingVertices.push_back(&polygon.back());
          // cerr << " --> Inserting new vertex @ " << polygon.back().position << endl;

        } else if (v.comp == 0 and 
                   (vprev->comp == -1 xor vnext->comp == -1)) {
          // This vertex is exactly in-plane, but has exactly one neighbor edge that will be entirely clipped.
          // No new vertex, but vptr will be hanging.
          hangingVertices.push_back(&v);
          // cerr << " --> Hanging vertex @ " << v.position << endl;

        }
      }
      TIME_PC2d_insertverts.stop();

      // For each hanging vertex, link to the neighbors that survive the clipping.
      TIME_PC2d_hanging.start();
      for (auto vptr: hangingVertices) {
        std::tie(vprev, vnext) = vptr->neighbors;
        CHECK(vptr->comp == 0 or vptr->comp == 2);
        CHECK(vprev->comp == -1 xor vnext->comp == -1);

        if (vprev->comp == -1) {
          // We have to search backwards.
          while (vprev->comp == -1) {
            vprev = vprev->neighbors.first;
          }
          CHECK(vprev != vptr);
          vptr->neighbors.first = vprev;

        } else {
          // We have to search forward.
          while (vnext->comp == -1) {
            vnext = vnext->neighbors.second;
          }
          CHECK(vnext != vptr);
          vptr->neighbors.second = vnext;

        }
      }

      // Remove the clipped vertices, compressing the polygon.
      TIME_PC2d_compress.start();
      for (auto vitr = polygon.begin(); vitr != polygon.end();) {
        if (vitr->comp < 0) {
          vitr = polygon.erase(vitr);
        } else {
          ++vitr;
        }
      }
      TIME_PC2d_compress.stop();

      // cerr << "After compression: " << polygon2string(polygon) << endl;

      // Is the polygon gone?
      if (polygon.size() < 3) polygon.clear();
      TIME_PC2d_hanging.stop();
    }
  }
  TIME_PC2d_planes.stop();
}

}
