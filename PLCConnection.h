#pragma once
#include "snap7.h"
#include <iostream>
#include "Tools.h"
#include "DataBlock.h"
#include "DataInform.h"

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

	//string--Area 映射表
	std::map<std::string, Area> stringToArea = {
		{"DB",Area::DB},
		{"I",Area::I},
		{"Q",Area::Q},
		{"M",Area::M},
		{"IB",Area::IB},
		{"QB",Area::QB},
		{"MB",Area::MB},
		{"IW",Area::IW},
		{"QW",Area::QW},
		{"MW",Area::MW},
		{"ID",Area::ID},
		{"QD",Area::QD},
		{"MD",Area::MD}
	};

	//string--VARENUM 映射表
	std::map<std::string, VARENUM> stringToVARENUM = {
		{"BOOL",VARENUM::VT_BOOL},
		{"INT",VARENUM::VT_I2},
		{"DINT",VARENUM::VT_I4},
		{"REAL",VARENUM::VT_R4}
	};

	int re = 0;

public:
	std::vector<DataBlock> blocks_;			// 数据块划分
	std::vector<DataInform> tagInform_;		// TAG信息

	//mark TAG信息应保存为全局，否则当写PLC时如何知道此TAG是在哪个PLC里
	std::unordered_map<std::string, int> tagIndex_;	// TAG名称--下标索引

	void TriggerFirstRead(); // 初始化数据
	void CreateBlocks(); // 创建数据块
	void InitBlocks();	 // 初始化数据块
	void UpdateBlocksBuffer();  // 更新数据缓冲区
	void TraverseDataforms();   // 遍历数据点

	void SwitchBuffer() { re = re ^ 1; }
};
