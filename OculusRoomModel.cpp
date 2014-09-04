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

	// Returns a quaternion representing rotation that transforms (0,0,1) vector
	// so that it's parallel to the given vector.
	auto direction = [](const Vector3f &dir){
		static const double epsilon = 1e-5f;
		double p;
		Quatf ret;
		if (dir.LengthSq() <= epsilon)
			return Quatf(0, 0, 0, 1);
		Vector3f dr = dir.Normalized();

		/* half-angle formula of trigonometry replaces expensive tri-functions to square root */
		ret.w = ::sqrt((dr[2] + 1) / 2) /*cos(acos(dr[2]) / 2.)*/;

		if (1 - epsilon < ret.w){
			ret.x = ret.y = ret.z = 0;
		}
		else if (ret.w < epsilon){
			ret.x = ret.z = 0;
			ret.y = 1;
		}
		else{
			Vector3f v = Vector3f(0,0,1).Cross(dr);
			p = sqrt(1 - ret.w * ret.w) / (v.Length());
			ret.x = v[0] * p;
			ret.y = v[1] * p;
			ret.z = v[2] * p;
		}
		return ret;
	};

	// Creates a bond cylinder between the given points
	auto addBond = [&](const Vector3f &p0, const Vector3f &p1){
		Vector3f diff = p1 - p0;
		Quatf dir = direction(diff);
		Model *cyl = new Model(Prim_Triangles);
		cyl->AddCylinder(0.05, diff.Length() * 0.5f);
		cyl->SetPosition((p0 + p1) * 0.5f);
		cyl->SetOrientation(dir);
		cyl->Fill = fills.LitTextures[Tex_Checker];
		scene->World.Add(Ptr<Model>(*cyl));
	};

	// Add an atom at specified position in space
	auto add = [&](float x, float y, float z){
		Model *sphere = new Model(Prim_Triangles);
		sphere->AddSphere(float(0.5 * scale));
		sphere->SetPosition(Vector3f(x, y, z));
		sphere->Fill = fills.LitTextures[Tex_Checker];
		scene->World.Add(Ptr<Model>(*sphere));
	};

	// Add method in vector form
	auto addv = [&](const Vector3f &v){
		add(v.x, v.y, v.z);
	};

	int cells = structure == Diamond ? 6 : 3;

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
					if(drawAtom)
						add(ix, iy, iz);
					if(drawBond){
						if(-cells <= ix - 1)
							addBond(Vector3f(ix - 1, iy, iz), Vector3f(ix, iy, iz));
						if(-cells <= iy - 1)
							addBond(Vector3f(ix, iy - 1, iz), Vector3f(ix, iy, iz));
						if(-cells <= iz - 1)
							addBond(Vector3f(ix, iy, iz - 1), Vector3f(ix, iy, iz));
					}
					break;
				case FCC:
					{
						auto positioner = [&](int ix, int iy, int iz){
							float xmod = (ix + cells + iy + cells + iz + cells) % 2 * sqrt(0.5f);
							return Vector3f(ix * sqrt(0.5f) + xmod, iy * sqrt(0.5f), iz * sqrt(0.5f));
						};
						if(drawAtom)
							addv(positioner(ix, iy, iz));
						if(drawBond){
							if(-cells <= iy - 1)
								addBond(positioner(ix, iy - 1, iz), positioner(ix, iy, iz));
							if(-cells <= iz - 1)
								addBond(positioner(ix, iy, iz - 1), positioner(ix, iy, iz));
							if(-cells <= iz - 1 && -cells <= iy - 1)
								addBond(positioner(ix, iy - 1, iz - 1), positioner(ix, iy, iz));
							if (-cells <= iz - 1 && iy + 1 < cells)
								addBond(positioner(ix, iy + 1, iz - 1), positioner(ix, iy, iz));
						}
						break;
					}
				case BCC:
					{
						auto bccPositioner = [&](int ix, int iy, int iz){
							float ymod = (iy + cells) % 2 * 0.5f;
							return Vector3f(ix + ymod, float(iy * sqrt(1. / 2.)), iz + ymod);
						};
						if(drawAtom)
							addv(bccPositioner(ix, iy, iz));
						if (drawBond){
							if (-cells <= iy - 1){
								int ymod = (iy + cells) % 2;
//								if (-cells <= ix + ymod - 1 && ix + ymod - 1 < cells)
									addBond(bccPositioner(ix, iy - 1, iz), bccPositioner(ix, iy, iz));
								if (-cells <= ix + ymod && ix + ymod < cells)
									addBond(bccPositioner(ix + ymod, iy - 1, iz), bccPositioner(ix, iy, iz));
								if (-cells <= iz + ymod && iz + ymod < cells)
									addBond(bccPositioner(ix, iy - 1, iz + ymod), bccPositioner(ix, iy, iz));
								if (-cells <= ix + ymod && ix + ymod < cells && -cells <= iz + ymod && iz + ymod < cells)
									addBond(bccPositioner(ix + ymod, iy - 1, iz + ymod), bccPositioner(ix, iy, iz));
							}
						}
						break;
					}
				case Diamond:
					{
						int xmod = (ix + cells) % 2;
						int ymod = (iy + cells) % 2;
						int zmod = (iz + cells) % 2;
						int xyzmod = (ix + cells + iy + cells + iz + cells) % 4;
						float fh = sqrt(1.f / 3.f);
						if (drawAtom){
							if (xmod == ymod && ymod == zmod && (xyzmod == 0 || xyzmod == 1))
								add(ix * fh, iy * fh, iz * fh);
						}
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
