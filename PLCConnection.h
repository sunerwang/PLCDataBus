#pragma once
#include "snap7.h"
#include <iostream>
#include "Tools.h"
#include "DataBlock.h"
#include "DataInform.h"

class PLCConnection {
public:
	PLCConnection();
	PLCConnection(std::string, int rack, int solt);
	~PLCConnection();

	bool ConnectPLC();  // ����PLC����������״̬
	bool Disconnection();  // �Ͽ����ӷ��ز����Ƿ�ɹ�
	TS7Client* GetClient(); // ��ȡ�ͻ���

	// Set ����
	void SetAddress(std::string add);
	void SetRack(int rack);
	void SetSolt(int solt);

	// ��ȡ������
	std::string ReadDataRegion(std::string address, Area area, VARENUM type, int size);
	void DataRead(Area area, int dbNumber, int offset, int size, unsigned char* buffer);

private:
	TS7Client* myClient_;
	std::string address_; // PLC��ַ
	int rack_;   // ����
	int solt_;   // �ۺ�

	//string--Area ӳ���
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
		{"MD",Area::MD}
	};

	//string--VARENUM ӳ���
	std::map<std::string, VARENUM> stringToVARENUM = {
		{"BOOL",VARENUM::VT_BOOL},
		{"INT",VARENUM::VT_I2},
		{"DINT",VARENUM::VT_I4},
		{"REAL",VARENUM::VT_R4}
	};

	int re = 0;

public:
	std::vector<DataBlock> blocks_;			// ���ݿ黮��
	std::vector<DataInform> tagInform_;		// TAG��Ϣ

	//mark TAG��ϢӦ����Ϊȫ�֣�����дPLCʱ���֪����TAG�����ĸ�PLC��
	std::unordered_map<std::string, int> tagIndex_;	// TAG����--�±�����

	void TriggerFirstRead(); // ��ʼ������
	void CreateBlocks(); // �������ݿ�
	void InitBlocks();	 // ��ʼ�����ݿ�
	void UpdateBlocksBuffer();  // �������ݻ�����
	void TraverseDataforms();   // �������ݵ�

	void SwitchBuffer() { re = re ^ 1; }
};
