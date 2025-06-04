#include "Init.h"

bool bExit = false;

Init::Init() {}
Init::~Init() {}

void Init::ReadProfile(std::string file) {
	std::ifstream profile;
	char buffer[256];
	std::vector<std::string> result;

	profile.open(file, std::ios::in);
	if (!profile) {
		// ����ļ��Ƿ�ɹ���
		std::cerr << "Unable to open file";
		exit(1); // ����ϵͳ����ֹ����
	}

	//���䲻ֹ����1��PLC����Ҫ��һ���޸�
	plc_.emplace_back(std::make_unique<PLCConnection>());

	int index = 0;
	//��ȡPLC��ַ�����ܺš��ۺ���Ϣ
	profile.getline(buffer, 256, '\n');
	plc_[index]->SetAddress(Tools::SplitString(buffer,',')[1]);
	profile.getline(buffer, 256, '\n');
	plc_[index]->SetRack(std::stoi(Tools::SplitString(buffer, ',')[1]));
	profile.getline(buffer, 256, '\n');
	plc_[index]->SetSolt(std::stoi(Tools::SplitString(buffer, ',')[1]));

	while (!profile.eof()) {
		profile.getline(buffer, 256, '\n');
		if (buffer[0] == '\0') continue; // ��������
		result = Tools::SplitString(buffer, ',');	// �ָ��ַ���, result[0]Ϊ���ƣ�result[1]Ϊ�������ͣ�result[2]Ϊ��ַ��result[3]Ϊ���ݿ���������
		plc_[index]->tagInform_.emplace_back(DataInform(result[0], result[2], stringToArea[result[3]], stringToVARENUM[result[1]]));
	}
	profile.close();

	sort(plc_[index]->tagInform_.begin(), plc_[index]->tagInform_.end(), DataInform::Compare);
	for (int i = 0; i < plc_[index]->tagInform_.size(); ++i) {
		plc_[index]->tagIndex_[plc_[index]->tagInform_[i].name_] = i;
	}

	index++;
}

//void Init::InitDataform() {
//	for (auto& entry : dataInform_) {
//		entry.value_ = plc_[i].ReadDataRegion(entry.address_, entry.area_, entry.type_, entry.size_);
//	}
//}
//
//void Init::CreateBlocks(){
//	int currentStart = -1;
//	DataBlock currentBlock;
//	std::vector<std::string> data;
//
//	for (auto& entry : dataInform_) {
//		data = Tools::SplitString(entry.address_);
//		int minioffset = -1;
//		if (data.size() == 4) minioffset = stoi(data[3]);
//		if (currentStart == -1) {
//			currentBlock.area_ = stringToArea[data[0]];
//			currentBlock.blockNum_ = stoi(data[1]);
//			currentBlock.startOffset_ = stoi(data[2]);
//			currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;
//			
//			entry.blockNum_ = blocks_.size();
//			entry.addInBlock_ = 0;
//			currentStart = 0;
//		}
//		else {
//			if (stringToArea[data[0]] != currentBlock.area_ || stoi(data[1]) != currentBlock.blockNum_) {
//				blocks_.emplace_back(currentBlock);
//				currentBlock.area_ = stringToArea[data[0]];
//				currentBlock.blockNum_ = stoi(data[1]);
//				currentBlock.startOffset_ = stoi(data[2]);
//				currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;
//
//				entry.blockNum_ = blocks_.size();
//				entry.addInBlock_ = 0;
//			}
//			else {
//				if (stoi(data[2]) - currentBlock.startOffset_ + entry.size_ >= 210) {
//					blocks_.emplace_back(currentBlock);
//					currentBlock.area_ = stringToArea[data[0]];
//					currentBlock.blockNum_ = stoi(data[1]);
//					currentBlock.startOffset_ = stoi(data[2]);
//					currentBlock.endOffset_ = entry.size_ + currentBlock.startOffset_;
//
//					entry.blockNum_ = blocks_.size();
//					entry.addInBlock_ = 0;
//				}
//				else {
//					currentBlock.endOffset_ = stoi(data[2]) + entry.size_;
//					entry.blockNum_ = blocks_.size();
//					entry.addInBlock_ = stoi(data[2]) - currentBlock.startOffset_;
//				}
//			}
//		}
//	}
//	blocks_.emplace_back(currentBlock);
//	return;
//}
//
//void Init::InitBlocks() {
//	for (int i = 0; i < blocks_.size(); ++i) {
//		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
//			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[1]);
//		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
//			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[0]);
//	}
//}
//
//void Init::UpdateBlocksBuffer(int re){
//	for (int i = 0; i < blocks_.size(); ++i) {
//		plc_.DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
//			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[re]);
//	}
//}
//
//void Init::TraverseDataforms(int re){
//
//	for (auto& entry : dataInform_) {
//		if (entry.type_ == VARENUM::VT_BOOL &&
//			entry.GetMiniOffset(entry.address_) !=
//			Tools::CompareBit(blocks_[entry.blockNum_].buffer_[re][entry.addInBlock_],
//				blocks_[entry.blockNum_].buffer_[re ^ 1][entry.addInBlock_])) {
//			continue;
//		}
//		if (!Tools::CompareByte(blocks_[entry.blockNum_].buffer_[re] + entry.addInBlock_,
//			blocks_[entry.blockNum_].buffer_[re ^ 1] + entry.addInBlock_, entry.size_)) {
//			unsigned char tempBuffer[4];
//			memcpy(tempBuffer, blocks_[entry.blockNum_].buffer_[re] + entry.addInBlock_, 4);
//			entry.value_ =
//				Tools::BinaryConversionOther(tempBuffer, entry.type_, entry.GetMiniOffset(entry.address_));
//			std::cout << entry.name_ << "  " << entry.value_ << std::endl;
//		}
//	}
//}

void Init::Run(std::string file) {
	//�������е�PLC����
	ReadProfile(file);

	for (auto& plc : plc_) {
		plc->CreateBlocks();
	}

	for (auto& plc : plc_) {
		plc->ConnectPLC();
		//mark Ϊ������������е�tag���һ�飬Ӧ����Ϊ�˳�ʼ�����ݣ������е�tag�㸳��ֵ
		plc->TriggerFirstRead();
		//plc->CreateBlocks();
		plc->InitBlocks();
	}

	FILETIME startFt, endFt;
	ULARGE_INTEGER startUi, endUi;
	double elapsedTime;

	while (1) {
		GetSystemTimePreciseAsFileTime(&startFt);
		startUi.LowPart = startFt.dwLowDateTime;
		startUi.HighPart = startFt.dwHighDateTime;

		for (auto& plc : plc_) {
			plc->UpdateBlocksBuffer();
			plc->TraverseDataforms();
			plc->SwitchBuffer();
			//std::cout << plc->blocks_.size() << std::endl;
		}

		GetSystemTimePreciseAsFileTime(&endFt);
		endUi.LowPart = endFt.dwLowDateTime;
		endUi.HighPart = endFt.dwHighDateTime;
		elapsedTime = static_cast<double>(endUi.QuadPart - startUi.QuadPart);
		elapsedTime /= 10000.0; // תΪms

		// std::cout << "���κ�ʱ��" << elapsedTime << "ms" << std::endl;

		Tools::PreciseSleep(20000000 - elapsedTime * 1000000);

		if (bExit) break;
	}
}

void Init::InquireData() {
	std::string name;
	while (1) {
		std::cout << "�������ѯ��������";
		std::cin >> name;

		//mark
		//std::cout << name << " " << dataInform_[tagsMap_[name]].value_ << std::endl;
	}
}