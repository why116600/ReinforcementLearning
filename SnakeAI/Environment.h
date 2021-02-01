#pragma once

#include <vector>

#include "Recorder.h"

class Environment
{
public:
	static const int _food = -1;
	static const int _action = 4;
private:
	int m_map[9];
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
	// ����һ�ζ����������ؽ���ֵ
	int Step(int action);
	// ��ǰ״̬�Ƿ��ս���
	bool IsTerminated() const;
	// ��ȡ���п��ܵĳ�ʼ״̬����
	static bool AllInitiations(std::vector<Environment>& dst, int width, int height);
	// ����ǰ״����ӡ����
	void Print();
	// ����ȡ���������п��ܵĽ�����
	int StepAll(int action, std::vector<Environment>& envs) const;
	// ���滷������
	bool Save(Recorder* pRecoder) const;
	// ���ػ�������
	bool Load(Recorder* pRecorder);
};

