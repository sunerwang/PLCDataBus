#include "Init.h"
#include "ThreadPool.h"

int main() {

	int ThreadNum = 3;
	ThreadPool pool(ThreadNum);
	Init* init = new Init(); // 一份profile文件对应一个Init对象

	pool.enqueue(&Init::Run, init, std::ref("testprofile.txt"));
	pool.enqueue(&Tools::CheckForEscExit);
	//pool.enqueue(&Init::InquireData, init);

	return 0;
}