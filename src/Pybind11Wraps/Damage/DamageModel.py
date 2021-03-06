#-------------------------------------------------------------------------------
# DamageModel
#-------------------------------------------------------------------------------
from PYB11Generator import *
from Physics import *
from RestartMethods import *

@PYB11template("Dimension")
class DamageModel(Physics):
    """DamageModel -- Base class for the damage physics models.
This class just provides the basic interface for damage models, and does 
not fill out the complete physics package interface."""

    PYB11typedefs = """
    typedef typename %(Dimension)s::Scalar Scalar;
    typedef typename %(Dimension)s::Vector Vector;
    typedef typename %(Dimension)s::Tensor Tensor;
    typedef typename %(Dimension)s::SymTensor SymTensor;
    typedef typename Physics<%(Dimension)s>::TimeStepType TimeStepType;

    typedef Field<%(Dimension)s, std::vector<double> > FlawStorageType;
"""

    def pyinit(self,
               nodeList = "SolidNodeList<%(Dimension)s>&",
               W = "const TableKernel<%(Dimension)s>&",
               crackGrowthMultiplier = "const double",
               flawAlgorithm = "const EffectiveFlawAlgorithm",
               flaws = "const FlawStorageType&"):
        "Constructor"

    #...........................................................................
    # Virtual methods
    @PYB11virtual
    @PYB11const
    def computeScalarDDDt(self,
                          dataBase = "const DataBase<%(Dimension)s>&",
                          state = "const State<%(Dimension)s>&",
                          time = "const Scalar",
                          dt = "const Scalar",
                          DDDt = "Field<%(Dimension)s, Scalar>&"):
        "Compute the generic Grady-Kipp (ala Benz-Asphaug) scalar damage time derivative."
        return "void"

    @PYB11virtual
    def preStepInitialize(self,
                          dataBase = "const DataBase<%(Dimension)s>&",
                          state = "State<%(Dimension)s>&",
                          derivs = "StateDerivatives<%(Dimension)s>&"):
        return "void"

    @PYB11virtual
    def registerState(self,
                      dataBase = "DataBase<%(Dimension)s>&",
                      state = "State<%(Dimension)s>&"):
        return "void"

    @PYB11virtual 
    def postStateUpdate(self,
                        time = "const Scalar",
                        dt = "const Scalar",
                        dataBase = "const DataBase<%(Dimension)s>&",
                        state = "State<%(Dimension)s>&",
                        derivs = "StateDerivatives<%(Dimension)s>&"):
        return "void"

    #...........................................................................
    # Methods
    def cullToWeakestFlaws(sefl):
        "Optional method to cull the set of flaws to the single weakest one on each point."
        return "void"

    @PYB11const
    def flawsForNode(self, index="const size_t"):
        "Get the set of flaw activation energies for the given node index."
        return "const std::vector<double>"

    #...........................................................................
    # Properties
    nodeList = PYB11property("const SolidNodeList<%(Dimension)s>&", returnpolicy="reference_internal",
                             doc="Access the SolidNodeList we're damaging.")
    kernel = PYB11property("const TableKernel<%(Dimension)s>&", returnpolicy="reference_internal",
                           doc="Access the kernel.")
    crackGrowthMultiplier = PYB11property("double")
    effectiveFlawAlgorithm = PYB11property("EffectiveFlawAlgorithm")
    excludeNodes = PYB11property("std::vector<int>", "excludeNodes", "excludeNodes",
                                 doc="Allow the user to specify a set of nodes to be excluded from damage.")
    youngsModulus = PYB11property("const Field<%(Dimension)s, Scalar>&", returnpolicy="reference_internal")
    longitudinalSoundSpeed = PYB11property("const Field<%(Dimension)s, Scalar>&", returnpolicy="reference_internal")
    flaws = PYB11property("const FlawStorageType&", returnpolicy="reference_internal",
                          doc="The raw set of flaw activation strains per point")
    effectiveFlaws = PYB11property("const Field<%(Dimension)s, Scalar>&", returnpolicy="reference_internal",
                                   doc="The processed flaw activation strain used per point -- depends on the choice for effectiveFlawAlgorithm")
    sumActivationEnergiesPerNode = PYB11property("Field<%(Dimension)s, Scalar>", 
                                                 doc="Compute a Field with the sum of the activation energies per node.")
    numFlawsPerNode = PYB11property("Field<%(Dimension)s, Scalar>",
                                    doc="Compute a Field with the number of flaws per node.")
    criticalNodesPerSmoothingScale = PYB11property("double", "criticalNodesPerSmoothingScale", "criticalNodesPerSmoothingScale",
                                                   doc="The effective critical number of nodes per smoothing scale, below which we assume all flaws are active on a node.")

#-------------------------------------------------------------------------------
# Add the restart methods
#-------------------------------------------------------------------------------
PYB11inject(RestartMethods, DamageModel)
