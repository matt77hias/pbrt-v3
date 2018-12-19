// integrators/falsecolor.cpp*
#include "integrators/falsecolor.h"
#include "sampling.h"
#include "interaction.h"
#include "paramset.h"
#include "camera.h"
#include "film.h"
#include "scene.h"

namespace pbrt {

	FalseColorIntegrator::FalseColorIntegrator(std::shared_ptr< const Camera > camera,
											   std::shared_ptr< Sampler > sampler,
											   const Bounds2i& pixel_bounds)
		: SamplerIntegrator(std::move(camera), std::move(sampler), pixel_bounds) {}

	FalseColorIntegrator::FalseColorIntegrator(const FalseColorIntegrator&) = default;

	FalseColorIntegrator::FalseColorIntegrator(FalseColorIntegrator&&) = default;

	FalseColorIntegrator::~FalseColorIntegrator() = default;

	Spectrum FalseColorIntegrator::Li(const RayDifferential& ray,
									  const Scene& scene,
									  Sampler& sampler,
									  MemoryArena& arena,
									  int depth) const {

		ProfilePhase p(Prof::SamplerIntegratorLi);
		Spectrum L;
		
		SurfaceInteraction isect;
		if (scene.Intersect(ray, &isect)) {
			isect.ComputeScatteringFunctions(ray, arena, true);
			if (isect.bsdf) {
				Vector3f wo = -ray.d;
				Vector3f wi;
				Float pdf;
				BxDFType flags;
				L = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, BSDF_ALL, &flags);
			}
		}

		return L;
	}

	FalseColorIntegrator* CreateFalseColorIntegrator(const ParamSet& params,
													 std::shared_ptr< Sampler > sampler,
													 std::shared_ptr< const Camera > camera) {
		
		int nb_bounds;
		const int* bounds = params.FindInt("pixelbounds", &nb_bounds);
		Bounds2i pixel_bounds = camera->film->GetSampleBounds();
		if (bounds) {
			if (nb_bounds != 4) {
				Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
					  nb_bounds);
			}
			else {
				pixel_bounds = Intersect(pixel_bounds,
										 Bounds2i{ { bounds[0u], bounds[2u] }, 
										           { bounds[1u], bounds[3u] } });
				if (0 == pixel_bounds.Area()) {
					Error("Degenerate \"pixelbounds\" specified.");
				}
			}
		}

		return new FalseColorIntegrator(std::move(camera),
										std::move(sampler),
										pixel_bounds);
	}
}