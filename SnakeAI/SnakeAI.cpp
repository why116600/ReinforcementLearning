// SnakeAI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <map>
#include <Windows.h>

#include "RL_DNN.h"
#include "FileRecorder.h"
#include "Environment.h"
#include "RLIteration.h"
#include "TDTrainer.h"


void TrainWithTD(TDTrainer& td, int nSize, double ebsilon, double lr, int nIter = -1)
{
	int maxr = 0;
	int r;
	int n;
	int b=1;
	std::vector<Environment> envs;
	for (int i = 0; i != nIter; i++)
	{
		n = 0;
		//b = (i % (nSize * nSize - 3)) + 3;
		//Environment::AllSpecificSnake(envs, nSize, nSize, b);
		Environment::AllInitiations(envs, nSize, nSize);
		for (int j = 0; j < (int)envs.size(); j++)
		{
			//r = td.QLearn(envs[j], ebsilon, lr);
			//r = td.ExpectSARSA(envs[j], ebsilon, lr);
			r = td.DoubleQLearn(envs[j], ebsilon, lr);
			if ((r+b) > maxr)
			{
				maxr = r+b;
				printf("Got progress[%d] at iteration %d\n", r+b, i);
			}
			else if ((r+b) >= (nSize * nSize))
			{
				//printf("Reach final at (%d,%d)\n", i, j);
				n++;
			}
		}
		if (n > 0)
		{
			printf("Iteration %d has %d final\n", i, n);
		}
		envs.clear();
	}
}

void TrainWithDNN(RL_DNN& dnn, double ebsilon, int nIter = -1)
{
	int maxr = 0;
	int r;
	int n;
	int b = 1;
	std::vector<Environment> envs;
	for (int i = 0; i != nIter; i++)
	{
		n = 0;
		Environment::AllInitiations(envs, 3, 3);
		for (int j = 0; j < (int)envs.size(); j++)
		{
			r = dnn.MonteCarloWithDNN(envs[j], ebsilon, -1);
			//r = dnn.SARSAWithDNN(envs[j], ebsilon);
			//r = dnn.DQN(envs[j], 1, ebsilon);
			if ((r + b) > maxr)
			{
				maxr = r + b;
				printf("Got progress[%d] at iteration %d\n", r + b, i);
			}
			else if ((r + b) >= 9)
			{
				//printf("Reach final at (%d,%d)\n", i, j);
				n++;
			}
		}
		if (n > 0)
		{
			printf("Iteration %d has %d final\n", i, n);
		}
		envs.clear();
	}

}

int main(int argc,char *argv[])
{
	const double ebsilon = 0.2;
	const double learn_rate = 0.1;
	FileRecorder* pRecorder = NULL;
	FILE* fp = NULL;
	int a;
	int round = 0;
	bool bLoad = false;
	DWORD dwT1, dwT2;
	RLIteration rl(3);
	TDTrainer td;
	RL_DNN dnn(3);
	if (argc > 1)
	{
		fp = fopen(argv[1], "rb");
		if (fp)
			pRecorder = new FileRecorder(fp);
	}
	if (fp)
	{
		bLoad = td.Load(pRecorder);
		delete pRecorder;
		fclose(fp);
	}
	if (!dnn.ConnectNN(TEXT("\\\\.\\pipe\\LiudadaNemaedPipe")))
	{
		printf("Cannot open pipe!\n");
		return -1;
	}
	dwT1 = GetTickCount();
	srand(dwT1);
	if(!bLoad)
	{
		
		//rl.InterationWithModel(60, 0.0);
		//TrainWithTD(td, 3, ebsilon, learn_rate, 30);
		TrainWithDNN(dnn, ebsilon, 300);
		dwT2 = GetTickCount();
		printf("cost time:%u\n", dwT2 - dwT1);
		if (argc > 1)
		{
			fp = fopen(argv[1], "wb");
			if (fp)
			{
				pRecorder = new FileRecorder(fp);
				td.Save(pRecorder);
				delete pRecorder;
				fclose(fp);
			}
		}
	}
	Environment env(3, 3);
	printf("First state:\n");
	env.Print();
	while (!env.IsTerminated())
	{
		//a = rl.GetAction(env);
		//a = td.GetAction(env, 0.0);
		a = dnn.GetAction(env, 0.0);
		if (a < 0)
			break;
		round++;
		if (env.Step(a))
			round = 0;
		printf("Now:\n");
		env.Print();
		if (round >= 100)
		{
			printf("Invalid round!\n");
			break;
		}
	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
