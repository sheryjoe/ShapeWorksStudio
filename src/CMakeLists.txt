PROJECT( ShapeWorksStudio )

cmake_minimum_required(VERSION 2.8)  


###########################################
# Flags for Microsoft Compiler
###########################################

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501 -DNTDDI_VERSION=0x05010000)
#  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
  ADD_DEFINITIONS(-DPSAPI_VERSION=1)
  # Disable Visual C++ Secure Warnings
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
  ADD_DEFINITIONS(-D_BIND_TO_CURRENT_CRT_VERSION=1)
  SET(CMAKE_CXX_FLAGS "/Oi ${CMAKE_CXX_FLAGS} /MP4")
ENDIF (WIN32 AND MSVC)


#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")
OPTION (BUILD_SHARED_LIBS "Build shared libraries" OFF)
OPTION (BUILD_TESTS "Build ShapeWorksStudio test applications." ON)
OPTION (INSTALL_TEST_FILES "Include ShapeWorksStudio test images and help files in the install." OFF)
OPTION (INSTALL_SOURCE  "Install ShapeWorksStudio source code." OFF)
OPTION (USE_PRECOMPILED_HEADERS "Use precompiled headers to speed up compilation" ON)

OPTION (LOAD_DEFAULT_PROJECT "Load default project" OFF)

MESSAGE(STATUS "** PRECOMPILED_HEADERS: ${USE_PRECOMPILED_HEADERS}")

set(CMAKE_BUILD_TYPE Release)

IF(LOAD_DEFAULT_PROJECT)
  ADD_DEFINITIONS(-DLOAD_DEFAULT_PROJECT)
ENDIF(LOAD_DEFAULT_PROJECT)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32 AND MSVC)

FIND_PACKAGE(VTK COMPONENTS
  vtkCommonCore
  vtkInfovisCore
  vtkInteractionStyle
  vtkViewsQt
  vtkFiltersCore
  vtkGUISupportQt
  vtkRenderingQt
)

INCLUDE( ${VTK_USE_FILE} )
MESSAGE(STATUS "** VTK_DIR: ${VTK_DIR}")
#MESSAGE(STATUS "** VTK_LIBRARIES: ${VTK_LIBRARIES}")

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
  INCLUDE (${ITK_USE_FILE})
ENDIF(ITK_FOUND)
MESSAGE(STATUS "** ITK_DIR: ${ITK_DIR}")
#MESSAGE(STATUS "** ITK_LIBRARIES: ${ITK_LIBRARIES}")

#set( Boost_DEBUG TRUE )
#SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/local/include/boost")
#SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/local/lib")
#set( Boost_USE_STATIC_LIBS ON )
#FIND_PACKAGE(Boost REQUIRED COMPONENTS system filesystem thread)
#link_directories ( ${Boost_LIBRARY_DIRS} )
#include_directories ( ${Boost_INCLUDE_DIRS} )

#MESSAGE(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
#MESSAGE(STATUS "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
#MESSAGE(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

#ADD_DEFINITIONS( -Wall )

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET( QT_USE_QT3SUPPORT TRUE )   
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTSVG TRUE )

#--------------------------------------------------------------------------------
# This sets the windows build that will need the special winmain@16 call. Qt provides
#  this for us in the qtmain.lib file. Using this cmake code will ensure we have it
#  linked into our build. Not needed on Unix/OS X/Linux which is why we have the 
#  IF(WIN32) conditional.
IF (WIN32)
    SET (QT_USE_QTMAIN TRUE)
ENDIF (WIN32)

# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
SET(QT_MIN_VERSION "4.7.4")
FIND_PACKAGE( Qt4 REQUIRED )

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

MESSAGE(STATUS "** QT_QMAKE_EXECUTABLE: ${QT_QMAKE_EXECUTABLE}")

