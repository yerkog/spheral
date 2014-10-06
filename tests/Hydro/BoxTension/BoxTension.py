#-------------------------------------------------------------------------------
# The Hydrostatic Equilibrium/Surface Tension Test
#-------------------------------------------------------------------------------
import shutil
from math import *
from Spheral2d import *
from SpheralTestUtilities import *
from SpheralGnuPlotUtilities import *
from findLastRestart import *
from GenerateNodeDistribution2d import *
from CubicNodeGenerator import GenerateSquareNodeDistribution
from CentroidalVoronoiRelaxation import *

import mpi
import DistributeNodes

title("2-D integrated hydro test --  Hydrostatic Equilibrium/Surface Tension Test")

#-------------------------------------------------------------------------------
# Generic problem parameters
#-------------------------------------------------------------------------------
commandLine(
    # Left state.
    rho1 = 1.0,
    P1 = 1.0,
    gamma1 = 1.5,

    # Top state
    rho2 = 1.0,
    P2 = 1.0,
    gamma2 = 1.5,

    # Bottom state
    rho3 = 1.0,
    P3 = 1.0,
    gamma3 = 1.5,

    # Right state
    rho4 = 1.0,
    P4 = 1.0,
    gamma4 = 1.5,
 
    # Middle state
    rho5 = 4.0,
    P5 = 1.0,
    gamma5 = 1.5,

    # Geometry 
    x0 = 0.0,
    x1 = 0.25,
    x2 = 0.75,
    x3 = 1.0,
    y0 = 0.0,
    y1 = 0.25,
    y2 = 0.75,
    y3 = 1.0,

    # Resolution and node seeding.
    nx1 = 25,
    ny1 = 100,

    nx2 = 50,
    ny2 = 25,

    nx3 = 50,
    ny3 = 25,

    nx4 = 25,
    ny4 = 100,

    nx5 = 50,
    ny5 = 50,

    nPerh = 1.51,

    SVPH = False,
    CSPH = False,
    ASPH = False,
    SPH = True,   # This just chooses the H algorithm -- you can use this with CSPH for instance.
    filter = 0.0,  # For CSPH
    Qconstructor = MonaghanGingoldViscosity,
    #Qconstructor = TensorMonaghanGingoldViscosity,
    boolReduceViscosity = False,
    nh = 5.0,
    aMin = 0.1,
    aMax = 2.0,
    linearConsistent = False,
    fcentroidal = 0.0,
    fcellPressure = 0.0,
    Cl = 1.0, 
    Cq = 0.75,
    Qlimiter = False,
    balsaraCorrection = True,
    epsilon2 = 1e-2,
    hmin = 1e-5,
    hmax = 0.5,
    hminratio = 0.1,
    cfl = 0.5,
    XSPH = True,
    epsilonTensile = 0.0,
    nTensile = 8,

    IntegratorConstructor = CheapSynchronousRK2Integrator,
    goalTime = 7.0,
    steps = None,
    vizCycle = 1,
    vizTime = 0.1,
    dt = 0.0001,
    dtMin = 1.0e-5, 
    dtMax = 0.1,
    dtGrowth = 2.0,
    maxSteps = None,
    statsStep = 10,
    HUpdate = IdealH,
    domainIndependent = False,
    rigorousBoundaries = False,
    dtverbose = False,

    densityUpdate = RigorousSumDensity, # VolumeScaledDensity,
    compatibleEnergy = True,
    gradhCorrection = False,

    useVoronoiOutput = False,
    clearDirectories = False,
    restoreCycle = None,
    restartStep = 200,
    dataDir = "dumps-boxtension-xy",
    )

# Decide on our hydro algorithm.
if SVPH:
    if ASPH:
        HydroConstructor = ASVPHFacetedHydro
    else:
        HydroConstructor = SVPHFacetedHydro
elif CSPH:
    if ASPH:
        HydroConstructor = ACSPHHydro
    else:
        HydroConstructor = CSPHHydro
else:
    if ASPH:
        HydroConstructor = ASPHHydro
    else:
        HydroConstructor = SPHHydro

# Build our directory paths.
densityUpdateLabel = {IntegrateDensity : "IntegrateDensity",
                      SumDensity : "SumDensity",
                      RigorousSumDensity : "RigorousSumDensity",
                      SumVoronoiCellDensity : "SumVoronoiCellDensity"}
