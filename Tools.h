#pragma once
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>

class Tools {
public:
	static std::vector<std::string> splitString(const char* input);     // 分割字符串
	static std::vector<std::string> splitString(const std::string& str);
	static void printAllBits(unsigned char* buffer, size_t length);   // 打印所有bit
	static int compareBit(unsigned char c1, unsigned char c2);  // 逐位对比并返回不同的位下标
	static bool compareByte(unsigned char* buffer1, unsigned char* buffer2, int size); // 判断是否相同
	static void preciseSleep(double nanoseconds);   // 纳秒级延时器
	static void checkForEscExit();
};
