.PHONY: run cloc

run:
	sh run.sh

cmake:
	mkdir -p build
	cd build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..

clean:
	rm -rf build/

cloc:
	cloc * --exclude-dir=build,external
