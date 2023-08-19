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


cross:
	docker run --rm -v $(CURDIR):/app -w /app $(DOCKER_IMAGE) make $(CROSS_RULE)

generate:
	cmake --preset $(CMAKE_PRESET) ./

build: generate
	cmake --build --preset $(CMAKE_PRESET) -j

run_debug:
	cd build/Debug/source/lantern/ && .$(PATH_SEP)lantern

run_release:
	cd build/RelWithDebInfo/source/lantern/ && .$(PATH_SEP)lantern

run: run_release

clean:
	$(RM_RECURSIVE) build
