release:
	mkdir -p build; cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && make -j 4

install: release
	mkdir -p /usr/local/include/artifact && cp src/lib/*.hpp /usr/local/include/artifact/ && cp build/src/lib/libartifact.so /usr/local/lib/ && cp build/src/afct/afct /usr/local/bin

uninstall:
	rm -rf /usr/local/include/artifact/ /usr/local/lib/libartifact.so /usr/local/bin/afct

debug:
	mkdir -p build; cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && make -j 4

format:
	find src/ -name "*.cpp" -or -name "*.hpp" -print0 | xargs -0 clang-format -i

tidy: debug
	find src/ -name "*.cpp" -print0 | xargs -0 clang-tidy -p build/ --format-style=.clang-format

clean:
	cd build && make clean && cd ..; rm -rf build

run: release
	cd build && src/afct/afct

test: release
	cd build && src/test/test