# Instructs the MSVC toolset to use the precompiled header PRECOMPILED_HEADER
# for each source file given in the collection named by SOURCES.
function(enable_precompiled_headers PRECOMPILED_HEADER SOURCES)
  if(MSVC)
    set(files ${${SOURCES}})

    get_filename_component(pch_basename ${PRECOMPILED_HEADER} NAME_WE)
    set(pch_header ${CMAKE_CURRENT_SOURCE_DIR}/${PRECOMPILED_HEADER})
    set(pch_file ${CMAKE_CURRENT_BINARY_DIR}/${pch_basename}.cc)
    set_source_files_properties(${pch_file}  PROPERTIES COMPILE_FLAGS "/Yc\"${pch_header}\"")

    # Generate precompiled header translation unit
    IF(NOT EXISTS "${pch_file}")
      FILE(WRITE ${pch_file} "// Precompiled header unity generated by CMake\n")
      FILE(APPEND ${pch_file} "#include <${pch_header}>\n")
    ENDIF(NOT EXISTS "${pch_file}")

    # Update properties of source files to use the precompiled header.
    # Additionally, force the inclusion of the precompiled header at beginning of each source file.
    foreach(source_file ${files} )
      get_filename_component(extension ${source_file} EXT)
      if ("${extension}" STREQUAL ".cc")
#        message ("Setting property on ${source_file}")
        set_source_files_properties(
          ${source_file}
          PROPERTIES COMPILE_FLAGS
          "/Yu\"${pch_header}\" /FI\"${pch_header}\""
          )
      endif("${extension}" STREQUAL ".cc")
      if ("${extension}" STREQUAL ".cxx")
#        message ("setting property on ${source_file}")
        set_source_files_properties(
          ${source_file}
          PROPERTIES COMPILE_FLAGS
          "/Yu\"${pch_header}\" /FI\"${pch_header}\""
          )
      endif("${extension}" STREQUAL ".cxx")
    endforeach(source_file)

    # Finally, update the source file collection to contain the precompiled header translation unit
    set(${SOURCES} ${pch_file} ${${SOURCES}} PARENT_SCOPE)
  endif(MSVC)
endfunction(enable_precompiled_headers)



### Application
SET(STUDIO_APPLICATION_HDRS
  Application/ShapeWorksStudioApp.h
  Application/Preferences.h
  Application/PreferencesWindow.h
  Application/ShapeworksWorker.h
)
SET(STUDIO_APPLICATION_SRCS
  Application/ShapeWorksStudioApp.cc
  Application/Preferences.cc
  Application/PreferencesWindow.cc
  Application/ShapeworksWorker.cc
)
SET(STUDIO_APPLICATION_UIS
  Application/ShapeWorksStudioApp.ui
  Application/PreferencesWindow.ui
)

### Data
SET(STUDIO_DATA_HDRS
  Data/Mesh.h
  Data/Shape.h
  Data/ItkToVtk.h
  Data/Project.h
  Data/MeshManager.h
  Data/MeshGenerator.h
  Data/MeshSettings.h
  Data/MeshWorkQueue.h
  Data/MeshWorker.h
  Data/MeshCache.h
)
SET(STUDIO_DATA_SRCS
  Data/Mesh.cc
  Data/Shape.cc
  Data/Project.cc
  Data/MeshManager.cc
  Data/MeshGenerator.cc
  Data/MeshSettings.cc
  Data/MeshWorkQueue.cc
  Data/MeshWorker.cc
  Data/MeshCache.cc
)

### Groom
SET(STUDIO_GROOM_HDRS
  Groom/GroomTool.h
)
SET(STUDIO_GROOM_SRCS
  Groom/GroomTool.cc
)
SET(STUDIO_GROOM_UIS
  Groom/GroomTool.ui
)

### Optimize
SET(STUDIO_OPTIMIZE_HDRS
  Optimize/OptimizeTool.h
)
SET(STUDIO_OPTIMIZE_SRCS
  Optimize/OptimizeTool.cc
)
SET(STUDIO_OPTIMIZE_UIS
  Optimize/OptimizeTool.ui
)

### Analyis
SET(STUDIO_ANALYSIS_HDRS
  Analysis/AnalysisTool.h
  Analysis/itkParticleShapeStatistics.h
  Analysis/itkParticleShapeStatistics.txx
  Analysis/bargraph.h
)
SET(STUDIO_ANALYSIS_SRCS
  Analysis/AnalysisTool.cc
  Analysis/bargraph.cpp
)
SET(STUDIO_ANALYSIS_UIS
  Analysis/AnalysisTool.ui
)

### Visualization
SET(STUDIO_VISUALIZATION_HDRS
  Visualization/Lightbox.h
  Visualization/Viewer.h
  Visualization/Visualizer.h
  Visualization/DisplayObject.h
  Visualization/StudioInteractorStyle.h
)
SET(STUDIO_VISUALIZATION_SRCS
  Visualization/Lightbox.cc
  Visualization/Viewer.cc
  Visualization/Visualizer.cc
  Visualization/DisplayObject.cc
  Visualization/StudioInteractorStyle.cc
)

