ROOT_DIR ?= ../..
SRC_DIR ?= .
PROJ_NAME ?= libcryptoplus
OUT_DIR ?= $(ROOT_DIR)/build

CPP_OUT ?= $(OUT_DIR)/cpp/cryptoplus
FLAGS := -std=c++11 -g -O3 -pthread -march=native

CPP_SRC_FILES := $(wildcard \
	${SRC_DIR}/*.cpp \
	${SRC_DIR}/**/*.cpp \
	${SRC_DIR}/**/**/*.cpp \
)
CPP_OBJS := $(patsubst %.cpp,%,${CPP_SRC_FILES})
CPP_OBJS_OBJS := $(patsubst %.cpp,%.o,${CPP_SRC_FILES})

CPP_INCLUDES := \
	$(SRC_DIR)

CPP_TARGET := $(CPP_OUT)/$(PROJ_NAME)

.PHONY: all
all: clean library

.PHONY: clean
clean:
	@echo "\033[33mClean cpp project...\033[0m"
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.o $(SRC_DIR)/**/**/*.o
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.gcno $(SRC_DIR)/**/**/*.gcno
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.gcda $(SRC_DIR)/**/**/*.gcda

%.o: %.cpp
	g++ $(FLAGS) \
		-fprofile-arcs \
		-ftest-coverage \
		$(foreach d, $(CPP_INCLUDES), -I$d) \
		-c $< -o $@; \

.PHONY: library
library: ${CPP_OBJS_OBJS}
	@mkdir -p $(CPP_OUT)
	@echo "\033[32mOutput:\033[0m\n$(CPP_TARGET)"
