#include "Init.h"

Init* Init::g_pInit = new (std::nothrow) Init();

Init::Init() {
	plc = nullptr; 
	myClient = nullptr;
}
Init::~Init() {
	if (myClient) {
		delete myClient;
		myClient = nullptr;
	}
}
Init* Init::GetInstance() {
	return g_pInit;
}
void Init::deleteInstance() {
	if (g_pInit) {
		delete g_pInit;
		g_pInit = nullptr;
	}
}
void Init::InitPLCData(MessagePLC* myPLC, std::map<std::string, DataInform>& data) {
	std::ifstream profile;
	profile.open("testprofile.txt", std::ios::in);
	char buffer[256];
	std::vector<std::string> result;

	if (!profile) {
		// 检查文件是否成功打开
		std::cerr << "Unable to open file";
		exit(1); // 调用系统来终止程序
	}
	profile.getline(buffer, 256, '\n');
	myPLC->address = Tools::splitString(buffer)[1];
	profile.getline(buffer, 256, '\n');
	myPLC->Rack = std::stoi(Tools::splitString(buffer)[1]);
	profile.getline(buffer, 256, '\n');
	myPLC->Solt = std::stoi(Tools::splitString(buffer)[1]);

	while (!profile.eof()) {
		profile.getline(buffer, 256, '\n');
		result = Tools::splitString(buffer);
		data[result[0]] = DataInform(result[2], result[3], result[1]);
	}
	profile.close();
}
void Init::InitTargetData() {

	g_pInit->plc = new MessagePLC;
	g_pInit->myClient = new TS7Client;
	g_pInit->InitPLCData(g_pInit->plc, g_pInit->dataInform);
	g_pInit->myClient->ConnectTo(g_pInit->plc->address.data(), g_pInit->plc->Rack, g_pInit->plc->Solt);
	//std::cout << g_pInit->plc->address.data() << " " << g_pInit->plc->Rack << " " << g_pInit->plc->Solt << std::endl;
	//std::cout << "连接状态：" << g_pInit->myClient->Connected() << std::endl;
	std::vector<std::pair<std::string, DataInform>> dataInformVector(g_pInit->dataInform.begin(), g_pInit->dataInform.end());
	sort(dataInformVector.begin(), dataInformVector.end(), compare);
	for (const auto& entry : dataInformVector) {
		FinalData[entry.first] = g_pInit->ReadDataRegion(entry.second.address, entry.second.type, entry.second.size);
		// cout << entry.second.address << endl;
	}
	g_pInit->blocks = g_pInit->createBlocks(dataInformVector);
}
bool Init::compare(const std::pair<std::string, DataInform>& a, const std::pair<std::string, DataInform>& b) {
	std::vector<std::string> resultA = Tools::splitString(a.second.address);
	std::vector<std::string> resultB = Tools::splitString(b.second.address);

	if (resultA[0] == resultB[0]) {
		if (resultA[1] == resultB[1]) {
			if (resultA[2] == resultB[2]) {
				return std::stoi(resultA[3]) < std::stoi(resultB[3]);
			}
			else return std::stoi(resultA[2]) < std::stoi(resultB[2]);
		}
		else return std::stoi(resultA[1]) < std::stoi(resultB[1]);
	}
	else return resultA[0] < resultB[0];

}
std::string Init::ReadDataRegion(std::string address, std::string type, int size) {
	int result, MiniOffset = -1;
	unsigned char buffer[4];
	// 解析地址
	std::vector<std::string> parseAdd = Tools::splitString(address);
	std::string area = parseAdd[0];
	int dbNumber = stoi(parseAdd[1]); // DB块号
	int offset = stoi(parseAdd[2]);  // 偏移量
	if (type == "BOOL" && area == "DB") MiniOffset = stoi(parseAdd[3]); // 字节中偏移量

	DataRead(area, dbNumber, offset, size, buffer);
	// 解析buffer中的数据
	return BinaryConversionOther(buffer, type, MiniOffset);
}
void Init::DataRead(std::string area, int dbNumber, int offset, int size, unsigned char* buffer) {
	int result;
	// 根据area选择不同的读取区域
	if (area == "DB") {
		result = myClient->DBRead(dbNumber, offset, size, buffer);
	}
	else if (area == "M") {
		result = myClient->ReadArea(S7AreaMK, dbNumber, offset, size, 1, buffer);
	}
	else if (area == "Q") {
		result = myClient->ReadArea(S7AreaPA, dbNumber, offset, size, 1, buffer);
	}
	else if (area == "I") {
		result = myClient->ReadArea(S7AreaPE, dbNumber, offset, size, 1, buffer);
	}
	else {
		throw std::invalid_argument("Invalid area specified");
	}
	// 检查读取结果
	if (result < 0) {
		throw std::runtime_error("Failed to read data from PLC");
	}
}
std::string Init::BinaryConversionOther(unsigned char* buffer, std::string type, int offset) {
	std::string re;
	if (type == "BOOL") {
		/*if (offset == -1) {
			throw invalid_argument("BOOL type requires mini offset");
		}*/
		uint8_t byte = buffer[0];
		bool bit = (byte >> offset) & 1;
		re = std::to_string(bit);
	}
	else if (type == "INT") {
		int16_t val = int(buffer[1]) + int(buffer[0]) * pow(2, 8);
		re = std::to_string(val);
	}
	else if (type == "DINT") {
		int32_t val = static_cast<int32_t>(buffer[3]) + static_cast<int32_t>(buffer[2]) * 256 +
			static_cast<int32_t>(buffer[1]) * 65536 + static_cast<int32_t>(buffer[0]) * 16777216;
		re = std::to_string(val);
	}
	else if (type == "REAL") {
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
std::vector<DataBlock> Init::createBlocks(const std::vector<std::pair<std::string, DataInform>>& sortedData) {
	std::vector<DataBlock> blocks;
	int currentStart = -1;
	DataBlock currentBlock;
	std::vector<std::string> data;

	for (const auto& entry : sortedData) {
		data = Tools::splitString(entry.second.address);
		int minioffset = -1;
		if (data.size() == 4) minioffset = stoi(data[3]);
		if (currentStart == -1) {
			currentBlock.area = data[0];
			currentBlock.blockNum = stoi(data[1]);
			currentBlock.startOffset = stoi(data[2]);
			currentBlock.endOffset = entry.second.size + currentBlock.startOffset;
			//currentBlock.BlockData.push_back(make_pair(std::make_pair(stoi(data[2]), minioffset), entry.first));
			dataInform[entry.first].blockNum = blocks.size();
			dataInform[entry.first].addInBlock = 0;
			currentStart = 0;
		}
		else {
			if (data[0] != currentBlock.area || stoi(data[1]) != currentBlock.blockNum) {
				blocks.push_back(currentBlock);
				//currentBlock.BlockData.clear();
				currentBlock.area = data[0];
				currentBlock.blockNum = stoi(data[1]);
				currentBlock.startOffset = stoi(data[2]);
				currentBlock.endOffset = entry.second.size + currentBlock.startOffset;
				//currentBlock.BlockData.push_back(make_pair(std::make_pair(stoi(data[2]), minioffset), entry.first));
				dataInform[entry.first].blockNum = blocks.size();
				dataInform[entry.first].addInBlock = 0;
			}
			else {
				if (stoi(data[2]) - currentBlock.startOffset + entry.second.size >= 210) {
					blocks.push_back(currentBlock);
					//currentBlock.BlockData.clear();
					currentBlock.area = data[0];
					currentBlock.blockNum = stoi(data[1]);
					currentBlock.startOffset = stoi(data[2]);
					currentBlock.endOffset = entry.second.size + currentBlock.startOffset;
					//currentBlock.BlockData.push_back(make_pair(std::make_pair(stoi(data[2]), minioffset), entry.first));
					dataInform[entry.first].blockNum = blocks.size();
					dataInform[entry.first].addInBlock = 0;
				}
				else {
					currentBlock.endOffset = stoi(data[2]) + entry.second.size;
					//currentBlock.BlockData.push_back(make_pair(std::make_pair(stoi(data[2]), minioffset), entry.first));
					dataInform[entry.first].blockNum = blocks.size();
					dataInform[entry.first].addInBlock = stoi(data[2]) - currentBlock.startOffset;
				}
			}
		}
	}
	blocks.push_back(currentBlock);
	return blocks;
}
void Init::ReadDataInform(std::vector<DataBlock> blocks, std::map<std::string, DataInform> dataInform) {
	int re = 1;

	for (int i = 0; i < blocks.size(); ++i) {

		DataRead(blocks[i].area, blocks[i].blockNum, blocks[i].startOffset, 
			blocks[i].endOffset - blocks[i].startOffset, blocks[i].buffer[0]);
		DataRead(blocks[i].area, blocks[i].blockNum, blocks[i].startOffset,
			blocks[i].endOffset - blocks[i].startOffset, blocks[i].buffer[1]);
	}

	FILETIME startFt, endFt;
	ULARGE_INTEGER startUi, endUi;
	double elapsedTime;

	while (1) {
		GetSystemTimePreciseAsFileTime(&startFt);
		startUi.LowPart = startFt.dwLowDateTime;
		startUi.HighPart = startFt.dwHighDateTime;
		for (int i = 0; i < blocks.size(); ++i) {
			DataRead(blocks[i].area, blocks[i].blockNum, blocks[i].startOffset,
				blocks[i].endOffset - blocks[i].startOffset, blocks[i].buffer[re]);
		}
		for (std::map<std::string, DataInform>::iterator it = dataInform.begin(); it != dataInform.end(); it++) {
			if (it->second.type == "BOOL" && 
				it->second.GetMiniOffset(it->second.address) != 
				Tools::compareBit(blocks[it->second.blockNum].buffer[re][it->second.addInBlock],
					blocks[it->second.blockNum].buffer[re^1][it->second.addInBlock])) {
				continue;
			}
			if (!Tools::compareByte(blocks[it->second.blockNum].buffer[re] + it->second.addInBlock,
				blocks[it->second.blockNum].buffer[re ^ 1] + it->second.addInBlock, it->second.size)) {
				unsigned char tempBuffer[4];
				memcpy(tempBuffer, blocks[it->second.blockNum].buffer[re] + it->second.addInBlock, 4);
				FinalData[it->first] =
					BinaryConversionOther(tempBuffer, it->second.type, it->second.GetMiniOffset(it->second.address));
				std::cout << it->first << "  " << FinalData[it->first] << std::endl;
			}
		}
		re = re ^ 1;

		GetSystemTimePreciseAsFileTime(&endFt);
		endUi.LowPart = endFt.dwLowDateTime;
		endUi.HighPart = endFt.dwHighDateTime;
		elapsedTime = static_cast<double>(endUi.QuadPart - startUi.QuadPart);
		elapsedTime /= 10000.0; // 100 纳秒间隔数转换为ms

		Tools::preciseSleep(20000000 - elapsedTime*1000000);

		//GetSystemTimePreciseAsFileTime(&endFt);
		//endUi.LowPart = endFt.dwLowDateTime;
		//endUi.HighPart = endFt.dwHighDateTime;
		//elapsedTime = static_cast<double>(endUi.QuadPart - startUi.QuadPart);
		//elapsedTime /= 10000.0; // 100 纳秒间隔数转换为ms
		//std::cout << "程序运行耗时: " << elapsedTime << " ms" << std::endl;
	}

}

//std::vector<std::vector<DataBlock>> Init::splitBlocks(std::vector<DataBlock> blocks, int maxThreadNum) {
//	std::vector<std::vector<DataBlock>> sBlocks(maxThreadNum);
//	for (int i = 0; i < blocks.size(); ++i) {
//		if (i < maxThreadNum) sBlocks[i].emplace_back(blocks[i]);
//		else sBlocks[i%maxThreadNum].emplace_back(blocks[i]);
//	}
//	return sBlocks;
//}
//void Init::ReadBlockData(DataBlock block) {
//	int result = -1;
//	int re = 1;
//	std::string area = block.area;
//	int dbNumber = block.blockNum;
//	int offset = block.startOffset;
//	int size = block.endOffset - block.startOffset;
//	unsigned char buffer[2][210];
//
//	Sleep(20);
//	DataRead(area, dbNumber, offset, size, buffer[0]);
//	DataRead(area, dbNumber, offset, size, buffer[1]);
//	/*Init::printAllBits(buffer[0],size);
//	std::cout << "-----------------------------------" << std::endl;
//	Init::printAllBits(buffer[1], size);
//	std::cout << "-----------------------------------" << std::endl;*/
//	while (1) {
//
//		DataRead(area, dbNumber, offset, size, buffer[re]);
//		for (int i = 0; i < block.endOffset - block.startOffset; ++i) {
//			if (buffer[re][i] != buffer[re ^ 1][i]) {
//				//cout << "变动的字节：" << i << endl;
//				for (int j = 0; j < block.BlockData.size(); j++) {
//					if ((i + block.startOffset) <= (block.BlockData[j].first.first + dataInform[block.BlockData[j].second].size)) {
//						if (dataInform[block.BlockData[j].second].type == "BOOL") {
//							int minioffset = Tools::compareBit(buffer[re][i], buffer[re ^ 1][i]);
//							//cout << "字节内偏移量：" << minioffset << endl;
//							if (block.BlockData[j].first.second != minioffset) continue;
//						}
//						/*cout << block.BlockData[j].second << endl;
//						cout << dataInform[block.BlockData[j].second].type << endl;
//						cout << "对应的数据项：" << j+1 << endl;*/
//						unsigned char tempBuffer[4];
//						memcpy(tempBuffer, buffer[re] + block.BlockData[j].first.first - block.startOffset, 4);
//						FinalData[block.BlockData[j].second] =
//							BinaryConversionOther(tempBuffer,
//								dataInform[block.BlockData[j].second].type,
//								block.BlockData[j].first.second);
//						i = block.BlockData[j].first.first + dataInform[block.BlockData[j].second].size - block.startOffset;
//						std::cout << block.BlockData[j].second << "  " << FinalData[block.BlockData[j].second] << std::endl;
//						/*printAllBits(tempBuffer, 4);
//						cout << "更新后的i值：" << i << endl;*/
//						break;
//					}
//				}
//
//			}
//		}
//		re = re ^ 1;
//		Sleep(20);
//	}
//}
//void Init::ReadMoreBlockData(std::vector<DataBlock> blocks) {
//	
//	int re = 1;
//	std::vector<std::string> areas;
//	std::vector<int> dbNumbers;
//	std::vector<int> offsets;
//	std::vector<int> sizes;
//	unsigned char buffers[10][2][210];
//
//	for (int i = 0; i < blocks.size(); ++i) {
//		areas.emplace_back(blocks[i].area);
//		dbNumbers.emplace_back(blocks[i].blockNum);
//		offsets.emplace_back(blocks[i].startOffset);
//		sizes.emplace_back(blocks[i].endOffset - blocks[i].startOffset);
//		
//		DataRead(areas[i], dbNumbers[i], offsets[i], sizes[i], buffers[i][0]);
//		DataRead(areas[i], dbNumbers[i], offsets[i], sizes[i], buffers[i][1]);
//	}
//	while (1) {
//		for (int k = 0; k < blocks.size(); ++k) {
//			DataRead(areas[k], dbNumbers[k], offsets[k],sizes[k], buffers[k][re]);
//			for (int i = 0; i < blocks[k].endOffset - blocks[k].startOffset; ++i) {
//				if (buffers[k][re][i] != buffers[k][re ^ 1][i]) {
//					//cout << "变动的字节：" << i << endl;
//					for (int j = 0; j < blocks[k].BlockData.size(); j++) {
//						if ((i + blocks[k].startOffset) <= (blocks[k].BlockData[j].first.first + dataInform[blocks[k].BlockData[j].second].size)) {
//							if (dataInform[blocks[k].BlockData[j].second].type == "BOOL") {
//								int minioffset = Tools::compareBit(buffers[k][re][i], buffers[k][re ^ 1][i]);
//								//cout << "字节内偏移量：" << minioffset << endl;
//								if (blocks[k].BlockData[j].first.second != minioffset) continue;
//							}
//							unsigned char tempBuffer[4];
//							memcpy(tempBuffer, buffers[k][re] + blocks[k].BlockData[j].first.first - blocks[k].startOffset, 4);
//							FinalData[blocks[k].BlockData[j].second] =
//								BinaryConversionOther(tempBuffer,
//									dataInform[blocks[k].BlockData[j].second].type,
//									blocks[k].BlockData[j].first.second);
//							i = blocks[k].BlockData[j].first.first + dataInform[blocks[k].BlockData[j].second].size - blocks[k].startOffset;
//							std::cout << blocks[k].BlockData[j].second << "  " << FinalData[blocks[k].BlockData[j].second] << std::endl;
//							break;
//						}
//					}
//
//				}
//			}
//		}
//		re = re ^ 1;
//		Sleep(20);
//	}
//}
