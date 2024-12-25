#pragma once
#include<algorithm>
#include"snap7.h"
#include"DataStructure.h"
#include "Tools.h"

struct MessagePLC
{
	std::string address; // PLC地址
	int Rack;   // 导轨
	int Solt;   // 槽号
};
struct DataBlock {
	std::string area; // DB、M、I、Q
	int blockNum;  // 块号
	int startOffset;  // 初始偏移量
	int endOffset;  // 结束偏移量
	// pair<pair<偏移量，位偏移量>，数据名>
	// std::vector<std::pair<std::pair<int, int>, std::string>> BlockData;
	unsigned char buffer[2][210]; // 缓存区
};

class Init {

public:

	static Init* GetInstance(); // 获取单例对象指针
	void deleteInstance();  // 删除指针对象
	void InitPLCData(MessagePLC* myPLC, std::map<std::string, DataInform>& data);  // 初始化PLC数据
	void InitTargetData();  // 初始化目标数据
	// 地址排序规则
	static bool compare(const std::pair<std::string, DataInform>& a, const std::pair<std::string, DataInform>& b);
	std::string ReadDataRegion(std::string address, std::string type, int size);  // 单点数据读取
	// 根据不同区域读取数据
	void DataRead(std::string area, int dbNumber, int offset, int size, unsigned char* buffer);  
	std::string BinaryConversionOther(unsigned char* buffer, std::string type, int offset);  // 解析数据
	// 根据已排序地址创建块
	std::vector<DataBlock> createBlocks(const std::vector<std::pair<std::string, DataInform>>& sortedData);
	// 轮询数据
	void ReadDataInform(std::vector<DataBlock> blocks, std::map<std::string, DataInform> dataInform);
	//std::vector<std::vector<DataBlock>> splitBlocks(std::vector<DataBlock> blocks, int maxThreadNum); // 分割块
	//void ReadBlockData(DataBlock block);  // 块数据读取
	//void ReadMoreBlockData(std::vector<DataBlock> blocks);

	MessagePLC* plc;  // plc信息
	TS7Client* myClient;  // snap7客户端
	std::vector<DataBlock> blocks;  // 分割出的块
	std::map<std::string, DataInform> dataInform;    // 数据信息
	std::map<std::string, std::string> FinalData;    // 当前数据值
	// std::vector<std::vector<DataBlock>> sBlocks;

private:
	Init();
	~Init();
	// 禁止外部拷贝构造
	Init(const Init& single) = delete;
	// 禁止外部赋值操作
	const Init& operator=(const Init& single) = delete;

private:
	static Init* g_pInit;
};
