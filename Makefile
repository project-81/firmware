###############################################################################
MK_INFO := https://pypi.org/project/vmklib
ifeq (,$(shell which mk))
$(warning "No 'mk' in $(PATH), install 'vmklib' with 'pip' ($(MK_INFO))")
endif
ifndef MK_AUTO
$(error target this Makefile with 'mk', not '$(MAKE)' ($(MK_INFO)))
endif
###############################################################################

.PHONY: compdb clean clean-toolchains host-jlink-docs

compdb:
	ninja -t compdb > compile_commands.json

THIRD_PARTY := $($(PROJ)_DIR)/third-party

host-jlink-docs:
	cd $(THIRD_PARTY)/jlink/Doc/Manuals && python -m http.server 0

TOOLCHAINS := $($(PROJ)_DIR)/toolchains

clean-compile-%:
	rm -rf $(BUILD_DIR)/$*

clean: clean-compile-arm-picolibc-eabi clean-compile-arm-none-eabi
	ninja -t clean
	ninja -t cleandead
	rm -f $($(PROJ)_DIR)/compile_commands.json

clean-toolchain-%:
	rm -rf $(TOOLCHAINS)/$*/build* $(TOOLCHAINS)/$*/include
	sudo rm -rf $(TOOLCHAINS)/$*/out
	rm -f $(BUILD_DIR)/crosstool-$*.txt

clean-toolchains: clean-toolchain-arm-picolibc-eabi \
                  clean-toolchain-arm-none-eabi
