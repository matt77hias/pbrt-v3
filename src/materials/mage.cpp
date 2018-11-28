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

	MAGEMaterial::MAGEMaterial(Spectrum base_color,
							   Float roughness,
							   Float metalness,
							   std::shared_ptr< Texture< Spectrum > > base_color_texture,
							   std::shared_ptr< Texture< Float > > roughness_texture,
							   std::shared_ptr< Texture< Float > > metalness_texture,
							   std::shared_ptr< Texture< Float > > bump_texture) noexcept
		: m_base_color(std::move(base_color)),
		m_roughness(roughness),
		m_metalness(metalness),
		m_base_color_texture(std::move(base_color_texture)),
		m_roughness_texture(std::move(roughness_texture)),
		m_metalness_texture(std::move(metalness_texture)),
		m_bump_texture(std::move(bump_texture)) {}

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
		
		if (m_bump_texture) {
			Bump(m_bump_texture, si);
		}

		si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

		const auto base_color = Saturate(RGBSpectrum(m_base_color * m_base_color_texture->Evaluate(*si)));
		const auto metalness  = Saturate(m_roughness  * m_roughness_texture->Evaluate(*si));
		const auto roughness  = Saturate(m_metalness  * m_metalness_texture->Evaluate(*si));
		const auto alpha      = std::max(Float(1e-3f), Sqr(roughness));

		static const Spectrum s_dielectric_F0 = Float(0.04f);
		const auto F_specular = Lerp(metalness, s_dielectric_F0, base_color);
		const auto F_diffuse  = (Spectrum(1.0f) - F_specular) * (1.0f - metalness);

		if (!F_diffuse.IsBlack()) {
			si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(F_diffuse));
		}
		if (!F_specular.IsBlack()) {
			si->bsdf->Add(ARENA_ALLOC(arena, BlinnPhongReflection)(F_specular, alpha));
		}
	}

	MAGEMaterial* CreateMAGEMaterial(const TextureParams& mp) {
		static const Spectrum s_default_base_color;

		auto base_color 
			= mp.FindSpectrum("base_color", s_default_base_color);
		const auto roughness
			= mp.FindFloat("roughness", Float(1));
		const auto metalness 
			= mp.FindFloat("metalness", Float(0));
		auto base_color_texture
			= mp.GetSpectrumTexture("texture_base_color", s_default_base_color);
		auto roughness_texture
			= mp.GetFloatTexture("texture_roughness", Float(1));
		auto metalness_texture
			= mp.GetFloatTexture("texture_metalness", Float(0));
		auto bump_texture 
			= mp.GetFloatTextureOrNull("texture_bump");

		return new MAGEMaterial(std::move(base_color),
								roughness,
								metalness,
								std::move(base_color_texture),
								std::move(roughness_texture),
								std::move(metalness_texture),
								std::move(bump_texture));
	}
}