### Util
SET(STUDIO_UTIL_HDRS
  Util/WheelEventForwarder.h
  )
SET(STUDIO_UTIL_SRCS
  Util/WheelEventForwarder.cc
  )

### TinyXML
SET (TINYXML_SRCS
  TinyXML/tinystr.cpp
  TinyXML/tinyxml.cpp
  TinyXML/tinyxmlparser.cpp
  TinyXML/tinyxmlerror.cpp
)
SET (TINYXML_HDRS
  TinyXML/tinystr.h
  TinyXML/tinyxml.h
)


### Studio
SET( STUDIO_SRCS
     main.cc
     ${STUDIO_APPLICATION_SRCS}
     ${STUDIO_DATA_SRCS}
     ${STUDIO_GROOM_SRCS}
     ${STUDIO_OPTIMIZE_SRCS}
     ${STUDIO_ANALYSIS_SRCS}
     ${STUDIO_VISUALIZATION_SRCS}
     ${STUDIO_UTIL_SRCS}
     ${TINYXML_SRCS}
)

SET( STUDIO_HDRS
     ${STUDIO_APPLICATION_HDRS}
     ${STUDIO_DATA_HDRS}
     ${STUDIO_GROOM_HDRS}
     ${STUDIO_OPTIMIZE_HDRS}
     ${STUDIO_ANALYSIS_HDRS}
     ${STUDIO_VISUALIZATION_HDRS}
     ${STUDIO_UTIL_HDRS}
     ${TINYXML_HDRS}
)

SET( STUDIO_MOC_HDRS
     Groom/GroomTool.h
     Optimize/OptimizeTool.h
     Analysis/AnalysisTool.h
     Analysis/bargraph.h
     Data/Project.h
     Data/MeshManager.h
     Data/MeshWorker.h
     Application/ShapeworksWorker.h
     Application/Preferences.h
     Visualization/Visualizer.h
     Visualization/Lightbox.h
     Util/WheelEventForwarder.h
)

SET( STUDIO_RCS
     Resources/ShapeWorksStudio.qrc
)

SOURCE_GROUP("Application" FILES ${STUDIO_APPLICATION_SRCS} ${STUDIO_APPLICATION_HDRS} ${STUDIO_APPLICATION_UIS})
SOURCE_GROUP("Visualization" FILES ${STUDIO_VISUALIZATION_SRCS} ${STUDIO_VISUALIZATION_HDRS})
SOURCE_GROUP("Data" FILES ${STUDIO_DATA_SRCS} ${STUDIO_DATA_HDRS})
SOURCE_GROUP("Groom" FILES ${STUDIO_GROOM_SRCS} ${STUDIO_GROOM_HDRS} ${STUDIO_GROOM_UIS})
SOURCE_GROUP("Optimize" FILES ${STUDIO_OPTIMIZE_SRCS} ${STUDIO_OPTIMIZE_HDRS} ${STUDIO_OPTIMIZE_UIS})
SOURCE_GROUP("Analysis" FILES ${STUDIO_ANALYSIS_SRCS} ${STUDIO_ANALYSIS_HDRS} ${STUDIO_ANALYSIS_UIS})
SOURCE_GROUP("TinyXML" FILES ${TINYXML_SRCS} ${TINYXML_HRDS})

# this command will generate rules that will run rcc on all files from STUDIO_RCS
# in result STUDIO_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( STUDIO_RC_SRCS ${STUDIO_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( STUDIO_UI_HDRS ${STUDIO_APPLICATION_UIS} 
  ${STUDIO_GROOM_UIS}
  ${STUDIO_OPTIMIZE_UIS} 
  ${STUDIO_ANALYSIS_UIS} 
)

# and finally this will run moc:
QT4_WRAP_CPP( STUDIO_MOC_SRCS ${STUDIO_APPLICATION_HDRS} ${STUDIO_MOC_HDRS} )


# Turn on precompiled headers
IF(USE_PRECOMPILED_HEADERS)
  enable_precompiled_headers(PrefixHeader.h STUDIO_SRCS)
  enable_precompiled_headers(PrefixHeader.h STUDIO_MOC_SRCS)
ENDIF(USE_PRECOMPILED_HEADERS)


# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )


