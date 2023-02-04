.PHONY: build

all: build

DOCKER_IMAGE=quay.io/richiesams/docker_gcc:9

ifeq ($(OS),Windows_NT)
CMD=cmd.exe
MKDIR=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) mkdir -p
RM_RECURSIVE=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) rm -rf
RM=docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) rm -f
CMAKE_PRESET?=Win_x64_Release
PATH_SEP=\\
else
MKDIR=mkdir -p
RM_RECURSIVE=rm -rf
RM=rm -f
CMAKE_PRESET?=Linux_x64_Release
PATH_SEP=/
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

generate: build_shaders
	cmake --preset $(CMAKE_PRESET) ./

build: generate
	cmake --build $(CMAKE_PRESET)

run_debug:
	cd build/Debug/source/lantern/ && .$(PATH_SEP)lantern

run_release:
	cd build/RelWithDebInfo/source/lantern/ && .$(PATH_SEP)lantern

run: run_release

clean: clean_shaders
	$(RM_RECURSIVE) build
