/**************************************************************************/
/*                         This file is part of:                          */
/*                      Marisa's Atelier of Danmaku                       */
/*                              2024/11/19                                */
/**************************************************************************/

#pragma once

struct MADVector2DF {
	float x = 0.0f;
	float y = 0.0f;
	MADVector2DF() {
		x = 0.0f;
		y = 0.0f;
	}
	MADVector2DF(float _x, float _y) {
		x = _x;
		y = _y;
	}
};

struct MADVector2DI {
	int x = 0;
	int y = 0;
};

struct MADVector3DF {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	MADVector3DF() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	MADVector3DF(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
};

struct MADVector3DI {
	int x = 0;
	int y = 0;
	int z = 0;
};
