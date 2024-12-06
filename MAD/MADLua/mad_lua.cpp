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

#include "mad_lua.h"

/**
 * 创建并初始化一个新的MADScript对象。
 * 该方法尝试将给定的脚本代码加载到Lua环境中。如果加载成功，则返回一个新的MADScript对象实例；
 * 否则，打印错误信息并返回nullptr。
 * (注意:该方法创建完脚本后不会执行,你需要手动执行一次脚本来初始化可能的全局变量等对象!)
 * 
 * @param _script 要加载和初始化的脚本代码
 * @return 加载成功返回新创建的MADScript对象指针；加载失败返回nullptr，并通过日志输出错误信息。
 */
MADScript* MADScript::CreateScript(const MADString& _script)
{
	/*Try to create script*/
	lua_State* l_LBuffer = luaL_newstate();
	int l_res = luaL_loadstring(l_LBuffer, _script.c_str());
	
	/*Return*/
	if (l_res == LUA_OK)
	{
		lua_close(l_LBuffer);
		MAD_LOG_INFO("Script loaded successfully.");
		return new MADScript(_script);
	}

	/*If failed*/
	MADString l_sb = "Script loaded failed.";
	l_sb.append("Error detail: \r\n");
	l_sb.append(lua_tostring(l_LBuffer,-1));

	lua_pop(l_LBuffer,1);
	lua_close(l_LBuffer);
	
	MAD_LOG_ERR(l_sb.c_str());
	return nullptr;
}

/**
 * MADScript类的私有构造函数，用于初始化脚本对象的基本成员。
 * 注意：此构造函数不负责加载脚本到Lua环境，请确保在调用前脚本已正确加载。
 *
 * @param _script 要保存的脚本代码引用
 */
MADScript::MADScript(const MADString& _script)
{
	ScriptText = _script;
	L = luaL_newstate();
	
	luaL_loadstring(L, ScriptText.c_str());
	luaL_openlibs(L);
	ScriptState = MADScriptState::Loaded;
}

/**
 * MADScript析构函数。
 * 在MADScript对象生命周期结束时，自动调用此函数以释放关联的Lua状态机资源。
 * 内部调用DeleteScript方法确保Lua环境正确清理。
 */
MADScript::~MADScript()
{
	DeleteScript();
}

/**
 * 获取当前脚本的文本内容。
 *
 * 如果脚本已经被删除（ScriptState为Deleted），则会记录一条警告日志，
 * 并返回一个空字符串。
 *
 * @return 脚本的文本内容，如果脚本已被删除，则返回空字符串。
 */
MADString MADScript::GetScriptText()
{
	if (ScriptState == MADScriptState::Deleted)
	{
		MAD_LOG_WARN("Try to get text from a script that had already deleted!");
		return "";
	}
	return  ScriptText;
}

/**
 * 直接运行当前MADScript对象中的Lua脚本。
 *
 * 此方法在当前Lua环境上执行脚本，不传入参数也不期待返回值。
 * 成功执行后，若脚本状态为Loaded，则更新其状态为Ready。
 * 如果尝试运行一个已删除的脚本，则会记录警告信息并返回。
 *
 * 注意：确保脚本已正确加载且未被标记为删除状态。
 *
 * @pre 脚本应已被成功加载（ScriptState为Loaded）且未被删除。
 */
void MADScript::RunDirectly()
{
	if (ScriptState == MADScriptState::Deleted)
	{
		MAD_LOG_WARN("Try to run a script that had already deleted!");
		return;
	}
	lua_pcall(L, 0, 0, 0);
	if (ScriptState == MADScriptState::Loaded)
	{
		ScriptState = MADScriptState::Ready;
	}
}

/**
 * 删除并清理MADScript对象所占用的资源。
 * 此方法将脚本状态标记为已删除，关闭Lua状态机，清空Lua状态机指针以及脚本文本内容。
 * 通常在脚本不再需要时调用，以确保资源得到正确释放。
 *
 * 注意：在对象生命周期结束时，析构函数会自动调用此方法，但也可以显式调用以立即释放资源。
 */
void MADScript::DeleteScript()
{
	ScriptState = MADScriptState::Deleted;
	lua_close(L);
	L = nullptr;
	ScriptText = "";
}

/**
 * 重新加载并初始化MADScript对象中的Lua脚本。
 * 在执行此操作前，确保已经删除了原有的脚本。如果尝试在未删除状态下重新加载，
 * 则会返回错误信息指出非法调用。
 *
 * @param _script 待重新加载的Lua脚本代码
 * @return 返回关于重载操作的详细调试信息对象
 *         - 成功时携带MAD_RESCODE_OK代码
 *         - 内存不足时携带MAD_RESCODE_MEM_OUT及错误详情
 *         - 语法错误时携带MAD_RESCODE_SYNTAX_ERROR及错误详情
 *         - 非法调用时直接返回默认构造的MADDebuggerInfo_HEAVY对象
 */
MADDebuggerInfo_HEAVY MADScript::ReloadScript(const MADString& _script)
{
	if (ScriptState != MADScriptState::Deleted)
	{
		MAD_LOG_ERR("Try to reload a script without delete.");
		return MADDebuggerInfo_HEAVY(MAD_RESCODE_ILLEGAL_CALL,"Try to reload a script without delete.");
	}
	
	/*Try to create script*/
	L = luaL_newstate();
	int l_res = luaL_loadstring(L, _script.c_str());
	
	/*Return if reloaded successfully*/
	if (l_res == LUA_OK)
	{
		luaL_openlibs(L);
		ScriptText = _script;
		ScriptState = MADScriptState::Loaded;
		MAD_LOG_INFO("Script reloaded successfully.");
		return MADDebuggerInfo_HEAVY(MAD_RESCODE_OK);
	}

	/*If failed*/
	MADString lErrorInfo = "Script loaded failed.";
	lErrorInfo.append("Error detail: \r\n");
	lErrorInfo.append(lua_tostring(L,-1));

	lua_pop(L,1);
	lua_close(L);
	
	MAD_LOG_ERR(lErrorInfo.c_str());
	L = nullptr;

	if (l_res == LUA_ERRMEM)
	{
		return MADDebuggerInfo_HEAVY(MAD_RESCODE_MEM_OUT,lErrorInfo);
	}
	if (l_res == LUA_ERRSYNTAX)
	{
		return MADDebuggerInfo_HEAVY(MAD_RESCODE_SYNTAX_ERROR,lErrorInfo);
	}
	
	return MADDebuggerInfo_HEAVY();
}

