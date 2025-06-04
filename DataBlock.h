#pragma once
#include <string>
#include "Tools.h"

#define MaxBufferLength 210

class DataBlock {
public:
	DataBlock();
	DataBlock(Area area, int blockNum, int startOffset, int endOffset);
	~DataBlock();
	
	Area area_;			// DB��M��I��Q
	int blockNum_;		// ���
	int startOffset_;	// ��ʼƫ����
	int endOffset_;		// ����ƫ����
	unsigned char buffer_[2][MaxBufferLength]; // ������

private:
	int re = 0;
};