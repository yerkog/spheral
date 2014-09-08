#ATS:if SYS_TYPE.startswith('darwin'):
#ATS:    t0 = test(SELF,       "--nr 10 --numViz 0 --timeStepChoice AccelerationRatio --steps=40 --restartStep 20 --dataDir 'Collisionless_Sphere_Collapse_AccelerationRatio' --clearDirectories True --outputFile 'Collisionless_sphere_collapse_AccelerationRatio_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_AccelerationRatio_data_darwin_20131123.txt'", np=1, label="Collisionless sphere gravitational collapse restart test (serial, acceleration ratio) INITIAL RUN")
#ATS:    t1 = testif(t0, SELF, "--nr 10 --numViz 0 --timeStepChoice AccelerationRatio --steps 20 --restartStep 100 --dataDir 'Collisionless_Sphere_Collapse_AccelerationRatio' --clearDirectories False --outputFile 'Collisionless_sphere_collapse_AccelerationRatio_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_AccelerationRatio_data_darwin_20131123.txt' --restoreCycle 20 --checkRestart True", np=1, label="Collisionless sphere gravitational collapse restart test (serial, acceleration ratio) RESTARTED CHECK")
#ATS:    t2 = test(SELF,       "--nr 10 --numViz 0 --timeStepChoice DynamicalTime --steps=40 --restartStep 20  --dataDir 'Collisionless_Sphere_Collapse_DynamicalTime'  --clearDirectories True --outputFile 'Collisionless_sphere_collapse_DynamicalTime_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_DynamicalTime_data_darwin_20131123.txt'", np=1, label="Collisionless sphere gravitational collapse restart test (serial, dynamical time) INITIAL RUN")
#ATS:    t3 = testif(t2, SELF, "--nr 10 --numViz 0 --timeStepChoice DynamicalTime --steps 20 --restartStep 100  --dataDir 'Collisionless_Sphere_Collapse_DynamicalTime' --clearDirectories False --outputFile 'Collisionless_sphere_collapse_DynamicalTime_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_DynamicalTime_data_darwin_20131123.txt' --restoreCycle 20 --checkRestart True", np=1, label="Collisionless sphere gravitational collapse restart test (serial, dynamical time) RESTARTED CHECK")
#ATS:else:
#ATS:    t0 = test(SELF,       "--nr 10 --numViz 0 --timeStepChoice AccelerationRatio --steps=40 --restartStep 20  --dataDir 'Collisionless_Sphere_Collapse_AccelerationRatio' --clearDirectories True --outputFile 'Collisionless_sphere_collapse_AccelerationRatio_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_AccelerationRatio_data_09282013.txt'", np=1, label="Collisionless sphere gravitational collapse restart test (serial, acceleration ratio) INITIAL RUN")
#ATS:    t1 = testif(t0, SELF, "--nr 10 --numViz 0 --timeStepChoice AccelerationRatio --steps 20 --restartStep 100 --dataDir 'Collisionless_Sphere_Collapse_AccelerationRatio' --clearDirectories False --outputFile 'Collisionless_sphere_collapse_AccelerationRatio_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_AccelerationRatio_data_09282013.txt' --restoreCycle 20 --checkRestart True", np=1, label="Collisionless sphere gravitational collapse restart test (serial, acceleration ratio) RESTARTED CHECK")
#ATS:    t2 = test(SELF,       "--nr 10 --numViz 0 --timeStepChoice DynamicalTime --steps=40 --restartStep 20   --dataDir 'Collisionless_Sphere_Collapse_DynamicalTime' --clearDirectories True --outputFile 'Collisionless_sphere_collapse_DynamicalTime_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_DynamicalTime_data_10312013.txt'", np=1, label="Collisionless sphere gravitational collapse restart test (serial, dynamical time) INITIAL RUN")
#ATS:    t3 = testif(t2, SELF, "--nr 10 --numViz 0 --timeStepChoice DynamicalTime --steps 20 --restartStep 100  --dataDir 'Collisionless_Sphere_Collapse_DynamicalTime' --clearDirectories False --outputFile 'Collisionless_sphere_collapse_DynamicalTime_data.txt' --comparisonFile 'Reference/Collisionless_sphere_collapse_DynamicalTime_data_10312013.txt' --restoreCycle 20 --checkRestart True", np=1, label="Collisionless sphere gravitational collapse restart test (serial, dynamical time) RESTARTED CHECK")

#-------------------------------------------------------------------------------
# Create a spherical distribution of collisionless points, which will of course 
# promptly collapse under their own self-gravity.
#-------------------------------------------------------------------------------
import shutil
from Spheral3d import *
from SpheralTestUtilities import *
from SpheralGnuPlotUtilities import *
from NodeHistory import *
from SpheralVisitDump import dumpPhysicsState
from GenerateNodeDistribution3d import *
from VoronoiDistributeNodes import distributeNodes3d
from math import *

print "3-D N-Body Gravity test -- collisionless sphere."

#-------------------------------------------------------------------------------
# Generic problem parameters
#-------------------------------------------------------------------------------
commandLine(

    # Initial particle stuff
    nr = 50,                       # radial number of particles to seed
    r0 = 1.0,                      # (AU) initial radius of the sphere
    M0 = 1.0,                      # (earth masses) total mass of the sphere
    plummerLength = 1.0e-2,        # (AU) Plummer softening scale
    opening = 1.0,                 # (dimensionless, OctTreeGravity) opening parameter for tree walk
    fdt = 0.1,                     # (dimensionless, OctTreeGravity) timestep multiplier

    # Problem control
    steps = None,                  # Optionally advance a fixed number of steps
    numCollapseTimes = 1.0,        # What time to advance to in units of the collapse time for the sphere

    # Which N-body method should we use?
    nbody = OctTreeGravity,
    timeStepChoice = AccelerationRatio,

    # Output
    clearDirectories = False,
    dataDir = "Collisionless_Sphere_Collapse",
    baseNameRoot = "sphere_collapse_%i",
    restoreCycle = None,
    restartStep = 100,
    numViz = 200,
    verbosedt = False,

    # Parameters purely for test checking
    checkRestart = False,
    outputFile = "None",
    comparisonFile = "None",
    )

# Convert to MKS units.
AU = 149597870700.0  # m
Mearth = 5.9722e24   # kg
r0 *= AU
M0 *= Mearth
plummerLength *= AU

# Compute the analytically expected collapse time.
G = MKS().G
rho0 = M0/(4.0/3.0*pi*r0*r0*r0)
tdyn = sqrt(3.0*pi/(16*G*rho0))
collapseTime = tdyn/sqrt(2.0)

# Miscellaneous problem control parameters.
dt = collapseTime / 100
goalTime = collapseTime * numCollapseTimes
dtMin, dtMax = 0.1*dt, 100.0*dt
dtGrowth = 2.0
maxSteps = None
statsStep = 10
smoothIters = 0
if numViz > 0:
    vizTime = goalTime / numViz
else:
    vizTime = goalTime

baseName = baseNameRoot % nr
restartDir = os.path.join(dataDir, "restarts")
visitDir = os.path.join(dataDir, "visit")
restartBaseName = os.path.join(restartDir, baseName + "_restart")

#-------------------------------------------------------------------------------
# Check if the necessary output directories exist.  If not, create them.
#-------------------------------------------------------------------------------
import os, sys
if mpi.rank == 0:
    if clearDirectories and os.path.exists(dataDir):
        shutil.rmtree(dataDir)
    if not os.path.exists(restartDir):
        os.makedirs(restartDir)
    if not os.path.exists(visitDir):
        os.makedirs(visitDir)
mpi.barrier()

#-------------------------------------------------------------------------------
# For now we have set up a fluid node list, even though this is collisionless
# problem.  Fix at some point!
# In the meantime, set up the hydro objects this script isn't really going to
# need.
#-------------------------------------------------------------------------------
WT = TableKernel(BSplineKernel(), 1000)
eos = GammaLawGasMKS3d(gamma = 5.0/3.0, mu = 1.0)

#-------------------------------------------------------------------------------
# Make the NodeList, and set our initial properties.
#-------------------------------------------------------------------------------
nodes = makeFluidNodeList("nodes", eos,
                          topGridCellSize = 100*r0,
                          xmin = -100.0*r0 * Vector.one,
                          xmax =  100.0*r0 * Vector.one)

if restoreCycle is None:
    generator = GenerateNodeDistribution3d(2*nr, 2*nr, 2*nr, rho0,
                                           distributionType = "lattice",
                                           xmin = (-r0, -r0, -r0),
                                           xmax = ( r0,  r0,  r0),
                                           rmin = 0.0,
                                           rmax = r0)
    distributeNodes3d((nodes, generator))
    output("mpi.reduce(nodes.numInternalNodes, mpi.MIN)")
    output("mpi.reduce(nodes.numInternalNodes, mpi.MAX)")
    output("mpi.reduce(nodes.numInternalNodes, mpi.SUM)")

    # Renormalize the node masses to get our desired total mass.
    mass = nodes.mass()
    msum = mpi.allreduce(sum(nodes.mass().internalValues()), mpi.SUM)
    fmass = M0/msum
    print "Renormalizing masses by ", fmass
    for i in xrange(nodes.numInternalNodes):
        mass[i] *= fmass

#-------------------------------------------------------------------------------
# DataBase
#-------------------------------------------------------------------------------
db = DataBase()
db.appendNodeList(nodes)

#-------------------------------------------------------------------------------
# Gimme gravity.
#-------------------------------------------------------------------------------
if nbody is NBodyGravity:
    gravity = NBodyGravity(plummerSofteningLength = plummerLength,
                           maxDeltaVelocity = 1e-2*r0/tdyn,
                           G = G)
