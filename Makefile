VERSION ?=1.0.0
OUT_DIR ?= build
IOS_OUT ?= $(OUT_DIR)/ios
RELEASE_NAME ?= cryptoplus-$(VERSION)
RELEASE_OUT ?= $(OUT_DIR)/release/$(RELEASE_NAME)
DEPS_DIR ?= deps
DEPS_CRYPTOPP_DIR ?= $(DEPS_DIR)/cryptopp
DEPS_GYP_DIR ?= $(DEPS_DIR)/gyp
DEPS_DJINNI_DIR := $(DEPS_DIR)/djinni
DJINNI_OUT_DIR ?= generated-src

all: os_build

full: full_deps full_build
full_deps: deps_build cryptopp_wasm cryptopp_android cryptopp_ios
full_build: lib_build wasm android jni ios
release: full_build pack_release

.PHONY: deps_build
deps_build: cryptopp_os gtest_build

.PHONY: lib_build
lib_build: test_vars djinni_build cpp_clean cpp_build cpp_test_clean cpp_test cpp_coverage pack_cpp

.PHONY: os_build
os_build: deps_build lib_build

.PHONY: ios_build
ios_build: cryptopp_ios djinni_build pack_ios

.PHONY: ios
ios: pack_ios

.PHONY: android_build
android_build: cryptopp_android djinni_build pack_android

.PHONY: android
android: pack_android

.PHONY: wasm_build
wasm_build: cryptopp_wasm djinni_build wasm_js_build wasm_js_test

.PHONY: wasm
wasm: wasm_js_build wasm_js_test

.PHONY: jni_build
jni_build: deps_build djinni_build jni_clean jni_jar_build

.PHONY: jni
jni: jni_clean jni_jar_build

# Build dependency Crypto++
.PHONY: cryptopp_os
cryptopp_os: cryptopp_clean_os cryptopp_build_os

.PHONY: cryptopp_clean_os
cryptopp_clean_os:
	@echo "\033[33mClean cryptopp project for OS...\033[0m"
	@cd $(DEPS_CRYPTOPP_DIR) && make clean

.PHONY: cryptopp_build_os
cryptopp_build_os:
	@echo "\033[33mBuild cryptopp project for OS...\033[0m"
	@cd $(DEPS_CRYPTOPP_DIR) && make static
	@echo "\033[33mPack cryptopp project...\033[0m"
	@rm -rf build/cpp/vendor/cryptopp
	@mkdir -p build/cpp/vendor/cryptopp
	@rsync deps/cryptopp/*.o build/cpp/vendor/cryptopp
	@rsync deps/cryptopp/*.h build/cpp/vendor/cryptopp
	@rsync deps/cryptopp/*.a build/cpp/vendor/cryptopp
	@rm build/cpp/vendor/cryptopp/test_cxx.o

.PHONY: cryptopp_ios
cryptopp_ios: cryptopp_clean_ios djinni_build cryptopp_build_ios

.PHONY: cryptopp_clean_ios
cryptopp_clean_ios:
	@echo "\033[33mClean cryptopp project for iOS...\033[0m"
	@cd $(DEPS_CRYPTOPP_DIR) && make clean

.PHONY: cryptopp_build_ios
cryptopp_build_ios: cryptopp_clean_ios
	@echo "\033[33mBuild cryptopp project for Universal iOS...\033[0m"
	@bash cryptopp-ios.sh

.PHONY: cryptopp_android
cryptopp_android: cryptopp_clean_android cryptopp_build_android

.PHONY: cryptopp_clean_android
cryptopp_clean_android:
	@echo "\033[33mClean cryptopp project for Android...\033[0m"
	@rm -rf android_project/cryptopp/obj
	@rm -rf android_project/cryptopp/libs
	@rm -rf build/android/vendor/cryptopp

.PHONY: cryptopp_build_android
cryptopp_build_android:
	@echo "\033[33mBuild cryptopp project for Android...\033[0m"
	@cd android_project/cryptopp && ./build.sh
	@mkdir -p build/android/vendor/cryptopp
	@rsync -rR android_project/cryptopp/libs/./* build/android/vendor/cryptopp
	@mkdir -p android_project/AndroidCryptoLibrary/cryptoplus/src/main/jniLibs
	@rsync -rR build/android/vendor/cryptopp/./**/*.so android_project/AndroidCryptoLibrary/cryptoplus/src/main/jniLibs

.PHONY: cryptopp_wasm
cryptopp_wasm: cryptopp_clean_wasm cryptopp_build_wasm

