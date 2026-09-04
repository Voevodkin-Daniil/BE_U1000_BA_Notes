#!/usr/bin/env bash

# Baikal MCU SDK
# Build example projects script.
# Syntax: <script_name> <--make-args args_for_make> <--keep>
# Example: build_examples.sh -m "MEM_REG_RAM=TCMA"

# Exit if any command has a non-zero exit status
set -e
# Exit if reference to any undefined variable
set -u
# Exit if any command in a pipeline fails
set -o pipefail

if [ $EUID -eq 0 ]; then
    echo "Do not run this script with root privileges"
    exit 0
fi

# Default variables values -----------------------------------------------------
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SDK_ROOT=$(realpath ${SCRIPT_DIR}/../..)
MAKE_ARGS=""
KEEP_ARTIFACTS=false

# Set ESC-codes for color control ----------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RESULT_COL='\033[75G'
NC='\033[0m' # No Color

# Help -------------------------------------------------------------------------
help () {
    echo "MCU SDK examples build script"
    echo "  -h, --help                - show this help"
    echo "  -m, --make-args <args>    - arguments to be passed to the make utility"
    echo "  -k, --keep                - do not clean built projects"
}

# Parse argements --------------------------------------------------------------
POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            help
            exit 0
            ;;

        -m|--make-args)
            MAKE_ARGS=$2
            shift # past argument
            shift # past value
            ;;

        -k|--keep)
            KEEP_ARTIFACTS=true
            shift # past argument
            ;;
        
         -*|--*)
            echo "Invalid argument: $1"
            exit 1
            ;;

        *)
            POSITIONAL_ARGS+=("$1")
            shift # past argument
            ;;
    esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

echo "  'Make' utility parameters : ${MAKE_ARGS}"

# Prepare exmaple project directories list -------------------------------------
# Skip sub-projects for Core0, Core1 and Core2
PROJECTS_LIST=$(find ${SDK_ROOT}/Projects/HAL_examples -type f -iname 'makefile' \
                -not -path '*/C0/*' \
                -not -path '*/C1/*' \
                -not -path '*/C2/*' \
                | xargs dirname | sort)
PROJECTS_LIST="${PROJECTS_LIST} \
               ${SDK_ROOT}/Projects/_template"

# Get number of projects -------------------------------------------------------
PROJECTS_NUM=$(echo ${PROJECTS_LIST} | wc --words)
echo -ne "${YELLOW}Total number of projects: ${PROJECTS_NUM}${NC}\n"

echo -ne "Projects list\n${GREEN}"
echo ${PROJECTS_LIST} | sed -r -e 's/ /\n/g' -e s@${SDK_ROOT}/Projects/@@g
echo -ne "${NC}"

# Tru to build each project, print the result and make clean if required -------
for PROJECT_DIR in ${PROJECTS_LIST}
do
    echo -ne "$(sed s@${SDK_ROOT}/Projects/@@g <<< ${PROJECT_DIR})${RESULT_COL}"
    make ${MAKE_ARGS} -C ${PROJECT_DIR} > /dev/null 2>&1 && echo -ne "${GREEN}OK${NC}\n" || echo -ne "${RED}FAIL${NC}\n"
    if [ "false" == ${KEEP_ARTIFACTS} ]; then
        make -C ${PROJECT_DIR} clean > /dev/null 2>&1
    fi
done
