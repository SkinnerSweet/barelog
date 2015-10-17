# The MIT License (MIT)
#
# Copyright (c) 2015 Thomas Bertauld <thomas.bertauld@gmail.com>
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#!/bin/bash

scriptversion=2015-10-15

set -e

#----------------------------------------------------

#############
# VARIABLES #
#############

Tinit=`date +%s`

# Colors and style variables :
RED='\e[0;31m'
NEUTRAL='\e[0;m'
BLUE='\e[34m'
BOLD='\033[1m'

#--------------

# Barelog parameters
BARELOG_LIBS="../libs"
BARELOG_HOST_INCLUDES="-I ../src/host/include -I ../src/common/include -I ../src/platforms"
BARELOG_TARGET_INCLUDES="-I ../src/target/include -I ../src/common/include -I ../src/platforms"

#--------------

# ESDK parameters
ESDK=${EPIPHANY_HOME}
LDF=/bsps/current/fast.ldf

#--------------

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

#--------------

# Generic-compilation parameters
CLEAN_FUNCTION=
CC_OPTIONS="-std=c99 -Wall -g -L ${BARELOG_LIBS}" #-save-temps (to see macros expension)

#--------------

# Host-compilation parameters
CROSS_PREFIX=""

SRC="src/main.c"
TARGET="Debug/main"
DEPENDENCIES="src/trace.c ${BARELOG_HOST_INCLUDES}"
#src/trace.c src/barelog_host_mem_manager.c src/barelog_event.c
EXTRALIBS="-lbarelog_host -le-hal -le-loader"

#--------------

# Epiphany-compilation parameters
E_SRC="src/e-main.c"
E_TARGET="Debug/e-main"
E_DEPENDENCIES="${BARELOG_TARGET_INCLUDES}"
#src/barelog_device_mem_manager.c src/barelog_logger.c src/barelog_event.c src/snprintf.c
E_EXTRALIBS="-lbarelog_logger -le-lib"

#----------------------------------------------------

#############
# FUNCTIONS #
#############

function clean {
  printf "${BOLD}\n"
  echo -e "-------------------------"
  echo -e "- Removing *.o files... -"
  echo -e "-------------------------${NEUTRAL}"
  
	find . -type f -name "*.o" | xargs rm -rf --verbose
}

#--------------

function mrProper {
	printf "${BOLD}\n"
	echo -e "-------------------------------"
	echo -e "- Removing all build files... -"
	echo -e "-------------------------------${NEUTRAL}"

	find . -type f -name "*~" \
	-o -name "*.o" \
	-o -name "*.elf" \
	-o -name "*.srec"| xargs rm -rf --verbose
}

#--------------

# Build HOST side application
function buildHost {

	CMD="${CROSS_PREFIX}gcc ${CC_OPTIONS} ${SRC} ${DEPENDENCIES} -o ${TARGET}.elf ${EINCS} ${ELIBS} ${EXTRALIBS}"
	
	printf "${BOLD}\n"
	echo -e "----------------------------"
	echo -e "- Building host program... -"
	echo -e "----------------------------${NEUTRAL}"
	echo "${CMD}"
	${CMD}
}

#--------------

# Build DEVICE side program
function buildEpiphany {

	CMD="e-gcc ${CC_OPTIONS} -T ${ELDF} ${E_SRC} ${E_DEPENDENCIES} -o ${E_TARGET}.elf ${E_EXTRALIBS}"

	printf "${BOLD}\n"
	echo -e "--------------------------------"
	echo -e "- Building epiphany program... -"
	echo -e "--------------------------------${NEUTRAL}"
	echo "${CMD}"
	${CMD}
}

#--------------

# Convert ebinary to SREC file
function convertBinary {

	CMD="e-objcopy --srec-forceS3 --output-target srec ${E_TARGET}.elf ${E_TARGET}.srec"

	printf "${BOLD}\n"
	echo -e "--------------------------"
	echo -e "- Converting binaries... -"
	echo -e "--------------------------${NEUTRAL}"
	echo "${CMD}"
	${CMD}
}

#----------------------------------------------------

####################
# SCRIPT BEGINNING #
####################

# Reading of the script's parameters :
while [ $# -gt 0 ]; do
	case $1 in
	--help|-h) DisplayHelp
		exit 0;;
    
	-c|--clean) CLEAN_FUNCTION=clean;;
    
	-C|--mrProper) CLEAN_FUNCTION=mrProper;;
    
	--src|-s) shift
		if [ -e "$1" ]; then
		SRC="$1";
		else
			printf "Error $1 : invalid source.\n"
			exit -1;
		fi;;
		
	--esrc|-es) shift
		if [ -e "$1" ]; then
		ESRC="$1";
		else
			printf "Error $1 : invalid source.\n"
			exit -1;
		fi;;
		
	--arch|-a) shift
		if [ "$1" == "ARM" -o "$1" == "arm" ]; then
			ESDK=${EPIPHANY_HOME_ARM}
			CROSS_PREFIX=arm-linux-gnueabihf-
		elif [ "$1" == "x86" -o "$1" == "x86_64" ]; then
			ESDK=${EPIPHANY_HOME}
			CROSS_PREFIX=""
		else
			printf "Error $1 : invalid parameter.\n"
			exit -1;
		fi;;
		
	-ldf) shift
		if [ "$1" == "fast" ]; then
			LDF=/bsps/current/fast.ldf
		elif [ "$1" == "legacy" ]; then
			LDF=/bsps/current/legacy.ldf
		elif [ "$1" == "internal" ]; then
			LDF=/bsps/current/internal.ldf
		else
			printf "Error $1 : invalid parameter.\n"
			exit -1;
		fi;;
	
	esac;
	shift;
done

#--------------

ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}${LDF}

#--------------

${CLEAN_FUNCTION}

buildHost

buildEpiphany

convertBinary

exit 0
