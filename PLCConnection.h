#pragma once
#include "snap7.h"
#include <iostream>
#include "Tools.h"

class PLCConnection {
public:
	PLCConnection();
	PLCConnection(std::string, int rack, int solt);
	~PLCConnection();

	bool ConnectPLC();  // 连接PLC并返回连接状态
	bool Disconnection();  // 断开连接返回操作是否成功
	TS7Client* GetClient(); // 获取客户端

	// Set 方法
	void SetAddress(std::string add);
	void SetRack(int rack);
	void SetSolt(int solt);

	// 读取单个点
	std::string ReadDataRegion(std::string address, Area area, VARENUM type, int size);
	void DataRead(Area area, int dbNumber, int offset, int size, unsigned char* buffer);

private:
	TS7Client* myClient_;
	std::string address_; // PLC地址
	int rack_;   // 导轨
	int solt_;   // 槽号
};
