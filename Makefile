###############################################################################
MK_INFO := https://pypi.org/project/vmklib
ifeq (,$(shell which mk))
$(warning "No 'mk' in $(PATH), install 'vmklib' with 'pip' ($(MK_INFO))")
endif
ifndef MK_AUTO
$(error target this Makefile with 'mk', not '$(MAKE)' ($(MK_INFO)))
endif
###############################################################################

.PHONY: all t clean clean-toolchains clean-compiles host-jlink-docs

THIRD_PARTY := $($(PROJ)_DIR)/third-party

CTAGS_ARGS := -f tags --languages=C,C++

# cd $($(PROJ)_DIR) && ctags $(CTAGS_ARGS) -a \
# 	-R third-party/json/single_include
t:
	rm -f $($(PROJ)_DIR)/tags
	cd $($(PROJ)_DIR) && ctags $(CTAGS_ARGS) \
		--exclude=src/third-party -R src
	cd $($(PROJ)_DIR) && ctags $(CTAGS_ARGS) -a \
		-R third-party/pico-sdk/src
	cd $($(PROJ)_DIR) && ctags $(CTAGS_ARGS) -a \
		-R third-party/picolibc/picolibc/newlib/libc/tinystdio

edit: t
	cd $($(PROJ)_DIR) && $(EDITOR)

host-jlink-docs:
	cd $(THIRD_PARTY)/jlink/Doc/Manuals && python -m http.server 0

TOOLCHAINS := $($(PROJ)_DIR)/toolchains

clean-compile-%:
	rm -rf $(BUILD_DIR)/$*

clean-compiles: clean-compile-arm-picolibc-eabi \
                clean-compile-arm-none-eabi \
                clean-compile-native \
                clean-compile-coverage

clean: clean-compiles
	ninja -t clean
	ninja -t cleandead
	rm -f $($(PROJ)_DIR)/compile_commands.json
	rm -rf $($(PROJ)_DIR)/src/generated/*

clean-toolchain-%:
	rm -rf $(TOOLCHAINS)/$*/build* $(TOOLCHAINS)/$*/include
	sudo rm -rf $(TOOLCHAINS)/$*/out
	rm -f $(BUILD_DIR)/crosstool-$*.txt

clean-toolchains: clean-toolchain-arm-picolibc-eabi \
                  clean-toolchain-arm-none-eabi
