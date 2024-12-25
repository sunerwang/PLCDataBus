#include "Init.h"
#include "ThreadPool.h"

int main() {

	int ThreadNum = 3;
	Init* init = Init::GetInstance();
	init->InitTargetData();
	ThreadPool pool(ThreadNum);

	pool.enqueue(&Init::ReadDataInform, init, std::ref(init->blocks), std::ref(init->dataInform));
	pool.enqueue(&Tools::checkForEscExit);

	return 0;
}