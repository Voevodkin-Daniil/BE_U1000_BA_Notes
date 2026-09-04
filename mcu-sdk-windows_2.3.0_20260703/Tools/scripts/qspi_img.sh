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

XIP_LOADER_DIR="${SDK_ROOT}/Projects/HAL_examples/QSPI/QSPI_XIPLoader"
APP_PROJECT_DIR="${SDK_ROOT}/Projects/HAL_examples/GPIO/GPIO_LEDBlink"
QSPI_IMAGE_NAME=${SCRIPT_DIR}/"qspi_img.bin"
QSPI_APP_OFFSET=0x10000

# Help -------------------------------------------------------------------------
help () {
    echo "MCU SDK: QSPI flash memory image build script (1.0, 2025.10.06)"
    echo "Combine the XIP loader and the Application firmware into a single ready-to-program image"
    echo "  -h, --help        show this help"
    echo "  -a, --app-prj     path to the Application project"
    echo "                        default: ${APP_PROJECT_DIR}"
    echo "  -m, --mem-offset  QSPI memory offset for the Application firmware"
    echo "                        default: ${QSPI_APP_OFFSET}"
    echo "  -o, --output      QSPI image output location"
    echo "                        default: ${QSPI_IMAGE_NAME}"
    echo "  -x, --xip-prj     path to the XIP loader project"
    echo "                        default: ${XIP_LOADER_DIR}"
}

# Parse argements --------------------------------------------------------------
POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            help
            exit 0
            ;;

        -a|--app_prj)
            APP_PROJECT_DIR=$2
            shift # past argument
            shift # past value
            ;;

        -m|--mem-offset)
            QSPI_APP_OFFSET=$2
            shift # past argument
            shift # past value
            ;;

        -o|--output-dir)
            QSPI_IMAGE_NAME=$2
            shift # past argument
            shift # past value
            ;;

        -x|--xip_prj)
            XIP_LOADER_DIR=$2
            shift # past argument
            shift # past value
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

echo "--------------------------------------------------------------------------------"
echo "QSPI image build parameters:"
echo "    XIP loader project path:   ${XIP_LOADER_DIR}"
echo "    Application project path:  ${APP_PROJECT_DIR}"
echo "    Application memory offset: ${QSPI_APP_OFFSET}"
echo "    QSPI image path:           ${QSPI_IMAGE_NAME}"

# Main procedure ---------------------------------------------------------------
echo -e "\n--- 1. Build XIP loader project ------------------------------------------------"
make -C ${XIP_LOADER_DIR} clean && make -C ${XIP_LOADER_DIR} QSPI_APP_OFFSET=${QSPI_APP_OFFSET}
XIP_LOADER_BIN_PATH=$(find ${XIP_LOADER_DIR} -type f -name "*.bin")

echo -e "\n--- 2. Build Application project -----------------------------------------------"
make -C ${APP_PROJECT_DIR} clean && make -C ${APP_PROJECT_DIR} MEM_REG_ROM=QSPI1 MEM_REG_ROM_OFFSET=${QSPI_APP_OFFSET}
APP_BIN_PATH=$(find ${APP_PROJECT_DIR} -type f -name "*.bin")

echo -e "\n--- 3. Build resulting binary image --------------------------------------------"
truncate -s $(( ${QSPI_APP_OFFSET} + $(wc -c <${APP_BIN_PATH}) )) ${QSPI_IMAGE_NAME}
dd if=${XIP_LOADER_BIN_PATH} of=${QSPI_IMAGE_NAME} bs=1 seek=0 conv=notrunc
dd if=${APP_BIN_PATH} of=${QSPI_IMAGE_NAME} bs=1 seek=$(( ${QSPI_APP_OFFSET} )) conv=notrunc
echo -e "\nThe resulting QSPI memory binary image ($(wc -c < ${QSPI_IMAGE_NAME}) bytes):\r\n${QSPI_IMAGE_NAME}"
