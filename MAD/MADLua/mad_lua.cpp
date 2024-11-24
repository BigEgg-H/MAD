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

		MadLua::MadLua() {
			ScripetData = nullptr;
			Is_Valid = false;

			LuaState = luaL_newstate();
			luaL_openlibs(LuaState);
		}

		MadLua::~MadLua() {
			lua_close(LuaState);
		}

		/// <summary>
		/// 将脚本代码加载为MADLuaScript对象,并运行以初始化.一旦调用此方法则该对象实例不可再重新初始化,
		/// 如果编译错误,需要清理该对象并重新创建一个新的实例.
		/// </summary>
		/// <param name="content">脚本代码</param>
		/// <returns>返回编译错误信息.如果编译成功则返回空字符串:""</returns>
		const char* MadLua::LoadScript(const char* content) {
			ScripetData = content;
			int res = luaL_loadstring(LuaState, ScripetData);
			if (res == LUA_OK) {
				lua_pushlightuserdata(LuaState, this);
				lua_setglobal(LuaState, "MADSYS_CONTEXT_PTR");
				lua_register(LuaState, "MADCallback", MadLua::LuaCallbackFunc);
				if (lua_pcall(LuaState, 0, 0, 0) == LUA_OK) {
					Is_Valid = true;
					return "";
				}
				return lua_tostring(LuaState, -1);
			}
			if (res == LUA_ERRMEM) {
				MAD_LOG_ERR("MAD mem out!\r\n");
			}
			return lua_tostring(LuaState, -1);
		}

		/// <summary>
		/// 返回对象是否可用.
		/// (注意:代码编译成功即可用,但代码不一定符合MAD标准,
		/// 若要检查握手是否成功请调用CheckFor_BulletGroup()方法)
		/// </summary>
		/// <returns>对象是否可用</returns>
		bool MadLua::IsValid() {
			return Is_Valid;
		}

		/// <summary>
		/// 从脚本中读取Number类型(即double)的全局变量的值.
		/// 如果读取失败则会返回0.0,并打印错误.
		/// </summary>
		/// <param name="_valueName">全局变量名称</param>
		/// <returns>指定变量的值</returns>
		double MadLua::GetValue_Number(const char* _valueName) {
			if (!Is_Valid) {
				MAD_LOG_ERR("[MADLua]:Try to read a value from a null script!");
				return 0.0;
			}
			if (lua_getglobal(LuaState, _valueName) == LUA_TNONE) {
				MAD_LOG_ERR("[MADLua]:Can't find number value by name:");
				MAD_LOG_ERR(_valueName);
				return 0.0;
			}
			int res;
			double buffer = lua_tonumberx(LuaState, -1, &res);
			lua_pop(LuaState, 1);
			if (res) {
				return buffer;
			}
			MAD_LOG_ERR("[MADLua]:Get number value failed!Value name:");
			MAD_LOG_ERR(_valueName);
			return 0.0;
		}

		/// <summary>
		/// 从脚本中读取Int类型的全局变量的值.
		/// 如果读取失败则会返回0,并打印错误.
		/// </summary>
		/// <param name="_valueName">全局变量名称</param>
		/// <returns>指定变量的值</returns>
		long long MadLua::GetValue_Interge(const char* _valueName) {
			if (!Is_Valid) {
				MAD_LOG_ERR("[MADLua]:Try to read a value from a null script!");
				return 0;
			}
			if (lua_getglobal(LuaState, _valueName) == LUA_TNONE) {
				MAD_LOG_ERR("[MADLua]:Can't find number value by name:");
				MAD_LOG_ERR(_valueName);
				return 0;
			}
			int res;
			long long buffer = lua_tointegerx(LuaState, -1, &res);
			lua_pop(LuaState, 1);
			if (res) {
				return buffer;
			}
			MAD_LOG_ERR("[MADLua]:Get number value failed!Value name:");
			MAD_LOG_ERR(_valueName);
			return 0;
		}

		/// <summary>
		/// 从脚本中读取String类型(即const char*)的全局变量的值.
		/// 如果读取失败则会返回"",并打印错误.
		/// </summary>
		/// <param name="_valueName">全局变量名称</param>
		/// <returns>指定变量的值</returns>
		MADString_c MadLua::GetValue_String(const char* _valueName) {
			if (lua_getglobal(LuaState, _valueName) == LUA_TNONE) {
				MAD_LOG_ERR("[MADLua]:Can't find string value by name:");
				MAD_LOG_ERR(_valueName);
				return "";
			}
			const char* buffer = lua_tostring(LuaState, -1);
			lua_pop(LuaState, 1);
			return buffer;
		}

		/// <summary>
		/// 尝试与脚本握手,检查是否符合MAD规范.
		/// </summary>
		/// <returns>握手是否成功</returns>
		bool MadLua::CheckFor_BulletGroup() {
			return true;
		}

		/// <summary>
		/// 刷新指定的弹幕实例,调用脚本中的Flush方法,并将bullet信息传递给函数
		/// </summary>
		/// <param name="_bullet">要刷新的bullet的信息</param>
		/// <returns>刷新后,bullet更新过的状态</returns>
		MADBulletFlushResData MadLua::FlushBulletInstance(const BulletInfo* _bullet) {
			/*flush bullet*/
			lua_getglobal(LuaState, "Flush");
			lua_pushnumber(LuaState, _bullet->AliveTime);
			lua_pushnumber(LuaState, _bullet->OriginPos.x);
			lua_pushnumber(LuaState, _bullet->OriginPos.y);
			lua_pushnumber(LuaState, _bullet->OriginDir.x);
			lua_pushnumber(LuaState, _bullet->OriginDir.y);
			lua_pcall(LuaState, 5, 4, 0);
			float _x = lua_tonumber(LuaState, -4);
			float _y = lua_tonumber(LuaState, -3);
			float _x_dir = lua_tonumber(LuaState, -2);
			float _y_dir = lua_tonumber(LuaState, -1);
			lua_pop(LuaState, 4);

			/*update bullet data*/
			return MADBulletFlushResData(_x, _y, _x_dir, _y_dir);
		}

		/// <summary>
		/// 对刚刚刷新的弹幕进行判定检测.
		/// </summary>
		/// <param name="_target">要检测的实体</param>
		/// <returns>是否发生碰撞</returns>
		int MadLua::CollisionTestForCurrentInstance(MADEntity& _target) {
			lua_getglobal(LuaState, "CollisionTest");
			lua_pushnumber(LuaState, _target.Position.x);
			lua_pushnumber(LuaState, _target.Position.y);
			lua_pushnumber(LuaState, _target.TestRadius);
			lua_pcall(LuaState, 3, 1, 0);
			int _buffer = lua_tointeger(LuaState, -1);
			lua_pop(LuaState, 1);

			return _buffer;
		}

		void MadLua::BindFunc_SpawnBullet(std::function<void(lua_State* _L)> _callback) {
			CallbackFuncs["SpawnBullet"] = _callback;
		}

		/// <summary>
		/// LuaAPI回调函数.请不要主动调用该函数,除非您清楚您在干什么!
		/// </summary>
		/// <param name="_L">LuaAPI Ptr</param>
		/// <returns>返回值的数量</returns>
		int MadLua::LuaCallbackFunc(lua_State* _L) {
			int para_num = lua_gettop(_L);
			if (para_num < 2) {
				MAD_LOG_ERR("[MAD]:Callback illegality error!");
				return 0;
			}
			MadLua* lua_buffer = (MadLua*)lua_touserdata(_L, 1);
			const char* func_name = lua_tostring(_L, 2);
			lua_buffer->CallbackFuncs[func_name](_L);

			return 0;
		}
