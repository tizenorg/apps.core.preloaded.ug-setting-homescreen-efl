CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ug-setting-homescreen-efl C)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(BINDIR "${prefix}/bin")

SET(VERSION 0.1.0)
SET(UGNAME "setting-homescreen-efl")

SET(SRCS
	homescreen-setting-efl.c
	homescreen-setting-main.c
	homescreen-setting-type.c
	homescreen-setting-data.c
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED elementary ui-gadget-1 evas dlog vconf ail pkgmgr-info)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} -fvisibility=hidden -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

IF(WAYLAND_SUPPORT)
    ADD_DEFINITIONS("-DWAYLAND")
ENDIF(WAYLAND_SUPPORT)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION 0.1.0)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/ug/lib)

# smack include
INSTALL(FILES ${CMAKE_SOURCE_DIR}/ug.setting-homescreen-efl.include DESTINATION /etc/smack/accesses2.d)

ADD_SUBDIRECTORY(po)
