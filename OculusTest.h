#ifndef OCULUSTEST_H
#define OCULUSTEST_H

#include "RenderTiny_D3D11_Device.h"

enum CrystalStructure
{
	Cube,
	FCC, // Face Centered Cubic
	BCC, // Body Centered Cubic
	Diamond,
	Num_CrystalStructure
};

/// Parameters to PopulateRoomScene()
struct SceneBuilder{
	CrystalStructure structure;
	double scale;
	SceneBuilder() : structure(Cube), scale(1.){}

	void ToggleStructure();
	void ResizeAtom(double d);
	void PopulateRoomScene(Scene* scene, RenderDevice* render);
};

extern SceneBuilder sbuilder;

#endif
