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
	bool drawAtom;
	bool drawBond;
	SceneBuilder() : structure(Cube), scale(0.5),
		drawAtom(true), drawBond(true){}

	void ToggleStructure();
	void ResizeAtom(double d);
	void ToggleDrawAtom();
	void ToggleDrawBond();
	void PopulateRoomScene(Scene* scene, RenderDevice* render);
};

extern SceneBuilder sbuilder;

#endif
