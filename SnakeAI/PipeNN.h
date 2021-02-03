#pragma once

#include <vector>

#include <Windows.h>
//���ڹܵ���������
class PipeNN
{
private:
	int m_iError;
	HANDLE m_hPipe;
private:
	// ��ȡָ����С�Ĺܵ�����
	bool ReadData(const void* pBuf, DWORD dwSize);
	// ��ȡ��������״
	bool GetShape(int msgType, std::vector<int>& shapes);
	// ��ȡ˫��������
	bool GetDoubleArray(std::vector<double>& arr);
	// ����˫��������
	bool SendDoubleArray(double arr[], int length);
public:
	PipeNN();
	~PipeNN();
	// �򿪹ܵ�����������TEXT("\\\\.\\pipe\\LiudadaNemaedPipe")
	bool OpenPipe(LPCTSTR szPipeName);
	// �رչܵ�
	void ClosePipe();
	// ��ȡ��һ�������Ĵ���
	int GetErrorCode();
	// ��ȡ���������������״
	bool GetInputShape(std::vector<int>& shapes);
	// ��ȡ���������������״
	bool GetOutputShape(std::vector<int>& shapes);
	// ����һ��ѵ������
	bool SendTrainData(double xdata[], int xlen, double ydata[], int ylen);
	bool StartTraining();
	// Ԥ������
	bool Predict(double xdata[], int xlen, double ydata[], int ylen);
};

