OS := $(shell uname -s)
CLANG_VERSION ?= $(shell clang --version | grep version | sed 's/[^0-9]*\([0-9]\{1,\}\.[0-9]\{1,\}\.[0-9]\{1,\}\).*/\1/')
CLANG_BASE_DIR ?= $(shell clang --version | grep InstalledDir | sed 's/InstalledDir: \(.*\)\/bin/\1/')
CLANG_SDK_PATH ?= $(CLANG_BASE_DIR)/lib/clang/$(CLANG_VERSION)/lib/darwin
ROOT_DIR ?= ../..
SRC_DIR ?= .
PROJ_NAME ?= libcryptoplus
TEST_NAME ?= cryptoplustest
PROJ_OUT ?= $(ROOT_DIR)/build/cpp
TEST_OUT ?= $(SRC_DIR)
COVERAGE_OUT ?= $(TEST_OUT)/coverage

GTEST_DIR:= $(ROOT_DIR)/deps/googletest/googletest
CPP_DIR := $(ROOT_DIR)/src/cpp

CPP_SRC_FILES := $(wildcard \
	${CPP_DIR}/*.cpp \
	${CPP_DIR}/**/*.cpp \
	${CPP_DIR}/**/**/*.cpp \
)
CPP_OBJS := $(patsubst ${CPP_DIR}/%,%,${CPP_SRC_FILES})
CPP_INCLUDES := \
	$(CPP_DIR)
CPP_TARGET := $(wildcard \
	${CPP_DIR}/*.o \
	${CPP_DIR}/**/*.o \
	${CPP_DIR}/**/**/*.o \
)

TEST_FILES := $(wildcard ${SRC_DIR}/*.cpp)
# TEST_LIST := $(patsubst ${SRC_DIR}/%.cpp,%,${TEST_FILES})
TEST_LIST := App_test
# TEST_LIST := Paillier_test
TEST_OBJS := $(patsubst %.cpp,%.o,${TEST_FILES})


FLAGS := -std=c++11 -isystem ${GTEST_DIR}/include -pthread
COVERAGE_FLAG:=
ifeq ($(OS),Darwin)
	COVERAGE_FLAG = -L$(CLANG_SDK_PATH) \
	-lclang_rt.profile_osx
else
	COVERAGE_FLAG = -lgcov
endif

.PHONY: all
all: compile_program clean compile_test

.PHONY: clean
clean:
	@echo "\033[33mClean unit test project...\033[0m"
	rm *.o ||:
	rm -rf $(TEST_OUT)/$(TEST_NAME) ||:
	rm -rf $(COVERAGE_OUT) ||:

.PHONY: compile_gtest
compile_gtest:
	@echo "\033[33mBuild google test...\033[0m"
	@cd $(GTEST_DIR) && mkdir -p build
	g++ $(FLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc \
		-o $(GTEST_DIR)/build/gtest-all.o
	g++ $(FLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest_main.cc \
		-o $(GTEST_DIR)/build/gtest_main.o
	ar rv $(GTEST_DIR)/build/gtest_main.a \
		$(GTEST_DIR)/build/gtest-all.o $(GTEST_DIR)/build/gtest_main.o

.PHONY: compile_program
compile_program:
	@echo "\033[33mBuild core project...\033[0m"
	@cd $(CPP_DIR) && make clean library

$(TEST_OUT):
	mkdir -p $(TEST_OUT)

$(COVERAGE_OUT):
	mkdir -p $(COVERAGE_OUT)

%.o: %.cpp
		g++ $(FLAGS) \
			$(foreach d, $(CPP_INCLUDES), -I$d) \
		-c $< -o $@;

.PHONY: compile_test
compile_test: $(TEST_OUT) $(TEST_OBJS)
	g++ $(FLAGS) -lpthread $(CPP_TARGET) \
		$(foreach f, $(TEST_LIST), $(TEST_OUT)/$f.o) \
		$(GTEST_DIR)/build/gtest_main.a -o $(TEST_OUT)/$(TEST_NAME) \
			$(COVERAGE_FLAG) \
			-lntl -lgmp -lm;
	@echo "\033[33mTest build done...\033[0m"

.PHONY: run_test
run_test:
	@echo "\033[32mTest Running:\033[0m"
	$(TEST_OUT)/$(TEST_NAME) --gtest_output="xml:$(TEST_OUT)/$(TEST_NAME).xml";

.PHONY: test_coverage
test_coverage: $(COVERAGE_OUT)
	@echo "\033[33mRun coverage report...\033[0m"
	cd $(CPP_DIR) && gcov $(foreach f, $(CPP_OBJS), $f) > /dev/null

	@lcov -q --no-external -c -b $(CPP_DIR)/app -d $(CPP_DIR)/app --ignore-errors graph -o $(COVERAGE_OUT)/coverage.info
	@lcov -r $(COVERAGE_OUT)/coverage.info '$(CPP_DIR)/app/*/*.hpp' '$(CPP_DIR)/app/*.hpp' '*.hpp' -o $(COVERAGE_OUT)/coverage.info
	@genhtml $(COVERAGE_OUT)/coverage.info -o $(COVERAGE_OUT)
