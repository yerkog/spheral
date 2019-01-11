#-------------------------------------------------------------------------------
# PeriodicBoundary
#-------------------------------------------------------------------------------
from PYB11Generator import *
from Boundary import *
from PlanarBoundary import *
from BoundaryAbstractMethods import *
from RestartMethods import *

@PYB11template("Dimension")
class PeriodicBoundary(PlanarBoundary):

    PYB11typedefs = """
    typedef typename %(Dimension)s::Scalar Scalar;
    typedef typename %(Dimension)s::Vector Vector;
    typedef typename %(Dimension)s::Tensor Tensor;
    typedef typename %(Dimension)s::SymTensor SymTensor;
    typedef typename %(Dimension)s::ThirdRankTensor ThirdRankTensor;
    typedef GeomPlane<%(Dimension)s> Plane;
"""

    #...........................................................................
    # Constructors
    def pyinit(self):
        "Default constructor"

    def pyinit1(self,
                plane1 = "const Plane&",
                plane2 = "const Plane&"):
        "Construct a periodic boundary mapping between the two (enter/exit) planes"

    #...........................................................................
    # Methods
    @PYB11virtual
    def cullGhostNodes(self,
                       flagSet = "const FieldList<%(Dimension)s, int>&",
                       old2newIndexMap = "FieldList<%(Dimension)s, int>&",
                       numNodesRemoved = "std::vector<int>&"):
        "Use a set of flags to cull out inactive ghost nodes."
        return "void"
    
    @PYB11virtual
    def reset(self,
              dataBase = "const DataBase<%(Dimension)s>&"):
        "Overridable hook for clearing out the boundary condition."
        return "void"

    @PYB11virtual
    @PYB11const
    def label(self):
        "Label for restart files"
        return "std::string"

    #...........................................................................
    # Properties
    enterPlane = PYB11property("const Plane&", "enterPlane", "setEnterPlane", doc="The first plane for periodic wrapping")
    exitPlane = PYB11property("const Plane&", "exitPlane", "setExitPlane", doc="The second plane for periodic wrapping")

#-------------------------------------------------------------------------------
# Inject methods
#-------------------------------------------------------------------------------
PYB11inject(BoundaryAbstractMethods, PeriodicBoundary, virtual=True, pure_virtual=False)
