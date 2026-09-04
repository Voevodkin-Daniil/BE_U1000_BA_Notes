# Makefile help and self debug variables and procedures
# This file is not required for the firmware building

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
MAKEFILE_DEBUG ?= ## Used to debug "make"; show internal variables and increase verbosity
# Show only listed variables
ifeq ($(MAKEFILE_DEBUG), 1)
    VARS_DEBUG_LIST := MAKECMDGOALS MAKEFLAGS MAKEFILE_LIST SHELL SHELLFLAGS PRJ_NAME TARGET\
	PRJ_DIR BUILD_TYPE SDK_DIR TC_PREFIX LD_SCRIPT V CORE_MODEL ARCH ABI CMODEL\
	OPT DEBUG CDEFS INC_DIR OUT_DIR CFLAGS ASFLAGS MEM_REG_ROMMEM_REG_RAM LDFLAGS\
	LIBFLAGS LDLIBS VPATH SPECS CCACHE SRC_AUTO_SUBDIR CSRC_DIR ASRC_DIR CSRC\
	ASRC OBJS

    $(foreach VAR, $(VARS_DEBUG_LIST),\
        $(info $(shell printf "%-15s" "$(VAR)")= $(value $(VAR))))
    $(error MAKEFILE_DEBUG end)# Uncomment if only variables show is required
    MAKEFLAGS += --print-directory
    MAKEFLAGS += --no-silent
	MAKEFLAGS += --just-print

# Show all user variables
# Note: CURDIR SHELL MAKEFILE_LIST .DEFAULT_GOAL and MAKEFLAGS are not user variables
else ifeq ($(MAKEFILE_DEBUG), 2)
    $(foreach VAR, $(.VARIABLES), $(if $(filter file, $(origin $(VAR))),\
        $(info $(shell printf "%-15s" "$(VAR)")= $(value $(VAR)))))

# Show all variables
else ifeq ($(MAKEFILE_DEBUG), 3)
    $(foreach VAR, $(.VARIABLES),\
        $(info $(shell printf "%-15s" "$(VAR)")= $(value $(VAR))))

# Show make's internal debug info and database. Don't actually run any recipe; just print them.
else ifeq ($(MAKEFILE_DEBUG), 4)
    MAKEFLAGS += --debug=all --print-data-base --dry-run
endif

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
.PHONY: help
help:## Show this help message
    # Dedicated comments
	$(Q)awk 'BEGIN\
		{FS = "##"}\
		/^## /\
		{printf "%s\n", substr($$0, 4)}\
		'\
		$(MAKEFILE_LIST)
	$(Q)echo ""
    # Targets
	$(Q)awk 'BEGIN\
	    {FS = ":.*## "; printf "Usage: <VAR_NAME=VALUE> make <TARGET_1> .. <TARTEG_n>\nTargets:\n"}\
		/^[ ]*[0-9a-zA-Z. $$\(\)_-]+:[0-9a-zA-Z. $$\(\)_-]*## .*/ {\
		gsub(/^[ ]*/,"",$$0);\
		printf "    %-18s %s\n", $$1, $$2;\
		}\
		'\
		$(MAKEFILE_LIST)
	$(Q)echo ""
    # Variables
	$(Q)awk 'BEGIN\
		{FS = "## "; printf "Variables: \n"}\
		/^[ ]*[0-9a-zA-Z_-]+[ ]*[:?+]?=[0-9a-zA-Z.,* +$$\(\)/=_-]*## .*/ {\
			gsub(/^[ ]*/,"",$$0);\
			gsub(/[ ]*[:?+]?=[0-9a-zA-Z.,* +$$\(\)/=_-]*/, "", $$1);\
			printf "    %-18s %s\n", $$1, $$2;\
		}\
		'\
		$(MAKEFILE_LIST)

#-... -- -.-. ..- .-.-.- ... -.. -.- .-.-.- ..--- ----- ..--- ..... -...- -.--.-
