/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/Benchmarks/memory/qspi.c
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI example source code
 *  @version    2.3.0
 *  @date       20260703
 *
 *  File content:
 *      - Flash memory service functions (registers write prepare; registers read;
 *          write complete wait)
 *      - Flash memory read/write/erase operations
 *      - Flash memory mode configuration functions
 *      - XIP mode enable/reset function
 * *****************************************************************************
 *  @copyright Copyright (c) 2025-2026 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mt25ql128aba.h"
#include "bmcu_cru.h"
#include "qspi.h"
#include "cli.h"

#ifndef IF_INSTANCE
#define IF_INSTANCE QSPI1
#endif
#define QSPI_FREQUENCY (50 * 1000 * 1000) ///< 50 Mhz

#ifdef TURN_OFF_PRINTF
#include "printf_off.h"
#endif


void
qspi_if_show (char *str) {
    (void) str;
    CRU_Clocks_TypeDef CRU_Clocks;
    size_t padding = 35;
    static const char *msg[] = {"enabled", "disabled", "ON", "OFF"};

    printf("\t1. Constant HW or HAL defined parameters:\r\n");
    printf("%-*s : %d\r\n", padding, "\t\tMax data framesize", QSPI_XFER_SIZE_MAX);
    printf("%-*s : %d\r\n", padding, "\t\tMax Rx delay clock cycles", QSPI_RX_DELAY_DEPTH_DEFAULT);
    printf("%-*s : %d\r\n", padding, "\t\tRx and Tx FIFOs size", QSPI_FIFO_DEPTH);
    printf("%-*s : %d\r\n", padding, "\t\tClock divider default value", QSPI_CLKDIVIDER_DEFAULT);

    printf( "\t2. Current interface settings\r\n");

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode is enabled\r\n"\
               "It is impossible to get QSPI interface settings\r\n"\
               "Disable XIP mode first\r\n");
        return;
    }

    printf("%-*s : %s\r\n", padding, "\t\tInterface state",
        (ENABLE == QSPI_IsEnabled(IF_INSTANCE)) ? msg[0] : msg[1]);
    printf("%-*s : ", padding, "\t\tTransfer mode");
    switch (QSPI_GetTransferMode(IF_INSTANCE)) {
        case QSPI_XFER_TXRX:
            printf("Tx and Rx");
            break;

        case QSPI_XFER_TX:
            printf("Tx");
            break;

        case QSPI_XFER_RX:
            printf("Rx");
            break;

        case QSPI_XFER_EEPROM:
            printf("EEPROM");
            break;
        default:
            break;
    }
    printf("\r\n");
    printf("%-*s : %d bits\r\n", padding, "\t\tData frame size",
        (QSPI_GetFrameSize(IF_INSTANCE) >> QSPI_CTRLR0_DFS_32_Pos) + 1);
    printf("%-*s : %s\r\n", padding, "\t\tEndianess",
        (ENABLE == QSPI_IsEnabledEndianConversion(IF_INSTANCE) ? msg[0] : msg[1]));
    printf("%-*s : %d\r\n", padding, "\t\tClock divider", QSPI_GetClkDivider(IF_INSTANCE));
    printf("%-*s : %s\r\n", padding, "\t\tClock polarity",
        (QSPI_POLARITY_LOW == QSPI_GetClockPolarity(IF_INSTANCE)) ? "low" : "high");
    printf("%-*s : %s\r\n", padding, "\t\tClock phase",
        (QSPI_PHASE_MIDDLE == QSPI_GetClockPhase(IF_INSTANCE)) ? "middle" : "start");
    printf("%-*s : %s\r\n", padding, "\t\tData frame format",
    (QSPI_FRAMEFORMAT_STD == QSPI_GetFrameFormat(IF_INSTANCE)) ? "standard" :
    ((QSPI_FRAMEFORMAT_DUAL == QSPI_GetFrameFormat(IF_INSTANCE)) ? "dual" : "quad"));
    printf("%-*s : %s\r\n", padding, "\t\tSlave select toggling",
        ((ENABLE == QSPI_IsEnabledSlaveSelToggle(IF_INSTANCE)) ? msg[0] : msg[1]));
    printf("%-*s : ", padding, "\t\tTransfer mode");
    switch (QSPI_GetAddrInstXferMode(IF_INSTANCE)) {
        case QSPI_XFER_STD:
            printf("instr and addr in standard mode");
            break;

        case QSPI_XFER_INST:
            printf("instr in standart mode, addr in \"Data frame format\" mode");
            break;

        case QSPI_XFER_BOTH:
            printf("instr and addr in \"Data frame format\" mode");
            break;

        default:
            break;
    }
    printf("\r\n");
    printf("%-*s : %d bits\r\n", padding, "\t\tAddress length", QSPI_GetAddrLen(IF_INSTANCE));
    printf("%-*s : ", padding, "\t\tinstruction length");
    switch (QSPI_GetInstLen(IF_INSTANCE)) {
        case QSPI_INSTLEN_0:
            printf("0 bits (no instruction)");
            break;

        case QSPI_INSTLEN_4:
            printf("4 bits");
            break;

        case QSPI_INSTLEN_8:
            printf("8 bits");
            break;

        case QSPI_INSTLEN_16:
            printf("16 bits");
            break;

        default:
            break;
    }
    printf("\r\n");
    printf("%-*s : %d\r\n", padding, "\t\tWait cycles",
        QSPI_GetWaitCycles(IF_INSTANCE) >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);
    printf("%-*s : %s\r\n", padding, "\t\tDDR (if Dual/Quad)",
        (ENABLE == QSPI_IsEnabledDDR(IF_INSTANCE) ? msg[0] : msg[1]));
    printf("%-*s : %s\r\n", padding, "\t\tDDR of instr",
        (ENABLE == QSPI_IsEnabledInstDDR(IF_INSTANCE) ? msg[0] : msg[1]));
    printf("%-*s : %s\r\n", padding, "\t\tTx-Rx shift regs local loop",
        (ENABLE == QSPI_IsEnabledShiftRegLoop(IF_INSTANCE)) ? msg[0] : msg[1]);
    printf("%-*s : %d\r\n", padding, "\t\tData frames to be cont. Rx-ed",
        QSPI_GetNumOfDataFrames(IF_INSTANCE));
    printf("%-*s : ", padding, "\t\tSlave select");
    switch (QSPI_GetSlaveSelect(IF_INSTANCE)) {
        case QSPI_SSEL_NOT_SELECTED:
            printf("not selected");
            break;

        case QSPI_SSEL_0:
            printf("0");
            break;

        case QSPI_SSEL_1:
            printf("1");
            break;

        case QSPI_SSEL_2:
            printf("2");
            break;

        default:
            break;
    }
    printf("\r\n");
    printf("%-*s : %d\r\n", padding, "\t\tTx FIFO level",
        QSPI_GetFIFOTxLvl(IF_INSTANCE));
    printf("%-*s : %d\r\n", padding, "\t\tRx FIFO level",
        QSPI_GetFIFORxLvl(IF_INSTANCE));
    printf("%-*s : %s\r\n", padding, "\t\tBUSY (Tx is in progress) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_BUSY) ? msg[2] : msg[3]));
    printf("%-*s : %s\r\n", padding, "\t\tTFNF (Tx FIFO not full) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF) ? msg[2] : msg[3]));
    printf("%-*s : %s\r\n", padding, "\t\tTFE (Tx FIFO is empty) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFE) ? msg[2] : msg[3]));
    printf("%-*s : %s\r\n", padding, "\t\tRFNE (Rx FIFO not empty) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE) ? msg[2] : msg[3]));
    printf("%-*s : %s\r\n", padding, "\t\tRFF (Rx FIFO is full) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFF) ? msg[2] : msg[3]));
    printf("%-*s : %s\r\n", padding, "\t\tDCOL (Data Collision Error) flag",
        (1 == QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_DCOL) ? msg[2] : msg[3]));
    printf("%-*s : %d\r\n", padding, "\t\tRx sample delay",
        QSPI_GetRxSampleDelay(IF_INSTANCE));

    printf("\t3. Clock setup\r\n");
    CRU_GetSystemClocksFreq(&CRU_Clocks);
    printf("%-*s : %lu\r\n", padding, "\t\tCCLK frequency",
        CRU_Clocks.CCLK_Frequency);
    printf("%-*s : %lu\r\n", padding, "\t\tPCLK0 frequency",
        CRU_Clocks.PCLK0_Frequency);
    printf("%-*s : %lu\r\n", padding, "\t\tPCLK1 frequency",
        CRU_Clocks.PCLK1_Frequency);

}

void
qspi_if_init (char *str) {
    (void) str;
    QSPI_InitStruct_TypeDef QSPI_InitStruct;
    CRU_Clocks_TypeDef CRU_Clocks;
    uint32_t freq;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode is enabled\r\n"\
               "It is impossible to get QSPI interface settings\r\n"\
               "Disable XIP mode first\r\n");
        return;
    }

    printf("Enable GPIO and QSPI clocks\r\n");
    if (QSPI1 == IF_INSTANCE) {
        CRU_APB1_EnableClock(CRU_APB1_PERIPH_GPIO1);
        CRU_APB1_EnableClock(CRU_APB1_PERIPH_QSPI1);

    } else if (QSPI0 == IF_INSTANCE) {
        CRU_APB0_EnableClock(CRU_APB0_PERIPH_GPIO0);
        CRU_APB0_EnableClock(CRU_APB0_PERIPH_QSPI0);
    }

    printf("Set GPIO mode to AF3 (QSPI mode)\r\n");
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_0, CRU_PIN_AF_3);      ///< QSPI1_CSN
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_1, CRU_PIN_AF_3);      ///< QSPI1_SCK
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_2, CRU_PIN_AF_3);      ///< QSPI1_IO0
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_3, CRU_PIN_AF_3);      ///< QSPI1_IO1
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_4, CRU_PIN_AF_3);      ///< QSPI1_IO2
    CRU_SetPinAF((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_5, CRU_PIN_AF_3);      ///< QSPI1_IO3

    /** GPIO driver strength control is required for QSPI high speed */
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_0, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_1, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_2, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_3, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_4, CRU_PIN_DRIVE_STRENGTH_3);
    CRU_SetPinDriveStrength((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_5, CRU_PIN_DRIVE_STRENGTH_3);

    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_0, CRU_PIN_PULL_UP);
    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_1, CRU_PIN_PULL_UP);
    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_2, CRU_PIN_PULL_UP);
    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_3, CRU_PIN_PULL_UP);
    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_4, CRU_PIN_PULL_UP);
    CRU_SetPinPull((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_5, CRU_PIN_PULL_UP);

    /** Input buffer enabling is required for data receiving in the Quad SPI mode */
    CRU_EnablePinInput((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_2);
    CRU_EnablePinInput((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_3);
    CRU_EnablePinInput((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_4);
    CRU_EnablePinInput((QSPI1 == IF_INSTANCE) ? CRU_PORT_B : CRU_PORT_A, CRU_PIN_5);

    QSPI_Disable(IF_INSTANCE);
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);

    printf("QSPI interface init (default values)\r\n");
    QSPI_StructInit(&QSPI_InitStruct);
    CRU_GetSystemClocksFreq(&CRU_Clocks);
    freq = ((QSPI1 == IF_INSTANCE) ? CRU_Clocks.PCLK1_Frequency : CRU_Clocks.PCLK0_Frequency);
    QSPI_InitStruct.ClkDivider = (freq / QSPI_FREQUENCY) & QSPI_SCKDV_MAX;
    printf("QSPI clock value: %lu MHz (divider value = %lu)\r\n",
        (long unsigned int) (QSPI_FREQUENCY / (1000 * 1000)), (long unsigned int) QSPI_InitStruct.ClkDivider);
    QSPI_Init(IF_INSTANCE, &QSPI_InitStruct);
}

void
qspi_if_set (char *str) {
    FunctionalState state = (cli_is_param_affirmative(str) ? ENABLE : DISABLE);

    printf("Set QSPI interface %s\r\n", (ENABLE == state) ? "enabled" :  "disabled");

    if (ENABLE == state) {
        QSPI_Enable(IF_INSTANCE);

    } else {
        QSPI_Disable(IF_INSTANCE);
    }
}

// Configuration functions -----------------------------------------------------
/*! Almost all "set" functions are valid only when QSPI interface is disabled */
static void
qspi_check_disabled (void) {
    if (ENABLE == QSPI_IsEnabled(IF_INSTANCE)) {
        printf("Warning: interface is enabled.\r\n");
        qspi_if_set(DISABLE);
    }
}

void
qspi_set_TransferDirection (char *str) {
    QSPI_XferMode_TypeDef TransferDirection = QSPI_XFER_TXRX;

    if (0 == strcmp("tx", str)) {
        TransferDirection = QSPI_XFER_TX;

    } else if (0 == strcmp("rx", str)) {
        TransferDirection = QSPI_XFER_RX;

    } else if (0 == strcmp("eeprom", str)) {
        TransferDirection = QSPI_XFER_EEPROM;
    }

    qspi_check_disabled();

    printf("QSPI set TransferDirection mode: ");
    switch (TransferDirection) {
        case QSPI_XFER_TXRX:
        printf("Tx and Rx");
        break;

        case QSPI_XFER_TX:
        printf("Tx");
        break;

        case QSPI_XFER_RX:
        printf("Rx");
        break;

        case QSPI_XFER_EEPROM:
        printf("EEPROM");
        break;
        default:
        break;
    }
    printf("\r\n");
    QSPI_SetTransferMode(IF_INSTANCE, TransferDirection);
}

void
qspi_set_DataFrameSize (char *str) {
    QSPI_DataFrameSize_TypeDef DataFrameSize  =
                 (QSPI_DataFrameSize_TypeDef)((atoi(str) - 1) << QSPI_CTRLR0_DFS_32_Pos);

    if (QSPI_DATA_FRAME_SIZE_4 > DataFrameSize || QSPI_DATA_FRAME_SIZE_32 < DataFrameSize) {
        printf("Error: invalid Data Frame Size value (should be 4..32)\r\n");
        return;
    }

    qspi_check_disabled();

    printf("QSPI set DataFrameSize: %d bits\r\n", (DataFrameSize >> QSPI_CTRLR0_DFS_32_Pos) + 1);
    QSPI_SetDataFrameSize(IF_INSTANCE, DataFrameSize);
}

void
qspi_set_EndiaConversion (char *str) {
    FunctionalState EndiaConversion =
    (cli_is_param_affirmative(str) ? ENABLE : DISABLE);

    qspi_check_disabled();

    printf( "QSPI set EndiaConversion: %s\r\n",
        (ENABLE == EndiaConversion ? "enabled" : "disabled"));

    if (ENABLE == EndiaConversion) {
        QSPI_EnableEndianConversion(IF_INSTANCE);

    } else {
        QSPI_DisableEndianConversion(IF_INSTANCE);
    }

}

void
qspi_set_ClkDivider (char *str) {
    uint16_t ClkDivider = atoi(str) & QSPI_SCKDV_MAX;

    qspi_check_disabled();

    printf("QSPI set ClkDivider: %d\r\n", ClkDivider);
    QSPI_SetClkDivider(IF_INSTANCE, ClkDivider);

}

void
qspi_set_ClockPolarity (char *str) {
    QSPI_Polarity_TypeDef SPIClockPolarity =
    ((0 == strcmp("high", str)) ? QSPI_POLARITY_HIGH : QSPI_POLARITY_LOW);

    qspi_check_disabled();

    printf( "QSPI set SPIClockPolarity: %s\r\n",
        (QSPI_POLARITY_HIGH == SPIClockPolarity) ? "high" : "low");
    QSPI_SetClockPolarity(IF_INSTANCE, SPIClockPolarity);

}

void
qspi_set_ClockPhase (char *str) {
    QSPI_Phase_TypeDef SPIClockPhase =
    ((0 == strcmp("start", str)) ? QSPI_PHASE_START : QSPI_PHASE_MIDDLE);

    qspi_check_disabled();

    printf( "QSPI set SPIClockPhase: %s\r\n",
        (QSPI_PHASE_MIDDLE == SPIClockPhase) ? "middle" : "start");
    QSPI_SetClockPhase(IF_INSTANCE, SPIClockPhase);
}

void
qspi_set_FrameFormat (char *str) {
    QSPI_FrameFormat_TypeDef SPIFrameFormat = QSPI_FRAMEFORMAT_QUAD;

    if (0 == strcmp("std", str)) {
        SPIFrameFormat = QSPI_FRAMEFORMAT_STD;

    } else if (0 == strcmp("dual", str)) {
        SPIFrameFormat = QSPI_FRAMEFORMAT_DUAL;
    }

    qspi_check_disabled();

    printf("QSPI set SPIFrameFormat: %s\r\n",
        (QSPI_FRAMEFORMAT_STD == SPIFrameFormat) ? "standard" :
        ((QSPI_FRAMEFORMAT_DUAL == SPIFrameFormat) ? "dual" : "quad"));
    QSPI_SetFrameFormat(IF_INSTANCE, SPIFrameFormat);
}

void
qspi_set_SlaveSelToggle (char *str) {
    FunctionalState SPISlaveSelToggle =
    (cli_is_param_affirmative(str) ? ENABLE : DISABLE);

    qspi_check_disabled();

    printf( "QSPI set SPISlaveSelToggle: %s\r\n",
        (ENABLE == SPISlaveSelToggle ? "enabled" : "disabled"));

    if (ENABLE == SPISlaveSelToggle) {
        QSPI_EnableSlaveSelToggle(IF_INSTANCE);

    } else {
        QSPI_EnableSlaveSelToggle(IF_INSTANCE);
    }
}

void
qspi_set_TransferMode (char *str) {
    QSPI_AddrInstXferMode_TypeDef AddrInstXferMode = QSPI_XFER_STD;

    if (0 == strncmp("inst", str, strlen("inst"))) {
        AddrInstXferMode = QSPI_XFER_INST;

    } else if (0 == strncmp("both", str, strlen("both"))) {
        AddrInstXferMode = QSPI_XFER_BOTH;
    }

    qspi_check_disabled();

    printf("QSPI set AddrInstXferMode: ");
    switch (AddrInstXferMode) {
        case QSPI_XFER_STD:
        printf("instr and addr in standard, i.e. not dual or quad, mode");
        break;

        case QSPI_XFER_INST:
        printf("instr in standart mode, addr in \"Data frame format\" mode");
        break;

        case QSPI_XFER_BOTH:
        printf("instr and addr in \"Data frame format\" mode");
        break;

        default:
        break;
    }
    printf("\r\n");
    QSPI_SetAddrInstXferMode(IF_INSTANCE, AddrInstXferMode);
}

void
qspi_set_AddrLen (char *str) {
    uint8_t addrlen = (uint8_t)atoi(str);

    if (QSPI_ADDRLEN_60 < addrlen || (addrlen & 0x3)) {
        printf("Error: invalid address length value (should be 0,4,8,..60)\r\n");
        return;
    }

    QSPI_AddrLen_TypeDef SPIAddrLen = (QSPI_AddrLen_TypeDef)addrlen;

    qspi_check_disabled();

    printf("QSPI set SPIAddrLen: %d bits\r\n", SPIAddrLen);
    QSPI_SetAddrLen(IF_INSTANCE, SPIAddrLen);
}

void
qspi_set_InstLen (char *str) {
    QSPI_InstLen_TypeDef InstLen;
    uint8_t instlen = (uint8_t)atoi(str);

    if (0 == instlen) {
       InstLen = QSPI_INSTLEN_0;

    } else if (4 == instlen) {
       InstLen = QSPI_INSTLEN_4;

    } else if (8 == instlen) {
       InstLen = QSPI_INSTLEN_8;

    } else if (16 == instlen) {
       InstLen = QSPI_INSTLEN_16;

    } else {
        printf("Error: invalid instr. length value (should be 0,4,8 or 16)\r\n");
        return;
    }

    qspi_check_disabled();

    printf("QSPI set InstLen: ");
    switch (InstLen) {
        case QSPI_INSTLEN_0:
        printf("0 bits (no instruction)");
        break;

        case QSPI_INSTLEN_4:
        printf("4 bits");
        break;

        case QSPI_INSTLEN_8:
        printf("8 bits");
        break;

        case QSPI_INSTLEN_16:
        printf("16 bits");
        break;

        default:
        break;
    }
    printf("\r\n");
    QSPI_SetInstLen(IF_INSTANCE, InstLen);
}

void
qspi_set_WaitCycles (char *str) {
    uint32_t waitcycles = atoi(str) << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos;

    if (0 != waitcycles &&
        (waitcycles < QSPI_WAITCYCLES_1 || waitcycles > QSPI_WAITCYCLES_31)) {
        printf("Error: invalid wait cycles value (should be 0..31)\r\n");
        return;
    }

    qspi_check_disabled();

    printf("QSPI set WaitCycles: %d\r\n", (int) (waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos));
    QSPI_SetWaitCycles(IF_INSTANCE, (QSPI_WaitCycles_TypeDef)waitcycles);
}

void
qspi_set_DDRMode (char *str) {
    FunctionalState DDRMode =
    (cli_is_param_affirmative(str) ? ENABLE : DISABLE);

    qspi_check_disabled();

    printf( "QSPI set config: DDRMode: %s\r\n",
        (ENABLE == DDRMode ? "enabled" : "disabled"));

    if (ENABLE == DDRMode) {
        QSPI_EnableDDR(IF_INSTANCE);

    } else {
        QSPI_DisableDDR(IF_INSTANCE);
    }
}

void
qspi_set_DDRInstMode (char *str) {
    FunctionalState DDRInstMode =
    (cli_is_param_affirmative(str) ? ENABLE : DISABLE);

    qspi_check_disabled();

    printf( "QSPI set config: DDRInstMode: %s\r\n",
        (ENABLE == DDRInstMode ? "enabled" : "disabled"));

    if (ENABLE == DDRInstMode) {
        QSPI_EnableInstDDR(IF_INSTANCE);

    } else {
        QSPI_DisableInstDDR(IF_INSTANCE);
    }
}

// Data transfer functions -----------------------------------------------------
/**
 *  The BUSY status is not set when the data are written into the transmit FIFO
 *  This bit gets set only when the target slave has been selected and the
 *  transfer is underway. After writing data into the transmit FIFO, the shift
 *  logic does not begin the serial transfer until a positive edge of the
 *  sclk_out signal is present. The delay in waiting for this positive edge
 *  depends on the baud rate of the serial transfer.
 *  Before polling the BUSY status, you should first poll the TFE status
 *  (waiting for 1) or wait for BAUDR * qspi_clk clock cycles.
 */
static inline void
qspi_wait_idle (void) {
    while (!QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFE));
    while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_BUSY));
}

