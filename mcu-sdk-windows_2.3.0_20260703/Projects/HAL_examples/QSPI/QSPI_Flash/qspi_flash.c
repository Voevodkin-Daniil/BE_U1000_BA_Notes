/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_Flash/qspi_flash.c
 *  @author     Baikal electronics SDK team
 *  @brief      QSPI flash IC example source code
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
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "qspi.h"
#include "cli.h"
#include "bmcu_gpio.h"
#include "bmcu_cru.h"
#include "qspi_flash.h"
#if defined(MT25Q) 
#include "mt25ql128aba.h"
#elif defined (IS25LP032)
#include "is25lp032d.h"
#else   ///< TBD: supprot of another SPI flash ICs
#endif

/**
 * @brief   Store the current SPI flash state to minimize redundant reg reads
 */
static struct {
    bool std_mode;      ///< Standard or Quad mode
} state = {
    .std_mode = true,
};

#define REG_READ_RETRY_CNT  (10U)
#define REG_READ_RETRY_INTERVAL  (100U)

#if defined(MT25Q) 
static void
flash_set_write (FunctionalState wrstate) {
    uint8_t cmd = ((ENABLE == wrstate) ? SPI_FLASH_INS_WREN : SPI_FLASH_INS_WRDI);

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }
}

static uint16_t
flash_reg_read (uint8_t reg_read_cmd) {
    uint16_t retval = 0;
    uint8_t buf[2] = {reg_read_cmd, 0};

    if (state.std_mode) {
        qspi_std_cmd_8b(buf, 1, buf, (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? 2 : 1);

    } else {
        qspi_enh_rd_8b(buf[0], 0, buf, (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? 2 : 1);
    }

    retval = (SPI_FLASH_INS_RDNVCR == reg_read_cmd) ? ((buf[1] << 8) | buf[0]) : buf[0];

    return retval;
}

/**
 * @brief   Wait for the write in progress bit
 * @note    When the operation is in progress, the program or erase controller bit
 *          of the flag status register is set to 0. In addition, the write in progress
 *          bit is set to 1. When the operation completes, the write in progress bit
 *          is cleared to 0.
 * @retval  0 if write in progress bit was erased without timeout, else 1
 */
static int
flash_wait_write (void) {
    int retval = 0;
    uint32_t try_cnt = REG_READ_RETRY_CNT;
    uint8_t statreg = flash_reg_read(SPI_FLASH_INS_RDSR);

    while ((1 == (REG_STATUS_WRITEINPROGRESS_MSK & statreg) && try_cnt--)) {
        statreg = flash_reg_read(SPI_FLASH_INS_RDSR);
        __delay_ms(REG_READ_RETRY_INTERVAL);
    }

    if (0 == try_cnt) {
        retval = 1;
    }

    return retval;
}

void
flash_get_id_cmd (char *str) {
    (void) str;
    uint8_t buf[DEVID_LEN] = {0};

    printf("Read SPI flash ID ");

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    if (state.std_mode) {
        printf("(std)");
        buf[0] = SPI_FLASH_INS_RDID;
        qspi_std_cmd_8b(buf, 1, buf, DEVID_LEN);

    } else {
        printf("(multi)");
        qspi_enh_rd_8b(SPI_FLASH_INS_MULT_IO_RDID, 0, buf, DEVID_LEN);
    }
    printf("\r\n");

    for (size_t i = 0; i < sizeof(buf)/sizeof(*buf); ++i) {
        printf("0x%02X ", buf[i]);
    }
    printf("\r\n");

    printf("Manufacturer ID (JEDEC): %svalid\r\n",
        REG_ID_MANUFACTURER_ID_VAL == buf[REG_ID_MANUFACTURER_ID_IDX] ? "" : "in");

    printf("Memory type: ");
    switch (buf[REG_ID_MEMTYPE_IDX]) {
        case REG_ID_MEMTYPE_3V:
            printf("3V");
            break;

        case REG_ID_MEMTYPE_1V8:
            printf("1.8V");
            break;

        default:
            printf("Error");
            break;
    }
    printf("\r\n");

    printf("Memory capacity: ");
    switch (buf[REG_ID_MEMCAP_IDX]) {
        case REG_ID_MEMCAP_2GB:
            printf("2Gb");
            break;

        case REG_ID_MEMCAP_1GB:
            printf("1Gb");
            break;

        case REG_ID_MEMCAP_512MB:
            printf("512Mb");
            break;

        case REG_ID_MEMCAP_256MB:
            printf("256Mb");
            break;

        case REG_ID_MEMCAP_128MB:
            printf("128Mb");
            break;

        case REG_ID_MEMCAP_64MB:
            printf("64Mb");
            break;

        default:
            printf("Error");
            break;
    }
    printf("\r\n");
}

void
flash_reset_cmd (char *str) {
    (void) str;
    uint8_t cmd;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    if (!state.std_mode) {
        printf("Error: reset is possible only in standard mode\r\n");
        return;
    }

    cmd = SPI_FLASH_INS_REN;            ///< Reset enable
    qspi_std_txrx_8b(&cmd, 1, &cmd);
    __delay_ms(SHSL2);

    printf("Memory reset\r\n");
    cmd = SPI_FLASH_INS_RMEM;
    qspi_std_txrx_8b(&cmd, 1, &cmd);
    __delay_ms(SHSL3);
}

void
flash_clear_flagsr_cmd (char *str) {
    (void) str;
    uint8_t cmd = SPI_FLASH_INS_CLRFSR;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    printf("Flash clear flag status register\r\n");
    printf("\tflag status: before = 0x%02X", flash_reg_read(SPI_FLASH_INS_RDFSR));

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }

    printf("\tafter = 0x%02X\r\n", flash_reg_read(SPI_FLASH_INS_RDFSR));
}

void
flash_erase_cmd (char *str) {
    (void) str;
    uint8_t cmd = SPI_FLASH_INS_BE;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    printf("Flash memory bulk erase: ");
    flash_set_write(ENABLE);
    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }

    printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");
}

