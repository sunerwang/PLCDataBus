#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include "snap7.h"

enum class Area {
	DB,
	M,
	MB,
	MW,
	MD,
	I,
	IB,
	IW,
	ID,
	Q,
	QB,
	QW,
	QD
};

/*已包含在windows.h中*/
//enum class VARENUM {
//	VT_EMPTY = 0,
//	VT_NULL = 1,
//	VT_I2 = 2,
//	VT_I4 = 3,
//	VT_R4 = 4,
//	VT_R8 = 5,
//	VT_CY = 6,
//	VT_DATE = 7,
//	VT_BSTR = 8,
//	VT_DISPATCH = 9,
//	VT_ERROR = 10,
//	VT_BOOL = 11,
//	VT_VARIANT = 12,
//	VT_UNKNOWN = 13,
//	VT_DECIMAL = 14,
//	VT_I1 = 16,
//	VT_UI1 = 17,
//	VT_UI2 = 18,
//	VT_UI4 = 19,
//	VT_I8 = 20,
//	VT_UI8 = 21,
//	VT_INT = 22,
//	VT_UINT = 23,
//	VT_VOID = 24,
//	VT_HRESULT = 25,
//	VT_PTR = 26,
//	VT_SAFEARRAY = 27,
//	VT_CARRAY = 28,
//	VT_USERDEFINED = 29,
//	VT_LPSTR = 30,
//	VT_LPWSTR = 31,
//	VT_RECORD = 36,
//	VT_INT_PTR = 37,
//	VT_UINT_PTR = 38,
//	VT_FILETIME = 64,
//	VT_BLOB = 65,
//	VT_STREAM = 66,
//	VT_STORAGE = 67,
//	VT_STREAMED_OBJECT = 68,
//	VT_STORED_OBJECT = 69,
//	VT_BLOB_OBJECT = 70,
//	VT_CF = 71,
//	VT_CLSID = 72,
//	VT_VERSIONED_STREAM = 73,
//	VT_BSTR_BLOB = 0xfff,
//	VT_VECTOR = 0x1000,
//	VT_ARRAY = 0x2000,
//	VT_BYREF = 0x4000,
//	VT_RESERVED = 0x8000,
//	VT_ILLEGAL = 0xffff,
//	VT_ILLEGALMASKED = 0xfff,
//	VT_TYPEMASK = 0xfff
//};

class Tools {
public:
	static std::vector<std::string> SplitString(const char* input, char splitchar);
	// 分割字符串
	static std::vector<std::string> SplitAddressString(const std::string& str);
	static void PrintAllBits(unsigned char* buffer, size_t length);   // 打印所有bit
	static int CompareBit(unsigned char c1, unsigned char c2);  // 逐位对比并返回不同的位下标
	static bool CompareByte(unsigned char* buffer1, unsigned char* buffer2, int size); // 判断是否相同
	static void PreciseSleep(double nanoseconds);   // 纳秒级延时器
	static void CheckForEscExit();  // 按键退出
	static std::string BinaryConversionOther(unsigned char* buffer, VARENUM type, int offset);  // 二进制转换
	static const int TypeSize_[32];  // 变量类型大小
	static int getS7Area(Area area);
	static int getS7WL(Area area);

};
