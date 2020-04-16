// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2004-2016 Christoph Oelckers
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//

#include "filesystem.h"
#include "m_png.h"
#include "c_dispatch.h"
#include "hw_ihwtexture.h"
#include "hw_material.h"
#include "texturemanager.h"
#include "c_cvars.h"

IHardwareTexture* CreateHardwareTexture();

//===========================================================================
//
// Constructor
//
//===========================================================================

FMaterial::FMaterial(FGameTexture * tx, int scaleflags)
{
	mShaderIndex = SHADER_Default;
	sourcetex = tx;
	imgtex = tx->GetTexture();

	if (tx->GetUseType() == ETextureType::SWCanvas && static_cast<FWrapperTexture*>(imgtex)->GetColorFormat() == 0)
	{
		mShaderIndex = SHADER_Paletted;
	}
	else if (tx->isHardwareCanvas())
	{
		if (tx->GetShaderIndex() >= FIRST_USER_SHADER)
		{
			mShaderIndex = tx->GetShaderIndex();
		}
		// no brightmap for cameratexture
	}
	else
	{
		if (tx->isWarped())
		{
			mShaderIndex = tx->isWarped(); // This picks SHADER_Warp1 or SHADER_Warp2
		}
		else if (imgtex->Normal && imgtex->Specular)
		{
			for (auto &texture : { imgtex->Normal, imgtex->Specular })
			{
				mTextureLayers.Push(texture);
			}
			mShaderIndex = SHADER_Specular;
		}
		else if (imgtex->Normal && imgtex->Metallic && imgtex->Roughness && imgtex->AmbientOcclusion)
		{
			for (auto &texture : { imgtex->Normal, imgtex->Metallic, imgtex->Roughness, imgtex->AmbientOcclusion })
			{
				mTextureLayers.Push(texture);
			}
			mShaderIndex = SHADER_PBR;
		}

		// Note that these layers must present a valid texture even if not used, because empty TMUs in the shader are an undefined condition.
		imgtex->CreateDefaultBrightmap();
		auto placeholder = TexMan.GameByIndex(1);
		if (imgtex->Brightmap)
		{
			mTextureLayers.Push(imgtex->Brightmap);
			mLayerFlags |= TEXF_Brightmap;
		}
		else	
		{ 
			mTextureLayers.Push(placeholder->GetTexture());
		}
		if (imgtex->Detailmap)
		{
			mTextureLayers.Push(imgtex->Detailmap);
			mLayerFlags |= TEXF_Detailmap;
		}
		else
		{
			mTextureLayers.Push(placeholder->GetTexture());
		}
		if (imgtex->Glowmap)
		{
			mTextureLayers.Push(imgtex->Glowmap);
			mLayerFlags |= TEXF_Glowmap;
		}
		else
		{
			mTextureLayers.Push(placeholder->GetTexture());
		}

		if (imgtex->shaderindex >= FIRST_USER_SHADER)
		{
			const UserShaderDesc &usershader = usershaders[imgtex->shaderindex - FIRST_USER_SHADER];
			if (usershader.shaderType == mShaderIndex) // Only apply user shader if it matches the expected material
			{
				for (auto &texture : imgtex->CustomShaderTextures)
				{
					if (texture == nullptr) continue;
					mTextureLayers.Push(texture);
				}
				mShaderIndex = tx->GetShaderIndex();
			}
		}
	}
	mScaleFlags = scaleflags;

	mTextureLayers.ShrinkToFit();
	imgtex->Material[scaleflags] = this;
	if (tx->isHardwareCanvas()) tx->SetTranslucent(false);
}

//===========================================================================
//
// Destructor
//
//===========================================================================

FMaterial::~FMaterial()
{
}


//===========================================================================
//
//
//
//===========================================================================

IHardwareTexture *FMaterial::GetLayer(int i, int translation, FTexture **pLayer) const
{
	FTexture *layer = i == 0 ? imgtex : mTextureLayers[i - 1];
	if (pLayer) *pLayer = layer;
	
	if (layer) return layer->GetHardwareTexture(translation, mScaleFlags);
	return nullptr;
}

//==========================================================================
//
// Gets a texture from the texture manager and checks its validity for
// GL rendering. 
//
//==========================================================================

FMaterial * FMaterial::ValidateTexture(FGameTexture * gtex, int scaleflags, bool create)
{
	if (gtex && gtex->isValid())
	{
		auto tex = gtex->GetTexture();
		if (!tex->ShouldExpandSprite()) scaleflags &= ~CTF_Expand;

		FMaterial *hwtex = tex->Material[scaleflags];
		if (hwtex == NULL && create)
		{
			hwtex = new FMaterial(gtex, scaleflags);
		}
		return hwtex;
	}
	return NULL;
}

void DeleteMaterial(FMaterial* mat)
{
	delete mat;
}


