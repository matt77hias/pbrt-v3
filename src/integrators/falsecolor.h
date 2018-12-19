#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_INTEGRATORS_FALSECOLOR_H
#define PBRT_INTEGRATORS_FALSECOLOR_H

// integrators/falsecolor.h*
#include "pbrt.h"
#include "integrator.h"

namespace pbrt {

	class FalseColorIntegrator : public SamplerIntegrator {

	public:

		explicit FalseColorIntegrator(std::shared_ptr< const Camera > camera,
									  std::shared_ptr< Sampler > sampler,
									  const Bounds2i& pixel_bounds);

		FalseColorIntegrator(const FalseColorIntegrator&);

		FalseColorIntegrator(FalseColorIntegrator&&);

		virtual ~FalseColorIntegrator();

		FalseColorIntegrator& operator=(const FalseColorIntegrator&) = delete;

		FalseColorIntegrator& operator=(FalseColorIntegrator&&) = delete;

		virtual Spectrum Li(const RayDifferential& ray,
							const Scene& scene,
							Sampler& sampler,
							MemoryArena& arena,
							int depth) const override;
	};

	FalseColorIntegrator* CreateFalseColorIntegrator(const ParamSet& params,
													 std::shared_ptr< Sampler > sampler,
													 std::shared_ptr< const Camera > camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_FALSECOLOR_H