void
flash_get_status_cmd (char *str) {
    (void) str;
    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    uint8_t sr    = flash_reg_read(SPI_FLASH_INS_RDSR);
    uint8_t fsr   = flash_reg_read(SPI_FLASH_INS_RDFSR);
    uint8_t vcr   = flash_reg_read(SPI_FLASH_INS_RDVCR);
    uint8_t vecr  = flash_reg_read(SPI_FLASH_INS_RDVECR);
    uint16_t nvcr = flash_reg_read(SPI_FLASH_INS_RDNVCR);

    printf("SPI flash registers:\r\n");
    printf("\t%-20s: 0x%02X\t", "status", sr);
    printf("(reg_wr_en:%s, wr_en_latch:%s)\r\n",
        ((0x01 << 7) & sr) ? "enabled" : "disabled",
        ((0x01 << 1) & sr) ? "set" : "clear");

    printf("\t%-20s: 0x%02X\t","flag status", fsr);
    printf("%s%s\r\n",
        ((0x01 << 5) & fsr) ? "ERASE_ERR " : "",
        ((0x01 << 4) & fsr) ? "PRGM_ERR " : "");

    printf("\t%-20s: 0x%02X\t", "volatile", vcr);
    printf("(fasr_rd_dummy:%d XIP:%s)\r\n",
        (vcr >> 4) & 0xFF,
        (REG_VC_XIP_MSK & vcr) ? "disable" : "enable");

    printf("\t%-20s: 0x%02X\t", "volatile enhanced", vecr);
    printf("(quad:%s)\r\n",
        (REG_VEC_QUAD_MSK & vecr) ? "disabled" : "enabled");

    printf("\t%-20s: 0x%.04X\r\n", "non volatile", nvcr);
};

void
flash_read_cmd (char *str) {
    size_t bufsize = SPI_FLASH_PAGE_SIZE;
    uint32_t address = 0;
    char *paramptr;
    QSPI_AddrLen_TypeDef prev_adlen;
    QSPI_WaitCycles_TypeDef prev_wcycles;
    uint32_t xip_addr, xip_data;
    size_t xip_data_len, xip_idx;

    paramptr = cli_get_param_ptr(str, 0);   ///< 1-st param: start address
    if (NULL != paramptr) {
        address = cli_param_strtohex(paramptr);
    }

    paramptr = cli_get_param_ptr(str, 1);   ///< 2-nd param: read size
    if (NULL != paramptr) {
        bufsize = atoi(paramptr);
    }

    uint8_t *buf = (uint8_t *)malloc(bufsize);
    if (NULL == buf) {
        printf("Error: memory allocation\r\n");
        return;
    }

    if (state.std_mode) {
        printf("Read SPI flash memory in standard mode\r\n");
        buf[0] = SPI_FLASH_INS_READ;
        buf[1] = (address >> 16) & 0xFF;
        buf[2] = (address >> 8) & 0xFF;
        buf[3] = address & 0xFF;
        qspi_std_cmd_8b(buf, 4, buf, bufsize);

    } else if (qspi_xip_isenabled()) {
        printf("Read SPI flash memory in XIP mode\r\n");
        bufsize &= ~0x03;
        xip_data_len = bufsize;
        xip_addr = ((IF_INSTANCE == QSPI0) ? XIP0_BASE : XIP1_BASE) + address;
        xip_idx = 0;

        while (xip_data_len) {
            xip_data = *(uint32_t *)(xip_addr + xip_idx);   ///< Direct SPI flash read
            buf[xip_idx] = xip_data & 0xFF;
            buf[xip_idx + 1] = xip_data >> 8  & 0xFF;
            buf[xip_idx + 2] = xip_data >> 16 & 0xFF;
            buf[xip_idx + 3] = xip_data >> 24 & 0xFF;
            xip_idx += sizeof(uint32_t);
            xip_data_len -= sizeof(uint32_t);
        }

    } else {
        printf("Read SPI flash memory in Quad fast read\r\n");
        prev_adlen = QSPI_GetAddrLen(IF_INSTANCE);
        prev_wcycles = QSPI_GetWaitCycles(IF_INSTANCE);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);
        QSPI_SetWaitCycles(IF_INSTANCE, QSPI_WAITCYCLES_10);
        qspi_enh_rd_8b(SPI_FLASH_INS_QIOFR, address, buf, bufsize);
        QSPI_SetAddrLen(IF_INSTANCE, prev_adlen);
        QSPI_SetWaitCycles(IF_INSTANCE, prev_wcycles);
    }

    printf("address: 0x%06X\r\nlength: %d\r\n", (unsigned int) address, bufsize);
    cli_hexdump(buf, bufsize, address);
    free(buf);
}

void
flash_write_cmd (char *str) {
    uint8_t buf[4];
    /**
     * @note Precompiled LED blink code fore EVU-LI and EVU-BA boards
     */
    uint8_t example[] = {
        0xb7, 0x96, 0x00, 0x11, 0xdc, 0x42, 0x21, 0x68, 0x37, 0x97, 0x00, 0x13,
        0xb3, 0xe7, 0x07, 0x01, 0xdc, 0xc2, 0x5c, 0x43, 0xb7, 0xf5, 0xfa, 0x02,
        0x37, 0x16, 0x00, 0x11, 0x93, 0xe7, 0x07, 0x40, 0x5c, 0xc3, 0x93, 0x85,
        0x05, 0x08, 0x93, 0x07, 0x10, 0x03, 0x93, 0x08, 0x90, 0x03, 0x88, 0x42,
        0x33, 0x45, 0x05, 0x01, 0x88, 0xc2, 0x08, 0x43, 0x13, 0x45, 0x05, 0x40,
        0x08, 0xc3, 0x1c, 0xc2, 0xf3, 0x22, 0x00, 0xc0, 0x73, 0x23, 0x00, 0xc0,
        0x33, 0x03, 0x53, 0x40, 0xe3, 0x6c, 0xb3, 0xfe, 0x85, 0x07, 0xe3, 0x9e,
        0x17, 0xfd, 0xb5, 0x47, 0x1c, 0xc2, 0xa9, 0x47, 0x1c, 0xc2, 0x82, 0x80
    };
    uint8_t *data = example;
    uint16_t len = sizeof(example);
    uint32_t address = 0x00000000;
    uint8_t *std_buf;
    char *paramptr;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    paramptr = cli_get_param_ptr(str, 0);   ///< 1-st param: start address
    if (NULL != paramptr) {
        address = cli_param_strtohex(paramptr);
    }

    paramptr = cli_get_param_ptr(str, 1);   ///< 2-nd param: string to be written
    if (NULL != paramptr) {
        data = (uint8_t *) paramptr;
        len = strlen(paramptr);
    }

    flash_set_write(ENABLE);

    if (state.std_mode) {
        std_buf = malloc(sizeof(buf)/sizeof(*buf) + len);

        if (NULL == std_buf) {
            printf("Error: memory allocation\r\n");
            return;
        }

        printf("Write data in standard mode\r\n");
        buf[0] = SPI_FLASH_INS_PP;
        buf[1] = (address >> 16) & 0xFF;
        buf[2] = (address >> 8) & 0xFF;
        buf[3] = address & 0xFF;
        memcpy(std_buf, buf, sizeof(buf)/sizeof(*buf));
        memcpy(std_buf + sizeof(buf)/sizeof(*buf), data, len);
        qspi_std_tx_8b(std_buf, sizeof(buf)/sizeof(*buf) + len);
        free(std_buf);

    } else {
        printf("Write data in Quad mode\r\n");
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);
        qspi_enh_wr_8b(SPI_FLASH_INS_QIFP, address, data, len);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_0);
    }

    printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");
}

void
flash_set_quad_cmd (char *str) {
    uint8_t buf[2];
    uint8_t regval;
    char *paramptr;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    printf("Quad mode ");
    paramptr = cli_get_param_ptr(str, 0);

    if (NULL == paramptr) {
        printf("Error: invalid parameters\r\n");
        return;

    } else if (cli_is_param_affirmative(paramptr)) {    ///< Enable Quad mode
        printf("enable\r\n");

        if (!state.std_mode) {
            printf("Already in Quad mode\r\n");
            return;
        }

        /// SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = flash_reg_read(SPI_FLASH_INS_RDVECR);
        /// Enable quad I/O command input (4-4-4 mode)
        regval = regval & ~REG_VEC_QUAD_MSK;
        /// Write back the Enhanced Volatile Configuration Register
        buf[0] = SPI_FLASH_INS_WRVECR;
        buf[1] = regval;
        flash_set_write(ENABLE);
        qspi_std_txrx_8b(buf, 2, buf);

        /// SPI flash memory: enter Quad I/O mode
        buf[0] = SPI_FLASH_INS_QUAD_ENTER;
        qspi_std_txrx_8b(buf, 1, buf);

        /// QSPI interface: switch to the Quad mode
        QSPI_SetFrameFormat(IF_INSTANCE, QSPI_FRAMEFORMAT_QUAD);
        QSPI_SetAddrInstXferMode(IF_INSTANCE, QSPI_XFER_BOTH);

        state.std_mode = false;

    } else {    ///< Disable Quad mode
        printf("disable\r\n");

        if (state.std_mode) {
            printf("Already in standard mode\r\n");
            return;
        }

        /// SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = flash_reg_read(SPI_FLASH_INS_RDVECR);
        /// Disable quad I/O command input (4-4-4 mode)
        regval = regval | REG_VEC_QUAD_MSK;
        /// Write back the Enhanced Volatile Configuration Register
        flash_set_write(ENABLE);
        qspi_enh_wr_8b(SPI_FLASH_INS_WRVECR, 0, &regval, 1);

        /// SPI flash memory: reset Quad I/O mode
        qspi_enh_wr_8b(SPI_FLASH_INS_QUAD_RESET, 0, &regval, 0);

        /// QSPI interface: switch to the standard mode
        QSPI_SetFrameFormat(IF_INSTANCE, QSPI_FRAMEFORMAT_STD);
        QSPI_SetAddrInstXferMode(IF_INSTANCE, QSPI_XFER_STD);

        state.std_mode = true;
    }
}

void
flash_set_xip_cmd (char *str) {
    uint8_t buf[2];
    uint8_t regval;
    QSPI_WaitCycles_TypeDef waitcycles = QSPI_WAITCYCLES_14;
    uint32_t tmp;
    char *paramptr;

    paramptr = cli_get_param_ptr(str, 0);

    if (NULL == paramptr) {
        printf("Error: invalid parameters\r\n");
        return;

    } else if (cli_is_param_affirmative(paramptr)) {    ///< Enable XIP mode
        if (qspi_xip_isenabled()) {
            printf("Already in XIP mode\r\n");
            return;
        }

        /// Get Wait cycles value and enable XIP mode
        printf("SPI flash: switch to the XIP mode\r\n");
        paramptr = cli_get_param_ptr(str, 1);

        /// 2-nd optional parameter: number of wait cycles
        if (NULL != paramptr) {
            tmp = atoi(paramptr) << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos;

            /// 2-nd param is valid (see MT25QL command definitions for Quad mode read operations)
            if (tmp >= QSPI_WAITCYCLES_10 && tmp <= QSPI_WAITCYCLES_14) {
                waitcycles = (QSPI_WaitCycles_TypeDef)tmp;
                printf("Waitcycles: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);

            } else {    ///< 2-nd param is invalid
                printf("Use default WaitCycles value: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);
            }

        }  else {
            printf("Use default WaitCycles value: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);
        }

        /// Swtich the SPI flash and QSPI interface to the Quad SPI mode
        flash_set_quad_cmd("enable");

        /// Get the Volatile Configuration Register current state
        regval = flash_reg_read(SPI_FLASH_INS_RDVCR);
        /// Configure SPI flash dummy cycles for FAST READ commands
        regval &= ~REG_VC_DCL_MSK;
        regval |= ((waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos) << REG_VC_DCL_POS);
        /// Clear XIP mode bit (i.e. enable XIP mode)
        regval &= ~REG_VC_XIP_MSK;
        /// Write back the Volatile Configuration Register
        flash_set_write(ENABLE);
        qspi_enh_wr_8b(SPI_FLASH_INS_WRVCR, 0, &regval, 1);
        flash_wait_write();

        /// Config QSPI interface waitcycles according to SPI flash settings
        QSPI_SetWaitCycles(IF_INSTANCE, waitcycles);

        /// Drive the XIP confirmation bit to 0 during FAST READ operation
        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_8);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_28);
        qspi_enh_rd_8b(SPI_FLASH_INS_FAST_READ, 0xFFFFFFE, buf, 0);

        /// Switch QSPI itself in XIP mode
        qspi_xip_enable();

    } else {    ///< Disable XIP mode
        if (!qspi_xip_isenabled()) {
            printf("Already non-XIP mode\r\n");
            return;
        }

        printf("SPI flash: switch to the non-XIP mode\r\n");

        /// Reset QSPI interface XIP mode
        qspi_xip_disable();

        /// Drive the XIP confirmation bit to 1 (the first dummy clock cycle)
        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_0);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_28);
        qspi_enh_rd_8b(SPI_FLASH_INS_FAST_READ, 0xFFFFFFF, buf, 0);

        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_8);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_0);

        printf("Set SPI flash dummy cycles to the default value\r\n");
        QSPI_SetWaitCycles(IF_INSTANCE, QSPI_WAITCYCLES_0);
        regval = flash_reg_read(SPI_FLASH_INS_RDVCR);
        regval &= ~REG_VC_DCL_MSK;
        regval |= ((QSPI_WAITCYCLES_15 >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos) << REG_VC_DCL_POS);
        /// The device automatically resets volatile configuration register bit 3 to 1.
        flash_set_write(ENABLE);
        qspi_enh_wr_8b(SPI_FLASH_INS_WRVCR, 0, &regval, 1);
        flash_wait_write();

        printf("Quad mode is still active now\r\n");
    }
}

void
flash_exec_cmd (char *str) {
    if (!qspi_xip_isenabled()) {
        printf("Error: XIP mode is disabled now. XIP mode is required for code execution from SPI flash memory\r\n");
        return;
    }

    void(*flashapp)(void) = (void(*)(void))(((IF_INSTANCE == QSPI0) ? XIP0_BASE : XIP1_BASE) + cli_param_strtohex(str));
    printf("Execute code from the SPI flash (start address: %p)\r\n", flashapp);
    flashapp();
}
#elif defined (IS25LP032)
static void
flash_set_write (FunctionalState wrstate) {
    uint8_t cmd = ((ENABLE == wrstate) ? SPI_FLASH_INS_WREN : SPI_FLASH_INS_WRDI);

    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }
}

static uint16_t
flash_reg_read (uint8_t reg_read_cmd) {
    uint16_t retval = 0;
    uint8_t buf[2] = {reg_read_cmd, 0};

    if (state.std_mode) {
        qspi_std_cmd_8b(buf, 1, buf, 1);

    } else {
        qspi_enh_rd_8b(buf[0], 0, buf, 1);
    }

    retval = buf[0];

    return retval;
}

/**
 * @brief   Wait for the write in progress bit
 * @note    When the operation is in progress, the program or erase controller bit
 *          of the flag status register is set to 0. In addition, the write in progress
 *          bit is set to 1. When the operation completes, the write in progress bit
 *          is cleared to 0.
 * @retval  0 if write in progress bit was erased without timeout, else 1
 */
static int
flash_wait_write (void) {
    int retval = 0;
    uint32_t try_cnt = REG_READ_RETRY_CNT;
    uint8_t statreg = flash_reg_read(SPI_FLASH_INS_RDSR);

    while ((1 == (REG_STATUS_WRITEINPROGRESS_MSK & statreg) && try_cnt--)) {
        statreg = flash_reg_read(SPI_FLASH_INS_RDSR);
        __delay_ms(REG_READ_RETRY_INTERVAL);
    }

    if (0 == try_cnt) {
        retval = 1;
    }

    return retval;
}

void
flash_get_id_cmd (char *str) {
    (void) str;
    uint8_t buf[DEVID_LEN] = {0};

    printf("Read SPI flash ID ");

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    if (state.std_mode) {
        printf("(std)");
        buf[0] = SPI_FLASH_INS_RDJDID;
        qspi_std_cmd_8b(buf, 1, buf, DEVID_LEN);

    } else {
        printf("(multi)");
        qspi_enh_rd_8b(SPI_FLASH_INS_RDJDIDQ, 0, buf, DEVID_LEN);
    }
    printf("\r\n");

    for (size_t i = 0; i < sizeof(buf)/sizeof(*buf); ++i) {
        printf("0x%02X ", buf[i]);
    }
    printf("\r\n");

    printf("Manufacturer ID (JEDEC): %svalid\r\n",
        REG_ID_MANUFACTURER_ID_VAL == buf[REG_ID_MANUFACTURER_ID_IDX] ? "" : "in");

    printf("Memory type: ");
    switch (buf[REG_ID_MEMTYPE_IDX]) {
        case REG_ID_MEMTYPE_3V:
            printf("3V");
            break;

        case REG_ID_MEMTYPE_1V8:
            printf("1.8V");
            break;

        default:
            printf("Error");
            break;
    }
    printf("\r\n");

    printf("Memory capacity: ");
    switch (buf[REG_ID_MEMCAP_IDX]) {
        case REG_ID_MEMCAP_256MB:
            printf("256Mb");
            break;

        case REG_ID_MEMCAP_128MB:
            printf("128Mb");
            break;

        case REG_ID_MEMCAP_64MB:
            printf("64Mb");
            break;

        case REG_ID_MEMCAP_32MB:
            printf("32Mb");
            break;   
            
        case REG_ID_MEMCAP_16MB:
            printf("32Mb");
            break;              

        default:
            printf("Error");
            break;
    }
    printf("\r\n");
}

void
flash_reset_cmd (char *str) {
    (void) str;
    uint8_t cmd;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    if (!state.std_mode) {
        printf("Error: reset is possible only in standard mode\r\n");
        return;
    }

    cmd = SPI_FLASH_INS_REN;            ///< Reset enable
    qspi_std_txrx_8b(&cmd, 1, &cmd);
    __delay_ms(SHSL2);

    printf("Memory reset\r\n");
    cmd = SPI_FLASH_INS_RMEM;
    qspi_std_txrx_8b(&cmd, 1, &cmd);
    __delay_ms(SHSL3);
}

void
flash_clear_flagsr_cmd (char *str) {
    (void) str;
    uint8_t buf[2] = {0};

    buf[0] = SPI_FLASH_INS_WRSR;
    buf[1] = 0;
    flash_set_write(ENABLE);
    qspi_std_txrx_8b(buf, 2, buf); 
    
    printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");    
}

void
flash_erase_cmd (char *str) {
    (void) str;
    uint8_t cmd = SPI_FLASH_INS_CER;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    printf("Flash memory bulk erase: ");
    flash_set_write(ENABLE);
    if (state.std_mode) {
        qspi_std_txrx_8b(&cmd, 1, &cmd);

    } else {
        qspi_enh_wr_8b(cmd, 0, &cmd, 0);
    }

    printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");
}

void
flash_get_status_cmd (char *str) {
    (void) str;
    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    uint8_t sr    = flash_reg_read(SPI_FLASH_INS_RDSR);


    printf("SPI flash registers:\r\n");
    printf("\t%-20s: 0x%02X\t", "status", sr);
    printf("(reg_wr_en:%s, wr_en_latch:%s)\r\n",
        ((0x01 << 7) & sr) ? "enabled" : "disabled",
        ((0x01 << 1) & sr) ? "set" : "clear");

};

void
flash_read_cmd (char *str) {
    size_t bufsize = SPI_FLASH_PAGE_SIZE;
    uint32_t address = 0;
    char *paramptr;
    QSPI_AddrLen_TypeDef prev_adlen;
    QSPI_WaitCycles_TypeDef prev_wcycles;
    uint32_t xip_addr, xip_data;
    size_t xip_data_len, xip_idx;

    paramptr = cli_get_param_ptr(str, 0);   ///< 1-st param: start address
    if (NULL != paramptr) {
        address = cli_param_strtohex(paramptr);
    }

    paramptr = cli_get_param_ptr(str, 1);   ///< 2-nd param: read size
    if (NULL != paramptr) {
        bufsize = atoi(paramptr);
    }

    uint8_t *buf = (uint8_t *)malloc(bufsize);
    if (NULL == buf) {
        printf("Error: memory allocation\r\n");
        return;
    }

    if (state.std_mode) {
        printf("Read SPI flash memory in standard mode\r\n");
        buf[0] = SPI_FLASH_INS_NORD;
        buf[1] = (address >> 16) & 0xFF;
        buf[2] = (address >> 8) & 0xFF;
        buf[3] = address & 0xFF;
        qspi_std_cmd_8b(buf, 4, buf, bufsize);

    } else if (qspi_xip_isenabled()) {
        printf("Read SPI flash memory in XIP mode\r\n");
        bufsize &= ~0x03;
        xip_data_len = bufsize;
        xip_addr = ((IF_INSTANCE == QSPI0) ? XIP0_BASE : XIP1_BASE) + address;
        xip_idx = 0;

        while (xip_data_len) {
            xip_data = *(uint32_t *)(xip_addr + xip_idx);   ///< Direct SPI flash read
            buf[xip_idx] = xip_data & 0xFF;
            buf[xip_idx + 1] = xip_data >> 8  & 0xFF;
            buf[xip_idx + 2] = xip_data >> 16 & 0xFF;
            buf[xip_idx + 3] = xip_data >> 24 & 0xFF;
            xip_idx += sizeof(uint32_t);
            xip_data_len -= sizeof(uint32_t);
        }

    } else {
        printf("Read SPI flash memory in Quad fast read\r\n");
        prev_adlen = QSPI_GetAddrLen(IF_INSTANCE);
        prev_wcycles = QSPI_GetWaitCycles(IF_INSTANCE);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);
        QSPI_SetWaitCycles(IF_INSTANCE, QSPI_WAITCYCLES_6);
        qspi_enh_rd_8b(SPI_FLASH_INS_FRQIO, address, buf, bufsize);
        QSPI_SetAddrLen(IF_INSTANCE, prev_adlen);
        QSPI_SetWaitCycles(IF_INSTANCE, prev_wcycles);
    }

    printf("address: 0x%06X\r\nlength: %d\r\n", (unsigned int) address, bufsize);
    cli_hexdump(buf, bufsize, address);
    free(buf);
}

void
flash_write_cmd (char *str) {
    uint8_t buf[4];
    /**
     * @note Precompiled LED blink code fore EVU-LI and EVU-BA boards
     */
    uint8_t example[] = {
        0xb7, 0x96, 0x00, 0x11, 0xdc, 0x42, 0x21, 0x68, 0x37, 0x97, 0x00, 0x13,
        0xb3, 0xe7, 0x07, 0x01, 0xdc, 0xc2, 0x5c, 0x43, 0xb7, 0xf5, 0xfa, 0x02,
        0x37, 0x16, 0x00, 0x11, 0x93, 0xe7, 0x07, 0x40, 0x5c, 0xc3, 0x93, 0x85,
        0x05, 0x08, 0x93, 0x07, 0x10, 0x03, 0x93, 0x08, 0x90, 0x03, 0x88, 0x42,
        0x33, 0x45, 0x05, 0x01, 0x88, 0xc2, 0x08, 0x43, 0x13, 0x45, 0x05, 0x40,
        0x08, 0xc3, 0x1c, 0xc2, 0xf3, 0x22, 0x00, 0xc0, 0x73, 0x23, 0x00, 0xc0,
        0x33, 0x03, 0x53, 0x40, 0xe3, 0x6c, 0xb3, 0xfe, 0x85, 0x07, 0xe3, 0x9e,
        0x17, 0xfd, 0xb5, 0x47, 0x1c, 0xc2, 0xa9, 0x47, 0x1c, 0xc2, 0x82, 0x80
    };
    uint8_t *data = example;
    uint16_t len = sizeof(example);
    uint32_t address = 0x00000000;
    uint8_t *std_buf;
    char *paramptr;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    paramptr = cli_get_param_ptr(str, 0);   ///< 1-st param: start address
    if (NULL != paramptr) {
        address = cli_param_strtohex(paramptr);
    }

    paramptr = cli_get_param_ptr(str, 1);   ///< 2-nd param: string to be written
    if (NULL != paramptr) {
        data = (uint8_t *) paramptr;
        len = strlen(paramptr);
    }

    flash_set_write(ENABLE);

    if (state.std_mode) {
        std_buf = malloc(sizeof(buf)/sizeof(*buf) + len);

        if (NULL == std_buf) {
            printf("Error: memory allocation\r\n");
            return;
        }

        printf("Write data in standard mode\r\n");
        buf[0] = SPI_FLASH_INS_PP;
        buf[1] = (address >> 16) & 0xFF;
        buf[2] = (address >> 8) & 0xFF;
        buf[3] = address & 0xFF;
        memcpy(std_buf, buf, sizeof(buf)/sizeof(*buf));
        memcpy(std_buf + sizeof(buf)/sizeof(*buf), data, len);
        qspi_std_tx_8b(std_buf, sizeof(buf)/sizeof(*buf) + len);
        free(std_buf);

    } else {
        printf("Write data in Quad mode\r\n");
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_24);
        qspi_enh_wr_8b(SPI_FLASH_INS_PPQ, address, data, len);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_0);
    }

    printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");
}

void
flash_set_quad_cmd (char *str) {
    uint8_t buf[2];
    uint8_t regval;
    char *paramptr;

    if (qspi_xip_isenabled()) {
        printf("Error: XIP mode enabled, operation rejected\r\n");
        return;
    }

    printf("Quad mode ");
    paramptr = cli_get_param_ptr(str, 0);

    if (NULL == paramptr) {
        printf("Error: invalid parameters\r\n");
        return;

    } else if (cli_is_param_affirmative(paramptr)) {    ///< Enable Quad mode
        printf("enable\r\n");

        if (!state.std_mode) {
            printf("Already in Quad mode\r\n");
            return;
        }

        /// SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = flash_reg_read(SPI_FLASH_INS_RDSR);
        /// Enable quad I/O command input (4-4-4 mode)
        regval |= REG_STATUS_QE_MSK;
        /// Write back the Enhanced Volatile Configuration Register
        buf[0] = SPI_FLASH_INS_WRSR;
        buf[1] = regval;
        flash_set_write(ENABLE);
        qspi_std_txrx_8b(buf, 2, buf);

        //wait for ready
        printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");

        /// SPI flash memory: enter Quad I/O mode
        buf[0] = SPI_FLASH_INS_QPIEN;  // QPI
        qspi_std_txrx_8b(buf, 1, buf);

        /// QSPI interface: switch to the Quad mode
        QSPI_SetFrameFormat(IF_INSTANCE, QSPI_FRAMEFORMAT_QUAD);
        QSPI_SetAddrInstXferMode(IF_INSTANCE, QSPI_XFER_BOTH);

        state.std_mode = false;

    } else {    ///< Disable Quad mode
        printf("disable\r\n");

        if (state.std_mode) {
            printf("Already in standard mode\r\n");
            return;
        }

        /// SPI flash memory: reset Quad I/O mode
        qspi_enh_wr_8b(SPI_FLASH_INS_QPIDI, 0, &regval, 0);

        /// QSPI interface: switch to the standard mode
        QSPI_SetFrameFormat(IF_INSTANCE, QSPI_FRAMEFORMAT_STD);
        QSPI_SetAddrInstXferMode(IF_INSTANCE, QSPI_XFER_STD);

        state.std_mode = true;

        /// SPI flash memory: get the current state of Enhanced Volatile Configuration Register
        regval = flash_reg_read(SPI_FLASH_INS_RDSR);
        /// Disable quad I/O command input (4-4-4 mode)
        regval &= ~REG_STATUS_QE_MSK;
        /// Write back the Enhanced Volatile Configuration Register
        buf[0] = SPI_FLASH_INS_WRSR;
        buf[1] = regval;
        flash_set_write(ENABLE);
        qspi_std_txrx_8b(buf, 2, buf);

        //wait for ready
        printf("%s\r\n", (0 == flash_wait_write()) ? "done" : "error");         
    }
}

