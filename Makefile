BOARD          ?= nrf54l15dk/nrf54l15/cpuapp
BUILD_DIR      ?= build
# Sysbuild names the application image's build subdirectory after the source directory.
APP_IMAGE      := $(notdir $(CURDIR))
COMPILE_DB     := $(BUILD_DIR)/$(APP_IMAGE)/compile_commands.json
CLANG_TIDY_DIR := $(BUILD_DIR)/$(APP_IMAGE)/clang-tidy-db
CLANG_TIDY_DB  := $(CLANG_TIDY_DIR)/compile_commands.json
SOURCES        := $(shell find src -name '*.cpp')

.PHONY: build flash tests static-analysis

build:
	west build -b $(BOARD) -d $(BUILD_DIR) --sysbuild .

flash:
	west flash -d $(BUILD_DIR)

tests:
	@echo "No unit test suite is configured yet!"

# Requires $(COMPILE_DB), which only exists after a successful `make build`. clang-tidy-20 can't
# use $(COMPILE_DB) directly - it uses clang's own driver, which rejects a few GCC-only flags in
# there outright and can't find the Zephyr SDK GCC's own libstdc++ headers from --sysroot alone
# (see scripts/prepare_clang_tidy_db.py for why). So this generates a filtered copy under
# $(CLANG_TIDY_DIR), runs against that, then deletes it - the copy is ~4MB, regenerating and
# discarding it every run is cheap and keeps $(COMPILE_DB) itself untouched for `make build`/clangd.
static-analysis: $(COMPILE_DB)
	python3 scripts/prepare_clang_tidy_db.py $(COMPILE_DB) $(CLANG_TIDY_DB)
	clang-tidy-20 -p $(CLANG_TIDY_DIR) --extra-arg=-Wno-vla-cxx-extension $(SOURCES); \
	status=$$?; rm -rf $(CLANG_TIDY_DIR); exit $$status

$(COMPILE_DB):
	$(error $(COMPILE_DB) not found - run 'make build' first)
