#include "Init.h"

Init::Init() {}
Init::~Init() {}

void Init::ReadProfile(std::string file) {
	std::ifstream profile;
	profile.open(file, std::ios::in);
	char buffer[256];
	std::vector<std::string> result;

	if (!profile) {
		// 检查文件是否成功打开
		std::cerr << "Unable to open file";
		exit(1); // 调用系统来终止程序
	}
	profile.getline(buffer, 256, '\n');
	plc_.SetAddress(Tools::SplitString(buffer)[1]);
	profile.getline(buffer, 256, '\n');
	plc_.SetRack(std::stoi(Tools::SplitString(buffer)[1]));
	profile.getline(buffer, 256, '\n');
	plc_.SetSolt(std::stoi(Tools::SplitString(buffer)[1]));

	while (!profile.eof()) {
		profile.getline(buffer, 256, '\n');
		result = Tools::SplitString(buffer);
		dataInform_.emplace_back(DataInform(result[0], result[2], stringToArea[result[3]], stringToVARENUM[result[1]]));
	}
	profile.close();
	sort(dataInform_.begin(), dataInform_.end(), DataInform::Compare);
	for (int i = 0; i < dataInform_.size();++i) {
		tagsMap_[dataInform_[i].name_] = i;
	}
}

void Init::InitDataform() {
	for (auto& entry : dataInform_) {
		entry.value_ = plc_.ReadDataRegion(entry.address_, entry.area_, entry.type_, entry.size_);
	}
}

void Init::CreateBlocks(){
	int currentStart = -1;
	DataBlock currentBlock;
	std::vector<std::string> data;

	for (auto& entry : dataInform_) {
		data = Tools::SplitString(entry.address_);
		int minioffset = -1;
		if (data.size() == 4) minioffset = stoi(data[3]);
		if (currentStart == -1) {
			currentBlock.area_ = stringToArea[data[0]];
			currentBlock.blockNum_ = stoi(data[1]);
			currentBlock.startOffset_ = stoi(data[2]);
			currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;
			
			entry.blockNum_ = blocks_.size();
			entry.addInBlock_ = 0;
			currentStart = 0;
		}
		else {
			if (stringToArea[data[0]] != currentBlock.area_ || stoi(data[1]) != currentBlock.blockNum_) {
				blocks_.emplace_back(currentBlock);
				currentBlock.area_ = stringToArea[data[0]];
				currentBlock.blockNum_ = stoi(data[1]);
				currentBlock.startOffset_ = stoi(data[2]);
				currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;

				entry.blockNum_ = blocks_.size();
				entry.addInBlock_ = 0;
			}
			else {
				if (stoi(data[2]) - currentBlock.startOffset_ + entry.size_ >= 210) {
					blocks_.emplace_back(currentBlock);
					currentBlock.area_ = stringToArea[data[0]];
					currentBlock.blockNum_ = stoi(data[1]);
					currentBlock.startOffset_ = stoi(data[2]);
					currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;

					entry.blockNum_ = blocks_.size();
					entry.addInBlock_ = 0;
				}
				else {
					currentBlock.endOffset_ = stoi(data[2]) + entry.size_;
					entry.blockNum_ = blocks_.size();
					entry.addInBlock_ = stoi(data[2]) - currentBlock.startOffset_;
				}
			}
		}
	}
	blocks_.emplace_back(currentBlock);
	return;
}

void Init::InitBlocks() {
	for (int i = 0; i < blocks_.size(); ++i) {
		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[1]);
		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[0]);
	}
}

void Init::UpdateBlocksBuffer(int re){
	for (int i = 0; i < blocks_.size(); ++i) {
		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[re]);
	}
}

void Init::TraverseDataforms(int re){

	for (auto& entry : dataInform_) {
		if (entry.type_ == VARENUM::VT_BOOL &&
			entry.GetMiniOffset(entry.address_) !=
			Tools::CompareBit(blocks_[entry.blockNum_].buffer_[re][entry.addInBlock_],
				blocks_[entry.blockNum_].buffer_[re ^ 1][entry.addInBlock_])) {
			continue;
		}
		if (!Tools::CompareByte(blocks_[entry.blockNum_].buffer_[re] + entry.addInBlock_,
			blocks_[entry.blockNum_].buffer_[re ^ 1] + entry.addInBlock_, entry.size_)) {
			unsigned char tempBuffer[4];
			memcpy(tempBuffer, blocks_[entry.blockNum_].buffer_[re] + entry.addInBlock_, 4);
			entry.value_ =
				Tools::BinaryConversionOther(tempBuffer, entry.type_, entry.GetMiniOffset(entry.address_));
			std::cout << entry.name_ << "  " << entry.value_ << std::endl;
		}
	}
}

void Init::Run(std::string file){

	int re = 0;
	ReadProfile(file);
	plc_.ConnectPLC();
	InitDataform();
	CreateBlocks();
	InitBlocks();

	FILETIME startFt, endFt;
	ULARGE_INTEGER startUi, endUi;
	double elapsedTime;

	while (1) {
		GetSystemTimePreciseAsFileTime(&startFt);
		startUi.LowPart = startFt.dwLowDateTime;
		startUi.HighPart = startFt.dwHighDateTime;

		UpdateBlocksBuffer(re);
		TraverseDataforms(re);
		re = re ^ 1;

		GetSystemTimePreciseAsFileTime(&endFt);
		endUi.LowPart = endFt.dwLowDateTime;
		endUi.HighPart = endFt.dwHighDateTime;
		elapsedTime = static_cast<double>(endUi.QuadPart - startUi.QuadPart);
		elapsedTime /= 10000.0; // 转为ms

		Tools::PreciseSleep(20000000 - elapsedTime * 1000000);
	}
}

void Init::InquireData() {
	std::string name;
	while (1) {
		std::cout << "输入需查询数据名：";
		std::cin >> name;
		std::cout << name << " " << dataInform_[tagsMap_[name]].value_ << std::endl;
	}
}