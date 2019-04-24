#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set=0, binding=0) uniform sampler2D inputTexture;

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;


vec4 FilmicTonemapping(vec4 color) {
	vec4 x = max(vec4(0.0), color - 0.004);
    color = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
	return vec4(clamp(color, 0.0, 1.0f).rbg, 1.0f);
}


void main() {
	float exposure = 1.0f;
	vec4 color = exposure * vec4(texture(inputTexture, texCoord).rgb, 1.0f);
	
	outColor = FilmicTonemapping(color);
}
