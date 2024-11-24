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

#ifndef MAD_LOG_ERR
#define MAD_LOG_ERR(str) printf(str)
#endif // !MAD_LOG_ERR(str)

typedef unsigned int MADDebuggerInfo_LIGHT;

/// <summary>
/// 重型错误信息结构体,
/// 包含完全的信息
/// </summary>
struct MADDebuggerInfo_HEAVY
{
	MADDebuggerInfo_LIGHT InfoCode;
	MADString Description;

	MADDebuggerInfo_HEAVY() {
		InfoCode = -1;
		Description = "NULL";
	}
	MADDebuggerInfo_HEAVY(const MADDebuggerInfo_HEAVY& _parent) {
		InfoCode = _parent.InfoCode;
		Description = _parent.Description;
	}
	MADDebuggerInfo_HEAVY(MADDebuggerInfo_LIGHT& _code) {
		InfoCode = _code;
		Description = "NULL";
	}
	MADDebuggerInfo_HEAVY(MADDebuggerInfo_LIGHT& _code, MADString& _desc) {
		InfoCode = _code;
		Description = _desc;
	}

	operator MADDebuggerInfo_LIGHT() const {
		return InfoCode;
	}

	operator MADString() const {
		return std::to_string(InfoCode) + MADString(": ") + Description;
	}
};

/// <summary>
/// MAD专用打印机,用于自定义输出消息
/// </summary>
typedef std::function<void(MADString)> MAD_Printer;

/// <summary>
/// 打印机种类
/// </summary>
enum class PrinterType
{
	/// <summary>
	/// 错误
	/// </summary>
	Error, 
	/// <summary>
	/// 警告
	/// </summary>
	Warning, 
	/// <summary>
	/// 消息
	/// </summary>
	Infomation
};

/// <summary>
/// 静态单例类
/// 用于统一错误信息的输出
/// </summary>
class MAD_Debugger
{
public:
	MAD_Debugger(const MAD_Debugger&) = delete;
	MAD_Debugger& operator=(const MAD_Debugger&) = delete;

public:
	/// <summary>
	/// 获取单例
	/// </summary>
	/// <returns>单例的引用</returns>
	static MAD_Debugger& GetInstance() {
		static MAD_Debugger instance; // 静态局部变量，只初始化一次
		return instance;
	}

	/// <summary>
	/// 设置打印机
	/// </summary>
	/// <param name="_type">打印机的种类</param>
	/// <param name="_target">打印机的标准函数对象</param>
	void SetPrinter(PrinterType _type, MAD_Printer _target){
		switch (_type)
		{
		case PrinterType::Error:
			MAD_Err_Printer = _target;
			break;
		case PrinterType::Warning:
			MAD_Warn_Printer = _target;
			break;
		case PrinterType::Infomation:
			MAD_Info_Printer = _target;
			break;
		default:
			break;
		}
	}

	/// <summary>
	/// 打印目标
	/// </summary>
	/// <param name="_type">类型</param>
	/// <param name="_target">目标</param>
	void Print(PrinterType _type, MADString _target) {
		switch (_type)
		{
		case PrinterType::Error:
			if (!MAD_Err_Printer)
			{
				break;
			}
			MAD_Err_Printer(_target);
			break;
		case PrinterType::Warning:
			if (!MAD_Warn_Printer)
			{
				break;
			}
			MAD_Warn_Printer(_target);
			break;
		case PrinterType::Infomation:
			if (!MAD_Info_Printer)
			{
				break;
			}
			MAD_Info_Printer(_target);
			break;
		default:
			break;
		}
	}

private:
	MAD_Printer MAD_Err_Printer = nullptr;
	MAD_Printer MAD_Warn_Printer = nullptr;
	MAD_Printer MAD_Info_Printer = nullptr;

private:
	MAD_Debugger() {/*Do NOT instantiation this class*/ };
	~MAD_Debugger() {/*Do NOT instantiation this class*/ };
};

/*MAD Err const value*/
#define MAD_RESCODE_UNKNOWN -1
#define MAD_RESCODE_OK 0

#define MAD_IS_OK(res) res == 0