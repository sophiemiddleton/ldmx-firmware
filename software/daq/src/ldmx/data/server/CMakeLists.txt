# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/data/server/CMakeLists.txt
# \brief  Drives the building of the LDMX data server methods
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  This package LDMX data server is a data source emulator meant
#  to test the LDMX Event Builder, basically being a surrogate for the
#  RCE data sources.
#
# \par
#  This file is part of the LDMX software platform. It is subject to 
#  the license terms in the LICENSE.txt file found in the top-level directory 
#  of this distribution and at: 
#
# \verbatim
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
#  \endverbatim
#
#  No part of the LDMX software platform, including this file, may be 
#  copied, modified, propagated, or distributed except according to the terms 
#  ontained in the LICENSE.txt file.
# 
# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
#
# DATE        WHO  WHAT
# ----------  ---  --------------------------------------------------------
# 2021.04.20  jjr  Created to drive the building of the ldmx/data/server
#                  subdirectory
# -------------------------------------------------------------------------



announce ()


# -------------------------------------------------------------------------
# LIBRARY: LDMX data server
# -------------------------

  add_library              (ldmx-data-server SHARED   GenericBatch.cc
                                                      GenericBatchHeader.cc
                                                      GenericBatchTail.cc
                                                      GenericEvent.cc
                                                      GenericHeader.cc
                                                      GenericMultiSample.cc
                                                      GenericTrailer.cc
                                                      GenericGenerator.cc

                                                      TriggerBatch.cc
                                                      TriggerHeader.cc
                                                      TriggerEvent.cc
                                                      TriggerGenerator.cc)

  set_target_soversion        (ldmx-data-server 0 0 0)
  target_standard_compilation (ldmx-data-server)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  ldmx-data-server
           EXPORT   ldmxDataServerTargets
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  include  (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION       ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
# -------------------------------------------------------------------------
