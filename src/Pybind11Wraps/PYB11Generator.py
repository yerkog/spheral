#-------------------------------------------------------------------------------
# PYB11Generator
#-------------------------------------------------------------------------------
import inspect
import sys
from PYB11utils import *
from PYB11Decorators import *
from PYB11STLmethods import *
from PYB11function import *
from PYB11class import *
from PYB11Publicist import *
from PYB11enum import *
from PYB11attr import *

#-------------------------------------------------------------------------------
# PYB11generateModule
#-------------------------------------------------------------------------------
def PYB11generateModule(modobj, name=None):
    if name is None:
        name = modobj.__name__
    with open(name + ".cc", "w") as f:
        ss = f.write
        PYB11generateModuleStart(modobj, ss, name)

        # enums
        PYB11generateModuleEnums(modobj, ss)

        # classes
        PYB11generateModuleClasses(modobj, ss)

        # STL types
        PYB11generateModuleSTL(modobj, ss)

        # methods
        PYB11generateModuleFunctions(modobj, ss)

        # Attributes
        PYB11generateModuleAttrs(modobj, ss)

        # Closing
        ss("}\n")
        f.close()

    return

#-------------------------------------------------------------------------------
# PYB11generateModuleStart
#
# All the stuff up to the methods.
#-------------------------------------------------------------------------------
def PYB11generateModuleStart(modobj, ss, name):

    # Compiler guard.
    ss("""//------------------------------------------------------------------------------
// Module %(name)s
//------------------------------------------------------------------------------
// Put Python includes first to avoid compile warnings about redefining _POSIX_C_SOURCE
#include "pybind11/pybind11.h"
#include "pybind11/stl_bind.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "pybind11/operators.h"

namespace py = pybind11;
using namespace pybind11::literals;

#define PYB11COMMA ,

""" % {"name" : name})

    # Includes
    if hasattr(modobj, "includes"):
        for inc in modobj.includes:
            ss('#include %s\n' % inc)
        ss("\n")

    # Use  namespaces
    if hasattr(modobj, "namespaces"):
        for ns in modobj.namespaces:
            ss("using namespace " + ns + ";\n")
        ss("\n")

    # Use objects from scopes
    if hasattr(modobj, "scopenames"):
        for scopename in modobj.scopenames:
            ss("using " + scopename + "\n")
        ss("\n")

    # Preamble
    if hasattr(modobj, "preamble"):
        if modobj.preamble:
            ss(modobj.preamble + "\n")
        ss("\n")

    # Some pybind11 types need their own preamble.
    for objname, obj in PYB11STLobjs(modobj):
        obj.preamble(modobj, ss, objname)
    ss("\n")

    # Trampolines
    PYB11generateModuleTrampolines(modobj, ss)

    # Trampolines
    PYB11generateModulePublicists(modobj, ss)

    # Declare the module
    ss("""
//------------------------------------------------------------------------------
// Make the module
//------------------------------------------------------------------------------
PYBIND11_MODULE(%(name)s, m) {

""" % {"name"     : name,
      })

    doc = inspect.getdoc(modobj)
    if doc:
        ss("  m.doc() = ")
        PYB11docstring(doc, ss)
        ss("  ;\n")
    ss("\n")

    # Are there any objects to import from other modules
    othermods = []
    for (kname, klass) in PYB11classes(modobj):
        klassattrs = PYB11attrs(klass)
        mods = klassattrs["module"]
        for otherklass in mods:
            othermod = mods[otherklass]
            if othermod not in othermods:
                othermods.append(othermod)
    if othermods:
        ss("  // Import external modules\n")
        for othermod in othermods:
            ss('  py::module::import("%s");\n' % othermod)
        ss("\n")

    return

