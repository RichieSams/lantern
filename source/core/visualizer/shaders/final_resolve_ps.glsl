#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set=0, binding=0) uniform sampler2D inputTexture;

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(texture(inputTexture, texCoord).rgb, 1.0f);
}
