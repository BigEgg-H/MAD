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

/*Third Party Libs*/
#include "LuaSource/lua.hpp"

/*MAD APIs*/
#include "MADBase/mad_base.h"
#include "MADProtocol/mad_protocol.h"
#include "MADLua/mad_lua.h"

/*open all libs for user by using namespace MAD*/
namespace MAD {
	using namespace MAD::Base;
	using namespace MAD::MADProtocol;
	using namespace MAD::MADLuaAPI;
}