# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/data/client/CMakeLists.txt
# \brief  Drives the building of the LDMX data reading (client) data
# \author JJRussell - russell@slac.stanford.edu
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
# 2021.04.20  jjr  Created to drive the building of the ldmx/data client
# -------------------------------------------------------------------------



announce ()


# -------------------------------------------------------------------------
# LIBRARY: LDMX data client
# ------------------------

  add_library                 (ldmx-data-client SHARED GenericBatch.cc
                                                       GenericBatchHeader.cc
                                                       GenericBatchTail.cc
                                                       GenericEvent.cc
                                                       GenericHeader.cc
                                                       GenericMultiSample.cc
                                                       GenericTrailer.cc)

  set_target_soversion        (ldmx-data-client 0 0 0)
  target_standard_compilation (ldmx-data-client)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  ldmx-data-client
           EXPORT   LdmxDataClientTargets
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)

  include (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION       ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
# -------------------------------------------------------------------------
