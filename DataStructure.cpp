#include "DataStructure.h"

// 默认构造
DataInform::DataInform() : address(""), area(""), type(""), size(0){}
// 带参构造
DataInform::DataInform(std::string address, std::string area, std::string type)
    : address(address), area(area), type(type),size(GetSize(type)) {
}

int DataInform::GetSize(std::string type) {
    std::map<std::string, int> SizeTable;
    SizeTable["BOOL"] = 1;
    SizeTable["INT"] = 2;
    SizeTable["DINT"] = 4;
    SizeTable["REAL"] = 4;
    SizeTable["NUMERIC"] = 4;
    return SizeTable[type];
}
int DataInform::GetOffset(std::string address){
	return std::stoi(Tools::splitString(address)[2]);
}
int DataInform::GetMiniOffset(std::string address){
	std::vector<std::string> result = Tools::splitString(address);
	if (result.size() == 4) return std::stoi(result[3]);
	else return -1;
}

// 赋值操作符
DataInform& DataInform::operator=(const DataInform& other) {
    if (this != &other) {
        address = other.address;
        area = other.area;
        type = other.type;
        size = other.size;
    }
    return *this;
}