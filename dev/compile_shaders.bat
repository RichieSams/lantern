@setlocal
@cd ../source/core/visualizer/shaders

@glslangValidator.exe -V -o fullscreen_triangle_vs.spv -S vert fullscreen_triangle_vs.glsl
@glslangValidator.exe -V -o final_resolve_ps.spv -S frag final_resolve_ps.glsl

@(echo #include "visualizer/shaders/fullscreen_triangle_vs.spv.h" & echo.) > fullscreen_triangle_vs.spv.cpp
@xxd.exe -i fullscreen_triangle_vs.spv >> fullscreen_triangle_vs.spv.cpp
@(echo #include "visualizer/shaders/final_resolve_ps.spv.h" & echo.) > final_resolve_ps.spv.cpp
@xxd.exe -i final_resolve_ps.spv >> final_resolve_ps.spv.cpp

@endlocal
