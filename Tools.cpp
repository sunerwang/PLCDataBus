#include "Tools.h"

std::vector<std::string> Tools::splitString(const char* input) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string token;

	// 按逗号分割字符串
	while (getline(ss, token, ',')) {
		result.push_back(token);
	}
	// profile文件中不能有空行，否则引起vector索引越界报错
	return result;
}
std::vector<std::string> Tools::splitString(const std::string& str) {
	std::vector<std::string> result;
	std::regex pattern("([a-zA-Z]+)([0-9]+)(\\.[0-9]+)?(\\.[0-9]+)?");
	std::smatch matches;

	if (regex_match(str, matches, pattern)) {
		for (size_t i = 1; i < matches.size(); ++i) {
			if (matches[i].matched) {
				std::string part = matches[i].str();
				// Remove the dot if present
				if (part.front() == '.') {
					part = part.substr(1); // Remove the first character (dot)
				}
				result.push_back(part);
			}
		}
	}

	return result;
}
void Tools::printAllBits(unsigned char* buffer, size_t length) {
	for (size_t i = 0; i < length; ++i) {
		for (int j = 0; j < 8; j++) {
			std::cout << ((buffer[i] >> j) & 1);
		}
		std::cout << " ";  // 每个字节之间加个空格
	}
	std::cout << std::endl;
}
int Tools::compareBit(unsigned char c1, unsigned char c2) {
	int result = 0;
	for (; result < 8; ++result) {
		// 逐位比较
		if ((c1 & 1) != (c2 & 1)) {
			break;
		}
		// 右移一位，比较下一位
		c1 >>= 1;
		c2 >>= 1;
	}
	return result;
}
bool Tools::compareByte(unsigned char* buffer1, unsigned char* buffer2, int size) {

	for (int i = 0; i < size; ++i) {

		unsigned char byte1 = buffer1[i];
		unsigned char byte2 = buffer2[i];

		for (int bit = 0; bit < 8; ++bit) {
			unsigned char bit1 = (byte1 >> bit) & 0x01;
			unsigned char bit2 = (byte2 >> bit) & 0x01;

			if (bit1 != bit2) {
				return 0;
			}
		}
	}

	return 1;
}
void Tools::preciseSleep(double nanoseconds) {
	if (nanoseconds < 0) return;

	LARGE_INTEGER frequency;        // 计时器频率
	LARGE_INTEGER start, end;       // 开始和结束时间
	double elapsedTime;             // 耗时

	// 获取计时器频率
	QueryPerformanceFrequency(&frequency);

	// 计算所需的计时器计数
	double counts = (nanoseconds * frequency.QuadPart) / 1000000000.0;

	// 获取开始时间
	QueryPerformanceCounter(&start);

	do {
		// 获取当前时间
		QueryPerformanceCounter(&end);

		// 计算时间差
		elapsedTime = static_cast<double>(end.QuadPart - start.QuadPart);

	} while (elapsedTime < counts);
}
void Tools::checkForEscExit() {
	while (1) {
	
		if (_kbhit()) { // 检测是否有按键按下
			char key = _getch(); // 获取按下的键
			if (key == 27) { // 27 是 Esc 键的 ASCII 码
				//std::cout << "检测到 Esc 键，退出程序。" << std::endl;
				exit(0); // 退出程序
			}
		}
	}
}