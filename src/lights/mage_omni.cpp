// lights/mage_omni.cpp*
#include "lights/mage_omni.h"
#include "scene.h"
#include "paramset.h"
#include "sampling.h"
#include "stats.h"

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
	
		Float DistanceAttenuationSmoothingFactor(float distance2,
												 float range2) noexcept {
			// Frostbite's smoothing:
			//
			//         [    distance^2]^2
			// saturate[1 - ----------]
			//         [      range^2 ]

			return Sqr(Saturate(Float(1) - distance2 / range2));
		}

		Float DistanceAttenuation(Float distance, Float range) noexcept {
			//                 1
			// df := -----------------------
			//       max(distance^2, 0.01^2)

			const auto distance2   = Sqr(distance);
			const auto range2      = Sqr(range);
			const auto attenuation = Float(1) / std::max(distance2, Float(1e-4f));
			const auto smoothing   = DistanceAttenuationSmoothingFactor(distance2, range2);

			return attenuation * smoothing;
		}
	}

	MAGEOmniLight::MAGEOmniLight(const Transform& light_to_world,
								 const MediumInterface& medium_interface,
								 Float range,
								 Spectrum I) 
		: Light(static_cast< int >(LightFlags::DeltaPosition),
				light_to_world,
				medium_interface),
		m_p(LightToWorld(Point3f())),
		m_range(range),
		m_I(std::move(I)) {}

	MAGEOmniLight::MAGEOmniLight(const MAGEOmniLight& light) = default;

	MAGEOmniLight::MAGEOmniLight(MAGEOmniLight&& light) = default;

	MAGEOmniLight::~MAGEOmniLight() = default;

	Spectrum MAGEOmniLight::Power() const {
		return Float(4) * Pi * m_I;
	}

	Spectrum MAGEOmniLight::Sample_Li(const Interaction& interaction,
									  const Point2f& u,
									  Vector3f* wi,
									  Float* pdf,
									  VisibilityTester* vis) const {

		ProfilePhase _(Prof::LightSample);
		
		const auto direction    = m_p - interaction.p;
		const auto distance     = direction.Length();
		const auto inv_distance = Float(1) / distance;

		*wi  = direction * inv_distance;
		*pdf = Float(1);
		*vis = VisibilityTester(interaction,
								Interaction(m_p, interaction.time, mediumInterface));

		return DistanceAttenuation(distance, m_range) * m_I;
	}

	Float MAGEOmniLight::Pdf_Li(const Interaction& interaction,
								const Vector3f& wi) const {
		return Float(0);
	}

	Spectrum MAGEOmniLight::Sample_Le(const Point2f& u1,
									  const Point2f& u2,
									  Float time,
									  Ray* ray,
									  Normal3f* n_light,
									  Float* pdf_pos,
									  Float* pdf_dir) const {

		ProfilePhase _(Prof::LightSample);
		
		*ray     = Ray(m_p, UniformSampleSphere(u1),
					   Infinity, time, mediumInterface.inside);
		*n_light = Normal3f(ray->d);
		*pdf_pos = Float(1);
		*pdf_dir = UniformSpherePdf();
		
		return m_I;
	}

	void MAGEOmniLight::Pdf_Le(const Ray& ray,
							   const Normal3f& n_light,
							   Float* pdf_pos,
							   Float* pdf_dir) const {

		ProfilePhase _(Prof::LightPdf);
		*pdf_pos = Float(0);
		*pdf_dir = UniformSpherePdf();
	}

	std::shared_ptr< MAGEOmniLight > CreateMAGEOmniLight(const Transform& light_to_world,
														 const Medium* medium,
														 const ParamSet& params) {

		const auto I     = params.FindOneSpectrum("I",     Spectrum(Float(1)));
		const auto scale = params.FindOneSpectrum("scale", Spectrum(Float(1)));
		const auto range = params.FindOneFloat("range",    Float(1));
		const auto p     = params.FindOnePoint3f("from",   Point3f());
		const auto l2w   = Translate(Vector3f(p.x, p.y, p.z)) * light_to_world;
		
		return std::make_shared< MAGEOmniLight >(l2w, medium, range, scale * I);
	}

}  // namespace pbrt