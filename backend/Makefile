.PHONY: install
install:
	mkdir -p build
	conan install . --output-folder=build --build=missing -pr conanprofile.txt

.PHONY: build
build:
	cd build; \
	cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug; \
	cmake --build . --config Debug;

.PHONY: build-release
build-release:
	mkdir -p build
	conan install . --output-folder=build --build=missing -pr conanprofile-release.txt
	cd build; \
	cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release; \
	cmake --build . --config Release; \
	cpack;

.PHONY: serve
serve:
	./build/api_server 0.0.0.0 8080

.PHONY: format
format:
	find include/ src/ -iname *.h -or -iname *.cpp | xargs clang-format -i

.PHONY: lint
lint:
	cppcheck --enable=all --std=c++17 --cppcheck-build-dir=build --project=build/compile_commands.json

.PHONY: clean
clean:
	rm -rf build/
