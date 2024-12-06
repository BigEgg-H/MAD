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

int main() {
	/*Debugger testing*/
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Error, test_err_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Warning, test_warn_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Infomation, test_info_printer);

	MADScript* mad_script = MADScript::CreateScript("print(\"Hello MAD!\")\nA=\"hello?\"");

	mad_script->RunDirectly();
	MAD_LOG_INFO(mad_script->GetValueString("A"));
}