###############################################################################
MK_INFO := https://pypi.org/project/vmklib
ifeq (,$(shell which mk))
$(warning "No 'mk' in $(PATH), install 'vmklib' with 'pip' ($(MK_INFO))")
endif
ifndef MK_AUTO
$(error target this Makefile with 'mk', not '$(MAKE)' ($(MK_INFO)))
endif
###############################################################################

.PHONY: clean-toolchains host-jlink-docs

.DEFAULT_GOAL := all

all:
	+@echo "TODO"

THIRD_PARTY := $($(PROJ)_DIR)/third-party

host-jlink-docs:
	cd $(THIRD_PARTY)/jlink/Doc/Manuals && python -m http.server 0

TOOLCHAINS  := $($(PROJ)_DIR)/toolchains

clean-toolchain-%:
	rm -rf $(TOOLCHAINS)/$*/build*
	rm -rf $(TOOLCHAINS)/$*/out
	rm -f $(BUILD_DIR)/crosstool-$*.txt

clean-toolchains: clean-toolchain-arm-picolibc-eabi