.PHONY: cryptopp_clean_wasm
cryptopp_clean_wasm:
	@echo "\033[33mClean cryptopp project for WebAssembly...\033[0m"
	@cd ./deps/cryptopp && make clean
	@rm -rf build/wasm/cpp/cryptopp

.PHONY: cryptopp_build_wasm
cryptopp_build_wasm:
	@echo "\033[33mBuild cryptopp project for WebAssembly...\033[0m"
	@cd ./deps/cryptopp && emmake make
	@echo "\033[33mPack cryptopp project for WebAssembly...\033[0m"
	@mkdir -p build/wasm/cpp/cryptopp
	@rsync deps/cryptopp/*.o build/wasm/cpp/cryptopp
	@rsync deps/cryptopp/*.h build/wasm/cpp/cryptopp
	@rsync deps/cryptopp/*.a build/wasm/cpp/cryptopp
	@rm build/wasm/cpp/cryptopp/test_cxx.o

# Build dependency Google Test
.PHONY: gtest_build
gtest_build:
	@cd test/cpp && make compile_gtest

# Build Djinni Interface
.PHONY: djinni_build
djinni_build:
	@echo "\033[33mBuild djinni interface...\033[0m"
	./run_djinni.sh

# Clean project
.PHONY: cpp_clean
cpp_clean:
	@cd src/cpp && make clean

# Build project
.PHONY: cpp_build
cpp_build:
	@cd src/cpp && make -j 16 library

# Print out environment variables
.PHONY: test_vars
test_vars:
	@echo "\033[33mPrint variables...\033[0m"
	@echo GYP_DIR=$(DEPS_GYP_DIR)
	@echo NDK_DIR=$(NDK_DIR)
	@echo OUT_DIR=$(OUT_DIR)
	@echo IOS_OUT=$(IOS_OUT)
	@echo DEPS_DIR=$(DEPS_DIR)
	@echo DEPS_DJINNI_DIR=$(DEPS_DJINNI_DIR)

# Run test case
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

# Build Docker Image for CI
.PHONY: docker
docker:
	@echo "\033[33mBuild Docker Image...\033[0m"
	docker build -t polyubtc/cpp-lib-runner docker-images/cpp-java
	docker build -t polyubtc/wasm-lib-runner docker-images/wasm-runner
	@echo "\033[33mPush Docker Image...\033[0m"
	docker push polyubtc/cpp-lib-runner
	docker push polyubtc/wasm-lib-runner

# Package necessary C++ compiled files
# eg. header and object files
.PHONY: pack_cpp
pack_cpp:
	@echo "\033[33mPack Crypto Library...\033[0m"
	@rm -rf build/cpp/cryptoplus
	@mkdir -p build/cpp/cryptoplus
	@rsync -r generated-src/cpp/*.hpp build/cpp/cryptoplus
	@mkdir -p build/cpp/cryptoplus/vendor/cryptopp
	@rsync build/cpp/vendor/cryptopp/*.o build/cpp/cryptoplus/vendor/cryptopp
	@rm -rf src/cpp/lib/*.gcno src/cpp/lib/**/*.gcno
	@rm -rf src/cpp/lib/*.gcda src/cpp/lib/**/*.gcda

