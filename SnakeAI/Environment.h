#pragma once

#include <vector>

#include "Recorder.h"

class Environment
{
public:
	static const int _food = -1;
	static const int _action = 4;
private:
	int* m_map;
	int m_width, m_height;
	std::vector<int> m_aSnake;
	int m_food;
	bool m_bDead;
private:
	Environment();
public:
	Environment(int width,int height);
	Environment(const Environment& env);
	~Environment();
	void Reset();
	bool operator < (const Environment& target) const;
	Environment& operator =(const Environment& target);
	// 进行一次动作，并返回奖励值
	int Step(int action);
	// 当前状态是否终结了
	bool IsTerminated() const;
	// 获取所有可能的初始状态环境
	static bool AllInitiations(std::vector<Environment>& dst, int width, int height);
	// 将当前状况打印出来
	void Print();
	// 将采取动作后所有可能的结果输出
	int StepAll(int action, std::vector<Environment>& envs) const;
	// 保存环境数据
	bool Save(Recorder* pRecoder) const;
	// 加载环境数据
	bool Load(Recorder* pRecorder);
	// 获取所有可能的特定长度的蛇的环境
	static bool AllSpecificSnake(std::vector<Environment>& dst, int width, int height, int length);
	// 获取食物的位置
	int GetFood() const;
	// 获取蛇的每个关节
	void GetSnake(std::vector<int>& aSnake) const;
	// 是否死了
	bool IsDead() const;
};

