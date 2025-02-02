/**************************************************************************/
/*                         This file is part of:                          */
/*                      Marisa's Atelier of Danmaku                       */
/*                              2024/11/19                                */
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
	InitLuaState();
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
 * 获取当前MADScript对象的脚本状态。
 *
 * 此方法返回脚本的当前状态，例如是否已被删除、已加载或准备就绪。
 *
 * @return 返回当前脚本的状态，其类型为MADScriptState枚举值。
 */
MADScriptState MADScript::GetScriptState()
{
	return ScriptState;
}

/**
 * 获取与MADScript对象关联的Lua状态机指针。
 * 如果脚本已被删除（ScriptState为Deleted），则此方法将记录错误信息并通过日志输出，
 * 并返回nullptr以指示无法获取Lua状态机。
 * 
 * 注意:
 * -危险操作,除非你知道你在做什么,否则不要使用这个方法。
 * -不要尝试通过指针改变lua状态,这可能会导致内部回调lua虚拟机时发生致命错误而崩溃。
 * 
 * @return 如果脚本有效，返回关联的lua_State指针；若脚本已被删除，则返回nullptr。
 */
lua_State* MADScript::GetLuaState()
{
	if (ScriptState == MADScriptState::Deleted)
	{
		MAD_LOG_ERR("Try to get script VM that had already deleted!");
		return nullptr;
	}
	return L;
}

/**
 * 直接运行当前MADScript对象中的脚本。
 * 根据脚本状态执行不同操作：
 * - 若脚本已被删除，则记录错误日志并返回。
 * - 若脚本已加载但未执行过（Ready状态），则在一个新的Lua虚拟机中运行脚本，以避免影响当前环境。
 * - 在其他情况下，尝试直接在当前Lua环境执行脚本，并处理可能的运行时错误。
 * 成功执行后，若脚本之前为Loaded状态，则更新其状态为Ready。
 *
 * 注意：此方法不适用于频繁调用执行的场景，特别是当ScriptState为Loaded时，
 *       因为它会为每次调用创建新的Lua虚拟机，这可能会导致性能问题。
 */
void MADScript::RunDirectly()
{
	if (ScriptState == MADScriptState::Deleted)
	{
		MAD_LOG_ERR("Try to run a script that had already deleted!");
		return;
	}
	if (ScriptState == MADScriptState::Ready)
	{
		MAD_LOG_WARN("Call RunDirectly on a Loaded script!This will do file in a isolated VM,instead of the current one.");
		{
			lua_State *L_copy = luaL_newstate();
			luaL_openlibs(L_copy);
			int error = luaL_dostring(L_copy, ScriptText.c_str());
			if (error) {
				MAD_LOG_ERR("[LuaScript]Script RunDirectly error: \"" + MADString(lua_tostring(L_copy, -1)) + "\"");
				lua_pop(L_copy, 1);
			}
			lua_close(L_copy);
		}
		return;
	}
	if (lua_pcall(L, 0, 0, NULL) != LUA_OK)
	{
		MAD_LOG_WARN("[LuaScript]Script runtime err caught in RunDirectly function.Lua Error: \"" + MADString(lua_tostring(L,-1)) + "\"");
		lua_pop(L,1);
	}
	if (ScriptState == MADScriptState::Loaded)
	{
		ScriptState = MADScriptState::Ready;
	}
}

/**
 * 调用Lua脚本中的'main'函数。
 * 此方法用于执行已准备就绪的脚本中定义的'main'函数，通常作为脚本入口点。
 * 在调用此方法之前，确保脚本状态为Ready，否则需要先使用RunDirectly初始化脚本。
 *
 * @throws 异常 不直接抛出异常，但会通过日志输出错误信息。
 *
 * 注意事项：
 * - 脚本必须包含一个名为'main'的全局函数作为执行入口。
 * - 如果脚本未准备好（即ScriptState不是Ready），此方法将记录错误并返回。
 * - 如果找不到'main'函数或在执行过程中遇到Lua运行时错误，将记录相应的警告或错误信息。
 */
