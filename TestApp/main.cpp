#include "../MAD/mad.h"

#include <iostream>
#include <chrono>
using namespace std;

void test_err_printer(const MADString& _str) {
	cout << "[MAD_TAPP_ERR]: " << _str << '\n';
}

void test_warn_printer(const MADString& _str) {
	cout << "[MAD_TAPP_WARN]: " << _str << '\n';
}

void test_info_printer(const MADString&  _str) {
	cout << "[MAD_TAPP_INFO]: " << _str << '\n';
}

int add(lua_State* _L)
{
	MAD_LOG_INFO("TEST-Register function ............OK");
	lua_pushfstring(_L, "[MAD_TAPP_INFO]: TEST-Call function ............OK");
	return 1;
}

#define TIME_POINT_START {auto start = std::chrono::high_resolution_clock::now();
#define TIME_POINT_END auto finish = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> elapsed = finish - start; MAD_LOG_INFO("TimePoint: " + to_string(elapsed.count()) + "s");}

int main() {
	/*Debugger testing*/
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Error, test_err_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Warning, test_warn_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Infomation, test_info_printer);

	/*Script testing*/
	MADScript* mad_script = MADScript::CreateScript("function a(b,c)\n\taaa = 1\n\tbbb = aaa + 10\nend");
	if (!mad_script)
		return 1;

	MADScriptDataStream arg_data = MADScriptDataStream();
	arg_data.push_back(MADScriptData(MADScriptValueType::String, new MADString("Hello args!")));
	arg_data.push_back(MADScriptData(MADScriptValueType::String, new MADString("yessssssssssssssssssssssssssssssssssssssssssss")));
	mad_script->RunDirectly();
	MADQCPack pack = mad_script->RegisterQuickCallPack("a",arg_data);
	const size_t allcount = 1000000;
	MAD_LOG_INFO("CallFunction");
	TIME_POINT_START
	for(size_t i = 0; i < allcount; i++)
	{
		mad_script->CallFunction("a",arg_data);
	}
	TIME_POINT_END
	MAD_LOG_INFO("QuickCallFunction");
	TIME_POINT_START
	for(size_t i = 0; i < allcount; i++)
	{
		mad_script->QuickCallFunction(pack);
	}
	TIME_POINT_END
	MAD_LOG_INFO("UnsafeFastCallFunction");
	TIME_POINT_START
	for(size_t i = 0; i < allcount; i++)
	{
		mad_script->UnsafeFastCallFunction("a");
	}
	TIME_POINT_END
	mad_script->UnregisterQuickCallPack(pack);
}