baseDir = os.path.join(dataDir,
                       HydroConstructor.__name__,
                       Qconstructor.__name__,
                       densityUpdateLabel[densityUpdate],
                       "linearConsistent=%s" % linearConsistent,
                       "XSPH=%s" % XSPH,
                       "nPerh=%3.1f" % nPerh,
                       "fcentroidal=%1.3f" % fcentroidal,
                       "fcellPressure = %1.3f" % fcellPressure,
                       "%ix%i" % (nx1 + nx2, ny1 + ny2))
restartDir = os.path.join(baseDir, "restarts")
restartBaseName = os.path.join(restartDir, "boxtension-xy-%ix%i" % (nx1 + nx2, ny1 + ny2))

vizDir = os.path.join(baseDir, "visit")
if vizTime is None and vizCycle is None:
    vizBaseName = None
else:
    vizBaseName = "boxtension-xy-%ix%i" % (nx1 + nx2, ny1 + ny2)

#-------------------------------------------------------------------------------
# Check if the necessary output directories exist.  If not, create them.
#-------------------------------------------------------------------------------
import os, sys
if mpi.rank == 0:
    if clearDirectories and os.path.exists(baseDir):
        shutil.rmtree(baseDir)
    if not os.path.exists(restartDir):
        os.makedirs(restartDir)
    if not os.path.exists(vizDir):
        os.makedirs(vizDir)
mpi.barrier()

#-------------------------------------------------------------------------------
# If we're restarting, find the set of most recent restart files.
#-------------------------------------------------------------------------------
if restoreCycle is None:
    restoreCycle = findLastRestart(restartBaseName)

#-------------------------------------------------------------------------------
# Material properties.
#-------------------------------------------------------------------------------
mu = 1.0
eos1 = GammaLawGasMKS(gamma1, mu, minimumPressure = 0.0)
eos2 = GammaLawGasMKS(gamma1, mu, minimumPressure = 0.0)
eos3 = GammaLawGasMKS(gamma1, mu, minimumPressure = 0.0)
eos4 = GammaLawGasMKS(gamma1, mu, minimumPressure = 0.0)
eos5 = GammaLawGasMKS(gamma1, mu, minimumPressure = 0.0)

#-------------------------------------------------------------------------------
# Interpolation kernels.
#-------------------------------------------------------------------------------
WT = TableKernel(BSplineKernel(), 1000)
WTPi = TableKernel(BSplineKernel(), 1000)
output("WT")
output("WTPi")
kernelExtent = WT.kernelExtent

#-------------------------------------------------------------------------------
# Make the NodeLists.
#-------------------------------------------------------------------------------
leftNodes = makeFluidNodeList("Left", eos1,
                              hmin = hmin,
                              hmax = hmax,
                              hminratio = hminratio,
                              nPerh = nPerh)
topNodes = makeFluidNodeList("Top", eos2,
                             hmin = hmin,
                             hmax = hmax,
                             hminratio = hminratio,
                             nPerh = nPerh)
bottomNodes = makeFluidNodeList("Bottom", eos3,
                                hmin = hmin,
                                hmax = hmax,
                                hminratio = hminratio,
                                nPerh = nPerh)
rightNodes = makeFluidNodeList("Right", eos4,
                                hmin = hmin,
                                hmax = hmax,
                                hminratio = hminratio,
                                nPerh = nPerh)
middleNodes = makeFluidNodeList("Middle", eos5,
                                hmin = hmin,
                                hmax = hmax,
                                hminratio = hminratio,
                                nPerh = nPerh)
nodeSet = (leftNodes, topNodes, bottomNodes, rightNodes, middleNodes)
for nodes in nodeSet:
    output("nodes.name")
    output("    nodes.hmin")
    output("    nodes.hmax")
    output("    nodes.hminratio")
    output("    nodes.nodesPerSmoothingScale")
del nodes