void MADScript::CallMain()
{
	if (ScriptState != MADScriptState::Ready)
	{
		MAD_LOG_ERR("Try to call main on an unready script,please call RunDirectly to init first!");
		return;
	}
	lua_getglobal(L, "main");
	if (lua_isnil(L,-1))
	{
		MAD_LOG_ERR("Can't find function named 'main' as the entrance function!");
		lua_pop(L,1);
		return;
	}
	if (lua_pcall(L, 0, 0, NULL) != LUA_OK)
	{
		MAD_LOG_WARN("[LuaScript]Script runtime err caught in CallMain function.Lua Error: \"" + MADString(lua_tostring(L,-1)) + "\"");
		lua_pop(L,1);
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
 *         
 * 注意:该方法不应该大量使用,其设计的目的是方便且有目的地获得并保存编译加载代码时的调试信息.使用该类时,请严格遵循RAII设计模式!
 */
MADDebuggerInfo_HEAVY MADScript::ReloadScript(const MADString& _script)
{
	if (ScriptState != MADScriptState::Deleted)
	{
		MAD_LOG_ERR("Try to reload a script without delete.");
		return MADDebuggerInfo_HEAVY(MAD_RESCODE_ILLEGAL_CALL,"Try to reload a script without delete.");
	}

	MAD_LOG_WARN("Try to reload a script.Please follow the RAII design pattern!If you clear what you are doing please ignore this warning.");
	/*Try to create script*/
	L = luaL_newstate();
	int l_res = luaL_loadstring(L, _script.c_str());
	
	/*Return if reloaded successfully*/
	if (l_res == LUA_OK)
	{
		InitLuaState();
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
long long MADScript::GetValueInteger(const char* _valueName)
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
		MADString lOutInfo = "Can't find globe Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return 0;
	}
	if (lua_isinteger(L, -1)) {
		long long value = lua_tointeger(L, -1);
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
		MADString lOutInfo = "Can't find globe Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
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
MADString MADScript::GetValueString(const char* _valueName)
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
		MADString lOutInfo = "Can't find globe Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
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
bool MADScript::GetValueBoolean(const char* _valueName)
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
		MADString lOutInfo = "Can't find globe Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
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
		MADString lOutInfo = "Can't find globe Value named: '";
		lOutInfo.append(_valueName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
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

/**
 * 获取指定名称的Lua值的类型。
 * 此方法会检查Lua全局表中是否存在给定名称的值，并确定其类型。
 *
 * @param _valueName 要查询的Lua值的名称。
 * @return 返回_MADScriptValueType_枚举值，表示查找值的类型。如果未找到或值为nil，则返回MADScriptValueType::Nil。
 *
 * 注意：
 * - 此函数不会执行Lua代码，仅用于检查已存在的全局变量类型。
 * - 在调用此函数后，栈顶对应的值会被弹出，以保持lua_State的一致性。
 */
MADScriptValueType MADScript::GetValueType(const char* _valueName)
{
	lua_getglobal(L, _valueName);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return MADScriptValueType::Nil;//没找到值或值为nil
	}
	else if (lua_isboolean(L, -1))
	{
		lua_pop(L, 1);
		return MADScriptValueType::Boolean;
	}
	else if (lua_isnumber(L, -1))
	{
		lua_pop(L, 1);
		return lua_isinteger(L, -1) ? MADScriptValueType::Integer : MADScriptValueType::Number;
	}
	else if (lua_isstring(L, -1))
	{
		lua_pop(L, 1);
		return MADScriptValueType::String;
	}
	else if (lua_islightuserdata(L, -1))
	{
		lua_pop(L, 1);
		return MADScriptValueType::LightUserdata;
	}
	else
	{
		lua_pop(L, 1);
		return MADScriptValueType::Unknown;//MAD不支持的类型
	}
}

/**
 * 根据指定的值名称获取脚本中的数据。
 * 此方法首先确定值的类型，然后根据类型安全地分配内存并复制该值。
 * 如果尝试获取未定义类型的值，则会记录警告信息并返回一个类型为未知（Unknown）的MADScriptData结构。
 *
 * @param _valueName 要获取其值的变量或属性的名称
 * @return 返回一个包含实际值及其类型的MADScriptData结构。
 *         如果值不存在或类型无法识别，则返回的MADScriptData的data字段为nullptr，且type可能为Unknown或Nil。
 */
MADScriptData MADScript::GetValue(const char* _valueName)
{
	MADScriptValueType valueType = GetValueType(_valueName);
	void* value = nullptr;

	switch (valueType)
	{
	case MADScriptValueType::Integer:
		value = new long long(GetValueInteger(_valueName));
		break;
	case MADScriptValueType::Number:
		value = new double(GetValueDouble(_valueName));
		break;
	case MADScriptValueType::String:
		{
			MADString strValue = GetValueString(_valueName);
			value = new MADString(strValue);
			break;
		}
	case MADScriptValueType::Boolean:
		value = new bool(GetValueBoolean(_valueName));
		break;
	case MADScriptValueType::LightUserdata:
		value = GetValueUserPtr(_valueName);
		break;
	case MADScriptValueType::Unknown:
		MAD_LOG_WARN("Try to get a value of an undefined type in MAD,Value name: " + MADString(_valueName));
		value = nullptr;
		break;
	case MADScriptValueType::Nil:
		value = nullptr;
		break;
	}

	return {valueType, value};
}

/**
 * 设置Lua全局变量的整数值。
 *
 * 此方法将给定的整数值 `_value` 与对应的变量名 `_valueName` 设置为Lua环境中的全局变量。
 *
 * @param _valueName 全局变量的名称，必须是有效的Lua变量名。
 * @param _value 要设置的整数值。
 */
void MADScript::SetValueInteger(const char* _valueName, const long long& _value)
{
	lua_pushinteger(L,_value);
	lua_setglobal(L,_valueName);
}

/**
 * 设置全局Lua变量的双精度浮点数值。
 *
 * 此方法将给定的双精度数值压入Lua栈，然后以指定的名字设置为全局变量。
 * 如果Lua状态机L未初始化或发生错误，则可能会影响操作的成功执行。
 *
 * @param _valueName 要设置的全局变量的名称
 * @param _value 要赋给全局变量的双精度浮点数值
 */
void MADScript::SetValueDouble(const char* _valueName, const double& _value)
{
	lua_pushnumber(L,_value);
	lua_setglobal(L,_valueName);
}

/**
 * 设置Lua全局变量的字符串值。
 *
 * 此方法将指定的字符串值 `_value` 设置到Lua环境中的全局变量 `_valueName`。
 * 它首先将字符串推入Lua栈，然后设置为全局变量。
 *
 * @param _valueName 要设置的全局变量的名称（C字符串）
 * @param _value 要设置的字符串值
 */
void MADScript::SetValueString(const char* _valueName, const MADString& _value)
{
	lua_pushstring(L,_value.c_str());
	lua_setglobal(L,_valueName);
}

/**
 * 向Lua环境设置一个布尔型全局变量。
 *
 * 此方法将给定的布尔值以全局变量的形式存储在Lua环境中，
 * 变量名由 `_valueName` 指定。这用于在脚本中设置或修改布尔类型的值。
 *
 * @param _valueName 要设置的全局变量的名称（C字符串）。
 * @param _value 要设置的布尔值。
 */
void MADScript::SetValueBoolean(const char* _valueName, const bool& _value)
{
	lua_pushboolean(L,_value);
	lua_setglobal(L,_valueName);
}

/**
 * 向Lua环境设置一个用户数据类型的全局变量。
 *
 * 此方法将给定的C指针作为轻用户数据推入Lua堆栈，并使用指定的名称将其设为全局变量。
 * 这使得在Lua脚本中可以通过该名称访问这个C指针。
 *
 * @param _valueName 全局变量的名称，用于在Lua环境中标识这个用户数据。
 * @param _value 要设置的用户数据指针，可以是任何C指针。
 */
void MADScript::SetValueUserPtr(const char* _valueName, void* _value)
{
	lua_pushlightuserdata(L,_value);
	lua_setglobal(L,_valueName);
}

/**
 * 向Lua环境注册一个C语言回调函数。
 *
 * 此方法将指定的C语言回调函数注册到当前Lua环境的全局表中，使其可以通过 Lua 脚本中指定的函数名进行调用。
 *
 * @param _funcName 要在Lua环境中注册的函数名称。
 * @param _target 要注册的C语言回调函数指针，其原型应与`lua_CFunction`一致。
 *
 * 注意：
 * - 函数注册后，即可在Lua脚本中通过 `_funcName` 调用此C函数。
 * - 若 `_funcName` 已存在于Lua全局表中，此函数将会覆盖原有同名函数。
 * - 受限于Lua的API,只能注册C风格的函数来达到目的。
 */
void MADScript::RegisterCFunction(const char* _funcName, MADScriptCallbackCFunction _target)
{
	if (ScriptState == MADScriptState::Deleted)
	{
		MAD_LOG_ERR(MADString("Try to register function \"")+
			MADString(_funcName)+
			MADString("\" on a deleted script!"));
		return;
	}
	lua_pushcfunction(L, _target);
	lua_setglobal(L, _funcName);
}

/**
 * 调用Lua脚本中的指定全局函数。
 * 此方法负责查找函数、压入参数、调用函数、处理返回值并将结果（如有）存入输出数据流。
 *
 * @param _funcName 要调用的Lua全局函数名。
 * @param _arg 包含要传递给函数的参数的数据流。
 * @param[out] out_ret 一个指向MADScriptDataStream的指针，用于接收Lua函数的返回值（若有）。若不需要返回值，可传入nullptr。
 * @return 返回调用结果的状态码，MAD_RESCODE_OK表示成功，其他值表示错误（如函数未找到）。
 *
 * 注意：
 * - 函数会自动管理Lua堆栈，调用前后保持堆栈平衡。
 * - 若提供的函数名在Lua环境中不存在，会记录错误日志并返回MAD_RESCODE_FUNC_NOT_FOUND。
 * - 支持多种类型的参数与返回值转换，但受限于Lua API，整数参数会被转为双精度浮点数返回。
 */
MADDebuggerInfo_LIGHT MADScript::CallFunction(
	const char* _funcName, const MADScriptDataStream& _arg,MADScriptDataStream* out_ret)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to call function from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to call function from a script without init,please run it directly first!");
		}
		return MAD_RESCODE_ILLEGAL_CALL;
	}
	
	lua_getglobal(L, _funcName); // 获取全局函数
	if (lua_isnil(L, -1))
	{
		MADString lOutInfo = "Can't find globe function named: '";
		lOutInfo.append(_funcName);
		lOutInfo.append("'.");
		MAD_LOG_ERR(lOutInfo);
		lua_pop(L, 1);
		return MAD_RESCODE_FUNC_NOT_FOUND;
	}

	if (!_arg.empty())
	{
		for (const auto& data : _arg) {
			switch (data.type) {
			case MADScriptValueType::LightUserdata:
				lua_pushlightuserdata(L, data.data);
				break;
			case MADScriptValueType::Number:
				lua_pushnumber(L,*static_cast<double*>(data.data));
				break;
			case MADScriptValueType::Boolean:
				lua_pushboolean(L,*static_cast<bool*>(data.data));
				break;
			case MADScriptValueType::Integer:
				lua_pushinteger(L,*static_cast<long long*>(data.data));
				break;
			case MADScriptValueType::String:
				lua_pushstring(L, static_cast<MADString*>(data.data)->c_str());
				break;
			case MADScriptValueType::Unknown:
				MAD_LOG_ERR("Try to push a unknown value to call lua function: \"" + MADString(_funcName) +"\"");
				lua_pushnil(L);
				break;
			case MADScriptValueType::Nil:
				lua_pushnil(L);
				break;
			}
		}
	}	
	
	int res = lua_pcall(L, (int)_arg.size(), LUA_MULTRET, 0); // 调用函数，允许多返回值
	if (res != LUA_OK)
	{
		MAD_LOG_ERR("Call function: \"" + MADString(_funcName) +"\""+" failed!Lua error: \"" + MADString(lua_tostring(L,-1))+"\"");
		lua_pop(L,1);
		return MAD_RESCODE_FUNC_FAILED;
	}

	// 确定返回值数量
	int num_returns = lua_gettop(L);

	// 如果需要收集返回值，可以遍历堆栈并处理
	if (out_ret != nullptr) {
		for (int i = 0; i < num_returns; ++i) {
			// 读取并处理每个返回值，转换并存入out_ret
			MADScriptData retData;
			switch (lua_type(L, -num_returns + i)) {
			case LUA_TNIL:
				retData = MADScriptData(MADScriptValueType::Nil);
				break;
			case LUA_TBOOLEAN:
				retData = MADScriptData(MADScriptValueType::Boolean,new bool(lua_toboolean(L, -num_returns + i)));
				break;
			case LUA_TLIGHTUSERDATA:
				retData = MADScriptData(MADScriptValueType::LightUserdata,lua_touserdata(L, -num_returns + i));
				break;
			case LUA_TNUMBER:
				// Note: 受限于Lua的API,无法返回int类型参数,只能返回double类型参数
				retData = MADScriptData(MADScriptValueType::Number,new double(lua_tonumber(L, -num_returns + i)));
				break;
			case LUA_TSTRING:
				{
					const char* str = lua_tostring(L, -num_returns + i);
					retData = MADScriptData(MADScriptValueType::String,new MADString(str));
				}
				break;
			default:
				MAD_LOG_ERR("Unsupported return value type from Lua function: \"" + MADString(_funcName)+"\"");
				retData = MADScriptData();
			}
			out_ret->push_back(retData);
		}
	}

	// 清理堆栈和错误处理
	lua_pop(L, num_returns); 

	// 返回调试信息或状态
	return MAD_RESCODE_OK;
}

/**
 * 快速调用Lua函数。
 * 根据提供的QuickCallFuncPack参数，从Lua注册表中获取函数引用并依次推入参数，
 * 然后尝试调用该函数。如果调用失败，会记录错误信息并通过日志输出。
 *
 * @param _pack 包含函数引用名和参数引用的QuickCallFuncPack结构体指针
 */
void MADScript::QuickCallFunction(MADQuickCallPack _pack)
{
	if (!_pack)
		return;
	
	QuickCallFuncPack* pack = static_cast<QuickCallFuncPack*>(_pack);
	
	if (pack->Owner != L)
	{
		MAD_LOG_ERR("Try to run a quick call pack on a different script VM,pack func name: \"" + pack->refName+"\"");
		return;
	}
	
	lua_getfield(L,LUA_REGISTRYINDEX,pack->refName.c_str());
	for(const MADString& arg_ref : pack->args)
	{
		lua_getfield(L,LUA_REGISTRYINDEX,arg_ref.c_str());
	}
	if (lua_pcall(L, static_cast<int>(pack->args.size()), 0, 0) != LUA_OK) {
		MAD_LOG_ERR("Quick call failed,lua error: " + MADString(lua_tostring(L, -1)));
		lua_pop(L, 1);
	}
}

/**
 * 注册并创建一个快速调用函数包。
 * 但需要大量的调用无需返回值但有大批量参数的函数时,使用快速调用函数包可以获得少量的性能提升(主要体现在有大量参数要入栈时).
 * 此方法用于注册Lua中的指定函数，并为该函数预先设置一系列参数，
 * 以便后续快速调用此函数时无需重复推入参数。它会检查函数是否存在，
 * 并处理参数的类型与压栈，最后返回一个包含调用所需信息的QuickCallFuncPack对象。
 *
 * @param _funcName 要注册的Lua函数名称
 * @param _arg 一个MADScriptDataStream对象，包含预设的参数列表及其类型
 * @return 若函数注册成功，则返回一个指向新创建的QuickCallFuncPack结构体的指针；
 *         若函数不存在或过程中发生错误，则返回nullptr，并通过MAD_LOG_ERR输出错误信息。
 */
MADQuickCallPack MADScript::RegisterQuickCallPack(const MADString& _funcName,
                                           const MADScriptDataStream& _arg)
{
	if (ScriptState != MADScriptState::Ready)
	{
		if (ScriptState == MADScriptState::Deleted)
		{
			MAD_LOG_ERR("Attempt to register quick call pack from a deleted Script!");
		}
		if (ScriptState == MADScriptState::Loaded)
		{
			MAD_LOG_ERR("Attempt to register quick call pack from a script without init,please run it directly first!");
		}
		return nullptr;
	}
	
	lua_getglobal(L, _funcName.c_str());
	MADString func_ref = "MAD" + _funcName;
	if (!lua_isfunction(L, -1)) {
		MAD_LOG_ERR("Can't find function: \"" + _funcName + "\" to register quick call pack!");
		lua_pop(L, 1);
		return nullptr;
	} else {
		lua_setfield(L, LUA_REGISTRYINDEX, func_ref.c_str());
	}
	
	QuickCallFuncPack* _pack_buffer = new QuickCallFuncPack();
	_pack_buffer->Owner = L;
	_pack_buffer->refName = func_ref;
	
	MADString arg_ref_head = "MAD" + _funcName + "ARG";
	if (!_arg.empty())
	{
		for (const auto& data : _arg) {
			switch (data.type) {
			case MADScriptValueType::LightUserdata:
				lua_pushlightuserdata(L, data.data);
				break;
			case MADScriptValueType::Number:
				lua_pushnumber(L,*static_cast<double*>(data.data));
				break;
			case MADScriptValueType::Boolean:
				lua_pushboolean(L,*static_cast<bool*>(data.data));
				break;
			case MADScriptValueType::Integer:
				lua_pushinteger(L,*static_cast<long long*>(data.data));
				break;
			case MADScriptValueType::String:
				lua_pushstring(L, static_cast<MADString*>(data.data)->c_str());
				break;
			case MADScriptValueType::Unknown:
				MAD_LOG_ERR("Try to push a unknown value to register a quick call pack from lua function: \"" + MADString(_funcName)+"\"");
				lua_pushnil(L);
				break;
			case MADScriptValueType::Nil:
				lua_pushnil(L);
				break;
			}
			MADString arg_ref_str = arg_ref_head + std::to_string(_pack_buffer->args.size());
			lua_setfield(L, LUA_REGISTRYINDEX, arg_ref_str.c_str());
			_pack_buffer->args.emplace_back(arg_ref_str);
		}
	}	
	
	return _pack_buffer;
}

/**
 * 取消注册快速调用函数包。
 * 该方法会从Lua注册表中移除与指定函数包关联的所有引用，并释放函数包所占用的资源。
 *
 * @param _pack 指向要取消注册的QuickCallFuncPack对象的指针。
 */
void MADScript::UnregisterQuickCallPack(MADQuickCallPack _pack)
{
	if (!_pack) return;

	QuickCallFuncPack* pack = static_cast<QuickCallFuncPack*>(_pack);
	
	lua_pushnil(L);
	lua_setfield(L, LUA_REGISTRYINDEX, pack->refName.c_str());
	
	for(const MADString& arg_ref : pack->args)
	{
		lua_pushnil(L);
		lua_setfield(L, LUA_REGISTRYINDEX, arg_ref.c_str());
	}

	delete pack;
}

/**
 * 在Lua环境中不安全地快速调用指定的全局函数。
 *
 * 此方法直接从Lua状态机中获取全局函数并执行，不进行任何参数检查或错误处理。
 * 因此，调用者需确保函数存在且调用时的Lua栈状态正确。若函数不存在或执行出错，
 * Lua虚拟机的状态可能会受到影响，但此方法本身不会报告错误或抛出异常。
 *
 * 注意:
 * - 当您确定函数_funcName存在并且无需参数和返回值的支持时,使用该函数能最大程度的获取性能支持.
 * - 不正确地使用会导致Lua虚拟机的崩溃甚至于程序的崩溃.
 *
 * @param _funcName 要调用的Lua全局函数的名称。
 */
void MADScript::UnsafeFastCallFunction(const char* _funcName) const
{
	lua_getglobal(L, _funcName);
	lua_pcall(L, 0, 0, 0);
}

/**
 * (内部回调函数,禁止主动调用)
 * 在Lua环境中复制数据到轻量级用户数据指针中。
 * 此方法从Lua堆栈中获取两个参数：第一个是轻量级用户数据指针（void*），第二个是要复制的值。
 * 支持复制的Lua值类型有布尔型、数值型（以double表示）、字符串型。
 *
 * 注意：
 * - 确保调用此函数时Lua堆栈顶有且仅有两个参数。
 * - 第一个参数必须是轻量级用户数据（lightuserdata）。
 * - 该函数用于内部回调,任何时候都不应该主动调用该函数!
 * - 受限于LuaAPI,只支持布尔型(bool)、数值型（double）、字符串(MADString)型三种类型的值可以通过复制到指针的方式与脚本通信!
 *
 * @param L 当前的Lua状态机指针。
 * @return 返回0，用于Lua的内部处理。不直接表示成功或失败，需根据日志判断。
 */
int MADScript::CopyData(lua_State* L)
{
	if (lua_gettop(L) < 2){
		MAD_LOG_ERR("[LuaScript]Illegal call for copy function.CopyData function need 2 arg to call.");
		lua_pop(L, lua_gettop(L));
		return 0;
	}
	if (!lua_islightuserdata(L,-2)){
		MAD_LOG_ERR("[LuaScript]Illegal call for copy function.First arg is not a valid userdata ptr (light userdata in lua, also void* in c).");
		lua_pop(L,2);		
		return 0;
	}
	switch (lua_type(L, -1)){
	case LUA_TNUMBER:
		*static_cast<double*>(lua_touserdata(L,-2)) = lua_tonumber(L, -1);
		break;
	case LUA_TBOOLEAN:
		*static_cast<bool*>(lua_touserdata(L,-2)) = lua_toboolean(L, -1);
		break;
	case LUA_TSTRING:
		*static_cast<MADString*>(lua_touserdata(L,-2)) = MADString(lua_tostring(L, -1));
		break;
	default:
		MAD_LOG_ERR("[LuaScript]Unsupported value type for Lua copy function.Please copy boolean, number or string.");
		return 0;
	}

	lua_pop(L,2);
	
	return 0;
}

/**
 * (内部回调函数,禁止主动调用)
 * 将lua栈中的数字参数复制到一个double类型的数组中。
 * 此方法假设第一个参数是一个轻质用户数据指针，指向一个double数组的起始位置。
 * 接下来的参数（最多至lua栈顶）应为数字，这些数字将按顺序复制到数组中。
 *
 * @param L Lua状态机指针
 * @return 无返回值。若调用不合法，会通过日志输出错误信息并清理lua栈。
 *
 * 注意：
 * - 至少需要2个参数：一个轻质用户数据指针和至少一个数字。
 * - 第一个参数必须是轻质用户数据类型，指向double数组。
 * - 数组必须足够大以容纳所有提供的数字参数。
 */
int MADScript::CopyNumberToArray(lua_State* L)
{
	if (lua_gettop(L) < 2){
		MAD_LOG_ERR("[LuaScript]Illegal call for copy function.CopyNumberToArray function need 2 arg at least to call.");
		lua_pop(L, lua_gettop(L));
		return 0;
	}
	if (!lua_islightuserdata(L,1)){
		MAD_LOG_ERR("[LuaScript]Illegal call for copy function.First arg is not a valid userdata ptr (light userdata in lua, also void* in c).");
		lua_pop(L, lua_gettop(L));		
		return 0;
	}
	
	double* target_ptr = static_cast<double*>(lua_touserdata(L,1));
	int numArgs = lua_gettop(L) - 1;
	for (int i = 0;i < numArgs; ++i)
	{
		*target_ptr = lua_tonumber(L,i + 2);
		target_ptr++;
	}
	
	return 0;
}

/**
 * (内部回调函数,禁止主动调用)
 * 初始化Lua状态机，注册基础库并添加自定义函数。
 * 此方法会在MADScript对象创建时被调用，用于准备Lua环境以便执行脚本。
 * 它首先通过luaL_openlibs打开所有默认的Lua库，然后注册一个名为"CopyData"的C函数到Lua环境中。
 *
 * 注意：该方法会在luaL_loadstring成功加载脚本之后被调用,请勿主动调用此函数。
 */
void MADScript::InitLuaState()
{
	luaL_openlibs(L);
	lua_register(L, "CopyData", CopyData);
	lua_register(L, "CopyNumberToArray", CopyNumberToArray);
}

