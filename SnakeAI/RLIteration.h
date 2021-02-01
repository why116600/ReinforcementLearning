#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

class RLIteration
{
private:
	int m_nSize;
	std::map<Environment, std::pair<double,int>> m_value_policy;//状态价值与策略
private:
	void SetValue(const Environment& env, double v);
	void SetAction(const Environment& env, int a);
public:
	RLIteration(int nSize);
	~RLIteration();
	// 获取目标环境当前状态的状态价值
	double GetValue(const Environment& env);
	// 根据当前状态获取要做的动作
	int GetAction(const Environment& env);
	// 有模型的价值迭代
	void InterationWithModel(int nIter=-1, double fTolerance=0.1);
	// 将学习结果保存下来
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
};

