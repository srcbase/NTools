#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__
#include <string>

// 日志输出窗口类信息
#define DBGER_WND_MAIN_CLASS		L"debugerWndClass"
#define DBGER_WND_MAIN_CAPTION		L"debugerWndCaption"
namespace gtc {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-9-22 日志类型	
enum BELogType {
	BELogTypeNormal,		// 常规调试信息
	BELogTypeNote,			// 注意
	BELogTypeWarning,		// 警告
	BELogTypeError			// 出错
};
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-9-22 通过WM_COPYDATA消息传递的数据结构
#define DBGER_MAX_LOG_LEN	1024
#define DBGER_LOG_TYPE		2			// 
struct BSLogData {
	BELogType	beType;
	wchar_t		pLogDesc[DBGER_MAX_LOG_LEN];
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	debugger类	2015-1-8
//	输出调试信息，设置调试断点，输入调试日志
////////////////////////////////////////////////////////////////////////////////////////////////////////
class debugger {
private:
	debugger(){}
public:
	/// Returns true if a debugger is available, false otherwise.
	/// On Windows, this function uses the IsDebuggerPresent()
	static bool isAvailable();

	/// Writes a message to the debugger log, if available, otherwise to
	/// standard error output.
	//static void message(const std::wstring &msg);
	//static void message(const std::string &msg);
	static void message(const wchar_t *pformat, ...);
	// 写日志文件
	static void write(const wchar_t *pformat, ...);
	// 将日志发送到调试进程
	static void send(const wchar_t *pformat, ...);
	//static void send_tick(const wchar_t *pformat, ...);
	static void sendA(const char *pformat, ...);
	//static void send(BELogType logType, const wchar_t *pformat, ...);
	//static void sendA(BELogType logType, const char *pformat, ...);
	/// Writes a message to the debugger log, if available, otherwise to
	/// standard error output.
	//static void message(const std::string &msg, const char *file, int line);

	/// Breaks into the debugger, if it is available.
	/// On Windows, this is done using the DebugBreak() function.
	static void enter();

	/// Writes a debug message to the debugger log and breaks into it.
	static void enter(const std::wstring &msg);

	/// Writes a debug message to the debugger log and breaks into it.
	static void enter(const std::wstring &msg, const char *file, int line);

	/// Writes a debug message to the debugger log and breaks into it.
	static void enter(const char *file, int line);


protected:
	// 预解析消息格式
	static BELogType preprocessFormat(const wchar_t *pformat, int &offset);
	static BELogType preprocessFormat(const char *pformat, int &offset);
};



} // end gtc



#endif