void
qspi_std_txrx_8b (uint8_t *txbuf, uint32_t txcntr, uint8_t *rxbuf) {
    /// For TxRx mode Rx-ed data counter == Tx-ed data counter
    uint32_t rxcntr = txcntr;

    /// Set transfer mode (i.e. data direction processing) to Tx and Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_TXRX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Prefill the Tx FIFO
    while (txcntr && QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(IF_INSTANCE, *txbuf++);
        txcntr--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Write data to Tx FIFO and read data from Rx FIFO
    while (txcntr) {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(IF_INSTANCE);
            rxcntr--;
        }

        if (txcntr && QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(IF_INSTANCE, *txbuf++);
            txcntr--;
        }
    }

    /// Receive the rest of the data
    while (rxcntr) {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(IF_INSTANCE);
            rxcntr--;
        }
    }

    /// Wait for the transfer to complete
    qspi_wait_idle();

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_std_cmd_8b (uint8_t *txbuf, uint32_t txcntr, uint8_t *rxbuf, uint32_t rxcntr) {
    /// Set transfer mode (i.e. data direction processing) to Tx and Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_EEPROM);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Set the number of data frames
    QSPI_SetNumOfDataFrames(IF_INSTANCE, rxcntr - 1);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Prefill the Tx FIFO
    while (txcntr && QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(IF_INSTANCE, *txbuf++);
        txcntr--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Write data to Tx FIFO. Received data is invalid yet
    while (txcntr) {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(IF_INSTANCE, *txbuf++);
            txcntr--;
        }
    }

    /*  Tx FIFO becomes empty, data on the receive line
        (rxd) is valid and is stored in the receive FIFO
    */
    while (!QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFE));

    /// Receive the rest of the data
    do {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE)) {
            *rxbuf++ = QSPI_ReceiveData8(IF_INSTANCE);
        }
    } while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_BUSY));

    /// No need to call qspi_wait_idle(): TFE and BUSY flash already checked

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_std_rx_8b (uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_RX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Set the number of data frames
    QSPI_SetNumOfDataFrames(IF_INSTANCE, len - 1);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Set slave select signal
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Transfer starts only when data is written to the Tx FIFO
    /// Fill Tx FIFO by the single "dummy" value to start the transfer
    QSPI_TransmitData8(IF_INSTANCE, buf[0]);

    /// Wait for the control data to be sent
    while (!QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFE));

    /// Data receive
    do {
        while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE)) {
            *buf++ = QSPI_ReceiveData8(IF_INSTANCE);
        }
    } while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_BUSY));

    /// No need to call qspi_wait_idle(): TFE and BUSY flash already checked

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_std_tx_8b (uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_TX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Prefill the Tx FIFO
    while (len && QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(IF_INSTANCE, *buf++);
        len--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Write data to Tx FIFO and read data from Rx FIFO
    while (len) {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(IF_INSTANCE, *buf++);
            len--;
        }
    }

    /// Wait for the transfer to complete
    qspi_wait_idle();

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_enh_rd_8b (uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_RX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Set the number of data frames
    QSPI_SetNumOfDataFrames(IF_INSTANCE, (0 == len) ? 1 : len - 1);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Transfer starts only when instruction and address data is written to the Tx FIFO
    if (QSPI_INSTLEN_0 != QSPI_GetInstLen(IF_INSTANCE)) {
        QSPI_TransmitData8(IF_INSTANCE, inst);
    }

    if (QSPI_ADDRLEN_0 != QSPI_GetAddrLen(IF_INSTANCE)) {
        QSPI_TransmitData32(IF_INSTANCE, addr);
    }

    /// Set slave select signal
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Wait for the control data to be sent
    while (!QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFE));

    /// Data receive
    do {
        while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_RFNE)) {
            *buf++ = QSPI_ReceiveData8(IF_INSTANCE);
        }
    } while (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_BUSY));

    /// No need to call qspi_wait_idle(): TFE and BUSY flash already checked

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_enh_wr_8b (uint8_t inst, uint32_t addr, uint8_t *buf, uint16_t len) {
    /// Set transfer mode (i.e. data direction processing) to Rx
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_TX);

    /// Set data frame size 8 bits
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);

    /// Enable the interface. Transfer will not begin while slave select is released
    QSPI_Enable(IF_INSTANCE);

    /// Write instruction and address data to the Tx FIFO
    if (QSPI_INSTLEN_0 != QSPI_GetInstLen(IF_INSTANCE)) {
        QSPI_TransmitData8(IF_INSTANCE, inst);
    }

    if (QSPI_ADDRLEN_0 != QSPI_GetAddrLen(IF_INSTANCE)) {
        QSPI_TransmitData32(IF_INSTANCE, addr);
    }

    /// Prefill the Tx FIFO
    while (len && QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
        QSPI_TransmitData8(IF_INSTANCE, *buf++);
        len--;
    }

    /// Set slave select signal (transfer begins at this moment)
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Write data to Tx FIFO and read data from Rx FIFO
    while (len) {
        if (QSPI_IsActiveFlag(IF_INSTANCE, QSPI_FLAG_TFNF)) {
            QSPI_TransmitData8(IF_INSTANCE, *buf++);
            len--;
        }
    }

    /// Wait for the transfer to complete
    qspi_wait_idle();

    /// Release slave select and disable the interface
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);
}

