#ifndef OCULUSTEST_H
#define OCULUSTEST_H

#include "RenderTiny_D3D11_Device.h"

enum CrystalStructure
{
	Cube,
//	FCC,
	BCC,
//	Diamond,
	Num_CrystalStructure
};

void PopulateRoomScene(Scene* scene, RenderDevice* render, CrystalStructure structure = Cube);
void ToggleStructure();

#endif