#-------------------------------------------------------------------------------
# Set the node properties.
#-------------------------------------------------------------------------------
if restoreCycle is None:
    generatorLeft = GenerateNodeDistribution2d(nx1, ny1, rho1,
                                               distributionType = "lattice",
                                               xmin = (x0, y0),
                                               xmax = (x1, y3),
                                               nNodePerh = nPerh,
                                               SPH = SPH)
    generatorTop = GenerateNodeDistribution2d(nx2, ny2, rho2,
                                              distributionType = "lattice",
                                              xmin = (x1, y2),
                                              xmax = (x2, y3),
                                              nNodePerh = nPerh,
                                              SPH = SPH)
    generatorBottom = GenerateNodeDistribution2d(nx3, ny3, rho3,
                                                 distributionType = "lattice",
                                                 xmin = (x1, y0),
                                                 xmax = (x2, y1),
                                                 nNodePerh = nPerh,
                                                 SPH = SPH)
    generatorRight = GenerateNodeDistribution2d(nx4, ny4, rho4,
                                                 distributionType = "lattice",
                                                 xmin = (x2, y0),
                                                 xmax = (x3, y3),
                                                 nNodePerh = nPerh,
                                                 SPH = SPH)
    generatorMiddle = GenerateNodeDistribution2d(nx5, ny5, rho5,
                                                 distributionType = "lattice",
                                                 xmin = (x1, y1),
                                                 xmax = (x2, y2),
                                                 nNodePerh = nPerh,
                                                 SPH = SPH)

    if mpi.procs > 1:
        from VoronoiDistributeNodes import distributeNodes2d
    else:
        from DistributeNodes import distributeNodes2d

    distributeNodes2d((leftNodes, generatorLeft),
                      (topNodes,generatorTop),
                      (bottomNodes, generatorBottom),
                      (rightNodes, generatorRight),
                      (middleNodes, generatorMiddle))
    for nodes in nodeSet:
        print nodes.name, ":"
        output("    mpi.reduce(nodes.numInternalNodes, mpi.MIN)")
        output("    mpi.reduce(nodes.numInternalNodes, mpi.MAX)")
        output("    mpi.reduce(nodes.numInternalNodes, mpi.SUM)")
    del nodes

    # Set node specific thermal energies
    for (nodes, gamma, rho, P) in ((leftNodes, gamma1, rho1, P1),
                                   (topNodes, gamma2, rho2, P2),
                                   (bottomNodes, gamma3, rho3, P3),
                                   (rightNodes, gamma4, rho4, P4),
                                   (middleNodes, gamma5, rho5, P5)):
        eps0 = P/((gamma - 1.0)*rho)
        nodes.specificThermalEnergy(ScalarField("tmp", nodes, eps0))
    del nodes

#-------------------------------------------------------------------------------
# Construct a DataBase to hold our node lists
#-------------------------------------------------------------------------------
db = DataBase()
output("db")
for nodes in nodeSet:
    db.appendNodeList(nodes)
del nodes
output("db.numNodeLists")
output("db.numFluidNodeLists")

#-------------------------------------------------------------------------------
# Construct the artificial viscosity.
#-------------------------------------------------------------------------------
q = Qconstructor(Cl, Cq)
q.epsilon2 = epsilon2
q.limiter = Qlimiter
q.balsaraShearCorrection = balsaraCorrection
output("q")
output("q.Cl")
output("q.Cq")
output("q.epsilon2")
output("q.limiter")
output("q.balsaraShearCorrection")

#-------------------------------------------------------------------------------
# Construct the hydro physics object.
#-------------------------------------------------------------------------------
if SVPH:
    hydro = HydroConstructor(WT, q,
                             cfl = cfl,
                             compatibleEnergyEvolution = compatibleEnergy,
                             densityUpdate = densityUpdate,
                             XSVPH = XSPH,
                             linearConsistent = linearConsistent,
                             generateVoid = False,
                             HUpdate = HUpdate,
                             fcentroidal = fcentroidal,
                             fcellPressure = fcellPressure,
                             xmin = Vector(x0 - (x3 - x0), y0 - (y3 - y0)),
                             xmax = Vector(x3 + (x3 - x0), y3 + (y3 - y0)))
elif CSPH:
    hydro = HydroConstructor(WT, WTPi, q,
                             filter = filter,
                             cfl = cfl,
                             compatibleEnergyEvolution = compatibleEnergy,
                             XSPH = XSPH,
                             densityUpdate = densityUpdate,
                             HUpdate = HUpdate)
