XBE_TITLE = 3dpinball-xbox
ISO_DEPS = $(OUTPUT_DIR)/cd_root
GEN_XISO = $(XBE_TITLE).iso
NXDK_CXX = y
NXDK_SDL = y

SOURCE_DIRS  = src
CPP_FILES    = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.cpp))
C_FILES      = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))
SRCS         = $(CPP_FILES) $(C_FILES)

include $(NXDK_DIR)/Makefile

$(GEN_XISO): $(ISO_DEPS)
$(OUTPUT_DIR)/cd_root: $(CURDIR)/cd_root $(OUTPUT_DIR)
	$(VE)cp -r '$<' '$@'

clean: clean_iso_deps
.PHONY: clean_iso_deps
clean_iso_deps:
	rm -f $(ISO_DEPS)
