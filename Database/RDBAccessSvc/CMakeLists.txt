################################################################################
# Package: RDBAccessSvc
################################################################################

# Declare the package name:
atlas_subdir( RDBAccessSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( RDBAccessSvcLib
                   RDBAccessSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS RDBAccessSvc
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel )

atlas_add_component( RDBAccessSvc
                     src/RDBAccessSvc.cxx
                     src/RDBRecordset.cxx
                     src/RDBRecord.cxx
                     src/RDBVersionAccessor.cxx
                     src/RDBQuery.cxx
                     src/SourceCompAlg.cxx
                     src/components/RDBAccessSvc_entries.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps CxxUtils GaudiKernel RDBAccessSvcLib )

atlas_add_executable( geomdb2sqlite
                      replicator/geomdb2sqlite.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} RDBAccessSvcLib )

atlas_add_executable( geomdb2oracle
                      replicator/geomdb2oracle.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} RDBAccessSvcLib CxxUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )
