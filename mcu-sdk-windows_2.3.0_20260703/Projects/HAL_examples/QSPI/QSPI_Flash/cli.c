/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/QSPI/QSPI_Flash/cli.c
 *  @author     Baikal electronics SDK team
 *  @brief      Command line interface
 *  @version    2.3.0
 *  @date       20260703
 *
 *  CLI provides convenient text mode interface for user interaction with the
 *  microcontroller firmware functionality: configure, get status, show data etc.
 *
 *  File content:
 *      - UART service functions
 *          UART0_IRQHandler()
 *          cli_char_rx_callback()
 *      - Commands list with short help and callback functions
 *          cli_cmd[]
 *      - Service functions
 *          cli_init()
 *          cli_clrscr()
 *          cli_prompt()
 *          cli_header()
 *      - Commands parameter parsing functions
 *          cli_get_param_ptr()
 *          cli_is_param_affirmative()
 *      - Data print function cli_hexdump()
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "cli.h"

#include "bmcu_uart.h"
#include "qspi.h"
#include "qspi_flash.h"

#define CLI_CMDLEN_MAX 80
volatile uint8_t cli_rx_char;   ///< New character code from the UART ISR

static void cmd_cli_help(char *args);
static void cmd_test(char *args);

/// CLI state tracking structure
static struct {
    uint32_t len;
    char str[CLI_CMDLEN_MAX + 1];
    int cmd_max_len;
} stat;

/// CLI commands list with short help and execution functions pointers
static struct {
    const char *name;
    const char *help;
    void (*executor)(char *args);
} cli_cmd[] = {
    {.name = "help", .help = "this help", .executor = cmd_cli_help},
    {.name = "test", .help = NULL,        .executor = cmd_test},

    // QSPI interface control commands
    {.name = "init",    .help = "Interface: initialize (clock, GPIO, default settings)",                   .executor = qspi_if_init},
    {.name = "stat",    .help = "Interface: show status",                                                  .executor = qspi_if_show},
    {.name = "ifset",   .help = "Interface: set (on/off)",                                                 .executor = qspi_if_set},
    {.name = "divider", .help = "Interface: set clock divider (0..0xFFFE)",                                .executor = qspi_set_ClkDivider},
    {.name = "dir",     .help = "Interface: set xfer type (txrx/tx/rx/eeprom)",                            .executor = qspi_set_TransferDirection},
    {.name = "framesz", .help = "Interface: set data frame size (4..32 bits)",                             .executor = qspi_set_DataFrameSize},
    {.name = "endian",  .help = "Interface: set endian conversion (on/off)",                               .executor = qspi_set_EndiaConversion},
    {.name = "pol",     .help = "SPI: set polarity (low/high)",                                            .executor = qspi_set_ClockPolarity},
    {.name = "phase",   .help = "SPI: set phase (start/middle)",                                           .executor = qspi_set_ClockPhase},
    {.name = "mod",     .help = "SPI: set frame format (std/dual/quad)",                                   .executor = qspi_set_FrameFormat},
    {.name = "sstog",   .help = "SPI: set slave select toggling (on/off)",                                 .executor = qspi_set_SlaveSelToggle},
    {.name = "ctrlmod", .help = "SPI: set xfer (Std/Dual/Quad) mode for instr. and addr. (std/inst/both)", .executor = qspi_set_TransferMode},
    {.name = "addrlen", .help = "SPI (enhanced mode): set address length (0,4,8..60 bits)",                .executor = qspi_set_AddrLen},
    {.name = "instlen", .help = "SPI (enhanced mode): set instruction length (0,4,8 or 16 bits)",          .executor = qspi_set_InstLen},
    {.name = "wcycles", .help = "SPI (enhanced mode): set wait cycles (0..31)",                            .executor = qspi_set_WaitCycles},
    {.name = "ddr",     .help = "SPI (enhanced mode): set DDR in dual/quad modes (on/off)",                .executor = qspi_set_DDRMode},
    {.name = "instddr", .help = "SPI (enhanced mode): set instruction DDR in Dual/Quad modes (on/off)",    .executor = qspi_set_DDRInstMode},

    // SPI flash memory  commands
    {.name = "id",      .help = "Flash: read ID",                             .executor = flash_get_id_cmd},
    {.name = "regs",    .help = "Flash: read serivce registers",              .executor = flash_get_status_cmd},
    {.name = "clrflag", .help = "Flash: clear flag register",                 .executor = flash_clear_flagsr_cmd},
    {.name = "reset",   .help = "Flash: reset memory",                        .executor = flash_reset_cmd},
    {.name = "erase",   .help = "Flash: memory erase",                        .executor = flash_erase_cmd},
    {.name = "quad",    .help = "Flash: set Quad mode (on/off)",              .executor = flash_set_quad_cmd},
    {.name = "read",    .help = "Flash: read memory region [addr [size]]",    .executor = flash_read_cmd},
    {.name = "write",   .help = "Flash: write memory region [addr [string]]", .executor = flash_write_cmd},
    {.name = "xip",     .help = "Flash: set XIP mode (on/off)",               .executor = flash_set_xip_cmd},
    {.name = "exec",    .help = "Flash: run the code [offset]",               .executor = flash_exec_cmd},
};

