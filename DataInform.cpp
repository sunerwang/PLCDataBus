#include "DataInform.h"

// 默认构造
DataInform::DataInform() : address_(""), area_(), type_(), size_(0),
		blockNum_(-1),addInBlock_(-1),value_("") {}
// 带参构造
DataInform::DataInform(std::string name, std::string address, Area area, VARENUM type)
    : name_(name),address_(address), area_(area), type_(type),size_(GetSize(type)),
	blockNum_(-1), addInBlock_(-1), value_("") {
}

int DataInform::GetSize(VARENUM type) {
    
    return Tools::TypeSize_[static_cast<int>(type)];
}

int DataInform::GetOffset(std::string address){
	return std::stoi(Tools::SplitAddressString(address)[2]);
}

int DataInform::GetMiniOffset(std::string address){
	std::vector<std::string> result = Tools::SplitAddressString(address);
	if (result.size() == 4) return std::stoi(result[3]);
	//else if(result[0] == "M") return std::stoi(result[2]);
	else return -1;
}

//mark 待优化
bool DataInform::Compare(const DataInform& a, const DataInform& b) {
	std::vector<std::string> resultA = Tools::SplitAddressString(a.address_);
	std::vector<std::string> resultB = Tools::SplitAddressString(b.address_);

	if (resultA[0] == resultB[0]) {
		if (resultA[1] == resultB[1]) {
			if (resultA[2] == resultB[2]) {
				return std::stoi(resultA[3]) < std::stoi(resultB[3]);
			}
			else {
				return std::stoi(resultA[2]) < std::stoi(resultB[2]);
			}
		}
		else {
			return std::stoi(resultA[1]) < std::stoi(resultB[1]);
		}
	}
	else {
		return resultA[0] < resultB[0];
	}
}

// 赋值操作符
DataInform& DataInform::operator=(const DataInform& other) {
    if (this != &other) {
        address_ = other.address_;
        area_ = other.area_;
        type_ = other.type_;
        size_ = other.size_;
    }
    return *this;
}