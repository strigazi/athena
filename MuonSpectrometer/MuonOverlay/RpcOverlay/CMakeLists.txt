################################################################################
# Package: RpcOverlay
################################################################################

# Declare the package name:
atlas_subdir( RpcOverlay )

#External dependencies:
find_package( GTest )

atlas_add_test( RpcOverlay_test
                SOURCES src/*.cxx test/RpcOverlay_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools MuonDigitContainer MuonOverlayBase StoreGateLib SGtests MuonIdHelpersLib ${GTEST_LIBRARIES} )

# Component(s) in the package:
atlas_add_component( RpcOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase StoreGateLib SGtests MuonIdHelpersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
