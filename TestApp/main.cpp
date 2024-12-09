#include "../MAD/mad.h"

#include <iostream>
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

int main() {
	/*Debugger testing*/
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Error, test_err_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Warning, test_warn_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Infomation, test_info_printer);

	/*Script testing*/
	MADScript* mad_script = MADScript::CreateScript("print(add())\nprint(str)\nCopyData(test_buffer,true)");
	mad_script->RegisterCFunction("add",add);
	mad_script->SetValueString("str", "[MAD_TAPP_INFO]: TEST-Set globe value ............OK");
	bool* test_buffer = new bool(false);
	mad_script->SetValueUserPtr("test_buffer", test_buffer);
	mad_script->RunDirectly();
	
	if (*test_buffer)
	{
		MAD_LOG_INFO("TEST-Copy user data ............OK");
	}

}