void
flash_set_xip_cmd (char *str) {
    uint8_t buf[2];
    QSPI_WaitCycles_TypeDef waitcycles = QSPI_WAITCYCLES_4;
    uint32_t tmp;
    char *paramptr;

    paramptr = cli_get_param_ptr(str, 0);

    if (NULL == paramptr) {
        printf("Error: invalid parameters\r\n");
        return;

    } else if (cli_is_param_affirmative(paramptr)) {    ///< Enable XIP mode
        if (qspi_xip_isenabled()) {
            printf("Already in XIP mode\r\n");
            return;
        }

        /// Get Wait cycles value and enable XIP mode
        printf("SPI flash: switch to the XIP mode\r\n");
        paramptr = cli_get_param_ptr(str, 1);

        /// 2-nd optional parameter: number of wait cycles
        if (NULL != paramptr) {
            tmp = atoi(paramptr) << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos;

            /// 2-nd param is valid (see MT25QL command definitions for Quad mode read operations)
            if (tmp >= QSPI_WAITCYCLES_8 && tmp <= QSPI_WAITCYCLES_14) {
                waitcycles = (QSPI_WaitCycles_TypeDef)tmp;
                printf("Waitcycles: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);

            } else {    ///< 2-nd param is invalid
                printf("Use default WaitCycles value: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);
            }

        }  else {
            printf("Use default WaitCycles value: %d\r\n", waitcycles >> QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos);
        }

        /// Swtich the SPI flash and QSPI interface to the Quad SPI mode
        flash_set_quad_cmd("enable");

        /// Config QSPI interface waitcycles according to SPI flash settings
        QSPI_SetWaitCycles(IF_INSTANCE, waitcycles);

        /// Drive the XIP confirmation bit to 0 during FAST READ operation
        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_8);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_32);
        qspi_enh_rd_8b(0xEB, 0x000000A0, buf, 0);

        /// Switch QSPI itself in XIP mode
        qspi_xip_enable();
            
    } else {    ///< Disable XIP mode
        if (!qspi_xip_isenabled()) {
            printf("Already non-XIP mode\r\n");
            return;
        }

        printf("SPI flash: switch to the non-XIP mode\r\n");

        /// Reset QSPI interface XIP mode
        qspi_xip_disable();

        /// Drive the Mode Bits to FF  to exit device from the AX read operation
        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_0);
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_32);        
        qspi_enh_rd_8b(0xEB, 0xFFFFFFFF, buf, 0);

        QSPI_SetWaitCycles(IF_INSTANCE, QSPI_WAITCYCLES_0); 
        QSPI_SetInstLen(IF_INSTANCE, QSPI_INSTLEN_8);       
        QSPI_SetAddrLen(IF_INSTANCE, QSPI_ADDRLEN_0);

        printf("Quad mode is still active now\r\n");
    }
}

void
flash_exec_cmd (char *str) {
    if (!qspi_xip_isenabled()) {
        printf("Error: XIP mode is disabled now. XIP mode is required for code execution from SPI flash memory\r\n");
        return;
    }

    void(*flashapp)(void) = (void(*)(void))(((IF_INSTANCE == QSPI0) ? XIP0_BASE : XIP1_BASE) + cli_param_strtohex(str));
    printf("Execute code from the SPI flash (start address: %p)\r\n", flashapp);
    flashapp();
}
#else
#endif
