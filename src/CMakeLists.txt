
#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#

# Creates a component library libURDriver-<target>.so
# and installs in the directory lib/orocos/URDriver/
#


orocos_component( URDriverRT_receiver URDriverRT_receiver-component.cpp) #
orocos_component( URDriver_program URDriver_program-component.cpp) #

# this component is largery unfinished
#orocos_component( URDriver_receiver URDriver_receiver-component.cpp) #


target_link_libraries(URDriver_program
                      ${LEXP_LIBRARIES})

