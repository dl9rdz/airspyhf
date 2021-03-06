# Copyright 2012 Jared Boone
# Copyright 2013/2014 Benjamin Vernoux
#
# This file is part of AirSpy (based on HackRF project).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

# Based heavily upon the libftdi cmake setup.

set(INSTALL_DEFAULT_BINDIR "bin" CACHE STRING "Appended to CMAKE_INSTALL_PREFIX")

if(MSVC)
add_library(libgetopt_static STATIC
    ../getopt/getopt.c
)
endif()

#add_executable(airspy_gpio airspy_gpio.c)
#install(TARGETS airspy_gpio RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

#add_executable(airspy_gpiodir airspy_gpiodir.c)
#install(TARGETS airspy_gpiodir RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

add_executable(airspyhf_lib_version airspyhf_lib_version.c)
install(TARGETS airspyhf_lib_version RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

#add_executable(airspy_si5351c airspy_si5351c.c)
#install(TARGETS airspy_si5351c RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

#add_executable(airspy_r820t airspy_r820t.c)
#install(TARGETS airspy_r820t RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

add_executable(airspyhf_info airspyhf_info.c)
install(TARGETS airspyhf_info RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

add_executable(airspyhf_rx airspyhf_rx.c)
install(TARGETS airspyhf_rx RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

if(NOT libairspyhf_SOURCE_DIR)
include_directories(${LIBAIRSPYHF_INCLUDE_DIR})
LIST(APPEND TOOLS_LINK_LIBS ${LIBAIRSPYHF_LIBRARIES})
else()
LIST(APPEND TOOLS_LINK_LIBS airspyhf)
endif()

if(MSVC)
LIST(APPEND TOOLS_LINK_LIBS libgetopt_static)
endif()

LIST(APPEND TOOLS_LINK_LIBS -lm)
#
#target_link_libraries(airspy_gpio ${TOOLS_LINK_LIBS})
#target_link_libraries(airspy_gpiodir ${TOOLS_LINK_LIBS})
target_link_libraries(airspyhf_lib_version ${TOOLS_LINK_LIBS})
#target_link_libraries(airspy_si5351c ${TOOLS_LINK_LIBS})
#target_link_libraries(airspy_r820t ${TOOLS_LINK_LIBS})
target_link_libraries(airspyhf_info ${TOOLS_LINK_LIBS})
target_link_libraries(airspyhf_rx ${TOOLS_LINK_LIBS})
