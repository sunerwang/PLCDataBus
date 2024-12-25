#pragma once
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "Tools.h"

class DataInform {
public:
    std::string address;
    std::string area;
    std::string type;
    int size;

    int blockNum;
    int addInBlock;

    // 默认构造函数
    DataInform();

    // 带参数的构造函数
    DataInform(std::string address, std::string area, std::string type);

    // Get 方法
    int GetSize(std::string type);
    int GetOffset(std::string address);
    int GetMiniOffset(std::string address);


    // 赋值操作符
    DataInform& operator=(const DataInform& other);
};

#endif // DATASTRUCTURE_H