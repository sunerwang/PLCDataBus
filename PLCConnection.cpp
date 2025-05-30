#include "PLCConnection.h"

PLCConnection::PLCConnection()
	:myClient_(new TS7Client), address_("127.0.0.1"), rack_(0), solt_(1) {}

PLCConnection::PLCConnection(std::string address, int rack, int solt)
	:myClient_(new TS7Client), address_(address), rack_(rack), solt_(solt) {}

PLCConnection::~PLCConnection() {

	if (myClient_->Connected()) Disconnection();

	if (myClient_) {
		delete myClient_;
		myClient_ = nullptr;
	}
}

bool PLCConnection::ConnectPLC() {
	myClient_->ConnectTo(address_.c_str(), rack_, solt_);
	return myClient_->Connected();
}

bool PLCConnection::Disconnection() {
	myClient_->Disconnect();
	return !myClient_->Connected();
}

TS7Client* PLCConnection::GetClient() {
	return myClient_;
}

void PLCConnection::SetAddress(std::string add) {
	address_ = add;
}

void PLCConnection::SetRack(int rack) {
	rack_ = rack;
}

void PLCConnection::SetSolt(int solt) {
	solt_ = solt;
}

std::string PLCConnection::ReadDataRegion(std::string address, Area area, VARENUM type, int size) {
	int result, offset = -1,MiniOffset = -1;
	unsigned char buffer[4];
	// 解析地址
	std::vector<std::string> parseAdd = Tools::SplitString(address);

	int dbNumber = stoi(parseAdd[1]); // DB块号
	if(parseAdd.size() >= 3) offset = stoi(parseAdd[2]);  // 偏移量
	if (type == VARENUM::VT_BOOL && area == Area::DB) MiniOffset = stoi(parseAdd[3]); // 字节中偏移量

	if (area != Area::DB) {
		MiniOffset = offset;
		offset = dbNumber;
		dbNumber = 0;
	}

	DataRead(area, dbNumber, offset, size, buffer);
	//Tools::PrintAllBits(buffer, 1);
	// 解析buffer中的数据
	return Tools::BinaryConversionOther(buffer, type, MiniOffset);
}

void PLCConnection::DataRead(Area area, int dbNumber, int offset, int size, unsigned char* buffer) {
	int result;
	// 根据area选择不同的读取区域
	if (area == Area::DB) {
		result = myClient_->DBRead(dbNumber, offset, size, buffer);

		//int TS7Client::DBRead(int DBNumber, int Start, int Size, void* pUsrData)
		//{
		//	return Cli_DBRead(Client, DBNumber, Start, Size, pUsrData);
		//}
	}
	else {
		result = myClient_->ReadArea(Tools::getS7Area(area), dbNumber, offset, size, Tools::getS7WL(area), buffer);

		//int TS7Client::ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void* pUsrData)
		//{
		//	return Cli_ReadArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);
		//}
	}
	/*else if (area == Area::M) {
		result = myClient_->ReadArea(S7AreaMK, dbNumber, offset, size, S7WLByte, buffer);
	}
	else if (area == Area::Q) {
		result = myClient_->ReadArea(S7AreaPA, dbNumber, offset, size, S7WLByte, buffer);
	}
	else if (area == Area::I) {
		result = myClient_->ReadArea(S7AreaPE, dbNumber, offset, size, 4, buffer);
	}
	else {
		throw std::invalid_argument("Invalid area specified");
	}*/
	// 检查读取结果
	if (result < 0) {
		throw std::runtime_error("Failed to read data from PLC");
	}
}

void PLCConnection::InitDataform() {
	for (auto& entry : tagInform_) {
		entry.value_ = ReadDataRegion(entry.address_, entry.area_, entry.type_, entry.size_);
	}
}

void PLCConnection::CreateBlocks() {
	int currentStart = -1;
	DataBlock currentBlock;
	std::vector<std::string> splitstr;

	for (auto& tag : tagInform_) {
		splitstr = Tools::SplitString(tag.address_);
		int minioffset = -1;
		if (splitstr.size() == 4) minioffset = stoi(splitstr[3]);
		if (currentStart == -1) {
			currentBlock.area_ = stringToArea[splitstr[0]];
			currentBlock.blockNum_ = stoi(splitstr[1]);
			currentBlock.startOffset_ = stoi(splitstr[2]);
			currentBlock.endOffset_ = tag.size_ + currentBlock.startOffset_;

			tag.blockNum_ = blocks_.size();
			tag.addInBlock_ = 0;
			currentStart = 0;
		}
		else {
			if (stringToArea[splitstr[0]] != currentBlock.area_ || stoi(splitstr[1]) != currentBlock.blockNum_) {
				blocks_.emplace_back(currentBlock);
				currentBlock.area_ = stringToArea[splitstr[0]];
				currentBlock.blockNum_ = stoi(splitstr[1]);
				currentBlock.startOffset_ = stoi(splitstr[2]);
				currentBlock.endOffset_ = tag.size_ + currentBlock.startOffset_;

				tag.blockNum_ = blocks_.size();
				tag.addInBlock_ = 0;
			}
			else {
				if (stoi(splitstr[2]) - currentBlock.startOffset_ + tag.size_ >= 210) {
					blocks_.emplace_back(currentBlock);
					currentBlock.area_ = stringToArea[splitstr[0]];
					currentBlock.blockNum_ = stoi(splitstr[1]);
					currentBlock.startOffset_ = stoi(splitstr[2]);
					currentBlock.endOffset_ = tag.size_ + currentBlock.startOffset_;

					tag.blockNum_ = blocks_.size();
					tag.addInBlock_ = 0;
				}
				else {
					currentBlock.endOffset_ = stoi(splitstr[2]) + tag.size_;
					tag.blockNum_ = blocks_.size();
					tag.addInBlock_ = stoi(splitstr[2]) - currentBlock.startOffset_;
				}
			}
		}
	}
	blocks_.emplace_back(currentBlock);
	return;
}

void PLCConnection::InitBlocks() {
	for (int i = 0; i < blocks_.size(); ++i) {
		DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[1]);
		DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[0]);
	}
}

void PLCConnection::UpdateBlocksBuffer() {
	for (int i = 0; i < blocks_.size(); ++i) {
		DataRead(blocks_[i].area_, blocks_[i].blockNum_, blocks_[i].startOffset_,
			blocks_[i].endOffset_ - blocks_[i].startOffset_, blocks_[i].buffer_[re]);
	}
}

void PLCConnection::TraverseDataforms() {

	for (auto& entry : tagInform_) {
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