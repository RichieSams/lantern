#include "materials/bsdf.h"
#include "materials/bsdf_lobe.h"

#include "math/vector_math.h"
#include "math/sampling.h"

#include "integrator/surface_interaction.h"

namespace Lantern {

BSDF::~BSDF() {
	for (int i =0; i < m_numLobes; ++i) {
		delete m_lobes[i];
	}
}

bool BSDF::AddLobe(BxDF *lobe) {
	if (m_numLobes >= kMaxBxDFLobes) {
		return false;
	}

	m_lobes[m_numLobes++] = lobe;
}

float3 BSDF::Eval(SurfaceInteraction &interaction, float3 inputDirection, BSDFLobe::Type allowedLobes) const {
	float3x3 frame = CreateCoordinateFrame(interaction.Shading.Normal);
	float3x3 inverseFrame = frame.inverse();

	float3 localInputDirection = inverseFrame * inputDirection;
	float3 localOutputDirection = inverseFrame * interaction.OutputDirection;
	if (localOutputDirection.z == 0.0f) {
		return float3(0.0f);
	}

	const bool reflect = dot(interaction.OutputDirection, interaction.Normal) * dot(inputDirection, interaction.Normal) > 0;

	float3 f(0.0f);
	for (int i = 0; i < m_numLobes; ++i) {
		if (m_lobes[i]->MatchesType(allowedLobes) && ((reflect && (m_lobes[i]->LobeType & BSDFLobe::Reflection) == BSDFLobe::Reflection) || (!reflect && (m_lobes[i]->LobeType & BSDFLobe::Transmission) == BSDFLobe::Transmission))) {
			f += m_lobes[i]->Eval(localOutputDirection, localInputDirection, interaction.TexCoord);
		}
	}

	return f;
}

float3 BSDF::Sample(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *inputDirection, float *pdf, BSDFLobe::Type allowedLobes, BSDFLobe::Type *sampledLobe, float IORi, float *IORo) const {
	// Choose which bxdf to sample
	BxDF *bxdf = nullptr;
	int numMatchingLobes = 0;
	{
		for (int i = 0; i < m_numLobes; ++i) {
			if (m_lobes[i]->MatchesType(allowedLobes)) {
				++numMatchingLobes;
			}
		}
		// Early out
		if (numMatchingLobes == 0) {
			*pdf = 0.0f;
			*sampledLobe = BSDFLobe::Null;
			return float3(0.0f);
		}

		int pickedIndex = sampler->NextDiscrete(numMatchingLobes);
		int count = pickedIndex;
		for (int i = 0; i < m_numLobes; ++i) {
			if (m_lobes[i]->MatchesType(allowedLobes) && count-- == 0) {
				bxdf = m_lobes[i];
				break;
			}
		}
	}

	// Create the change of coordinate matrix
	float3x3 frame = CreateCoordinateFrame(interaction.Shading.Normal);
	float3x3 inverseFrame = frame.inverse();

	float3 localOutputDirection = inverseFrame * interaction.OutputDirection;
	if (localOutputDirection.z == 0.0f) {
		*pdf = 0.0f;
		*sampledLobe = BSDFLobe::Null;
		return float3(0.0f);
	}

	// Sample the chosen BxDF
	float3 localInputDirection;
	*pdf = 0.0f;
	float3 f = bxdf->Sample(sampler, localOutputDirection, &localInputDirection, pdf, IORi, IORo, interaction.TexCoord);
	*sampledLobe = bxdf->LobeType;

	if (*pdf == 0.0f) {
		*sampledLobe = BSDFLobe::Null;
		return float3(0.0f);
	}

	*inputDirection = frame * localInputDirection;

	// Compute the overall pdf by adding the pdfs of the non-sampled BxDFs
	// But only if non-specular
	if ((bxdf->LobeType & BSDFLobe::Specular) == BSDFLobe::Specular && numMatchingLobes > 1) {
		for (int i = 0; i < m_numLobes; ++i) {
			// Don't double count the sampled BxDF
			if (m_lobes[i] == bxdf) {
				continue;
			}

			if (m_lobes[i]->MatchesType(allowedLobes)) {
				*pdf += m_lobes[i]->Pdf(localOutputDirection, localInputDirection, interaction.TexCoord);
			}
		}
	}

	// Evalute the BSDF with the sampled direction
	if (!(bxdf->LobeType & BSDFLobe::Specular)) {
		bool reflect = dot(*inputDirection, interaction.Normal) * dot(interaction.OutputDirection, interaction.Normal) > 0.0f;

		f = float3(0.0f);
		for (int i = 0; i < m_numLobes; ++i) {
			if (m_lobes[i]->MatchesType(allowedLobes) && ((reflect && (m_lobes[i]->LobeType & BSDFLobe::Reflection) == BSDFLobe::Reflection) || (!reflect && (m_lobes[i]->LobeType & BSDFLobe::Transmission) == BSDFLobe::Transmission))) {
				f += m_lobes[i]->Eval(localOutputDirection, localInputDirection, interaction.TexCoord);
			}
		}
	}

	return f;
}

float BSDF::Pdf(SurfaceInteraction &interaction, float3 inputDirection, BSDFLobe::Type allowedLobes) const {
	if (m_numLobes == 0.0f) {
		return 0.0f;
	}

	// Create the change of coordinate matrix
	float3x3 frame = CreateCoordinateFrame(interaction.Shading.Normal);
	float3x3 inverseFrame = frame.inverse();

	float3 localInputDirection = inverseFrame * inputDirection;
	float3 localOutputDirection = inverseFrame * interaction.OutputDirection;
	if (localOutputDirection.z == 0.0f) {
		return 0.0f;
	}
	
	// Calculate the total pdf
	float pdf = 0.0f;
	int numMatchingLobes = 0;
	for (int i = 0; i < m_numLobes; ++i) {
		if (m_lobes[i]->MatchesType(allowedLobes)) {
			++numMatchingLobes;
			pdf += m_lobes[i]->Pdf(localOutputDirection, localInputDirection, interaction.TexCoord);
		}
	}

	return numMatchingLobes > 0 ? pdf / numMatchingLobes : 0.0f;
}

} // End of namespace Lantern