#pragma once

#include <map>

#include "FileRecorder.h"
#include "Environment.h"

//ʱ���ַ���ѧϰ
class TDTrainer
{
private:
	std::map<std::pair<Environment, int>, double> m_action_value;//������ֵ
	std::map< std::pair<Environment, int>, int> m_hit_time;
private:
	// ����lr��ѧϰ�ʸ��¶�����ֵ
	void UpdateActionValue(const Environment& env, int action, double value, double lr,int index=0);
	// ���㵱ǰĳ״̬-�����Եķ��ʴ���
	int HitSA(const Environment& env, int action);
public:
	TDTrainer();
	~TDTrainer();
	// ��ȡ������ֵ
	double GetValue(const Environment& env, int action, int index = 0);
	// ebsilon̰�Ĳ��Ի�ȡ����
	int GetAction(const Environment& env, double ebsilon, int index = 0);
	// ʹ�ö��鶯����ֵ��ͬ��������
	int GetWholeAction(const Environment& env, double ebsilon, int nWhole);
	// ����SARSAʱ�����㷨
	int ExpectSARSA(Environment& env, double ebsilon, double lr,int maxstep=-1);
	// ��ѧϰ�����������
	bool Save(Recorder* pRecorder) const;
	bool Load(Recorder* pRecorder);
	// Qѧϰ
	int QLearn(Environment& env, double ebsilon, double lr);
	// ˫��Qѧϰ
	int DoubleQLearn(Environment& env, double ebsilon, double lr);
	// ���ؿ���
	int MonteCarlo(Environment& env, double ebsilon, double lr, int maxstep=-1);
};

