#!/usr/bin/env bash

# Baikal MCU SDK
# Coremark project images build script.
# Syntax: <script_name>
# Example: build_images.sh

# Set ESC-codes for color control ----------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

NAME=coremark

for PARAM_ITERATIONS in 1 10 100 1000 10000
do
    for RUN_MODE in RUN_CORE0 RUN_CORE1 RUN_CORE01_PAR
    do
        for CLK_CCLK in 25 100 200
        do
            for MEM_REG_ROM in TCMA EFLASH QSPI1
            do
                for OPT in 0 1 2 3 s fast
                do
                    if [[ "$MEM_REG_ROM" == "QSPI1" ]]; then
                        for QSPI_CLK in 25 50  
                        do
                            echo -ne "> Make ${NAME}: make PARAM_ITERATIONS=$PARAM_ITERATIONS RUN_MODE=$RUN_MODE CLK_CCLK=$CLK_CCLK MEM_REG_ROM=$MEM_REG_ROM QSPI_CLK=$QSPI_CLK OPT=$OPT\t"
                            make PARAM_ITERATIONS=$PARAM_ITERATIONS RUN_MODE=$RUN_MODE CLK_CCLK=$CLK_CCLK MEM_REG_ROM=$MEM_REG_ROM QSPI_CLK=$QSPI_CLK OPT=$OPT > /dev/null 2>&1 && echo -ne "${GREEN}OK${NC}\n" || echo -ne "${RED}FAIL${NC}\n"
                        done
                    else
                        echo -ne "> Make ${NAME}: make PARAM_ITERATIONS=$PARAM_ITERATIONS RUN_MODE=$RUN_MODE CLK_CCLK=$CLK_CCLK MEM_REG_ROM=$MEM_REG_ROM OPT=$OPT\t"
                        make PARAM_ITERATIONS=$PARAM_ITERATIONS RUN_MODE=$RUN_MODE CLK_CCLK=$CLK_CCLK MEM_REG_ROM=$MEM_REG_ROM OPT=$OPT > /dev/null 2>&1 && echo -ne "${GREEN}OK${NC}\n" || echo -ne "${RED}FAIL${NC}\n"
                    fi
                done
            done
        done
    done
done