// UART IRQ handler ------------------------------------------------------------
void __attribute__ ((interrupt))
UART0_IRQHandler (void) {
    if (READ_REG(UART0->LSR) & UART_LSR_DR) {
        cli_rx_char = UART_ReceiveData8b(UART0);
    }
}

// Service functions -----------------------------------------------------------
static inline void
cli_clrscr (void) {
    printf("\033[2J\033[H\033[30m");
}

static inline void
cli_prompt (void) {
    printf("\033[0m>\033[36m ");
}

static inline void
cli_header (void) {
    cli_clrscr();
    printf("\033[93mQSPI example\t\033[90mbuild: %s %s\r\n", __DATE__, __TIME__);
    cli_prompt();
}

static int
cli_cmd_search_idx (char *str) {
    int result = -1;
    int str_len = strlen(str);

    for (size_t i = 0; i < sizeof (cli_cmd) / sizeof (*cli_cmd); ++i) {
        if (0 == strncmp(str, cli_cmd[i].name, str_len)) {
            result = i;
            break;
        }
    }

    return result;
}

static void
cli_str_parse (char *str) {
    bool cmd_found_f = false;

    printf("\r\n");

    for (size_t i = 0; i < sizeof (cli_cmd) / sizeof (*cli_cmd); ++i) {
        if (0 == strncmp(cli_cmd[i].name, str, strlen(cli_cmd[i].name))) {
            cmd_found_f = true;
            str = str + strlen(cli_cmd[i].name);

            while (isblank(*str)) {
                str++;
            }

            printf("\033[0m");
            cli_cmd[i].executor(str);
            break;
        }
    }

    if (false == cmd_found_f) {
        printf("\033[31mUnknown command \"%s\". \"help\" for more info\r\n", str);
    }

    cli_prompt();
}

void
cli_char_rx_callback (uint8_t rx_char) {
    /// Backspace
    if ('\b' == rx_char || 0x7F == rx_char) {
        if (0 != stat.len) {
            stat.len--;
            printf("\b \b");
        }

    /// Allowed characters
    } else if (stat.len < CLI_CMDLEN_MAX &&
              (isalnum(rx_char) || ' ' == rx_char || '_' == rx_char)) {
        stat.str[stat.len++] = rx_char;
        putchar(rx_char);

    /// Return
    } else if (stat.len > 0 && ('\n' == rx_char || '\r' == rx_char)) {
        stat.str[stat.len] = '\0';
        cli_str_parse(stat.str);
        stat.len = 0;

    /// Tab - autocomplete (first match)
    } else if ('\t' == rx_char) {
        stat.str[stat.len] = '\0';
        int cmd_found_idx = cli_cmd_search_idx(stat.str);

        if (cmd_found_idx >= 0) {
            printf("%s", cli_cmd[cmd_found_idx].name + stat.len);
            strcpy(&stat.str[stat.len], cli_cmd[cmd_found_idx].name + stat.len);
            stat.len = strlen(cli_cmd[cmd_found_idx].name);

        } else {
        }

    /// Ctrl+l - clear screen
    } else if (0x0C == rx_char) {
        stat.len = 0;
        cli_header();
    }

    cli_rx_char = 0;
}

