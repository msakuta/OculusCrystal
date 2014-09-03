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

//-------------------------------------------------------------------------------------
// ***** Room Model

// This model is hard-coded out of axis-aligned solid-colored slabs.
// Room unit dimensions are in meters. Player starts in the middle.
//

enum BuiltinTexture
{
    Tex_None,
    Tex_Checker,
    Tex_Block,
    Tex_Panel,
    Tex_Count
};

struct Slab
{
    float x1, y1, z1;
    float x2, y2, z2;
    Color c;
};

struct SlabModel
{
    int   Count;
    Slab* pSlabs;
    BuiltinTexture tex;
};

Slab FloorSlabs[] =
{
    // Floor
    { -10.0f,  -0.1f,  -20.0f,  10.0f,  0.0f, 20.1f,  Color(128,128,128) }
};

SlabModel Floor = {sizeof(FloorSlabs)/sizeof(Slab), FloorSlabs, Tex_Checker};


Slab CeilingSlabs[] =
{
    { -10.0f,  4.0f,  -20.0f,  10.0f,  4.1f, 20.1f,  Color(128,128,128) }
};

SlabModel Ceiling = {sizeof(FloorSlabs)/sizeof(Slab), CeilingSlabs, Tex_Panel};

Slab RoomSlabs[] =
{
    // Left Wall
    { -10.1f,   0.0f,  -20.0f, -10.0f,  4.0f, 20.0f,  Color(128,128,128) },
    // Back Wall
    { -10.0f,  -0.1f,  -20.1f,  10.0f,  4.0f, -20.0f, Color(128,128,128) },

    // Right Wall
    {  10.0f,  -0.1f,  -20.0f,  10.1f,  4.0f, 20.0f,  Color(128,128,128) },
};

SlabModel Room = {sizeof(RoomSlabs)/sizeof(Slab), RoomSlabs, Tex_Block};

const float doorWidth = 0.75f;
const float doorHeight = 2.5f;
const float ceilHeight = 4.f;
const float wallThick = 0.05f;
const float centerWallZ = -3.f;
const float centerDoorZ = 5.f;

static Slab WallSlabs[] =
{
	// Center wall
	{ -5.f + doorWidth, 0.0f, centerWallZ - wallThick, 5.0f - 0.5f, ceilHeight, centerWallZ + wallThick, Color(128, 128, 128) },
	// Left Wall
	{ -10.0f, 0.0f, centerWallZ - wallThick, -5.0f - doorWidth, ceilHeight, centerWallZ + wallThick, Color(128, 128, 128) },
	// Right Wall
	{ 5.0f + 0.5f, 0.0f, centerWallZ - wallThick, 10.0f, ceilHeight, centerWallZ + wallThick, Color(128, 128, 128) },
	// Left door top
	{ -5.f - doorWidth, doorHeight, centerWallZ - wallThick, -5.0f + doorWidth, ceilHeight, centerWallZ + wallThick, Color(128, 128, 128) },
	// Right door top
	{ 5.f - doorWidth, doorHeight, centerWallZ - wallThick, 5.0f + doorWidth, ceilHeight, centerWallZ + wallThick, Color(128, 128, 128) },
	// Front to Back Wall (near back)
	{ -wallThick, 0.0f, centerWallZ, wallThick, ceilHeight, centerDoorZ - doorWidth, Color(128, 128, 128) },
	// Front to Back Wall (near front)
	{ -wallThick, 0.0f, centerDoorZ + doorWidth, wallThick, ceilHeight, 20.0f, Color(128, 128, 128) },
	// Front to Back Wall (near front)
	{ -wallThick, doorHeight, centerDoorZ - doorWidth, wallThick, ceilHeight, centerDoorZ + doorWidth, Color(128, 128, 128) },
};

static SlabModel Wall = { sizeof(WallSlabs) / sizeof(Slab), WallSlabs, Tex_Block };

