#pragma once

#include <vector>

#include <Windows.h>
//基于管道的神经网络
class PipeNN
{
private:
	int m_iError;
	HANDLE m_hPipe;
private:
	// 读取指定大小的管道数据
	bool ReadData(const void* pBuf, DWORD dwSize);
	// 获取张量的形状
	bool GetShape(int msgType, std::vector<int>& shapes);
	// 获取双精度数组
	bool GetDoubleArray(std::vector<double>& arr);
	// 发送双精度数组
	bool SendDoubleArray(double arr[], int length);
public:
	PipeNN();
	~PipeNN();
	// 打开管道，样例名称TEXT("\\\\.\\pipe\\LiudadaNemaedPipe")
	bool OpenPipe(LPCTSTR szPipeName);
	// 关闭管道
	void ClosePipe();
	// 获取上一个操作的错误
	int GetErrorCode();
	// 获取网络输入的张量形状
	bool GetInputShape(std::vector<int>& shapes);
	// 获取网络输出的张量形状
	bool GetOutputShape(std::vector<int>& shapes);
	// 发送一组训练数据
	bool SendTrainData(double xdata[], int xlen, double ydata[], int ylen);
	bool StartTraining();
	// 预测数据
	bool Predict(double xdata[], int xlen, double ydata[], int ylen);
};

