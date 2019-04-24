/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set=0, binding=0) uniform sampler2D inputTexture;

layout(push_constant) uniform ToneMapperConstants {
	int selection;
	float exposure;
} toneMapperConstants;

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

#define CLAMP_TONEMAPPING 0
#define FILMIC_TONEMAPPING 1


vec4 ClampTonemapping(vec4 color) {
	return clamp(color, 0.0f, 1.0f);
}

vec4 FilmicTonemapping(vec4 color) {
	vec4 x = max(vec4(0.0f), color - 0.004f);
    color = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);

	color.a = 1.0f;
    return clamp(color, 0.0f, 1.0f);
}

void main() {
    vec4 color = toneMapperConstants.exposure * vec4(texture(inputTexture, texCoord).rgb, 1.0f);
    
	if (toneMapperConstants.selection == CLAMP_TONEMAPPING) {
		outColor = ClampTonemapping(color);
	} else if (toneMapperConstants.selection == FILMIC_TONEMAPPING) {
		outColor = FilmicTonemapping(color);
	}
}
