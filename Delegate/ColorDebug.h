#pragma once

#include <iostream>
#include <stdio.h>
#include <string>

struct SFileLine
{
	SFileLine(const char* _file, const char* _function, size_t _line);

	const char* file;
	const char* function;
	size_t line;
};
#define __GET_FILE_LINE() SFileLine(__FILE__, __FUNCTION__, __LINE__)

void AppendLog(std::string& str, const std::string& value);
void AppendLog(std::string& str, const char* value);

template<typename T>
void AppendLog(std::string& str, T value)
{
	str += std::to_string(value);
}

std::string GetLogFileName(const std::string& fileName);

#if _WIN32 || _WIN64
#include <Windows.h>

//   0 = 黑色      8 = 灰色
//   1 = 蓝色      9 = 淡蓝色
//   2 = 绿色      A = 淡绿色
//   3 = 浅绿色    B = 淡浅绿色
//   4 = 红色      C = 淡红色
//   5 = 紫色      D = 淡紫色
//   6 = 黄色      E = 淡黄色
//   7 = 白色      F = 亮白色
// FOREGROUND_INTENSITY 颜色高亮

//前景颜色（字体颜色）
enum ConsoleForegroundColor
{
	enmCFC_Red = FOREGROUND_RED,
	enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	enmCFC_Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
	enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Gray = FOREGROUND_INTENSITY,
	enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Black = 0,
};

//背景颜色（字体底色）
enum ConsoleBackGroundColor
{
	enmCBC_Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
	enmCBC_Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	enmCBC_Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	enmCBC_Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	enmCBC_Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	enmCBC_Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_Black = 0,
};

//设置控制台颜色
void SetConsoleColor(ConsoleForegroundColor foreColor = enmCFC_White, ConsoleBackGroundColor backColor = enmCBC_Black);

template<typename... Args>
void ColorDebug(ConsoleForegroundColor foreColor, ConsoleBackGroundColor backColor, const SFileLine& logData, const Args&... args)
{
	SetConsoleColor(foreColor, backColor);

	std::string prefix;
	std::cout << GetLogFileName(logData.file);
	std::cout << " [" << logData.function << ":";
	std::cout << logData.line << "]: ";

	//C++17 折叠表达式
	((AppendLog(prefix, args), prefix += " "), ...);

	std::cout << prefix << std::endl;
	SetConsoleColor();
}

//#define StringFormat(format, ...) printf((std::string("%s:%s:%d ") + format + "\n").c_str(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DEBUG_LOG(...) ColorDebug(enmCFC_Green, enmCBC_Black, __GET_FILE_LINE(), ##__VA_ARGS__)
#define WARNING_LOG(...) ColorDebug(enmCFC_Yellow, enmCBC_Black, __GET_FILE_LINE(), ##__VA_ARGS__)
#define ERROR_LOG(...) ColorDebug(enmCFC_Red, enmCBC_Black, __GET_FILE_LINE(), ##__VA_ARGS__)

#endif