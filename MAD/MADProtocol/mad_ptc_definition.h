/**************************************************************************/
/*                         This file is part of:                          */
/*                      Marisa's Atelier of Danmaku                       */
/*                              2024/11/19                                */
/**************************************************************************/
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
#pragma once

#include "../MADBase/mad_base.h"

/// <summary>
/// 各个弹幕的数据储存结构体
/// </summary>
struct BulletInfo {
	float AliveTime;

	MADVector2DF OriginPos;
	MADVector2DF OriginDir;

	long long TeamMask;

	BulletInfo() {
		TeamMask = 1;
		AliveTime = 0.0f;
		OriginPos = MADVector2DF();
		OriginDir = MADVector2DF();
	}
	BulletInfo(const MADVector2DF& _pos, const MADVector2DF& _dir, const long long& _team_mask) {
		TeamMask = _team_mask;
		AliveTime = 0.0f;
		OriginPos = _pos;
		OriginDir = _dir;
	}
	BulletInfo(const BulletInfo& _parent) {
		AliveTime = _parent.AliveTime;
		OriginPos = _parent.OriginPos;
		OriginDir = _parent.OriginDir;
		TeamMask = _parent.TeamMask;
	}
};

/// <summary>
/// 刷新弹幕实例时传入的弹幕实例数据
/// </summary>
struct MADBulletFlushResData {
	float Position_X, Position_Y;
	float Dir_X, Dir_Y;

	MADBulletFlushResData(float _p_x, float _p_y, float _d_x, float _d_y) {
		Position_X = _p_x;
		Position_Y = _p_y;
		Dir_X = _d_x;
		Dir_Y = _d_y;
	}
	MADBulletFlushResData(const MADBulletFlushResData& _parent) {
		Position_X = _parent.Position_X;
		Position_Y = _parent.Position_Y;
		Dir_X = _parent.Dir_X;
		Dir_Y = _parent.Dir_Y;
	}
};

struct MADEntity {
	MADVector2DF Position;
	float TestRadius;
	long long  TeamMask;
	void** UserData;

	MADEntity() {
		Position = MADVector2DF();
		TestRadius = 0.0f;
		TeamMask = 0;
		UserData = nullptr;
	}
	MADEntity(MADVector2DF _Position, float _TestRadius, int _TeamCode) {
		Position = _Position;
		TestRadius = _TestRadius;
		TeamMask = _TeamCode;
		UserData = nullptr;
	}
	MADEntity(const MADEntity& _parent) {
		Position = _parent.Position;
		TestRadius = _parent.TestRadius;
		TeamMask = 0;
		UserData = _parent.UserData;
	}
};
