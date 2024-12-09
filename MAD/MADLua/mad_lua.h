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

#include <string>
#include <vector>

#include "../MADBase/mad_base.h"

enum class MADScriptState { Deleted, Loaded, Ready };

typedef int (*MADScriptCallbackCFunction)(lua_State*);

/**
 * \brief MADScriptValueType 枚举定义了MADScriptData结构体中可存储的数据类型。
 *
 * 枚举成员包括：
 * - Unknown: 未知类型,不要使用,除非代码发生错误
 * - Nil: 空值,可用于占位
 * - Boolean: 布尔值,bool
 * - LightUserdata: 轻量级用户数据,void*
 * - Number: 数值（浮点型）,double
 * - String: 字符串,特指MADString,MAD的API会自动完成const char*与MADString的转换,在暴露的API中,请放心的使用MADString进行任何必要的字符串操作.
 * - Integer: 整数值,long long
 *
 * 这些类型用于精确地表示Lua脚本中的数据类型，并在MADScriptData中进行引用。
 */
enum class MADScriptValueType { Unknown = -1, Nil = 0, Boolean = 1, LightUserdata = 2, Number = 3, String = 4 , Integer = 5};

/**
 * \brief MADScriptData 结构体用于封装 Lua 脚本中的变量值及其类型。
 *
 * 此结构体包含两个成员：一个用于存储数据的指针 `data` 和一个表示数据类型的枚举 `type`。
 * 提供了一个构造函数用于初始化数据类型和数据指针，支持默认构造为未知类型和空指针。
 *
 * \note
 * - 使用时确保正确管理 `data` 指向的内存。
 * - 枚举类型 `MADScriptValueType` 定义了支持的数据类型。
 */
struct MADScriptData
{
	void* data;
	MADScriptValueType type;

	MADScriptData(MADScriptValueType _type = MADScriptValueType::Unknown,void* _data = nullptr){
		type = _type;
		data = _data;
	}
};

typedef std::vector<MADScriptData> MADScriptDataStream;

/**
 * MADScript 类提供了管理 Lua 脚本的功能，包括加载、运行、删除脚本以及与 Lua 状态机交互的能力。
 * 用户应通过工厂方法创建实例，并利用提供的接口进行操作。
 * 
 * 注意:
 * -使用过程中请遵循RAII原则,尽量避免使用ReloadScript方法,除非您知道您在做什么!
 * -该类采取工厂模式,请通过CreateScript来创建该类,而不是直接创建。
 */
class MADScript
{
/*Create operator*/
public:
	static MADScript* CreateScript(const MADString& _script);
	~MADScript();

/*Factory method,Do NOT create it directly*/
private:
	MADScript(const MADString& _script);

/*User interface*/
public:
	/*Get Data*/
	MADString GetScriptText();
	MADScriptState GetScriptState();
	lua_State* GetLuaState();

	/*Init script*/
	void RunDirectly();
	void DeleteScript();
	MADDebuggerInfo_HEAVY ReloadScript(const MADString& _script);

	/*Get value*/
	long long GetValueInteger(const char* _valueName);
	double GetValueDouble(const char* _valueName);
	MADString GetValueString(const char* _valueName);
	bool GetValueBoolean(const char* _valueName);
	void* GetValueUserPtr(const char* _valueName);

	/*Set value*/
	void SetValueInteger(const char* _valueName,const long long& _value);
	void SetValueDouble(const char* _valueName,const double& _value);
	void SetValueString(const char* _valueName,const MADString& _value);
	void SetValueBoolean(const char* _valueName,const bool& _value);
	void SetValueUserPtr(const char* _valueName,void* _value);
	
	/*Function*/
	void RegisterCFunction(const char* _funcName,MADScriptCallbackCFunction _target);
	MADDebuggerInfo_LIGHT CallFunction(const char* _funcName,const MADScriptDataStream& _arg, MADScriptDataStream* out_ret = nullptr);

	/*Lua API Function*/
	static int CopyData(lua_State* L);
	
private:
	/*Script Data*/
	MADString ScriptText;
	MADScriptState ScriptState;
	
	/*Lua Data*/
	lua_State* L;

	/*Common function*/
	void InitLuaState();
};
