cd ../source/core/visualizer/shaders

glslangValidator -V -o fullscreen_triangle_vs.spv -S vert fullscreen_triangle_vs.glsl
glslangValidator -V -o final_resolve_ps.spv -S frag final_resolve_ps.glsl

echo '#include "visualizer/shaders/fullscreen_triangle_vs.spv.h"' > fullscreen_triangle_vs.spv.cpp
echo '' >> fullscreen_triangle_vs.spv.cpp
xxd -i fullscreen_triangle_vs.spv >> fullscreen_triangle_vs.spv.cpp

echo '#include "visualizer/shaders/final_resolve_ps.spv.h"' > final_resolve_ps.spv.cpp
echo '' >> final_resolve_ps.spv.cpp
xxd -i final_resolve_ps.spv >> final_resolve_ps.spv.cpp
