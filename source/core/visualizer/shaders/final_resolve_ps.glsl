#version 450

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(texCoord, 0.0f, 1.0);
}
