#include "ColorDebug.h"

SFileLine::SFileLine(const char* _file, const char* _function, size_t _line)
	: file(_file),
	function(_function),
	line(_line)
{

}

void AppendLog(std::string& str, const std::string& value)
{
	str += value;
}

void AppendLog(std::string& str, const char* value)
{
	str += value;
}

void AppendLog(std::string& str, char* value)
{
	str += value;
}

void AppendLog(std::string& str, const char& value)
{
	str += value;
}

std::string GetLogFileName(const std::string& fileName)
{
	const char* finder = "\\";

	auto fpos = fileName.find_last_of(finder);
	if (fpos == std::string::npos)
	{
		finder = "/";
		fpos = fileName.find_last_of(finder);
		if (fpos == std::string::npos)
			return std::string();
	}

	int counter = 0;
	std::string result;
	for (auto pos = fpos; pos != std::string::npos; pos = fileName.rfind(finder, pos - 1))
	{
		result = fileName.substr(pos + 1);

		//只要文件名和目录名
		if (++counter >= 1/*2*/)
			break;
	}

	return result;
}

#if _WIN32 || _WIN64

void SetConsoleColor(ConsoleForegroundColor foreColor /*= enmCFC_White*/, ConsoleBackGroundColor backColor /*= enmCBC_Black*/)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);		//获取输出句柄
	SetConsoleTextAttribute(handle, foreColor | backColor);	//设置控制台句柄的文本颜色
}

#endif