# elif nbody is FractalGravity:
#     gravity = FractalGravity(G = G,
#                              xmin = Vector(-1.5*r0, -1.5*r0, -1.5*r0),
#                              xmax = Vector( 1.5*r0,  1.5*r0,  1.5*r0),
#                              periodic = False,
#                              ngrid = 64,
#                              nlevelmax = 1,
#                              minHighParticles = 10,
#                              padding = 0,
#                              maxDeltaVelocity = 1e-2*v0)
elif nbody is OctTreeGravity:
    gravity = OctTreeGravity(G = G,
                             softeningLength = plummerLength,
                             opening = opening,
                             ftimestep = fdt,
                             timeStepChoice = timeStepChoice)

#-------------------------------------------------------------------------------
# Construct a time integrator.
#-------------------------------------------------------------------------------
integrator = SynchronousRK2Integrator(db)
integrator.appendPhysicsPackage(gravity)
integrator.lastDt = dtMin
integrator.dtMin = dtMin
integrator.dtMax = dtMax
integrator.dtGrowth = dtGrowth
integrator.verbose = verbosedt

#-------------------------------------------------------------------------------
# Build the problem controller to follow the problem evolution.
#-------------------------------------------------------------------------------
control = SpheralController(integrator, WT,
                            statsStep = statsStep,
                            restartStep = restartStep,
                            restartBaseName = restartBaseName,
                            vizBaseName = baseName,
                            vizDir = visitDir,
                            vizTime = vizTime,
                            vizMethod = dumpPhysicsState)

#-------------------------------------------------------------------------------
# If we're restarting, read in the restart file.
#-------------------------------------------------------------------------------
if restoreCycle:
    control.loadRestartFile(restoreCycle)

#-------------------------------------------------------------------------------
# Advance to the end time.
#-------------------------------------------------------------------------------
if not steps is None:
    control.step(steps)
else:
    print "Advancing to %g sec = %g years" % (goalTime, goalTime/(365.24*24*3600))
    control.advance(goalTime)

#-------------------------------------------------------------------------------
# If requested, write out the state in a global ordering to a file.
#-------------------------------------------------------------------------------
if outputFile != "None":
    outputFile = os.path.join(dataDir, outputFile)
    from SpheralGnuPlotUtilities import multiSort
    xprof = mpi.reduce(nodes.positions().internalValues(), mpi.SUM)
    vprof = mpi.reduce(nodes.velocity().internalValues(), mpi.SUM)
    Hprof = mpi.reduce(nodes.Hfield().internalValues(), mpi.SUM)
    phi = gravity.potential()
    phiprof = mpi.reduce(phi[0].internalValues(), mpi.SUM)
    mof = mortonOrderIndices(db)
    mo = mpi.reduce(mof[0].internalValues(), mpi.SUM)
    if mpi.rank == 0:
        from SpheralGnuPlotUtilities import multiSort
        multiSort(mo, xprof, vprof, Hprof, phiprof)
        f = open(outputFile, "w")
        f.write(("# " + 27*"%15s " + "\n") % ("x", "y", "z", "vx", "vy", "vz", "Hxx", "Hxy", "Hxz", "Hyy", "Hyz", "Hzz", "phi", "mortonOrder",
                                              "x_uu", "y_uu", "z_uu", "vx_uu", "vy_uu", "vz_uu", 
                                              "Hxx_uu", "Hxy_uu", "Hxz_uu", "Hyy_uu", "Hyz_uu", "Hzz_uu", "phi_uu"))
        for (xi, vi, Hi, phii, moi) in zip(xprof, vprof, Hprof, phiprof, mo):
            f.write((13*" %16.12e" + 14*" %i" + "\n") % (xi.x, xi.y, xi.z, vi.x, vi.y, vi.z, 
                                                         Hi.xx, Hi.xy, Hi.xz, Hi.yy, Hi.yz, Hi.zz, phii, moi,
                                                         unpackElementUL(packElementDouble(xi.x)),
                                                         unpackElementUL(packElementDouble(xi.y)),
                                                         unpackElementUL(packElementDouble(xi.z)),
                                                         unpackElementUL(packElementDouble(vi.x)),
                                                         unpackElementUL(packElementDouble(vi.y)),
                                                         unpackElementUL(packElementDouble(vi.z)),
                                                         unpackElementUL(packElementDouble(Hi.xx)),
                                                         unpackElementUL(packElementDouble(Hi.xy)),
                                                         unpackElementUL(packElementDouble(Hi.xz)),
                                                         unpackElementUL(packElementDouble(Hi.yy)),
                                                         unpackElementUL(packElementDouble(Hi.yz)),
                                                         unpackElementUL(packElementDouble(Hi.zz)),
                                                         unpackElementUL(packElementDouble(phii))))
        f.close()

        #---------------------------------------------------------------------------
        # Also we can optionally compare the current results with another file.
        #---------------------------------------------------------------------------
        if comparisonFile != "None":
            import filecmp
            assert filecmp.cmp(outputFile, comparisonFile)
