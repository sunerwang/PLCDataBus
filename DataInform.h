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

    // Ĭ�Ϲ��캯��
    DataInform();

    // �������Ĺ��캯��
    DataInform(std::string name, std::string address, Area area, VARENUM type);

    // Get ����
    int GetSize(VARENUM type);
    int GetOffset(std::string address);
    int GetMiniOffset(std::string address);

    // ��ַ����
    static bool Compare(const DataInform& a, const DataInform& b);


    // ��ֵ������
    DataInform& operator=(const DataInform& other);
};

#endif // DATASTRUCTURE_H