#include "Tools.h"

extern bool bExit;

std::vector<std::string> Tools::SplitString(const char* input, char splitchar) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string token;

	// 分割字符串
	while (getline(ss, token, splitchar)) {
		result.push_back(token);
	}
	// profile文件中不能有空行，否则引起vector索引越界报错
	return result;
}

std::vector<std::string> Tools::SplitAddressString(const std::string& str) {
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

void Tools::PrintAllBits(unsigned char* buffer, size_t length) {
	for (size_t i = 0; i < length; ++i) {
		for (int j = 0; j < 8; j++) {
			std::cout << ((buffer[i] >> j) & 1);
		}
		std::cout << " ";  // 每个字节之间加个空格
	}
	std::cout << std::endl;
}

int Tools::CompareBit(unsigned char c1, unsigned char c2) {
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

bool Tools::CompareByte(unsigned char* buffer1, unsigned char* buffer2, int size) {

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

void Tools::PreciseSleep(double nanoseconds) {
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

void Tools::CheckForEscExit() {
	while (1) {
	
		if (_kbhit()) { // 检测是否有按键按下
			char key = _getch(); // 获取按下的键
			if (key == 27) { // 27 是 Esc 键的 ASCII 码
				//std::cout << "检测到 Esc 键，退出程序。" << std::endl;
				//exit(0); // 退出程序
				bExit = true;
				Sleep(1000);
				break;
			}
		}
	}
}

std::string Tools::BinaryConversionOther(unsigned char* buffer, VARENUM type, int offset) {
	std::string re;
	if (type == VARENUM::VT_BOOL) {
		/*if (offset == -1) {
			throw invalid_argument("BOOL type requires mini offset");
		}*/
		uint8_t byte = buffer[0];
		bool bit = (byte >> offset) & 1;
		re = std::to_string(bit);
	}
	else if (type == VARENUM::VT_I2) {
		int16_t val = int(buffer[1]) + int(buffer[0]) * pow(2, 8);
		re = std::to_string(val);
	}
	else if (type == VARENUM::VT_I4) {
		int32_t val = static_cast<int32_t>(buffer[3]) + static_cast<int32_t>(buffer[2]) * 256 +
			static_cast<int32_t>(buffer[1]) * 65536 + static_cast<int32_t>(buffer[0]) * 16777216;
		re = std::to_string(val);
	}
	else if (type == VARENUM::VT_R4) {
		uint32_t val = static_cast<uint32_t>(buffer[3]) + static_cast<uint32_t>(buffer[2]) * 256 +
			static_cast<uint32_t>(buffer[1]) * 65536 + static_cast<uint32_t>(buffer[0]) * 16777216;
		float fval = *reinterpret_cast<float*>(&val);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(6) << fval;
		re = ss.str();
	}
	else {
		throw std::invalid_argument("Unsupported data type");
	}
	return re;
}

const int Tools::TypeSize_[32] = { 0,0,2,4,4,8,4,8,
								  8,4,4,1,4,4,16,-1,
								  1,1,2,4,8,8,4,4,
								  0,2,4,4,4,4,8,16};


int Tools::getS7Area(Area area) {
	switch (area) {
		case Area::DB: return S7AreaDB;
		case Area::M:  return S7AreaMK;
		case Area::MB: return S7AreaMK;
		case Area::MW: return S7AreaMK;
		case Area::MD: return S7AreaMK;
		case Area::I:  return S7AreaPE;
		case Area::IB: return S7AreaPE;
		case Area::IW: return S7AreaPE;
		case Area::ID: return S7AreaPE;
		case Area::Q:  return S7AreaPA;
		case Area::QB: return S7AreaPA;
		case Area::QW: return S7AreaPA;
		case Area::QD: return S7AreaPA;
		default: return -1;
		}
	}

int Tools::getS7WL(Area area) {
	switch (area) {
		case Area::DB: return S7WLByte;
		case Area::M:  return S7WLByte;
		case Area::MB: return S7WLByte;
		case Area::MW: return S7WLWord;
		case Area::MD: return S7WLDWord;
		case Area::I:  return S7WLByte;
		case Area::IB: return S7WLByte;
		case Area::IW: return S7WLWord;
		case Area::ID: return S7WLDWord;
		case Area::Q:  return S7WLByte;
		case Area::QB: return S7WLByte;
		case Area::QW: return S7WLWord;
		case Area::QD: return S7WLDWord;
		default: return -1;
		}
	}
