#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

class RLIteration
{
private:
	int m_nSize;
	std::map<Environment, std::pair<double,int>> m_value_policy;//״̬��ֵ�����
private:
	void SetValue(const Environment& env, double v);
	void SetAction(const Environment& env, int a);
public:
	RLIteration(int nSize);
	~RLIteration();
	// ��ȡĿ�껷����ǰ״̬��״̬��ֵ
	double GetValue(const Environment& env);
	// ���ݵ�ǰ״̬��ȡҪ���Ķ���
	int GetAction(const Environment& env);
	// ��ģ�͵ļ�ֵ����
	void InterationWithModel(int nIter=-1, double fTolerance=0.1);
	// ��ѧϰ�����������
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
};

