// SnakeAI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <map>
#include <Windows.h>

#include "FileRecorder.h"
#include "Environment.h"
#include "RLIteration.h"



int main(int argc,char *argv[])
{
	FileRecorder* pRecorder = NULL;
	FILE* fp = NULL;
	int a;
	int round = 0;
	bool bLoad = false;
	DWORD dwT1, dwT2;
	RLIteration rl(3);
	if (argc > 1)
	{
		fp = fopen(argv[1], "r");
		if (fp)
			pRecorder = new FileRecorder(fp);
	}
	if (fp)
	{
		bLoad = rl.Load(pRecorder);
		delete pRecorder;
		fclose(fp);
	}
	dwT1 = GetTickCount();
	if(!bLoad)
	{
		
		rl.InterationWithModel(60, 0.0);
		dwT2 = GetTickCount();
		printf("cost time:%u\n", dwT2 - dwT1);
		if (argc > 1)
		{
			fp = fopen(argv[1], "w");
			if (fp)
			{
				pRecorder = new FileRecorder(fp);
				rl.Save(pRecorder);
				delete pRecorder;
				fclose(fp);
			}
		}
	}
	srand(dwT1);
	Environment env(3, 3);
	printf("First state:\n");
	env.Print();
	while (!env.IsTerminated())
	{
		a = rl.GetAction(env);
		if (a < 0)
			break;
		round++;
		if (env.Step(a))
			round = 0;
		printf("Now:\n");
		env.Print();
		if (round >= 10)
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
