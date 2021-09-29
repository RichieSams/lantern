.PHONY: build

all: build

generate_debug: source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp source/core/visualizer/shaders/final_resolve_ps.spv.cpp
	mkdir -p build/debug
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug ./

generate_release: source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp source/core/visualizer/shaders/final_resolve_ps.spv.cpp
	mkdir -p build/release
	cmake -B build/release -DCMAKE_BUILD_TYPE=RelWithDebInfo ./

build/shaders/fullscreen_triangle_vs.spv: source/core/visualizer/shaders/fullscreen_triangle_vs.glsl
	mkdir -p build/shaders
	glslangValidator -V -o $@ -S vert $<

source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp: build/shaders/fullscreen_triangle_vs.spv
	echo "#include \"visualizer/shaders/fullscreen_triangle_vs.spv.h\"" > $@
	echo "" >> $@
	cd build/shaders && xxd -i fullscreen_triangle_vs.spv >> ../../$@

build/shaders/final_resolve_ps.spv: source/core/visualizer/shaders/final_resolve_ps.glsl
	mkdir -p build/shaders
	glslangValidator -V -o $@ -S frag $<

source/core/visualizer/shaders/final_resolve_ps.spv.cpp: build/shaders/final_resolve_ps.spv
	echo "#include \"visualizer/shaders/final_resolve_ps.spv.h\"" > $@
	echo "" >> $@
	cd build/shaders && xxd -i final_resolve_ps.spv >> ../../$@


build_debug: generate_debug
	$(MAKE) -C build/debug -j

build_release: generate_release
	$(MAKE) -C build/release -j

build: build_release

run_debug: build_debug
	cd build/debug/source/lantern/ && ./lantern

run_release: build_release
	cd build/release/source/lantern/ && ./lantern

run: run_release

clean:
	rm -rf ./build
	rm -f source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp
	rm -f source/core/visualizer/shaders/final_resolve_ps.spv.cpp
