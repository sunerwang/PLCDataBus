#pragma once
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include "Tools.h"
#include <variant>

class DataInform {
public:
    std::string name_;
    std::string address_;
    Area area_;
    VARENUM type_;
    int size_;

    int blockNum_;
    int addInBlock_;
     
    std::string value_;

    // 默认构造函数
    DataInform();

    // 带参数的构造函数
    DataInform(std::string name, std::string address, Area area, VARENUM type);

    // Get 方法
    int GetSize(VARENUM type);
    int GetOffset(std::string address);
    int GetMiniOffset(std::string address);

    // 地址排序
    static bool Compare(const DataInform& a, const DataInform& b);


    // 赋值操作符
    DataInform& operator=(const DataInform& other);
};

#endif // DATASTRUCTURE_H