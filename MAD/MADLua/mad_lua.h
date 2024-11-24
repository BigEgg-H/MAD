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

#include <map>
#include <functional>
#include <string>

#include "../MADBase/mad_base.h"
#include "../MADProtocol/mad_protocol.h"

class MadLua {
public:
	MadLua();
	~MadLua();

	const char* LoadScript(const char* content);
	bool IsValid();

	/*Common function*/
public:
	double GetValue_Number(const char* _valueName);
	long long GetValue_Interge(const char* _valueName);
	MADString_c GetValue_String(const char* _valueName);

	/*Bullet Functions*/
public:
	bool CheckFor_BulletGroup();
	MADBulletFlushResData FlushBulletInstance(const BulletInfo* _bullet);
	int CollisionTestForCurrentInstance(MADEntity& _target);

	void BindFunc_SpawnBullet(std::function<void(lua_State* _L)> _callback);

	/*For lua API*/
public:
	static int LuaCallbackFunc(lua_State* _L);

private:
	lua_State* LuaState;

	std::map<std::string, std::function<void(lua_State* _L)>> CallbackFuncs;

	const char* ScripetData;
	bool Is_Valid;
};

/// <summary>
/// 弹幕模组的描述结构体
/// </summary>
struct BulletTypeInfo {
	float LifeTime;
	MadLua* BulletScript;
	MADString_c TextureResourcePath;
	void** UserData;

	BulletTypeInfo() {
		LifeTime = 0.0f;
		BulletScript = nullptr;
		TextureResourcePath = "";
		UserData = nullptr;
	}
	BulletTypeInfo(const BulletTypeInfo& _parent) {
		LifeTime = _parent.LifeTime;
		BulletScript = _parent.BulletScript;
		TextureResourcePath = _parent.TextureResourcePath;
		UserData = _parent.UserData;
	}
};

//TODO: Finish lua API
class MADScript
{
public:
	MADScript* LoadScript(MADString _script);

private:
	MADScript(MADString _script);
	~MADScript();

private:
	/*Script Data*/
	MADString Script;

	/*Lua Data*/
	lua_State* L;
};



