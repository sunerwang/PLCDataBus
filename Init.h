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
	void Run(std::string file);  // 执行
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
	std::vector<std::unique_ptr<PLCConnection>> plc_; // PLC信息

};