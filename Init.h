#pragma once
#include <algorithm>
#include "DataBlock.h"
#include "DataInform.h"
#include "PLCConnection.h"
#include "Tools.h"

class Init {

public:
	Init();
	~Init();
	void ReadProfile(std::string);	// 读取配置文件
	void Run(std::string file);		// 执行
	void InquireData();				// 查询数据

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
		{"MD",Area::MD},
	};

	//string--VARENUM 映射表
	std::unordered_map<std::string, VARENUM> stringToVARENUM = {
		{"BOOL",VARENUM::VT_BOOL},
		{"INT",VARENUM::VT_I2},
		{"DINT",VARENUM::VT_I4},
		{"ULINT",VARENUM::VT_UI8},
		{"UINT",VARENUM::VT_UI2},
		{"UDINT",VARENUM::VT_UI4},
		{"LWORD",VARENUM::VT_UI8},
		{"REAL",VARENUM::VT_R4},
		{"DOUBLE",VARENUM::VT_R8},
		{"STRING",VARENUM::VT_BSTR},
		//{"S5TIME",VARENUM::VT_DATE},
	};
private:
	std::vector<std::unique_ptr<PLCConnection>> plc_; // PLC信息
};