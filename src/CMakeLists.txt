
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
orocos_component( URDriverRT_receiver URDriverRT_receiver-component.cpp) # ...you may add multiple source files
orocos_component( URDriver_receiver URDriver_receiver-component.cpp) # ...you may add multiple source files

#
# You may add multiple orocos_component statements.

#
# Additional headers:
#
# Installs in the include/orocos/URDriver/ directory
#
#orocos_install_headers(URDriver-component.hpp) # ...you may add multiple header files
#
# You may add multiple orocos_install_headers statements.

 add_executable(swap_test swap_test.cpp )