Slab FixtureSlabs[] =
{
    // Right side shelf
    {   9.5f,   0.75f,  3.0f,  10.1f,  2.5f,   3.1f,  Color(128,128,128) }, // Verticals
    {   9.5f,   0.95f,  3.7f,  10.1f,  2.75f,  3.8f,  Color(128,128,128) },
    {   9.5f,   1.20f,  2.5f,  10.1f,  1.30f,  3.8f,  Color(128,128,128) }, // Horizontals
    {   9.5f,   2.00f,  3.0f,  10.1f,  2.10f,  4.2f,  Color(128,128,128) },

    // Right railing    
    {   5.0f,   1.1f,   20.0f,  10.0f,  1.2f,  20.1f, Color(128,128,128) },
    // Bars
    {   9.0f,   1.1f,   20.0f,   9.1f,  0.0f,  20.1f, Color(128,128,128) },
    {   8.0f,   1.1f,   20.0f,   8.1f,  0.0f,  20.1f, Color(128,128,128) },
    {   7.0f,   1.1f,   20.0f,   7.1f,  0.0f,  20.1f, Color(128,128,128) },
    {   6.0f,   1.1f,   20.0f,   6.1f,  0.0f,  20.1f, Color(128,128,128) },
    {   5.0f,   1.1f,   20.0f,   5.1f,  0.0f,  20.1f, Color(128,128,128) },

    // Left railing    
    {  -10.0f,   1.1f, 20.0f,   -5.0f,   1.2f, 20.1f, Color(128,128,128) },
    // Bars
    {  -9.0f,   1.1f,   20.0f,  -9.1f,  0.0f,  20.1f, Color(128,128,128) },
    {  -8.0f,   1.1f,   20.0f,  -8.1f,  0.0f,  20.1f, Color(128,128,128) },
    {  -7.0f,   1.1f,   20.0f,  -7.1f,  0.0f,  20.1f, Color(128,128,128) },
    {  -6.0f,   1.1f,   20.0f,  -6.1f,  0.0f,  20.1f, Color(128,128,128) },
    {  -5.0f,   1.1f,   20.0f,  -5.1f,  0.0f,  20.1f, Color(128,128,128) },

    // Bottom Floor 2
    { -15.0f,  -6.1f,   18.0f,  15.0f, -6.0f, 30.0f,  Color(128,128,128) },
};

SlabModel Fixtures = {sizeof(FixtureSlabs)/sizeof(Slab), FixtureSlabs};

Slab FurnitureSlabs[] =
{
    // Table
    {  -1.8f, 0.7f, 1.0f,  0.0f,      0.8f, 0.0f,      Color(128,128,88) },
    {  -1.8f, 0.7f, 0.0f, -1.8f+0.1f, 0.0f, 0.0f+0.1f, Color(128,128,88) }, // Leg 1
    {  -1.8f, 0.7f, 1.0f, -1.8f+0.1f, 0.0f, 1.0f-0.1f, Color(128,128,88) }, // Leg 2
    {   0.0f, 0.7f, 1.0f,  0.0f-0.1f, 0.0f, 1.0f-0.1f, Color(128,128,88) }, // Leg 2
    {   0.0f, 0.7f, 0.0f,  0.0f-0.1f, 0.0f, 0.0f+0.1f, Color(128,128,88) }, // Leg 2

    // Chair
    {  -1.4f, 0.5f, -1.1f, -0.8f,       0.55f, -0.5f,       Color(88,88,128) }, // Set
    {  -1.4f, 1.0f, -1.1f, -1.4f+0.06f, 0.0f,  -1.1f+0.06f, Color(88,88,128) }, // Leg 1
    {  -1.4f, 0.5f, -0.5f, -1.4f+0.06f, 0.0f,  -0.5f-0.06f, Color(88,88,128) }, // Leg 2
    {  -0.8f, 0.5f, -0.5f, -0.8f-0.06f, 0.0f,  -0.5f-0.06f, Color(88,88,128) }, // Leg 2
    {  -0.8f, 1.0f, -1.1f, -0.8f-0.06f, 0.0f,  -1.1f+0.06f, Color(88,88,128) }, // Leg 2
    {  -1.4f, 0.97f,-1.05f,-0.8f,       0.92f, -1.10f,      Color(88,88,128) }, // Back high bar
};

SlabModel Furniture = {sizeof(FurnitureSlabs)/sizeof(Slab), FurnitureSlabs};

Slab PostsSlabs[] = 
{
    // Posts
    {  0,  0.0f, 0.0f,   0.1f, 1.3f, 0.1f, Color(128,128,128) },
    {  0,  0.0f, 0.4f,   0.1f, 1.3f, 0.5f, Color(128,128,128) },
    {  0,  0.0f, 0.8f,   0.1f, 1.3f, 0.9f, Color(128,128,128) },
    {  0,  0.0f, 1.2f,   0.1f, 1.3f, 1.3f, Color(128,128,128) },
    {  0,  0.0f, 1.6f,   0.1f, 1.3f, 1.7f, Color(128,128,128) },
    {  0,  0.0f, 2.0f,   0.1f, 1.3f, 2.1f, Color(128,128,128) },
    {  0,  0.0f, 2.4f,   0.1f, 1.3f, 2.5f, Color(128,128,128) },
    {  0,  0.0f, 2.8f,   0.1f, 1.3f, 2.9f, Color(128,128,128) },
    {  0,  0.0f, 3.2f,   0.1f, 1.3f, 3.3f, Color(128,128,128) },
    {  0,  0.0f, 3.6f,   0.1f, 1.3f, 3.7f, Color(128,128,128) },
};

