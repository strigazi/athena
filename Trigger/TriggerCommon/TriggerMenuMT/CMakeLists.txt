# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerMenuMT )

# External dependencies:
find_package( six )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6,E101 --enable-extension=ATL900,ATL901,ATL902
   ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/scripts
   POST_EXEC_SCRIPT nopost.sh )

#----------------------------------
# Function to build trigger menu:
function( atlas_build_lvl1_trigger_menu menu )

   # Don't do anything in release recompilation dryrun mode. In all other
   # modes, proceed as usual.
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Command to build trigger menu. The idea is that ${menu}.stamp gets
   # created as the last command, should the menu generation succeed such that 
   # after a successful menu generation it wouldn't be attempted again.
   # In order for the installation step to not try to re-generate
   # the menu in case it was the generation itself that failed, another
   # stamp file, ${menu}.attempted.stamp is created as the first command.
   # The menu is then only generated as part of the installation step if
   # this ${menu}.attempted.stamp file doesn't even exist.
   
   add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp
      COMMAND ${CMAKE_COMMAND} -E touch
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh -r ${CMAKE_PROJECT_VERSION} ${menu}
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
      #COMMAND ${CMAKE_COMMAND} -E touch           # do not create stamp file to force rebuild every time
      #${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp
      DEPENDS "Package_$<JOIN:$<TARGET_PROPERTY:ATLAS_PACKAGES_TARGET,ATLAS_PACKAGES>,;Package_>" )

   # Create custom target and add it to package dependencies
   add_custom_target( build_menu_${menu} ALL SOURCES
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp )

   # In case the file generation failed, because it wasn't even attempted
   # (failure in another package), then try to run the generation as part
   # of the installation. Note that apparently chaining commands inside a
   # single execute_process(...) call doesn't work correctly during installation
   # for some reason. Hence it's taken apart into 3 separate calls.
   install( CODE "if( NOT EXISTS
                     ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp )
                     message( WARNING \"Generating trigger menu ${menu}\"
                              \" during the installation\" )
                     execute_process( COMMAND ${CMAKE_COMMAND} -E touch
                        ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp )
                     execute_process(
                        COMMAND ${CMAKE_COMMAND} -E make_directory
                        ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
                     execute_process(
                        COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh
                        -r ${CMAKE_PROJECT_VERSION} ${menu} ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
                  endif()" )

   # Install the generated XML files. Note that this installation rule is
   # executed after the previous code. So by this time the files should be
   # in place, if they could be produced.
   install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      DESTINATION ${CMAKE_INSTALL_XMLDIR}/TriggerMenuMT
      USE_SOURCE_PERMISSIONS
      FILES_MATCHING PATTERN "*.xml" )

   # Create a target that will depend on all the other targets, and will
   # print the "right message" at the end of the build. Notice that
   # we can't rely on the Package_TriggerMenuXML target here, since
   # the XML generation depends on all package targets being ready before
   # it could start. So it would cause a circular dependency to make the
   # menu targets be dependencies of the package target.
   if( NOT TARGET TriggerMenuMTMain )
      add_custom_target( TriggerMenuMTMain ALL
         COMMAND ${CMAKE_COMMAND} -E echo
         "TriggerMenuMT: Package build succeeded" )
   endif()
   add_dependencies( TriggerMenuMTMain build_menu_${menu} )

endfunction ( atlas_build_lvl1_trigger_menu )



#----------------------------------
# Install files from the package:
atlas_install_python_modules( python/*.py
                              python/LVL1MenuConfig
                              python/L1
                              python/HLTMenuConfig
			                  POST_BUILD_CMD ${ATLAS_FLAKE8})

atlas_install_scripts( scripts/generateMenuMT.py
                       scripts/generateL1MenuMT.sh
                       scripts/generateLVL1MenuMT.py
                       scripts/generateL1TopoMenuMT.py
                       scripts/test_HLTmenu.sh
                       scripts/generateL1MenuRun3.py
                       scripts/trigCompareOldandNewL1Menus.py
                       scripts/verify_menu_config.py
                       scripts/test_full_menu_cf.py )

atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.dtd data/*.xml )

#----------------------------------
# Tests:
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT_newJO )
atlas_add_test( generateMenuMT_newJO
                SCRIPT python -m TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT_newJO
                PROPERTIES TIMEOUT 500 )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_ViewCFTest )
atlas_add_test( ViewCFTest
		        SCRIPT python -m unittest -v TriggerMenuMT.HLTMenuConfig.Test.ViewCFTest
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_ViewCFTest
		        POST_EXEC_SCRIPT nopost.sh )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT )
atlas_add_test( generateMenuMT
		        SCRIPT bash test_HLTmenu.sh
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT
                PROPERTIES TIMEOUT 4000 )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_full_menu_cf )
atlas_add_test( full_menu_cf
		        SCRIPT scripts/test_full_menu_cf.sh
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_full_menu_cf 
                PROPERTIES TIMEOUT 500)


#----------------------------------
# List of menus to be created:
atlas_build_lvl1_trigger_menu( LS2_v1 )
atlas_build_lvl1_trigger_menu( Physics_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( PhysicsP1_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( MC_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( Cosmic_run3_v1 )
atlas_build_lvl1_trigger_menu( PhysicsP1_HI_run3_v1 )
atlas_build_lvl1_trigger_menu( Dev_HI_run3_v1 )
atlas_build_lvl1_trigger_menu( LS2_v1_TriggerValidation_mc_prescale )


