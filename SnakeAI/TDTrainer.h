#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

//ʱ���ַ���ѧϰ
class TDTrainer
{
private:
	std::map<std::pair<Environment, int>, double> m_action_value;//������ֵ
private:
	// ����lr��ѧϰ�ʸ��¶�����ֵ
	void UpdateActionValue(const Environment& env, int action, double value, double lr);
public:
	TDTrainer();
	~TDTrainer();
	// ��ȡ������ֵ
	double GetValue(const Environment& env, int action);
	// ebsilon̰�Ĳ��Ի�ȡ����
	int GetAction(const Environment& env, double ebsilon);
	// ����SARSAʱ�����㷨
	int ExpectSARSA(Environment& env, double ebsilon, double lr,int maxstep=-1);
	// ��ѧϰ�����������
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
};

