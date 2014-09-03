/************************************************************************************

Filename    :   OculusRoomModel.cpp
Content     :   Creates a simple room scene from hard-coded geometry
Created     :   October 4, 2012

Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#include "OculusTest.h"
#include "RenderTiny_D3D11_Device.h"


enum BuiltinTexture
{
    Tex_None,
    Tex_Checker,
    Tex_Block,
    Tex_Panel,
    Tex_Count
};

// Temporary helper class used to initialize fills used by model. 
class FillCollection
{
public:
    Ptr<ShaderFill> LitSolid;
    Ptr<ShaderFill> LitTextures[4];

    FillCollection(RenderDevice* render);
  
};

FillCollection::FillCollection(RenderDevice* render)
{
    Ptr<Texture> builtinTextures[Tex_Count];

    // Create floor checkerboard texture.
    {
        Color checker[256*256];
        for (int j = 0; j < 256; j++)
            for (int i = 0; i < 256; i++)
                checker[j*256+i] = (((i/4 >> 5) ^ (j/4 >> 5)) & 1) ?
                Color(180,180,180,255) : Color(80,80,80,255);
        builtinTextures[Tex_Checker] = *render->CreateTexture(Texture_RGBA|Texture_GenMipmaps, 256, 256, checker);
        builtinTextures[Tex_Checker]->SetSampleMode(Sample_Anisotropic|Sample_Repeat);
    }

    // Ceiling panel texture.
    {
        Color panel[256*256];
        for (int j = 0; j < 256; j++)
            for (int i = 0; i < 256; i++)
                panel[j*256+i] = (i/4 == 0 || j/4 == 0) ?
                Color(80,80,80,255) : Color(180,180,180,255);
        builtinTextures[Tex_Panel] = *render->CreateTexture(Texture_RGBA|Texture_GenMipmaps, 256, 256, panel);
        builtinTextures[Tex_Panel]->SetSampleMode(Sample_Anisotropic|Sample_Repeat);
    }

    // Wall brick textures.
    {
        Color block[256*256];
        for (int j = 0; j < 256; j++)
            for (int i = 0; i < 256; i++)
                block[j*256+i] = (((j/4 & 15) == 0) || (((i/4 & 15) == 0) && ((((i/4 & 31) == 0) ^ ((j/4 >> 4) & 1)) == 0))) ?
                Color(60,60,60,255) : Color(180,180,180,255);
        builtinTextures[Tex_Block] = *render->CreateTexture(Texture_RGBA|Texture_GenMipmaps, 256, 256, block);
        builtinTextures[Tex_Block]->SetSampleMode(Sample_Anisotropic|Sample_Repeat);
    }

    LitSolid = *new ShaderFill(*render->CreateShaderSet());
    LitSolid->GetShaders()->SetShader(render->LoadBuiltinShader(Shader_Vertex, VShader_MVP)); 
    LitSolid->GetShaders()->SetShader(render->LoadBuiltinShader(Shader_Fragment, FShader_LitGouraud)); 

    for (int i = 1; i < Tex_Count; i++)
    {
        LitTextures[i] = *new ShaderFill(*render->CreateShaderSet());
        LitTextures[i]->GetShaders()->SetShader(render->LoadBuiltinShader(Shader_Vertex, VShader_MVP)); 
        LitTextures[i]->GetShaders()->SetShader(render->LoadBuiltinShader(Shader_Fragment, FShader_LitTexture)); 
        LitTextures[i]->SetTexture(0, builtinTextures[i]);
    }

}





// Adds atoms to form crystal system
void SceneBuilder::PopulateRoomScene(Scene* scene, RenderDevice* render)
{
	FillCollection fills(render);

	scene->World.Clear();

	auto add = [&](float x, float y, float z){
		Model *sphere = new Model(Prim_Triangles);
		sphere->AddSphere(float(0.5 * scale));
		sphere->SetPosition(Vector3f(x, y, z));
		sphere->Fill = fills.LitTextures[Tex_Checker];
		scene->World.Add(Ptr<Model>(*sphere));
	};

	int cells = structure == Diamond ? 6 : 4;

	for (int ix = -cells; ix < cells; ix++)
	{
		for (int iy = -cells; iy < cells; iy++)
		{
			for (int iz = -cells; iz < cells; iz++)
			{
				switch (structure)
				{
				default:
				case Cube:
					add(ix, iy, iz);
					break;
				case FCC:
					{
						float xmod = (ix + cells + iy + cells + iz + cells) % 2 * sqrt(0.5f);
						add(ix * sqrt(0.5f) + xmod, iy * sqrt(0.5f), iz * sqrt(0.5f));
						break;
					}
				case BCC:
					{
						float ymod = (iy + cells) % 2 * 0.5f;
						add(ix + ymod, float(iy * sqrt(1. / 2.)), iz + ymod);
						break;
					}
				case Diamond:
					{
						int xmod = (ix + cells) % 2;
						int ymod = (iy + cells) % 2;
						int zmod = (iz + cells) % 2;
						int xyzmod = (ix + cells + iy + cells + iz + cells) % 4;
						float fh = sqrt(1.f / 3.f);
						if (xmod == ymod && ymod == zmod && (xyzmod == 0 || xyzmod == 1))
							add(ix * fh, iy * fh, iz * fh);
						break;
					}
				}
			}
		}
	}

    scene->SetAmbient(Vector4f(0.65f,0.65f,0.65f,1));
	scene->Lighting.LightCount = 0;
    scene->AddLight(Vector3f(-2,4,-2), Vector4f(8,8,8,1));
    scene->AddLight(Vector3f(3,4,-3),  Vector4f(2,1,1,1));
    scene->AddLight(Vector3f(-4,3,25), Vector4f(3,6,3,1));
}