# Package necessary files for iOS development
# eg. C++ files, Djinni generated Objective-C support library and interface wrapper
.PHONY: pack_ios
pack_ios:
	@echo "\033[33mPack Djinni Bridge for iOS...\033[0m"
	@echo "\033[33mPack Crypto Library...\033[0m"
	@rm -rf build/cpp/cryptoplus
	@mkdir -p build/cpp/cryptoplus
	@rsync -r generated-src/cpp/*.hpp build/cpp/cryptoplus
	@rm -rf build/ios
	@mkdir -p build/ios
	@rsync -r build/cpp/cryptoplus/* build/ios/cpp
	@rsync -r generated-src/objc/* build/ios/cpp-bridge
	@rsync -r deps/djinni/support-lib/*.hpp build/ios/cpp-bridge-support
	@rsync -r deps/djinni/support-lib/objc/* build/ios/cpp-bridge-support/objc
	@cp deps/cryptopp/libcryptopp.a build/ios/libcryptopp.a
	@cd src/cpp && bash ios.sh

.PHONY: pack_android
pack_android:
	@echo "\033[33mBuild Android Library (.aar)...\033[0m"
	@rm -rf android_project/AndroidCryptoLibrary/cryptoplus/.externalNativeBuild
	@cd android_project/AndroidCryptoLibrary && ./gradlew assembleRelease
	@rm -rf build/android/aar
	@mkdir -p build/android/aar
	@cp android_project/AndroidCryptoLibrary/cryptoplus/build/outputs/aar/* build/android/aar

# TODO: remove
# Build Emscripten for CryptoPP to WebAssembly compatible
.PHONY: build_emscripten
build_emscripten: cryptopp_wasm

CPP_SRC_FILES := $(wildcard \
	src/cpp/lib/*.cpp \
	src/cpp/lib/**/*.cpp \
	src/cpp/lib/**/**/*.cpp \
)
CPP_SRC_FILES := $(patsubst src/%,%,${CPP_SRC_FILES})
WASM_WRAPPER_FILES := $(wildcard \
	src/wasm/*.hpp \
	src/wasm/**/*.cpp \
	src/wasm/**/**/*.cpp \
)
WASM_WRAPPER_FILES := $(patsubst src/%,%,${WASM_WRAPPER_FILES})
WASM_WRAPPER_OUT := ./src/WasmWrapper.cpp

.PHONY: wasm_js_build
wasm_js_build:
	@echo "\033[33mBuild Crypto Library for WebAssembly...\033[0m"
	@rm -rf build/wasm/js
	@mkdir build/wasm/js
	@rm -rf ${WASM_WRAPPER_OUT}
	@touch ${WASM_WRAPPER_OUT}
	@echo "\033[33mAuto-generate wrapper file: ${WASM_WRAPPER_OUT}\033[0m"
	@echo "/*\n * This file is auto generate\n */" >> ${WASM_WRAPPER_OUT}
	@echo "\n// C++ sources" >> ${WASM_WRAPPER_OUT}
	@$(foreach f, ${CPP_SRC_FILES}, \
		echo "#include \"$f\"" >> ${WASM_WRAPPER_OUT}; \
	)
	@echo "\n// WebAssembly wrapper sources" >> ${WASM_WRAPPER_OUT}
	@$(foreach f, ${WASM_WRAPPER_FILES}, \
		echo "#include \"$f\"" >> ${WASM_WRAPPER_OUT}; \
	)

	@echo "\033[33mCompile through EMScripten...\033[0m"
	@emcc ${WASM_WRAPPER_OUT} ./build/wasm/cpp/cryptopp/libcryptopp.a \
		-std=c++11 \
		-I./build/wasm/cpp/cryptopp \
		-I./generated-src/cpp \
		-I./src/cpp/lib \
		-L./build/wasm/cpp/cryptopp \
		-o ./build/wasm/js/cryptoplus.js \
		-s WASM=1 \
		-Wall \
		-s MODULARIZE=1 \
		-O3 \
		-s DISABLE_EXCEPTION_CATCHING=0 \
		--bind

.PHONY: web
web: wasm_js_build

.PHONY: wasm_js_test
wasm_js_test:
	@echo "\033[33mTest Crypto Library for WebAssembly...\033[0m"
	@npm install
	@npm test

.PHONY: jni_clean
jni_clean:
	@echo "\033[33mClean Crypto Library for JNI...\033[0m"
	@rm -rf build/jni/cryptoplus

.PHONY: jni_jar_build
jni_jar_build:
	@echo "\033[33mBuild Crypto Library for JNI...\033[0m"
	@cd src/cpp && make jni_library
	@cd java_project/JavaCryptoLibrary && gradle build
	@cp java_project/JavaCryptoLibrary/build/libs/cryptoplus.jar ./build/jni/cryptoplus

.PHONY: pack_release
pack_release:
	@echo "\033[33mPackage for Crypto Library v${VERSION}...\033[0m"
	@rm -rf ${RELEASE_OUT}
	@rm -rf ${OUT_DIR}/release/${RELEASE_NAME}.zip
	@mkdir -p ${RELEASE_OUT}/examples
	@rsync -r ${OUT_DIR}/android/aar/./ ${RELEASE_OUT}/android
	@rsync -r --exclude cryptoplus ${OUT_DIR}/ios/./ ${RELEASE_OUT}/ios
	@cp ${OUT_DIR}/ios/cryptoplus/libcryptoplus.a ${RELEASE_OUT}/ios
	@rsync -r ${OUT_DIR}/wasm/js/./ ${RELEASE_OUT}/js
	@rsync -r ${OUT_DIR}/jni/cryptoplus/./ ${RELEASE_OUT}/java
	@cp java_project/JavaCryptoLibrary/src/test/java/hk/edu/polyu/comp/LibraryTest.java ${RELEASE_OUT}/examples
	@cp test/js/cryptoplus.test.js ${RELEASE_OUT}/examples
	@cp ./src/*.djinni ${RELEASE_OUT}/
	@cd ${OUT_DIR}/release && zip -r ${RELEASE_NAME}.zip ./$(RELEASE_NAME)/
