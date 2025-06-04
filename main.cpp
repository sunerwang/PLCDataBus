#include "Init.h"
#include "ThreadPool.h"

extern bool bExit;

int main() {

	//PLCConnection plc = PLCConnection("140.32.1.147",0,1);
	//bool i = plc.ConnectPLC();
	//std::cout << i << std::endl;
	//return 0;


	int test;
	//int clock_2Hz, clock_1Hz, clock_0_5Hz;
	//std::ofstream jsonFile("data.json");
	//if (!jsonFile.is_open()) {
	//	std::cerr << "Failed to open data.json!" << std::endl;
	//	return -1;
	//}
	//jsonFile << "[]"; // 初始化空的JSON数组
	//jsonFile.close();
	//std::vector<std::map<std::string, int>> dataPoints;

	//while (1) {
	//	clock_2Hz = std::stoi(plc.ReadDataRegion("M10.3", Area::M, VARENUM::VT_BOOL, 1));
	//	clock_1Hz = std::stoi(plc.ReadDataRegion("M10.5", Area::M, VARENUM::VT_BOOL, 1));
	//	clock_0_5Hz = std::stoi(plc.ReadDataRegion("M10.7", Area::M, VARENUM::VT_BOOL, 1));
	//	std::map<std::string, int> entry = {
	//		{"clock_2Hz", clock_2Hz},
	//		{"clock_1Hz", clock_1Hz},
	//		{"clock_0_5Hz", clock_0_5Hz}
	//	};
	//	dataPoints.push_back(entry);
	//	// 写入整个数组到 JSON 文件
	//	std::ofstream outFile("data.json");
	//	outFile << "[";
	//	for (size_t i = 0; i < dataPoints.size(); ++i) {
	//		if (i != 0) outFile << ",";
	//		outFile << "{\"clock_2Hz\":" << dataPoints[i]["clock_2Hz"]
	//			<< ",\"clock_1Hz\":" << dataPoints[i]["clock_1Hz"]
	//			<< ",\"clock_0_5Hz\":" << dataPoints[i]["clock_0_5Hz"] << "}";
	//	}
	//	outFile << "]";
	//	outFile.close();

	//	test = std::stoi(plc.ReadDataRegion("MW6", Area::MW, VARENUM::VT_I2, 1));
	//	std::cout << test << std::endl;
	//	Tools::PreciseSleep(500000000);
	//
	//}
	//std::cout << i << std::endl;
	//return 0;
	int ThreadNum = 3;
	ThreadPool pool(ThreadNum);
	Init* init = new Init(); // 一份profile文件对应一个Init对象

	//init->Run("testprofile.txt");

	pool.enqueue(&Init::Run, init, std::ref("testprofile.txt"));
	//pool.enqueue(&Tools::CheckForEscExit);
	//pool.enqueue(&Init::InquireData, init);



	while (1) {

		if (_kbhit()) { // 检测是否有按键按下
			char key = _getch(); // 获取按下的键
			if (key == 27) { // 27 是 Esc 键的 ASCII 码
				//std::cout << "检测到 Esc 键，退出程序。" << std::endl;
				//exit(0); // 退出程序
				bExit = true;
				Sleep(2000);
				break;
			}
		}
	}

	delete init;

	return 0;
}