#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_MATERIALS_MAGE_H
#define PBRT_MATERIALS_MAGE_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

// materials/mage.h*
#include "material.h"
#include "spectrum.h"

//-----------------------------------------------------------------------------
// Declarations
//-----------------------------------------------------------------------------
namespace pbrt {

	//-------------------------------------------------------------------------
	// MAGEMaterial
	//-------------------------------------------------------------------------

	class MAGEMaterial : public Material {
		
		public:
		
			//-----------------------------------------------------------------
			// Constructors and Destructors
			//-----------------------------------------------------------------
			
			explicit MAGEMaterial(std::shared_ptr< Texture< Spectrum > > base_color,
								  std::shared_ptr< Texture< Float > > roughness,
								  std::shared_ptr< Texture< Float > > metalness,
								  std::shared_ptr< Texture< Float > > bump_texture) noexcept;
			
			MAGEMaterial(const MAGEMaterial& material);
			
			MAGEMaterial(MAGEMaterial&& material);
			
			virtual ~MAGEMaterial();
			
			//-----------------------------------------------------------------
			// Assignment Operators
			//-----------------------------------------------------------------
			
			MAGEMaterial& operator=(const MAGEMaterial& material);
			
			MAGEMaterial& operator=(MAGEMaterial&& material);
			
			//-----------------------------------------------------------------
			// Member Methods
			//-----------------------------------------------------------------
			
			virtual void ComputeScatteringFunctions(SurfaceInteraction* si, 
			                                        MemoryArena& arena,
													TransportMode mode,
													bool allowMultipleLobes) const override;
		
		private:
		
			//-----------------------------------------------------------------
			// Member Variables
			//-----------------------------------------------------------------
			
			std::shared_ptr< Texture< Spectrum > > m_base_color;

			std::shared_ptr< Texture< Float > > m_roughness;

			std::shared_ptr< Texture< Float > > m_metalness;

			std::shared_ptr< Texture< Float > > m_bump;
	};
	
	//-------------------------------------------------------------------------
	// MAGEMaterial Factory
	//-------------------------------------------------------------------------

	MAGEMaterial* CreateMAGEMaterial(const TextureParams& mp);
}

#endif  // PBRT_MATERIALS_MAGE_H
