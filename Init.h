#pragma once
#include<algorithm>
#include "DataBlock.h"
#include"DataInform.h"
#include "PLCConnection.h"
#include "Tools.h"

class Init {

public:

	Init();
	~Init();
	void ReadProfile(std::string); // 读取配置文件

	//void InitDataform(); // 初始化数据
	//void CreateBlocks(); // 创建数据块
	//void InitBlocks();	 // 初始化数据块
	//void UpdateBlocksBuffer(int re);  // 更新数据缓冲区
	//void TraverseDataforms(int re);   // 遍历数据点

	void Run(std::string file);  // 执行上述函数
	void InquireData();   // 查询数据

	//string--Area 映射表
	std::map<std::string, Area> stringToArea = {
		{"DB",Area::DB},
		{"I",Area::I},
		{"Q",Area::Q},
		{"M",Area::M}
	};

	//string--VARENUM 映射表
	std::map<std::string, VARENUM> stringToVARENUM = {
		{"BOOL",VARENUM::VT_BOOL},
		{"INT",VARENUM::VT_I2},
		{"DINT",VARENUM::VT_I4},
		{"REAL",VARENUM::VT_R4}
	};
private:
	std::vector<PLCConnection> plc_; // PLC信息
	//std::vector<DataBlock> blocks_;  // 数据块划分
	//std::vector<DataInform> dataInform_; // 数据点信息
	//std::map<std::string, int> tagsMap_; // 数据名称--下标索引

};
