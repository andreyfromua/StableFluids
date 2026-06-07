# Convenience wrapper around CMake. CMake remains the source of truth for
# compiler flags, dependency resolution, and source-file lists.

BUILD_DIR        ?= build
RELEASE_DIR      ?= build-release
GENERATOR        ?=
CMAKE_GENERATOR  := $(if $(GENERATOR),-G "$(GENERATOR)",)
JOBS             ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

.PHONY: all configure build run test release clean format shaders help

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_GENERATOR)

build: configure
	cmake --build $(BUILD_DIR) --parallel $(JOBS)

run: build
	$(BUILD_DIR)/stable_fluids

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

release:
	cmake -S . -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release $(CMAKE_GENERATOR)
	cmake --build $(RELEASE_DIR) --parallel $(JOBS)

clean:
	rm -rf $(BUILD_DIR) $(RELEASE_DIR)

format:
	@which clang-format >/dev/null 2>&1 || { echo "clang-format not found"; exit 1; }
	@find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i

shaders:
	@which glslangValidator >/dev/null 2>&1 || { \
	    echo "glslangValidator not found; skipping shader validation"; exit 0; }
	@for f in shaders/*.vert shaders/*.frag; do \
	    [ -f "$$f" ] || continue; \
	    echo "Validating $$f"; \
	    glslangValidator "$$f"; \
	done

help:
	@echo "Targets:"
	@echo "  configure  Configure a debug build in $(BUILD_DIR)"
	@echo "  build      Build the configured project"
	@echo "  run        Build and run the simulator"
	@echo "  test       Build and run CTest"
	@echo "  release    Configure and build an optimized release in $(RELEASE_DIR)"
	@echo "  clean      Remove generated build directories"
	@echo "  format     Run clang-format over the C++ sources"
	@echo "  shaders    Validate GLSL files with glslangValidator if available"
