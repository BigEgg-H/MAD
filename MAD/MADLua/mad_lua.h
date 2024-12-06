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

#include <functional>
#include <string>

#include "../MADBase/mad_base.h"

enum class MADScriptState { Deleted, Loaded, Ready };

class MADScript
{
/*Object operator*/
public:
	static MADScript* CreateScript(const MADString& _script);
	~MADScript();

/*Factory method,Do NOT create it directly*/
private:
	MADScript(const MADString& _script);

/*User interface*/
public:
	MADString GetScriptText();
	
	void RunDirectly();
	void DeleteScript();
	MADDebuggerInfo_HEAVY ReloadScript(const MADString& _script);

	int GetValueInteger(const char* _valueName);
	double GetValueDouble(const char* _valueName);
	MADString_c GetValueString(const char* _valueName);
	bool GetValueBoolen(const char* _valueName);
	void* GetValueUserPtr(const char* _valueName);
	
private:
	/*Script Data*/
	MADString ScriptText;
	MADScriptState ScriptState;
	
	/*Lua Data*/
	lua_State* L;
};
