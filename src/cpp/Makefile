ROOT_DIR ?= ../..
SRC_DIR ?= .
PROJ_NAME ?= libcryptoplus
OUT_DIR ?= $(ROOT_DIR)/build
CRYPTOPP_DIR ?= $(ROOT_DIR)/build/cpp/vendor/cryptopp
DJINNI_SUP_DIR ?= $(ROOT_DIR)/deps/djinni/support-lib
DJINNI_GEN_DIR ?= $(ROOT_DIR)/generated-src
CPP_OUT ?= $(OUT_DIR)/cpp/cryptoplus
JNI_OUT ?= $(OUT_DIR)/jni/cryptoplus
FLAGS := -std=c++11 -g -O2 -pthread -march=native
JAVA_HOME ?= $(shell jrunscript -e 'java.lang.System.out.println(new java.io.File(java.lang.System.getProperty("java.home")).getParent());')

ifeq ($(findstring :,$(JAVA_HOME)),)
	JAVA_HOME_CLEAN := $(JAVA_HOME)
else
	JAVA_HOME_CLEAN := $(shell echo "/${JAVA_HOME}" | sed 's/\\\\/\\//g' | sed 's/://' | sed 's/ /\\\\ /')
endif

CPP_SRC_FILES := $(wildcard \
	${SRC_DIR}/*.cpp \
	${SRC_DIR}/**/*.cpp \
	${SRC_DIR}/**/**/*.cpp \
)
CPP_OBJS := $(patsubst %.cpp,%,${CPP_SRC_FILES})

JNI_SRC_FILES := $(wildcard \
	${SRC_DIR}/*.cpp \
	${SRC_DIR}/**/*.cpp \
	${SRC_DIR}/**/**/*.cpp \
	$(DJINNI_SUP_DIR)/*.cpp \
	$(DJINNI_SUP_DIR)/jni/*.cpp \
	$(DJINNI_GEN_DIR)/cpp/*.cpp \
	$(DJINNI_GEN_DIR)/jni/*.cpp \
)
JNI_OBJS := $(patsubst %.cpp,%,${JNI_SRC_FILES})

CPP_INCLUDES := \
	$(CRYPTOPP_DIR) \
	$(DJINNI_GEN_DIR)/cpp \
	$(SRC_DIR)

JNI_INCLUDES := \
	$(CRYPTOPP_DIR) \
	$(DJINNI_SUP_DIR)/ \
	$(DJINNI_SUP_DIR)/jni \
	$(DJINNI_GEN_DIR)/cpp \
	$(DJINNI_GEN_DIR)/jni \
	$(SRC_DIR)

CPP_TARGET := $(CPP_OUT)/$(PROJ_NAME)

all: clean library

clean:
	@echo "\033[33mClean cpp project...\033[0m"
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.o $(SRC_DIR)/**/**/*.o
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.gcno $(SRC_DIR)/**/**/*.gcno
	rm -rf $(CPP_OUT) $(SRC_DIR)/**/*.gcda $(SRC_DIR)/**/**/*.gcda
	rm -rf $(DJINNI_GEN_DIR)/**/*.o

library:
	@echo "\033[33mBuild cpp library...\033[0m"
	@mkdir -p $(CPP_OUT)
	$(foreach f, $(CPP_OBJS), \
		g++ $(FLAGS) \
			-fprofile-arcs \
			-ftest-coverage \
			$(foreach d, $(CPP_INCLUDES), -I$d) \
			-c $f.cpp -o $f.o; \
	)
	@echo "\033[32mOutput:\033[0m\n$(CPP_TARGET)"

# jni_library: clean library
jni_library:
	@echo "\033[33mBuild JNI library...\033[0m"
	@rm -rf $(JNI_OUT)
	@mkdir -p $(JNI_OUT)
	$(foreach f, ${JNI_OBJS}, \
		g++ $(FLAGS) -fPIC \
			-I$(JAVA_HOME_CLEAN)/include \
			-I$(JAVA_HOME_CLEAN)/include/linux \
			-I$(JAVA_HOME_CLEAN)/include/win32 \
			-I$(JAVA_HOME_CLEAN)/include/darwin \
			$(foreach d, $(JNI_INCLUDES), -I$d) \
			-c $f.cpp -o $f.o; \
	)
	g++ -o $(JNI_OUT)/libcryptoplus.so \
		$(foreach d, $(JNI_OBJS), $d.o) \
		-L$(CRYPTOPP_DIR) \
		-lcryptopp \
		-shared
	@rm $(DJINNI_SUP_DIR)/jni/*.o
