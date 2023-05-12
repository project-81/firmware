###############################################################################
MK_INFO := https://pypi.org/project/vmklib
ifeq (,$(shell which mk))
$(warning "No 'mk' in $(PATH), install 'vmklib' with 'pip' ($(MK_INFO))")
endif
ifndef MK_AUTO
$(error target this Makefile with 'mk', not '$(MAKE)' ($(MK_INFO)))
endif
###############################################################################

.PHONY: clean gdbgui

.DEFAULT_GOAL := gdbgui

THIRD_PARTY := $($(PROJ)_DIR)/third-party

gdbgui:
	$(THIRD_PARTY)/gdbgui/build/executable/gdbgui.pex

clean:
	rm -rf $(THIRD_PARTY)
