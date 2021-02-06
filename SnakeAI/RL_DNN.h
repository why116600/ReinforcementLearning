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
	// 更新某动作价值
	bool UpdateValue(const Environment& env, int action, double value);
public:
	RL_DNN(int nSize);
	~RL_DNN();
	// 连接管道神经网络
	bool ConnectNN(LPCTSTR szPipeName);
	// 关闭管道神经网络
	void CloseNN();
	// 获取动作价值
	bool GetValue(const Environment& env, double ret[],int len);
	// 获取当前要做的动作
	int GetAction(const Environment& env, double ebsilon);
	// 使用深度网络的SARSA算法
	int SARSAWithDNN(Environment& env,double ebsilon);
	// 深度Q学习
	int DQN(Environment& env, int nPlayback, double ebsilon);
	// 使用深度网络的蒙特卡罗
	int MonteCarloWithDNN(Environment& env, double ebsilon, int nIter=-1);
};

