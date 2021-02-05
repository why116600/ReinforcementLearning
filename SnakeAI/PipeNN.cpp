#include "PipeNN.h"



PipeNN::PipeNN()
	:m_hPipe(INVALID_HANDLE_VALUE)
	, m_iError(0)
{
}


PipeNN::~PipeNN()
{
}


// �򿪹ܵ�����������TEXT("\\\\.\\pipe\\LiudadaNemaedPipe")
bool PipeNN::OpenPipe(LPCTSTR szPipeName)
{
	//DWORD dwMode = PIPE_READMODE_BYTE;
	if (!WaitNamedPipe(szPipeName, 1000))
		return false;
	return (m_hPipe=CreateFile(szPipeName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL))!=INVALID_HANDLE_VALUE;
}


// �رչܵ�
void PipeNN::ClosePipe()
{
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}

// ��ȡָ����С�Ĺܵ�����
bool PipeNN::ReadData(const void* pBuf, DWORD dwSize)
{
	DWORD dwRead = 0;
	DWORD s;
	char* buffer = (char*)pBuf;
	while (dwRead < dwSize)
	{
		if (!ReadFile(m_hPipe, &buffer[dwRead], dwSize - dwRead, &s, NULL))
		{
			ClosePipe();
			return false;
		}
		dwRead += s;
	}
	return true;
}


// ��ȡ��һ�������Ĵ���
int PipeNN::GetErrorCode()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	return m_iError;
}

// ��ȡ��������״
bool PipeNN::GetShape(int msgType, std::vector<int>& shapes)
{
	if (m_hPipe == INVALID_HANDLE_VALUE)
		return false;
	int n, m, t;
	int arr[100];
	DWORD dwSize;
	if (!WriteFile(m_hPipe, &msgType, sizeof(msgType), &dwSize, NULL))
	{
		ClosePipe();
		return false;
	}
	if (!ReadData(&t, sizeof(t)))
		return false;
	if (t != msgType)
	{
		if (t >= 0)
		{
			ClosePipe();
			return false;
		}
		m_iError = t;
		return false;
	}
	if (!ReadData(&n, sizeof(n)))
		return false;
	for (int i = 0; i < n; i++)
	{
		if (!ReadData(&m, sizeof(m)))
			return false;
		shapes.push_back(m);
	}
	m_iError = 0;
	return true;
}

// ��ȡ���������������״
bool PipeNN::GetInputShape(std::vector<int>& shapes)
{
	return GetShape(1, shapes);
}


// ��ȡ���������������״
bool PipeNN::GetOutputShape(std::vector<int>& shapes)
{
	return GetShape(2, shapes);
}



// ��ȡ˫��������
bool PipeNN::GetDoubleArray(std::vector<double>& arr)
{
	int n;
	double f;
	if (!ReadData(&n, sizeof(n)))
		return false;
	for (int i = 0; i < n; i++)
	{
		if (!ReadData(&f, sizeof(f)))
			return false;
		arr.push_back(f);
	}
	return true;
}


// ����˫��������
bool PipeNN::SendDoubleArray(double arr[], int length)
{
	DWORD dw;
	if (!WriteFile(m_hPipe, &length, sizeof(length), &dw, NULL))
	{
		ClosePipe();
		return false;
	}
	if (!WriteFile(m_hPipe, arr, sizeof(double)*length, &dw, NULL))
	{
		ClosePipe();
		return false;
	}
	return true;
}


// ����һ��ѵ������
bool PipeNN::SendTrainData(double xdata[], int xlen, double ydata[], int ylen)
{
	int msgType = 3;
	int res;
	DWORD dw;
	if (!WriteFile(m_hPipe, &msgType, sizeof(msgType), &dw, NULL))
	{
		ClosePipe();
		return false;
	}
	if (!SendDoubleArray(xdata, xlen))
		return false;
	if (!SendDoubleArray(ydata, ylen))
		return false;
	if (!ReadData(&res, sizeof(res)))
		return false;
	if (res != msgType)
	{
		if (res >= 0)
		{
			ClosePipe();
			return false;
		}
		m_iError = res;
		return false;
	}
	return true;
}


bool PipeNN::StartTraining()
{
	int msgType = 4;
	int res;
	DWORD dw;
	if (!WriteFile(m_hPipe, &msgType, sizeof(msgType), &dw, NULL))
	{
		ClosePipe();
		return false;
	}
	if (!ReadData(&res, sizeof(res)))
		return false;
	if (res != msgType)
	{
		if (res >= 0)
		{
			ClosePipe();
			return false;
		}
		m_iError = res;
		return false;
	}
	return true;
}


// Ԥ������
bool PipeNN::Predict(double xdata[], int xlen, double ydata[], int ylen)
{
	int msgType = 5;
	int res;
	DWORD dw;
	std::vector<double> result;
	if (!WriteFile(m_hPipe, &msgType, sizeof(msgType), &dw, NULL))
	{
		ClosePipe();
		return false;
	}
	if (!SendDoubleArray(xdata, xlen))
		return false;
	if (!ReadData(&res, sizeof(res)))
		return false;
	if (res != msgType)
	{
		if (res >= 0)
		{
			ClosePipe();
			return false;
		}
		m_iError = res;
		return false;
	}
	if (!GetDoubleArray(result))
		return false;
	for (int i = 0; i < (int)result.size() && i < ylen; i++)
		ydata[i] = result[i];
	return true;
}
