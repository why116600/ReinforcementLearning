#pragma once

#include <set>
#include <vector>

#include "PipeNN.h"
#include "Environment.h"
#include "ExperimentItem.h"

class RL_DNN
{
private:
	PipeNN m_nn;
	int m_nSize;
	std::set<ExperimentItem> m_expers;
	std::vector<ExperimentItem> m_aExper;
private:
	// ����ĳ������ֵ
	bool UpdateValue(const Environment& env, int action, double value);
public:
	RL_DNN(int nSize);
	~RL_DNN();
	// ���ӹܵ�������
	bool ConnectNN(LPCTSTR szPipeName);
	// �رչܵ�������
	void CloseNN();
	// ��ȡ������ֵ
	bool GetValue(const Environment& env, double ret[],int len);
	// ��ȡ��ǰҪ���Ķ���
	int GetAction(const Environment& env, double ebsilon);
	// ʹ����������SARSA�㷨
	int SARSAWithDNN(Environment& env,double ebsilon);
	// ���Qѧϰ
	int DQN(Environment& env, int nPlayback, double ebsilon);
	// ʹ�������������ؿ���
	int MonteCarloWithDNN(Environment& env, double ebsilon, int nIter=-1);
};

