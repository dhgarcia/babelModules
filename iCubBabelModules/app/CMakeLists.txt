cmake_minimum_required(VERSION 2.6)

set(APPNAME iCubBabelModules)

# ALL the icub and yarp stuff, even if we don't need it all
#find_package(YARP)
#find_package(ICUB)
#find_package(ICUBcontrib)
include(YarpInstallationHelpers)
#include(ICUBcontribHelpers)
#list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH} ${ICUB_MODULE_PATH} ${ICUBCONTRIB_MODULE_PATH})
#icubcontrib_set_default_prefix()

file(GLOB conf ${CMAKE_CURRENT_SOURCE_DIR}/conf/*.ini)
file(GLOB templates ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.template)
file(GLOB scripts ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.xml)

yarp_install(FILES ${conf} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${APPNAME})
yarp_install(FILES ${templates} DESTINATION ${ICUBCONTRIB_APPLICATIONS_TEMPLATES_INSTALL_DIR})
yarp_install(FILES ${scripts}   DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR})
