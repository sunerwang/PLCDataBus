#include "DataBlock.h"

DataBlock::DataBlock()
	:area_(), blockNum_(-1), startOffset_(-1), endOffset_(-1) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 210; ++j) {
            buffer_[i][j] = 0; // 默认值
        }
    }
}

DataBlock::DataBlock(Area area, int blockNum, int startOffset, int endOffset)
	:area_(area), blockNum_(blockNum),startOffset_(startOffset),endOffset_(endOffset){
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 210; ++j) {
            buffer_[i][j] = 0; // 默认值
        }
    }
}

DataBlock::~DataBlock() {
    
}