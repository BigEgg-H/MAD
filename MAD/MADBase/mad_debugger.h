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
/// ���ʹ�����Ϣ�ṹ��,
/// ������ȫ����Ϣ
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
/// MADר�ô�ӡ��,�����Զ��������Ϣ
/// </summary>
typedef std::function<void(MADString)> MAD_Printer;

/// <summary>
/// ��ӡ������
/// </summary>
enum class PrinterType
{
	/// <summary>
	/// ����
	/// </summary>
	Error, 
	/// <summary>
	/// ����
	/// </summary>
	Warning, 
	/// <summary>
	/// ��Ϣ
	/// </summary>
	Infomation
};

/// <summary>
/// ��̬������
/// ����ͳһ������Ϣ�����
/// </summary>
class MAD_Debugger
{
public:
	MAD_Debugger(const MAD_Debugger&) = delete;
	MAD_Debugger& operator=(const MAD_Debugger&) = delete;

public:
	/// <summary>
	/// ��ȡ����
	/// </summary>
	/// <returns>����������</returns>
	static MAD_Debugger& GetInstance() {
		static MAD_Debugger instance; // ��̬�ֲ�������ֻ��ʼ��һ��
		return instance;
	}

	/// <summary>
	/// ���ô�ӡ��
	/// </summary>
	/// <param name="_type">��ӡ��������</param>
	/// <param name="_target">��ӡ���ı�׼��������</param>
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
	/// ��ӡĿ��
	/// </summary>
	/// <param name="_type">����</param>
	/// <param name="_target">Ŀ��</param>
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