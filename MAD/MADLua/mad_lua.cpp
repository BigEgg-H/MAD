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
		MAD_LOG_ERR("Try to run a script that had already deleted!");
		return;
	}
	lua_pcall(L, 0, 0, NULL);
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
 *         
 * 注意:该方法不应该大量使用,该方法设计目的仅为方便调试以及内部回调.使用该类时,请严格遵循RAII设计模式
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
				MAD_LOG_ERR("Try to push a unknown value to call lua function: " + MADString(_funcName));
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
		MAD_LOG_ERR("Call function: " + MADString(_funcName) +" failed!");
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
				MAD_LOG_ERR("Unsupported return value type from Lua function: " + MADString(_funcName));
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
		MAD_LOG_ERR("Illegal call for copy function.This function need 2 arg to call.");
		lua_pop(L, lua_gettop(L));
		return 0;
	}
	if (!lua_islightuserdata(L,-2)){
		MAD_LOG_ERR("Illegal call for copy function.First arg is not a valid userdata ptr (light userdata in lua, also void* in c).");
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
		MAD_LOG_ERR("Unsupported value type for Lua copy function.Please copy boolean, number or string.");
		return 0;
	}

	lua_pop(L,2);
	
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
}