# optionally build the test files
IF ( BUILD_TESTS )

  INCLUDE(CTest)
  ENABLE_TESTING()

  # ADD_TEST(QuantTest QuantTest "${CMAKE_CURRENT_SOURCE_DIR}")
  # ADD_EXECUTABLE(QuantTest test/quanttest.cc)
  # TARGET_LINK_LIBRARIES(QuantTest ${ITK_LIBRARIES})

  # ADD_TEST(HistTestNormal HistTestNormal "${CMAKE_CURRENT_SOURCE_DIR}")
  # ADD_EXECUTABLE(HistTestNormal test/histtest_normal.cc)
  # TARGET_LINK_LIBRARIES(HistTestNormal ${ITK_LIBRARIES})

  # ADD_TEST(HistTestMasked HistTestMasked "${CMAKE_CURRENT_SOURCE_DIR}")
  # ADD_EXECUTABLE(HistTestMasked test/histtest_masked.cc)
  # TARGET_LINK_LIBRARIES(HistTestMasked ${ITK_LIBRARIES})

  # ADD_TEST(HistTestThresholded HistTestThresholded "${CMAKE_CURRENT_SOURCE_DIR}")
  # ADD_EXECUTABLE(HistTestThresholded test/histtest_thresholded.cc)
  # TARGET_LINK_LIBRARIES(HistTestThresholded ${ITK_LIBRARIES})

  # ADD_TEST(HistTestMaskedThresholded HistTestMaskedThresholded "${CMAKE_CURRENT_SOURCE_DIR}")
  # ADD_EXECUTABLE(HistTestMaskedThresholded test/histtest_masked_thresholded.cc)
  # TARGET_LINK_LIBRARIES(HistTestMaskedThresholded ${ITK_LIBRARIES})

ENDIF(BUILD_TESTS)

# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE ShapeWorksStudio.icns) 
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.icns)
ENDIF(APPLE)

#IF(WIN32)
#  SET(STUDIO_SRCS ${STUDIO_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ShapeWorksStudio.rc)
#ENDIF(WIN32)

if(MSVC)
  SOURCE_GROUP(moc_sources FILES ${STUDIO_MOC_SRCS})
endif(MSVC)

# here we instruct CMake to build STUDIO executable from all of the source files
ADD_EXECUTABLE( ShapeWorksStudio MACOSX_BUNDLE WIN32 
  ${STUDIO_SRCS}
  ${STUDIO_HDRS}
  ${STUDIO_MOC_SRCS} 
  ${STUDIO_RC_SRCS} 
  ${STUDIO_UI_HDRS} 
  ${STUDIO_MOC_HDRS}
  )

TARGET_LINK_LIBRARIES( ShapeWorksStudio
  ${QT_LIBRARIES} 
  ${ITK_LIBRARIES}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${VTK_LIBRARIES} 
  )
  
# we need to have the shapeworks command line tools for studio to work.
find_file(ShapeworksCLT_BINARY_DIR
  NAMES
  shapeworks
  shapeworks/bin
  shapeworks/build/bin
  shapeworks/build/bin/Release
  PATHS
  $ENV{HOME}
  "C:/"
  $ENV{HOME}/Documents
  )
SET(required_deps
  ShapeWorksGroom
  ShapeWorksRun
  )
foreach (file ${required_deps})
  find_file(${file}-dependency
    NAMES
    ${file}
    ${file}.exe
    PATHS
    ${ShapeworksCLT_BINARY_DIR}
    NO_DEFAULT_PATH
    )
  if (${${file}-dependency} MATCHES "${file}-dependency-NOTFOUND")
    set(ShapeworksCLT_BINARY_DIR "ShapeworksCLT_BINARY_DIR-NOTFOUND" CACHE PATH "Shapeworks command line tools binary directory" FORCE)
  endif()
  unset(${file}-dependency CACHE)
endforeach()

if (${ShapeworksCLT_BINARY_DIR} MATCHES "ShapeworksCLT_BINARY_DIR-NOTFOUND")
  message(ERROR " Shapeworks command line tools binary directory not found. Set ShapeworksCLT_BINARY_DIR in CMake.")
