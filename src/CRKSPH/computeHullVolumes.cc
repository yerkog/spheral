//---------------------------------Spheral++------------------------------------
// Compute the volume per point based on convex hulls.
//------------------------------------------------------------------------------
#include "computeHullVolumes.hh"
#include "Field/Field.hh"
#include "Field/FieldList.hh"
#include "Neighbor/ConnectivityMap.hh"
#include "NodeList/NodeList.hh"
#include "Hydro/HydroFieldNames.hh"

namespace Spheral {

using namespace std;

using FieldSpace::Field;
using FieldSpace::FieldList;
using NeighborSpace::ConnectivityMap;
using NodeSpace::NodeList;

template<typename Dimension>
void
computeHullVolumes(const ConnectivityMap<Dimension>& connectivityMap,
                   const typename Dimension::Scalar kernelExtent,
                   const FieldList<Dimension, typename Dimension::Vector>& position,
                   const FieldList<Dimension, typename Dimension::SymTensor>& H,
                   FieldList<Dimension, typename Dimension::FacetedVolume>& polyvol,
                   FieldList<Dimension, typename Dimension::Scalar>& volume) {

  // Pre-conditions.
  const size_t numNodeLists = volume.size();
  REQUIRE(position.size() == numNodeLists);
  REQUIRE(H.size() == numNodeLists);
  REQUIRE(kernelExtent > 0.0);

  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::SymTensor SymTensor;
  typedef typename Dimension::FacetedVolume FacetedVolume;

  // Walk the FluidNodeLists.
  const double kernelExtent2 = kernelExtent*kernelExtent;
  for (size_t nodeListi = 0; nodeListi != numNodeLists; ++nodeListi) {

    // Iterate over the nodes in this node list.
    for (typename ConnectivityMap<Dimension>::const_iterator iItr = connectivityMap.begin(nodeListi);
         iItr != connectivityMap.end(nodeListi);
         ++iItr) {
      const int i = *iItr;

      // Get the state for node i.
      const Vector& ri = position(nodeListi, i);
      const SymTensor& Hi = H(nodeListi, i);

      // Collect the positions of all neighbors *within i's sampling volume*.
      vector<Vector> positionsInv(1, Vector::zero);
      const vector<vector<int> >& fullConnectivity = connectivityMap.connectivityForNode(nodeListi, i);
      CHECK(fullConnectivity.size() == numNodeLists);
      for (size_t nodeListj = 0; nodeListj != numNodeLists; ++nodeListj) {
        const vector<int>& connectivity = fullConnectivity[nodeListj];
        for (vector<int>::const_iterator jItr = connectivity.begin();
             jItr != connectivity.end();
             ++jItr) {
          const int j = *jItr;
          const Vector& rj = position(nodeListj, j);
          const Vector rji = rj - ri;
          const Scalar etai2 = (Hi*rji).magnitude2();
          if (etai2 < kernelExtent2) {
            const Vector rjiHat = rji.unitVector();
            positionsInv.push_back(1.0/sqrt(rji.magnitude2() + 1.0e-30) * rjiHat);
          }
        }
      }

      // Build the hull of the inverse.
      const FacetedVolume hullInv(positionsInv);

      // Use the vertices selected by the inverse hull to construct the
      // volume of the node.
      vector<Vector> positions;
      const vector<Vector>& vertsInv = hullInv.vertices();
      CHECK((Dimension::nDim == 1 and vertsInv.size() == 2) or
            (Dimension::nDim == 2 and vertsInv.size() >= 3) or
            (Dimension::nDim == 3 and vertsInv.size() >= 4));
      for (typename std::vector<Vector>::const_iterator itr = vertsInv.begin();
           itr != vertsInv.end();
           ++itr) {
        if (itr->magnitude2() < 1.0e-30) {
          positions.push_back(Vector::zero);
        } else {
          positions.push_back(1.0/sqrt(itr->magnitude2()) * itr->unitVector());
        }
      }

      // And we have it.
      // polyvol(nodeListi, i) = FacetedVolume(positions, hullInv.facetVertices());
      polyvol(nodeListi, i) = FacetedVolume(positions);
      volume(nodeListi, i) = polyvol(nodeListi, i).volume();
    }
  }
}

}

