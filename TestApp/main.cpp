#include "../MAD/mad.h"

#include <iostream>
#include <chrono>
using namespace std;

void test_err_printer(const MADString& _str) {
	cout << "[MAD_TestAPP_ERR]: " << _str << '\n';
}

void test_warn_printer(const MADString& _str) {
	cout << "[MAD_TestAPP_WARN]: " << _str << '\n';
}

void test_info_printer(const MADString&  _str) {
	cout << "[MAD_TestAPP_INFO]: " << _str << '\n';
}

#define TIME_POINT_START {auto start = std::chrono::high_resolution_clock::now();
#define TIME_POINT_END auto finish = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> elapsed = finish - start; MAD_LOG_INFO("TimePoint: " + to_string(elapsed.count()) + "s");}

int main()
{
	/*Debugger testing*/
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Error, test_err_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Warning, test_warn_printer);
	MAD_Debugger::GetInstance().SetPrinter(PrinterType::Information, test_info_printer);

	/*Script testing*/
	MADScript* mad_script = MADScript::CreateScript("CopyNumberToArray(ptr,1,1,1,1,1)");
	if (!mad_script)
		return 1;

	double* TestBuffer = new double[5]();
	mad_script->SetValueUserPtr("ptr",TestBuffer);
	mad_script->RunDirectly();
	mad_script->CallMain();
}