else:
    hydro = HydroConstructor(WT,
                             WTPi,
                             q,
                             cfl = cfl,
                             compatibleEnergyEvolution = compatibleEnergy,
                             gradhCorrection = gradhCorrection,
                             XSPH = XSPH,
                             densityUpdate = densityUpdate,
                             HUpdate = HUpdate,
                             epsTensile = epsilonTensile,
                             nTensile = nTensile)
output("hydro")
output("hydro.kernel()")
output("hydro.PiKernel()")
output("hydro.cfl")
output("hydro.compatibleEnergyEvolution")
output("hydro.densityUpdate")
output("hydro.HEvolution")

packages = [hydro]

#-------------------------------------------------------------------------------
# Construct the MMRV physics object.
#-------------------------------------------------------------------------------

if boolReduceViscosity:
    #q.reducingViscosityCorrection = True
    evolveReducingViscosityMultiplier = MorrisMonaghanReducingViscosity(q,nh,aMin,aMax)
    
    packages.append(evolveReducingViscosityMultiplier)


#-------------------------------------------------------------------------------
# Create boundary conditions.
#-------------------------------------------------------------------------------
#xPlane0 = Plane(Vector(x0, y0), Vector( 1.0,  0.0))
#xPlane1 = Plane(Vector(x3, y0), Vector(-1.0,  0.0))
#yPlane0 = Plane(Vector(x0, y0), Vector( 0.0,  1.0))
#yPlane1 = Plane(Vector(x0, y3), Vector( 0.0, -1.0))
xPlane0 = Plane(Vector(0.0, 0.0), Vector( 1.0,  0.0))
xPlane1 = Plane(Vector(1.0, 0.0), Vector(-1.0,  0.0))
yPlane0 = Plane(Vector(0.0, 0.0), Vector( 0.0,  1.0))
yPlane1 = Plane(Vector(0.0, 1.0), Vector( 0.0, -1.0))

xbc = PeriodicBoundary(xPlane0, xPlane1)
ybc = PeriodicBoundary(yPlane0, yPlane1)
bcSet=[xbc, ybc]

for p in packages:
    for bc in bcSet:
        p.appendBoundary(bc)

#-------------------------------------------------------------------------------
# Construct a time integrator, and add the physics packages.
#-------------------------------------------------------------------------------
integrator = IntegratorConstructor(db)
for p in packages:
    integrator.appendPhysicsPackage(p)
integrator.lastDt = dt
integrator.dtMin = dtMin
integrator.dtMax = dtMax
integrator.dtGrowth = dtGrowth
integrator.domainDecompositionIndependent = domainIndependent
integrator.verbose = dtverbose
integrator.rigorousBoundaries = rigorousBoundaries
output("integrator")
output("integrator.havePhysicsPackage(hydro)")
output("integrator.lastDt")
output("integrator.dtMin")
output("integrator.dtMax")
output("integrator.dtGrowth")
output("integrator.domainDecompositionIndependent")
output("integrator.rigorousBoundaries")
output("integrator.verbose")

#-------------------------------------------------------------------------------
# Make the problem controller.
#-------------------------------------------------------------------------------
if useVoronoiOutput:
    import SpheralVoronoiSiloDump
    vizMethod = SpheralVoronoiSiloDump.dumpPhysicsState
else:
    import SpheralVisitDump
    vizMethod = SpheralVisitDump.dumpPhysicsState
control = SpheralController(integrator, WT,
                            statsStep = statsStep,
                            restartStep = restartStep,
                            restartBaseName = restartBaseName,
                            restoreCycle = restoreCycle,
                            vizMethod = vizMethod,
                            vizBaseName = vizBaseName,
                            vizDir = vizDir,
                            vizStep = vizCycle,
                            vizTime = vizTime,
                            skipInitialPeriodicWork = (HydroConstructor in (SVPHFacetedHydro, ASVPHFacetedHydro)),
                            SPH = SPH)
output("control")

#-------------------------------------------------------------------------------
# Advance to the end time.
#-------------------------------------------------------------------------------
if not steps is None:
    control.step(steps)

else:
    control.advance(goalTime, maxSteps)
    control.updateViz(control.totalSteps, integrator.currentTime, 0.0)
    control.dropRestartFile()
