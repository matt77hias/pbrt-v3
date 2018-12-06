#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_LIGHTS_MAGE_OMNI_H
#define PBRT_LIGHTS_MAGE_OMNI_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

// lights/mage_omni.h*
#include "pbrt.h"
#include "light.h"
#include "shape.h"

//-----------------------------------------------------------------------------
// Declarations
//-----------------------------------------------------------------------------
namespace pbrt {

	//-------------------------------------------------------------------------
	// MAGEOmniLight
	//-------------------------------------------------------------------------

	class MAGEOmniLight : public Light {

	public:

		//-----------------------------------------------------------------
		// Constructors and Destructors
		//-----------------------------------------------------------------

		explicit MAGEOmniLight(const Transform& light_to_world,
							   const MediumInterface& medium_interface,
							   Float range,
							   Spectrum I);

		MAGEOmniLight(const MAGEOmniLight& light);

		MAGEOmniLight(MAGEOmniLight&& light);

		virtual ~MAGEOmniLight();

		//-----------------------------------------------------------------
		// Assignment Operators
		//-----------------------------------------------------------------

		MAGEOmniLight& operator=(const MAGEOmniLight& light) = delete;

		MAGEOmniLight& operator=(MAGEOmniLight&& light) = delete;

		//-----------------------------------------------------------------
		// Member Methods
		//-----------------------------------------------------------------

		virtual Spectrum Power() const override;

		virtual Spectrum Sample_Li(const Interaction& interaction, 
								   const Point2f& u, 
								   Vector3f* wi,
								   Float* pdf, 
								   VisibilityTester* vis) const override;
		
		virtual Float Pdf_Li(const Interaction& interaction,
							 const Vector3f& wi) const override;
		
		virtual Spectrum Sample_Le(const Point2f& u1,
								   const Point2f& u2,
								   Float time,
								   Ray* ray,
								   Normal3f* n_light,
								   Float* pdf_pos,
								   Float* pdf_dir) const override;
		
		virtual void Pdf_Le(const Ray& ray,
							const Normal3f& n_light, 
							Float* pdf_pos,
							Float* pdf_dir) const override;

	private:

		//-----------------------------------------------------------------
		// Member Variables
		//-----------------------------------------------------------------

		Point3f m_p;
		Float m_range;
		Spectrum m_I;
	};

	//-------------------------------------------------------------------------
	// MAGEOmniLight Factory
	//-------------------------------------------------------------------------

	std::shared_ptr< MAGEOmniLight > CreateMAGEOmniLight(const Transform& light_to_world,
														 const Medium* medium,
														 const ParamSet& params);

}  // namespace pbrt

#endif  // PBRT_LIGHTS_MAGE_OMNI_H