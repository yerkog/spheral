//---------------------------------Spheral++----------------------------------//
// RefineNodeIterator -- The version of the NodeIterator that goes over all
// refine nodes in a list of NodeLists based on their Neighbor states.
//
// Created by J. Michael Owen, Tue Mar 18 21:32:13 PST 2003
//----------------------------------------------------------------------------//
#include <algorithm>

#include "RefineNodeIterator.hh"

namespace Spheral {

using namespace std;
using NodeSpace::NodeList;

//------------------------------------------------------------------------------
// Default constructor.
//------------------------------------------------------------------------------
template<typename Dimension>
RefineNodeIterator<Dimension>::
RefineNodeIterator():
  NodeIteratorBase<Dimension>(),
  mNodeIDItr(0) {
}

//------------------------------------------------------------------------------
// Construct with the given NodeList.
//------------------------------------------------------------------------------
template<typename Dimension>
RefineNodeIterator<Dimension>::
RefineNodeIterator(typename vector<NodeList<Dimension>*>::const_iterator nodeListItr,
                   typename vector<NodeList<Dimension>*>::const_iterator nodeListBegin,
                   typename vector<NodeList<Dimension>*>::const_iterator nodeListEnd):
  NodeIteratorBase<Dimension>(),
  mNodeIDItr(0) {
  initialize(nodeListItr,
             nodeListBegin,
             nodeListEnd,
             vector<int>::const_iterator(0));
  ENSURE(valid());
}

//------------------------------------------------------------------------------
// Construct with the given NodeList and node ID.
//------------------------------------------------------------------------------
template<typename Dimension>
RefineNodeIterator<Dimension>::
RefineNodeIterator(typename vector<NodeList<Dimension>*>::const_iterator nodeListItr, 
                   typename vector<NodeList<Dimension>*>::const_iterator nodeListBegin,
                   typename vector<NodeList<Dimension>*>::const_iterator nodeListEnd,
                   vector<int>::const_iterator IDItr):
  NodeIteratorBase<Dimension>(),
  mNodeIDItr(IDItr) {
  initialize(nodeListItr,
             nodeListBegin,
             nodeListEnd,
             IDItr);
  ENSURE(valid());
}

//------------------------------------------------------------------------------
// Copy constructor
//------------------------------------------------------------------------------
template<typename Dimension>
RefineNodeIterator<Dimension>::
RefineNodeIterator(const RefineNodeIterator<Dimension>& rhs):
  NodeIteratorBase<Dimension>(rhs),
  mNodeIDItr(rhs.mNodeIDItr) {
  ENSURE(valid() == rhs.valid());
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
template<typename Dimension>
RefineNodeIterator<Dimension>::
~RefineNodeIterator() {
}

//------------------------------------------------------------------------------
// Valid test.
//------------------------------------------------------------------------------
template<typename Dimension>
bool
RefineNodeIterator<Dimension>::
valid() const {

  // NodeIteratorBase test.
  const bool baseTest = NodeIteratorBase<Dimension>::valid();

  // Verify that the node ID corresponds to a refine node.
  bool refineTest;
  if (mNodeListItr != mNodeListEnd) {
    refineTest = (mNodeID == *mNodeIDItr && 
                  find((*mNodeListItr)->neighbor().refineNeighborBegin(),
                       (*mNodeListItr)->neighbor().refineNeighborEnd(),
                       *mNodeIDItr) != (*mNodeListItr)->neighbor().refineNeighborEnd());
  } else {
    refineTest = mNodeID == 0;
  }

  return baseTest && refineTest;
}

//------------------------------------------------------------------------------
// Private initialization method.
//------------------------------------------------------------------------------
template<typename Dimension>
void
RefineNodeIterator<Dimension>::
initialize(typename vector<NodeList<Dimension>*>::const_iterator nodeListItr,
           typename vector<NodeList<Dimension>*>::const_iterator nodeListBegin,
           typename vector<NodeList<Dimension>*>::const_iterator nodeListEnd,
           vector<int>::const_iterator IDItr) {

  // Pre-conditions.
  REQUIRE((nodeListItr == nodeListEnd && IDItr == vector<int>::const_iterator(0)) ||
          (nodeListItr < nodeListEnd && 
           IDItr >= (*nodeListItr)->neighbor().refineNeighborBegin() &&
           IDItr <= (*nodeListItr)->neighbor().refineNeighborEnd()));

  if (nodeListItr < nodeListEnd) {
    mNodeID = *IDItr;
  } else {
    mNodeID = 0;
  }    
  mFieldID = distance(nodeListBegin, nodeListItr);
  mNodeListBegin = nodeListBegin;
  mNodeListEnd = nodeListEnd;
  mNodeListItr = nodeListItr;

  // Post conditions.
  ENSURE(valid());
}

}

