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
	myClient_->ConnectTo(address_.data(), rack_, solt_);
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
	int result, MiniOffset = -1;
	unsigned char buffer[4];
	// 解析地址
	std::vector<std::string> parseAdd = Tools::SplitString(address);

	int dbNumber = stoi(parseAdd[1]); // DB块号
	int offset = stoi(parseAdd[2]);  // 偏移量
	if (type == VARENUM::VT_BOOL && area == Area::DB) MiniOffset = stoi(parseAdd[3]); // 字节中偏移量

	DataRead(area, dbNumber, offset, size, buffer);
	// 解析buffer中的数据
	return Tools::BinaryConversionOther(buffer, type, MiniOffset);
}

void PLCConnection::DataRead(Area area, int dbNumber, int offset, int size, unsigned char* buffer) {
	int result;
	// 根据area选择不同的读取区域
	if (area == Area::DB) {
		result = myClient_->DBRead(dbNumber, offset, size, buffer);
	}
	else if (area == Area::M) {
		result = myClient_->ReadArea(S7AreaMK, dbNumber, offset, size, 1, buffer);
	}
	else if (area == Area::Q) {
		result = myClient_->ReadArea(S7AreaPA, dbNumber, offset, size, 1, buffer);
	}
	else if (area == Area::I) {
		result = myClient_->ReadArea(S7AreaPE, dbNumber, offset, size, 1, buffer);
	}
	else {
		throw std::invalid_argument("Invalid area specified");
	}
	// 检查读取结果
	if (result < 0) {
		throw std::runtime_error("Failed to read data from PLC");
	}
}

void PLCConnection::InitDataform() {
	for (auto& entry : dataInform_) {
		entry.value_ = ReadDataRegion(entry.address_, entry.area_, entry.type_, entry.size_);
	}
}

void PLCConnection::CreateBlocks() {
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