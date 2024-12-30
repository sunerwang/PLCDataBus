#pragma once
#include <string>
#include "Tools.h"

#define MaxBufferLength 210

class DataBlock {
public:
	DataBlock();
	DataBlock(Area area, int blockNum, int startOffset, int endOffset);
	~DataBlock();
	
	Area area_; // DB、M、I、Q
	int blockNum_;  // 块号
	int startOffset_;  // 初始偏移量
	int endOffset_;  // 结束偏移量
	unsigned char buffer_[2][MaxBufferLength]; // 缓存区
};