SlabModel Posts = {sizeof(PostsSlabs)/sizeof(Slab), PostsSlabs};


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



// Helper function to create a model out of Slab arrays.
Model* CreateModel(Vector3f pos, SlabModel* sm, const FillCollection& fills)
{
    Model* m = new Model(Prim_Triangles);
    m->SetPosition(pos);

    for(int i=0; i< sm->Count; i++)
    {
        Slab &s = sm->pSlabs[i];
        m->AddSolidColorBox(s.x1, s.y1, s.z1, s.x2, s.y2, s.z2, s.c);
    }

    if (sm->tex > 0)
        m->Fill = fills.LitTextures[sm->tex];
    else
        m->Fill = fills.LitSolid;
    return m;
}


// Adds sample models and lights to the argument scene.
void SceneBuilder::PopulateRoomScene(Scene* scene, RenderDevice* render)
{
	FillCollection fills(render);

	scene->World.Clear();

//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 0), &Room, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0,0,0),  &Wall,       fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 0), &Floor, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 0), &Ceiling, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 0), &Fixtures, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 0), &Furniture, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0, 0, 4), &Furniture, fills)));
//	scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(-3, 0, 3), &Posts, fills)));

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


// Render a debug marker static in rift (special request for eye-tracking)
void renderSphere(RenderDevice* render, Vector3f ViewAdjust, float metresLeft, float metresUp, float metresAway, float metresRadius,
				unsigned char red,unsigned char green,unsigned char blue)
{
	//Get textures, if haven't already
	static FillCollection * pfills;  
	static bool firstTime = true;
	if (firstTime)
	{
		firstTime=false;
		pfills = new FillCollection(render);
	}

	//Create object
	Scene*  scene = new Scene;
	Slab CubeSlabs[] =
	{
	#if 0 //Simple cube
		 { metresLeft-metresRadius,  metresUp-metresRadius, metresAway-metresRadius,
		   metresLeft+metresRadius,  metresUp+metresRadius, metresAway+metresRadius,  Color(red,green,blue) }
	#else //Blob
		 { metresLeft-0.33f*metresRadius,  metresUp-metresRadius, metresAway-0.33f*metresRadius,
		   metresLeft+0.33f*metresRadius,  metresUp+metresRadius, metresAway+0.33f*metresRadius,  Color(red,green,blue) },
		 { metresLeft-metresRadius,  metresUp-0.33f*metresRadius, metresAway-0.33f*metresRadius,
		   metresLeft+metresRadius,  metresUp+0.33f*metresRadius, metresAway+0.33f*metresRadius,  Color(red,green,blue) },
		 { metresLeft-0.33f*metresRadius,  metresUp-0.33f*metresRadius, metresAway-metresRadius,
		   metresLeft+0.33f*metresRadius,  metresUp+0.33f*metresRadius, metresAway+metresRadius,  Color(red,green,blue) },
		 { metresLeft-0.71f*metresRadius,  metresUp-0.71f*metresRadius, metresAway-0.71f*metresRadius,
		   metresLeft+0.71f*metresRadius,  metresUp+0.71f*metresRadius, metresAway+0.71f*metresRadius,  Color(red,green,blue) },

	#endif

	};
	SlabModel Cube = {sizeof(CubeSlabs)/sizeof(Slab), CubeSlabs, Tex_None};
    scene->World.Add(Ptr<Model>(*CreateModel(Vector3f(0,0,0),  &Cube,  *pfills)));
    scene->SetAmbient(Vector4f(1.0f,1.0f,1.0f,1));

	//Render object
    Matrix4f view = Matrix4f::LookAtRH(Vector3f(0,0,0), Vector3f(0,0,0) + Vector3f(0,0,1), Vector3f(0,1,0)); 
	scene->Render(render, Matrix4f::Translation(ViewAdjust) * view);

	//Delete object
	delete scene;
}

