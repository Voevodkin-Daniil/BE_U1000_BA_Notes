# Service makefile
# Intended to simplify the main makefile (common.mk)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Valid value lists
MCU_MODEL_VALID   += BE_U1000# List of available MCU models
MCU_MODEL_VALID   += BMCU_U
MCU_MODEL_DEFAULT := BE_U1000

CORE_NUM_VALID   += CORE_0# List of available MCU core numbers
CORE_NUM_VALID   += CORE_1
CORE_NUM_VALID   += CORE_0_1# Universal for Core 0 and Core 1
CORE_NUM_VALID   += CORE_2
CORE_NUM_DEFAULT := CORE_0

BOARD_VALID += EVU_BA_1_2# List of supported development boards (BSP)
BOARD_VALID += EVU_BA_2_0
BOARD_VALID += EVU_BA_2_1
BOARD_VALID += EVU_BA_2_3
BOARD_VALID += EVU_BA_2_5
BOARD_VALID += EVU_LI_2_0
BOARD_VALID += EVU_LI_2_1

MATH_VER_VALID   += IN_ROM# List of math librarie modes
MATH_VER_VALID   += STANDARD
MATH_VER_DEFAULT := IN_ROM

FLOAT_POINT_MODE_VALID += SOFT# List of supported floating point modes (ISA "F")
FLOAT_POINT_MODE_VALID += HARD
FLOAT_POINT_MODE_DEFAULT := HARD

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Default values of variables
# If QSPI memory is selected as the ROM region, typically some kind of XIP loader
# should be located at the beginning of QSPI flash memory region (0x00 address)
MEM_REG_ROM_QSPI_DEFAULT_OFFSET := 0x10000

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Special predefined values to be written to the firmware's header.
# This header data structur is used by the MCU bootROM to identify the firmware
# and data copy (for example, copy data QSPI -> TCMA in case of QSPI boot mode)
TCMA_MAGIC_NUMBER   := 0x0B10D522
TCMB_MAGIC_NUMBER   := 0x0B10DDA2
EFLASH_MAGIC_NUMBER := 0x0BEF5262
QSPI1_MAGIC_NUMBER  := 0x00000000

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# .NOTPARALLEL: <target_name> valid only for make --version >= 4.4
NOTPARALLEL_MAKE_REQ_VER = 4.4
MAKE_VER_MAJ = $(shell echo $(MAKE_VERSION) | cut -f1 -d.)
MAKE_VER_MIN = $(shell echo $(MAKE_VERSION) | cut -f2 -d.)
NOTPARALLEL_MAKE_REQ_VER_MAJ = $(shell echo $(NOTPARALLEL_MAKE_REQ_VER) | cut -f1 -d.)
NOTPARALLEL_MAKE_REQ_VER_MIN = $(shell echo $(NOTPARALLEL_MAKE_REQ_VER) | cut -f2 -d.)
NOTPARALLEL_MAKE_REQ_OK := $(shell [ $(MAKE_VER_MAJ) -gt $(NOTPARALLEL_MAKE_REQ_VER_MAJ) -o \
                                  \( $(MAKE_VER_MAJ) -eq $(NOTPARALLEL_MAKE_REQ_VER_MAJ) -a \
                                     $(MAKE_VER_MIN) -ge $(NOTPARALLEL_MAKE_REQ_VER_MIN) \) ] && echo true)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Use ccache if available
CCACHE := $(shell command -v ccache 2> /dev/null)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
CDEFS += -D'BUILD_TIMESTAMP="$(shell date +"%Y-%m-%d, %H:%M:%S")"'
CDEFS += -D'GIT_BRANCH="$(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo -n "GIT_NOBRANCH")"'
CDEFS += -D'GIT_COMMIT="$(shell git rev-parse --short HEAD 2>/dev/null || echo -n "GIT_NOCOMMIT")"'
CDEFS += -D'GIT_COMMITDATE="$(shell git log -n 1 --format=%cd --date=format:"%Y.%m.%d,%H:%M:%S" 2>/dev/null || echo -n "GIT_NODATE")"'

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
# Alternative: recursive wildcard (use it instead of $(shell find . -name '*.c'))
# Search for all .c files in all subdirs
# taken from https://stackoverflow.com/a/18258352
# Use example:  CSRC = $(call rwildcard,.,*.c)
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Derive the directory containing this Makefile
# MAKEFILE_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

# $(call assert,condition,message)
define assert
	$(if $1,,$(error Assertion failed: $2))
endef

# $(call assert-file-exists,wildcard-pattern)
define assert-file-exists
	$(call assert,$(wildcard $1),$1 does not exist)
endef

# $(call assert-not-null,make-variable)
define assert-not-null
	$(call assert,$($1),The variable "$1" is null)
endef

# Check if all of listed variables are defined
# MISSING = $(shell for x in '$(VAR1)' '$(VAR2)' '$V(VARN)'; do [ \"$$x\" == \"\" ] && echo 1; done)
# ifneq ($(MISSING),)
# 	$(info Not all required variables are defined)
# endif

# Print list of variables line by line
# Example:
# SOME_VARIABLE := on two three
# SOME_VARIABLE_SPLIT := $(subst $(SPACE),$(n),$(SOME_VARIABLE))
# $(NULL) is empty string
NULL :=
# $(SPACE) is one space
SPACE := $(NULL) $(NULL)
# New line. Could be used in make info/warning/error messages as "test $ntest"
define n


endef

# .---. .----. .-- -.-.-. ..- --. .-- -- .- -.-.-. ---. -.-- -. .-- -- .. -- -.-