void
cli_init (void) {
    setbuf(stdout, NULL);   ///< Disable output buffering
    cli_header();

    for (size_t i = 0; i < sizeof(cli_cmd)/sizeof(*cli_cmd); ++i) {
        if (strlen(cli_cmd[i].name) >= (size_t) stat.cmd_max_len) {
            stat.cmd_max_len = strlen(cli_cmd[i].name);
        }
    }
}

// Parameters parse functions --------------------------------------------------
char*
cli_get_param_ptr (char *str, int n) {
    char *retval = str;

    while (' ' == *retval) {
        retval++;
    }

    while (n--) {
        retval = strchr(retval, ' ');

        if (NULL != retval) {
            while (' ' == *retval) {
                retval++;
            }
        }
    }

    if (NULL != retval && 0 == strlen(retval)) {
        retval = NULL;
    }

    return retval;
}

int
cli_is_param_affirmative (char *str) {
    int retval = 0;
    const char* affirmwords[] = {"enable", "ENABLE",
                                 "on", "ON",
                                 "up", "UP",
                                 "yes", "YES",
                                 "y", "Y",
                                 "1"};

    for (size_t i = 0; i < (sizeof(affirmwords) / sizeof(*affirmwords)); ++i) {
        if (0 == strncmp(str, affirmwords[i], strlen(affirmwords[i]))) {
            retval = 1;
        }
    }

    return retval;
}

uint32_t
cli_param_strtohex (char *str) {    ///< strtol could be used instead
    uint32_t retval = 0;
    char ch;
    char chshift;

    if (0 == strncmp(str, "0x", strlen("0x"))) {
        str += strlen("0x");
    }

    for(;;) {
        ch = *str++;

        if (ch >= '0' && ch <= '9') {
            chshift = '0';

        } else if (ch >= 'a' && ch <= 'f') {
            chshift = 'a' - 10;

        } else if (ch >= 'A' && ch <= 'F') {
            chshift = 'A' - 10;

        } else {
            break;
        }

        retval = retval * 16 + ch - chshift;
    }

    return  retval;
}

// CLI command executors -------------------------------------------------------
static inline void
cmd_cli_help (char *args) {
    (void) args;
    printf("\033[37m");
    printf("\t\"Ctrl+l\" to clear screen\r\n");
    printf("\t\"Tab\" to command autocomplete\r\n");
    printf("\033[0m");
    for (size_t i = 0; i < sizeof(cli_cmd)/sizeof(*cli_cmd); ++i) {
        if (NULL != cli_cmd[i].help) {
            printf("%-*s - %s\r\n", stat.cmd_max_len, cli_cmd[i].name, cli_cmd[i].help);
        }
    }
}

static inline void
cmd_test (char *args) {
    printf("Test\r\n");

    if ('\0' == *args) {
        printf("No args\r\n");

    } else {
        printf("Args (len = %d): ", strlen(args));

        for (size_t i = 0; i < strlen(args); ++i) {
            printf("0x%.02X ", args[i]);
        }

        puts("\r\n");
    }
}

// Info representation functions -----------------------------------------------
void
cli_hexdump (uint8_t *buf, uint32_t len, uint32_t startval) {
    const uint8_t linelen = 16;

    printf("Addr\\ofst|");
    for (size_t i = 0; i < linelen; ++i) {
        printf("%2X ", i);
    }
    printf(" | ");
    for (size_t i = 0; i < linelen; ++i) {
        printf("%1X", i);
    }
    printf("\r\n---------+-------------------------------------------------+-----------------\r\n");

    for (size_t i = 0; i < len; ++i) {
        if (0 == (i % linelen)) {           ///< Print address
            printf("%06Xh: | ", (unsigned int) startval);
            startval += linelen;
        }

        printf("%02X ", buf[i]);            ///< Print data as hex

        if (0 == ((i + 1) % linelen)) {     ///< Print data as characters
            printf("| ");
            for (size_t j = 0; j < linelen; ++j) {
                printf("%c", (isgraph(buf[i - linelen + j + 1])) ? buf[i - linelen + j + 1] : '.');
            }

            printf("\r\n");
        }
    }

    printf("\r\n");
}
