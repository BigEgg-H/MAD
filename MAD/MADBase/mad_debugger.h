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
	MADDebuggerInfo_HEAVY(MADDebuggerInfo_LIGHT _code) {
		InfoCode = _code;
		Description = "NULL";
	}
	MADDebuggerInfo_HEAVY(MADDebuggerInfo_LIGHT _code, MADString _desc) {
		InfoCode = _code;
		Description = _desc;
	}

	/**
	 * @brief 重载为轻量级错误信息类型转换操作符
	 *
	 * 将当前的MADDebuggerInfo_HEAVY对象转换为MADDebuggerInfo_LIGHT类型，
	 * 仅保留错误代码部分信息。
	 *
	 * @return 返回MADDebuggerInfo_LIGHT类型的错误代码
	 */
	operator MADDebuggerInfo_LIGHT() const {
		return InfoCode;
	}

	/**
	 * 重载为MADString类型的转换操作符，
	 * 将重型错误信息结构体转换为格式化的字符串表示。
	 * 格式为：错误码: 错误描述
	 *
	 * @return 返回一个包含错误码和描述的格式化字符串
	 */
	operator MADString() const {
		return std::to_string(InfoCode) + MADString(": ") + Description;
	}

	/**
	 * @brief 该运算符重载函数将重型错误信息结构体转换为布尔值。
	 * @return 如果 `InfoCode` 成员的值为0，则返回true，表示没有错误；否则返回false，表示存在错误。
	 *
	 * 这个转换操作允许将 `MADDebuggerInfo_HEAVY` 实例直接用在需要布尔值的上下文中，
	 * 例如在条件判断中简洁地检查是否发生错误。
	 */
	operator bool() const {
		return InfoCode == 0;
	}
};

/// <summary>
/// MAD专用打印机,用于自定义输出消息
/// </summary>
typedef std::function<void(const MADString&)> MAD_Printer;

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
	Information
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
	void SetPrinter(PrinterType _type, const MAD_Printer& _target){
		switch (_type)
		{
		case PrinterType::Error:
			MAD_Err_Printer = _target;
			break;
		case PrinterType::Warning:
			MAD_Warn_Printer = _target;
			break;
		case PrinterType::Information:
			MAD_Info_Printer = _target;
			break;
		}
	}

	/// <summary>
	/// 打印目标
	/// </summary>
	/// <param name="_type">类型</param>
	/// <param name="_target">目标</param>
	void Print(PrinterType _type, const MADString& _target) {
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
		case PrinterType::Information:
			if (!MAD_Info_Printer)
			{
				break;
			}
			MAD_Info_Printer(_target);
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
#define MAD_RESCODE_SYNTAX_ERROR 1
#define MAD_RESCODE_MEM_OUT 2
#define MAD_RESCODE_ILLEGAL_CALL 3
#define MAD_RESCODE_FUNC_NOT_FOUND 4
#define MAD_RESCODE_FUNC_FAILED 5

#define MAD_IS_OK(res) res == 0

/*MAD Err help function*/
#define MAD_LOG_ERR(str) MAD_Debugger::GetInstance().Print(PrinterType::Error,str)
#define MAD_LOG_WARN(str) MAD_Debugger::GetInstance().Print(PrinterType::Warning,str)
#define MAD_LOG_INFO(str) MAD_Debugger::GetInstance().Print(PrinterType::Information,str)