/**
 * 从Lua环境中获取指定名称的全局整数值。
 * 在调用此方法前，确保脚本状态为Ready，否则会返回错误或无效结果。
 * 如果值不存在、类型不匹配或脚本状态异常，将记录错误信息并通过日志输出。
 *
 * @param _valueName 要获取的全局变量的名称
 * @return 如果找到且为整数类型，则返回该全局变量的整数值；否则返回0。
 */
int MADScript::GetValueInteger(const char* _valueName)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to read value from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to read value from a script without init,please run it directly first!");
		}
		return 0;
	}

	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globle Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		return 0;
	}
	if (lua_isinteger(L, -1)) {
		int value = lua_tointeger(L, -1);
		lua_pop(L, 1);
		return value;
	} else {
		MADString lOutInfo = "Type mismatch: Value '";
		lOutInfo.append(_valueName);
		lOutInfo.append("' is not an integer value.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return 0;
	}
}

/**
 * 从Lua环境获取指定名称的全局double型变量值。
 *
 * 在脚本状态为Ready时，此方法尝试获取Lua环境中的全局变量 `_valueName` 对应的double值。
 * 如果变量不存在、类型不匹配或脚本状态异常，将返回0.0并记录错误信息。
 *
 * @param _valueName 要获取的全局变量的名称。
 * @return 变量存在且为double类型时返回其值；其他情况返回0.0。
 */
double MADScript::GetValueDouble(const char* _valueName)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to read value from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to read value from a script without init,please run it directly first!");
		}
		return 0.0;
	}

	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globle Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		return 0.0;
	}
	if (lua_isnumber(L, -1)) {
		double value = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return value;
	} else {
		MADString lOutInfo = "Type mismatch: Value '";
		lOutInfo.append(_valueName);
		lOutInfo.append("' is not an double value.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return 0.0;
	}
}

/**
 * 从Lua环境中获取指定全局变量的字符串值。
 *
 * 此方法首先检查脚本状态是否就绪。若脚本已被删除，则记录错误信息并返回空字符串；
 * 若脚本未初始化（仅加载），同样记录错误信息并返回空字符串。之后尝试从Lua全局表中获取
 * 给定名称的值。如果找到的值不是字符串类型或不存在，将记录相应的错误信息并返回空字符串。
 *
 * @param _valueName 要获取其值的全局变量名称。
 * @return 如果成功获取到字符串值，则返回该值；否则返回空字符串。
 */
MADString_c MADScript::GetValueString(const char* _valueName)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to read value from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to read value from a script without init,please run it directly first!");
		}
		return "";
	}

	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globle Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		return "";
	}
	if (lua_isstring(L, -1)) {
		const char* value = lua_tostring(L, -1);
		lua_pop(L, 1);
		return value;
	} else {
		MADString lOutInfo = "Type mismatch: Value '";
		lOutInfo.append(_valueName);
		lOutInfo.append("' is not an string value.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return "";
	}
}

/**
 * 尝试从Lua环境获取指定的布尔型全局变量值。
 *
 * 在调用此方法前，确保脚本状态为Ready，即已加载并初始化完毕。
 * 如果脚本被删除或未初始化，将返回false并记录错误信息。
 * 如果指定的全局变量不存在或非布尔类型，也会返回false并记录相应的错误信息。
 *
 * @param _valueName 要获取的布尔型全局变量的名称
 * @return 若获取成功，返回变量的布尔值；否则返回false。
 */
bool MADScript::GetValueBoolen(const char* _valueName)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to read value from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to read value from a script without init,please run it directly first!");
		}
		return false;
	}

	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globle Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		return false;
	}
	if (lua_isboolean(L, -1)) {
		bool value = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return value;
	} else {
		MADString lOutInfo = "Type mismatch: Value '";
		lOutInfo.append(_valueName);
		lOutInfo.append("' is not an bool value.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return false;
	}
}

/**
 * 从Lua环境中获取指定名称的全局用户数据指针。
 *
 * 此方法检查脚本状态是否就绪，若脚本已被删除或尚未初始化，则返回nullptr并记录错误信息。
 * 接着尝试从Lua全局变量中获取指定名称的值，如果该值存在且为轻量级用户数据（light userdata），
 * 则返回该用户数据指针；如果找不到该值或其类型不匹配，则记录错误并返回nullptr。
 *
 * @param _valueName 要获取的全局变量的名称。
 * @return 如果找到且类型匹配，返回对应的用户数据指针；否则返回nullptr。
 */
void* MADScript::GetValueUserPtr(const char* _valueName)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to read value from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to read value from a script without init,please run it directly first!");
		}
		return nullptr;
	}

	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globle Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		return nullptr;
	}
	if (lua_islightuserdata(L, -1)) {
		void* value = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return value;
	} else {
		MADString lOutInfo = "Type mismatch: Value '";
		lOutInfo.append(_valueName);
		lOutInfo.append("' is not an user data value.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return nullptr;
	}
}