void
qspi_xip_enable (void) {
    /// QSPI configuration is possible while the interface is disabled
    qspi_check_disabled();

    QSPI_SetClockPolarity(IF_INSTANCE, QSPI_POLARITY_LOW);
    QSPI_SetClockPhase(IF_INSTANCE, QSPI_PHASE_MIDDLE);

    /// Only read operations allowed in the XIP mode
    QSPI_SetTransferMode(IF_INSTANCE, QSPI_XFER_RX);

    /// 32-bit data width fot the XIP reads
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_32);

    QSPI_SetNumOfDataFrames(IF_INSTANCE, 0);
    QSPI_EnableEndianConversion(IF_INSTANCE);
    QSPI_EnableSlaveSelToggle(IF_INSTANCE);
    QSPI_SetFrameFormat(IF_INSTANCE, QSPI_FRAMEFORMAT_QUAD);
    QSPI_SetAddrInstXferMode(IF_INSTANCE, QSPI_XFER_BOTH);
    QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_0);

    /// 16 MB max
    QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);

    QSPI_DisableDDR(IF_INSTANCE);
    QSPI_DisableInstDDR(IF_INSTANCE);

    QSPI_Enable(IF_INSTANCE);
    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_0);

    /// Enable xip_en sideband signal
    if (QSPI1 == IF_INSTANCE) {
        CRU_QSPI1_XIP_Enable();

    } else {
        CRU_QSPI0_XIP_Enable();
    }
}

///  Disables the QSPI master to behave as a memory mapped I/O
void
qspi_xip_disable (void) {
    if (QSPI1 == IF_INSTANCE) {    ///< Disable xip_en sideband signal
        CRU_QSPI1_XIP_Disable();

    } else {
        CRU_QSPI0_XIP_Disable();
    }

    QSPI_SetSlaveSelect(IF_INSTANCE, QSPI_SSEL_NOT_SELECTED);
    QSPI_Disable(IF_INSTANCE);

    QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_8);
    QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);
    QSPI_SetDataFrameSize(IF_INSTANCE, QSPI_DATA_FRAME_SIZE_8);
    QSPI_DisableEndianConversion(IF_INSTANCE);
}

bool
qspi_xip_isenabled (void) {
    bool retval = false;

    if (QSPI1 == IF_INSTANCE) {
        retval = CRU_QSPI1_XIP_IsEnabled();

    } else {
        retval = CRU_QSPI0_XIP_IsEnabled();
    }

    return retval;
}
