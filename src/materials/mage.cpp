// materials/mage.cpp*
#include "materials/mage.h"
#include "interaction.h"
#include "paramset.h"
#include "reflection.h"
#include "texture.h"

namespace pbrt {

	namespace {

		template< typename T >
		inline T Saturate(T x) noexcept {
			return Clamp(x, T(0), T(1));
		}

		template<>
		inline Spectrum Saturate(Spectrum x) noexcept {
			return x.Clamp(Float(0), Float(1));
		}

		constexpr Float Sqr(Float x) noexcept {
			return x * x;
		}
	}

	MAGEMaterial::MAGEMaterial(std::shared_ptr< Texture< Spectrum > > base_color,
							   std::shared_ptr< Texture< Float > > roughness,
							   std::shared_ptr< Texture< Float > > metalness,
							   std::shared_ptr< Texture< Float > > bump) noexcept
		: m_base_color(std::move(base_color)),
		m_roughness(std::move(roughness)),
		m_metalness(std::move(metalness)),
		m_bump(std::move(bump)) {}

	MAGEMaterial::MAGEMaterial(const MAGEMaterial& material) = default;

	MAGEMaterial::MAGEMaterial(MAGEMaterial&& material) = default;

	MAGEMaterial::~MAGEMaterial() = default;

	MAGEMaterial& MAGEMaterial
		::operator=(const MAGEMaterial& material) = default;

	MAGEMaterial& MAGEMaterial
		::operator=(MAGEMaterial&& material) = default;

	void MAGEMaterial::ComputeScatteringFunctions(SurfaceInteraction* si,
												  MemoryArena& arena,
												  TransportMode mode,
												  bool allowMultipleLobes) const {
		if (m_bump) {
			Bump(m_bump, si);
		}

		si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

		const auto base_color = Saturate(m_base_color->Evaluate(*si));
		const auto metalness  = Saturate(m_roughness->Evaluate(*si));
		const auto roughness  = Saturate(m_metalness->Evaluate(*si));
		const auto alpha      = std::max(Float(1e-3f), Sqr(roughness));

		static const Spectrum s_dielectric_F0 = Float(0.04f);
		const auto R_specular = Lerp(metalness, s_dielectric_F0, base_color);
		const auto R_diffuse  = (Spectrum(1.0f) - R_specular) * (1.0f - metalness) * base_color;

		if (!R_diffuse.IsBlack()) {
			si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(R_diffuse));
		}
		if (!R_specular.IsBlack()) {
			si->bsdf->Add(ARENA_ALLOC(arena, BlinnPhongReflection)(R_specular, alpha));
		}
	}

	MAGEMaterial* CreateMAGEMaterial(const TextureParams& mp) {
		static const Spectrum s_default_base_color;

		auto base_color = mp.GetSpectrumTexture("base_color", s_default_base_color);
		auto roughness  = mp.GetFloatTexture("roughness", Float(1));
		auto metalness  = mp.GetFloatTexture("metalness", Float(0));
		auto bump       = mp.GetFloatTextureOrNull("bump");

		return new MAGEMaterial(std::move(base_color),
								std::move(roughness),
								std::move(metalness),
								std::move(bump));
	}
}