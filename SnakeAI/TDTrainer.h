#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

//时序差分法的学习
class TDTrainer
{
private:
	std::map<std::pair<Environment, int>, double> m_action_value;//动作价值
private:
	// 按照lr的学习率更新动作价值
	void UpdateActionValue(const Environment& env, int action, double value, double lr);
public:
	TDTrainer();
	~TDTrainer();
	// 获取动作价值
	double GetValue(const Environment& env, int action);
	// ebsilon贪心策略获取动作
	int GetAction(const Environment& env, double ebsilon);
	// 期望SARSA时序差分算法
	int ExpectSARSA(Environment& env, double ebsilon, double lr,int maxstep=-1);
	// 将学习结果保存下来
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
};

