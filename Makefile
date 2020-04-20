VERSION ?=1.0.0
OUT_DIR ?= build
RELEASE_NAME ?= cryptoplus-$(VERSION)
RELEASE_OUT ?= $(OUT_DIR)/release/$(RELEASE_NAME)
DEPS_DIR ?= deps

all: deps_build lib_build

.PHONY: deps_build
deps_build: gtest_build

.PHONY: lib_build
lib_build: cpp_clean cpp_build cpp_test_clean cpp_test cpp_coverage pack_cpp

# Build dependency Google Test
.PHONY: gtest_build
gtest_build:
	@cd test/cpp && make compile_gtest

# Clean project
.PHONY: cpp_clean
cpp_clean:
	@cd src/cpp && make clean

# Build project
.PHONY: cpp_build
cpp_build:
	@cd src/cpp && make -j 16 library

# Clean test case
.PHONY: cpp_test_clean
cpp_test_clean:
	@cd test/cpp && make clean

# Run test case
.PHONY: cpp_test
cpp_test:
	@cd test/cpp && make -j 16 compile_test
	@cd test/cpp && make run_test

.PHONY: cpp_coverage
cpp_coverage:
	@cd test/cpp && make test_coverage

# Package necessary C++ compiled files
# eg. header and object files
.PHONY: pack_cpp
pack_cpp:
	@echo "\033[33mPack Crypto Library...\033[0m"
	@rm -rf build/cpp
	@mkdir -p build/cpp
	@rm -rf src/cpp/**/*.gcno src/cpp/**/*.gcno
	@rm -rf src/cpp/**/**/*.gcno src/cpp/**/**/*.gcno
	@rm -rf src/cpp/**/*.gcda src/cpp/**/*.gcda
	@rm -rf src/cpp/**/**/*.gcda src/cpp/**/**/*.gcda
	@rsync -avzq --exclude="*.cpp" --exclude="Makefile" src/cpp/./ build/cpp

cpp_doc:
	@echo "\033[33mGenerate readme document...\033[0m"
	@md-to-pdf README.md
	@mv README.pdf ./docs
