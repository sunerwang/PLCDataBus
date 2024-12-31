#include "Init.h"
#include "ThreadPool.h"

extern bool bExit;

int main() {

	//PLCConnection plc = PLCConnection("140.32.1.141",0,1);
	//bool i = plc.ConnectPLC();
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