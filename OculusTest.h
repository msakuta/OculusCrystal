#ifndef OCULUSTEST_H
#define OCULUSTEST_H

#include "RenderTiny_D3D11_Device.h"

enum CrystalStructure
{
	Cube,
	FCC, // Face Centered Cubic
	BCC, // Body Centered Cubic
//	Diamond,
	Num_CrystalStructure
};

void PopulateRoomScene(Scene* scene, RenderDevice* render, CrystalStructure structure = Cube);
void ToggleStructure();

#endif
