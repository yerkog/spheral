include_directories(.)
set(Physics_inst
    GenericBodyForce
    GenericHydro
    Physics
   )

set(Physics_sources )

instantiate(Physics_inst Physics_sources)

set(Physics_headers
    GenericBodyForce.hh
    GenericHydro.hh
    GenericHydroInline.hh
    Physics.hh
    PhysicsInline.hh
    )

spheral_add_cxx_library(Physics)

install(TARGETS              Spheral_Physics
        EXPORT               ${PROJECT_NAME}-targets
        DESTINATION          lib
        INCLUDES DESTINATION include
        )

install(FILES       ${Physics_headers}
        DESTINATION include/Physics
        )

list(APPEND SPHERAL_PYTHON_DEPENDS Spheral_Physics)
set(SPHERAL_PYTHON_DEPENDS "${SPHERAL_PYTHON_DEPENDS}" PARENT_SCOPE)
