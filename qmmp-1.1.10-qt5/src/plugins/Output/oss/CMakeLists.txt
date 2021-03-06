project(liboss)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#OSS
CHECK_INCLUDE_FILE(sys/soundcard.h SYS_SOUNDCARD_H_FOUND)
IF(SYS_SOUNDCARD_H_FOUND)
ADD_DEFINITIONS(-DHAVE_SYS_SOUNDCARD_H)
ENDIF(SYS_SOUNDCARD_H_FOUND)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(liboss_SRCS
  outputossfactory.cpp
  outputoss.cpp
  settingsdialog.cpp
)

SET(liboss_HDRS
  outputoss.h
)

SET(liboss_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(liboss_RCC_SRCS ${liboss_RCCS})

SET(liboss_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(liboss_UIS_H ${liboss_UIS})

ADD_LIBRARY(oss MODULE ${liboss_SRCS} ${liboss_UIS_H}
  ${liboss_RCC_SRCS} ${liboss_HDRS})
add_dependencies(oss qmmp)
target_link_libraries(oss Qt5::Widgets -lqmmp)
install(TARGETS oss DESTINATION ${LIB_DIR}/qmmp/Output)

