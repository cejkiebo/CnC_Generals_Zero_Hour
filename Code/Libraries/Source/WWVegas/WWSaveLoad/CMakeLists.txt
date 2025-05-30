add_library(wwsaveload STATIC
    definition.cpp
    definitionfactory.cpp
    definitionfactorymgr.cpp
    definitionmgr.cpp
    parameter.cpp
    persistfactory.cpp
    pointerremap.cpp
    saveload.cpp
    saveloadstatus.cpp
    saveloadsubsystem.cpp
    twiddler.cpp
    wwsaveload.cpp

    # If you want to list headers too, just for IDE visibility:
    # definition.h
    # definitionclassids.h
    # definitionfactory.h
    # ...
)

set_target_properties(wwsaveload PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(wwsaveload PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwdebug"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwmath"
)

target_compile_definitions(wwsaveload PRIVATE
    ${GNG_COMPILE_DEFINITIONS}

    $<$<CONFIG:DebugE>:_DEBUG>
    $<$<CONFIG:DebugE>:WWDEBUG>
    $<$<CONFIG:DebugE>:WIN32>
    $<$<CONFIG:DebugE>:PARAM_EDITING_ON>

    $<$<CONFIG:Profile>:NDEBUG>
    $<$<CONFIG:Profile>:WWDEBUG>
    $<$<CONFIG:Profile>:WIN32>

    $<$<CONFIG:ProfileE>:NDEBUG>
    $<$<CONFIG:ProfileE>:WWDEBUG>
    $<$<CONFIG:ProfileE>:WIN32>
    $<$<CONFIG:ProfileE>:PARAM_EDITING_ON>
)

target_compile_options(wwsaveload PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(wwsaveload PRIVATE
    ${GNG_LINK_OPTIONS}
)
