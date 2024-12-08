#include "../MAD/mad.h"

#include <iostream>
using namespace std;

void test_err_printer(MADString _str) {
	cout << "[MAD_TAPP_ERR]: " << _str << '\n';
}

void test_warn_printer(MADString _str) {
	cout << "[MAD_TAPP_WARN]: " << _str << '\n';
}

void test_info_printer(MADString _str) {
	cout << "[MAD_TAPP_INFO]: " << _str << '\n';
}

int add(lua_State* _L)
{
	MAD_LOG_INFO("OK");
	lua_pushfstring(_L, "Hello MAD!");
	return 1;
}

int main() {
	/*Debugger testing*/
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Error, test_err_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Warning, test_warn_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Infomation, test_info_printer);

	MADScript* mad_script = MADScript::CreateScript("add()");
	mad_script->RegisterCFunction("add",add);
	mad_script->RunDirectly();
	MADScriptDataStream args = MADScriptDataStream();
	MADScriptDataStream out = MADScriptDataStream();
	mad_script->CallFunction("add",args,&out);
	if (out[0].type == MADScriptValueType::String)
	{
		MAD_LOG_INFO(static_cast<const char*>(out[0].data));
	}
	
}