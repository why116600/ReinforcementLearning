#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

//时序差分法的学习
class TDTrainer
{
private:
	std::map<std::pair<Environment, int>, double> m_action_value;//动作价值
	std::map< std::pair<Environment, int>, int> m_hit_time;
private:
	// 按照lr的学习率更新动作价值
	void UpdateActionValue(const Environment& env, int action, double value, double lr,int index=0);
	// 计算当前某状态-动作对的访问次数
	int HitSA(const Environment& env, int action);
public:
	TDTrainer();
	~TDTrainer();
	// 获取动作价值
	double GetValue(const Environment& env, int action, int index = 0);
	// ebsilon贪心策略获取动作
	int GetAction(const Environment& env, double ebsilon, int index = 0);
	// 使用多组动作价值共同决定动作
	int GetWholeAction(const Environment& env, double ebsilon, int nWhole);
	// 期望SARSA时序差分算法
	int ExpectSARSA(Environment& env, double ebsilon, double lr,int maxstep=-1);
	// 将学习结果保存下来
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
	// Q学习
	int QLearn(Environment& env, double ebsilon, double lr);
	// 双重Q学习
	int DoubleQLearn(Environment& env, double ebsilon, double lr);
	// 蒙特卡罗
	int MonteCarlo(Environment& env, double ebsilon, double lr, int maxstep=-1);
};

