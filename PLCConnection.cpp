#include "PLCConnection.h"

PLCConnection::PLCConnection()
	:myClient_(new TS7Client), address_("127.0.0.1"), rack_(0), solt_(1) {}

PLCConnection::PLCConnection(std::string address, int rack, int solt)
	:myClient_(new TS7Client), address_(address), rack_(rack), solt_(solt){}

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