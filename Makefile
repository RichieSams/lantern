.PHONY: build

all: build

DOCKER_IMAGE=quay.io/richiesams/docker_gcc:9

ifeq ($(OS),Windows_NT)
CMD=cmd.exe
MKDIR=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) mkdir -p
RM_RECURSIVE=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) rm -rf
RM=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) rm -f
else
MKDIR=mkdir -p
RM_RECURSIVE=rm -rf
RM=rm -f
endif

build/shaders/fullscreen_triangle_vs.spv: source/core/visualizer/shaders/fullscreen_triangle_vs.glsl
	$(MKDIR) build/shaders
	glslangValidator -V -o $@ -S vert $<

source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp: build/shaders/fullscreen_triangle_vs.spv
	echo "#include \"visualizer/shaders/fullscreen_triangle_vs.spv.h\"" > $@
	echo "" >> $@
	cd build/shaders && xxd -i fullscreen_triangle_vs.spv >> ../../$@

build/shaders/final_resolve_ps.spv: source/core/visualizer/shaders/final_resolve_ps.glsl
	$(MKDIR) build/shaders
	glslangValidator -V -o $@ -S frag $<

source/core/visualizer/shaders/final_resolve_ps.spv.cpp: build/shaders/final_resolve_ps.spv
	echo "#include \"visualizer/shaders/final_resolve_ps.spv.h\"" > $@
	echo "" >> $@
	cd build/shaders && xxd -i final_resolve_ps.spv >> ../../$@


build_shaders:
ifeq ($(OS),Windows_NT)
	make CROSS_RULE="source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp source/core/visualizer/shaders/final_resolve_ps.spv.cpp" cross
else
	make source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp source/core/visualizer/shaders/final_resolve_ps.spv.cpp
endif

cross:
	docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) make $(CROSS_RULE)

clean_shaders:
	$(RM) source/core/visualizer/shaders/fullscreen_triangle_vs.spv.cpp
	$(RM) source/core/visualizer/shaders/final_resolve_ps.spv.cpp

generate_debug: build_shaders
	$(MKDIR) build/debug
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug ./

generate_release: build_shaders
	$(MKDIR) build/release
	cmake -B build/release -DCMAKE_BUILD_TYPE=RelWithDebInfo ./

build_debug: generate_debug
	make -C build/debug -j

build_release: generate_release
	make -C build/release -j

build: build_release

run_debug: build_debug
	cd build/debug/source/lantern/ && ./lantern

run_release: build_release
	cd build/release/source/lantern/ && ./lantern

run: run_release

clean: clean_shaders
	$(RM_RECURSIVE) build