else()
  #copy the binaries from ShapeworksCLT to Cleaver binary dir
  message(STATUS "ShapeworksCLT found at: ${ShapeworksCLT_BINARY_DIR}")
  FILE(GLOB SHAPEWORKS_DEPS
    ${ShapeworksCLT_BINARY_DIR}/ShapeWorksGroom*
    ${ShapeworksCLT_BINARY_DIR}/ShapeWorksRun*
    )
  foreach(file ${SHAPEWORKS_DEPS})
		add_custom_command(TARGET ShapeWorksStudio POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${file} 
				   $<TARGET_FILE_DIR:ShapeWorksStudio>)
  endforeach()
  # platform specific filenames
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DGROOM_EXECUTABLE="ShapeWorksGroom.exe")
    add_definitions(-DOPTIMIZE_EXECUTABLE="ShapeWorksRun.exe")
  else()
    add_definitions(-DGROOM_EXECUTABLE="ShapeWorksGroom")
    add_definitions(-DOPTIMIZE_EXECUTABLE="ShapeWorksRun")
  endif()
endif()

# INSTALLATION AND PACKAGING
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio")
IF(APPLE)
  SET(plugin_dest_dir ShapeWorksStudio.app/Contents/MacOS)
  SET(qtconf_dest_dir ShapeWorksStudio.app/Contents/Resources)
  SET(contents_dest_dir ShapeWorksStudio.app/Contents)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/ShapeWorksStudio.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ShapeWorksStudio.exe")
ENDIF(WIN32)



#--------------------------------------------------------------------------------
# Install the STUDIO application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS ShapeWorksStudio
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

#--------------------------------------------------------------------------------
# Install needed Qt .nib files and others
IF(APPLE)
INSTALL(DIRECTORY "${QT_QTGUI_LIBRARY_RELEASE}/Versions/Current/Resources" DESTINATION ${contents_dest_dir} COMPONENT Runtime)
ENDIF(APPLE)



#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)


#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS ${QT_LIBRARY_DIRS})

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)


# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)


# Grab the source code files for installation
#IF(INSTALL_SOURCE)
# FILE(GLOB root_files "*.h" "*.txx" "*.cxx" "*.txt")
# INSTALL(FILES ${root_files}          DESTINATION source)
# FILE(GLOB test_files "test/*.h" "test/*.txx" "test/*.cxx" "test/*.txt")
# INSTALL(FILES ${test_files}          DESTINATION source/test)
#ENDIF(INSTALL_SOURCE)

INSTALL(FILES README.txt DESTINATION .)

# Install test images and help files in the bin directory
#IF (INSTALL_TEST_FILES)
#  SET(TEST_DATA   
#  list of files here ...
# )
#  INSTALL(FILES ${TEST_DATA} DESTINATION bin)
#ENDIF(INSTALL_TEST_FILES)



# Install the necessary shared ITK Dlls
# TODO: Assumes the install or package is a Release build.  Need to add logic
# to find debug vs shared builds and switch on Mac or Windows.s 
#IF (BUILD_SHARED_LIBS)
#  INSTALL(FILES ${ITK_LIBRARY_DIRS}/Release/ITKCommon.dll DESTINATION bin)
#  INSTALL(FILES ${ITK_LIBRARY_DIRS}/Debug/ITKCommon.dll DESTINATION bin)
#ENDIF(BUILD_SHARED_LIBS)


# CPACK CONFIGURATION
#INCLUDE(InstallRequiredSystemLibraries)

#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ShapeWorksStudio Research Sandbox")
#SET(CPACK_PACKAGE_VENDOR "CARMA Center Image Processing Division")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/PackageDescription.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
#SET(CPACK_PACKAGE_VERSION_MAJOR "0")
#SET(CPACK_PACKAGE_VERSION_MINOR "1")
#SET(CPACK_PACKAGE_VERSION_PATCH "0")
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "ShapeWorksStudio ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PA#CKAGE_VERSION_PATCH}")
#IF(WIN32 AND NOT UNIX)
#  # There is a bug in NSI that does not handle full unix paths properly. Make
#  # sure there is at least one set of four (4) backlashes.
#  #  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#  #  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ShapeWorksStudio Research Sandbox")
#  #  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
#  #  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
#  #  SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
#  SET(CPACK_NSIS_MODIFY_PATH ON)
#ELSE(WIN32 AND NOT UNIX)
#  #  SET(CPACK_STRIP_FILES "bin/MyExecutable")
#    SET(CPACK_SOURCE_STRIP_FILES "")
#ENDIF(WIN32 AND NOT UNIX)

#SET(CPACK_PACKAGE_EXECUTABLES )
#INCLUDE(CPack)


#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
endif()
