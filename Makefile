###############################################################################
MK_INFO := https://pypi.org/project/vmklib
ifeq (,$(shell which mk))
$(warning "No 'mk' in $(PATH), install 'vmklib' with 'pip' ($(MK_INFO))")
endif
ifndef MK_AUTO
$(error target this Makefile with 'mk', not '$(MAKE)' ($(MK_INFO)))
endif
###############################################################################

.PHONY: clean-toolchains

.DEFAULT_GOAL := all

all:
	+@echo "TODO"

TOOLCHAINS  := $($(PROJ)_DIR)/toolchains

clean-toolchain-%:
	rm -rf $(TOOLCHAINS)/$*/build*
	rm -rf $(TOOLCHAINS)/$*/out
	rm -f $(BUILD_DIR)/crosstool-$*.txt

clean-toolchains: clean-toolchain-arm-picolibc-eabi
