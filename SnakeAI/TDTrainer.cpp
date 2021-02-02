#include <stdio.h>

#include "TDTrainer.h"



TDTrainer::TDTrainer()
{
}


TDTrainer::~TDTrainer()
{
}


// 获取动作价值
double TDTrainer::GetValue(const Environment& env, int action)
{
	std::pair<Environment, int> p(env,action);
	if (!m_action_value.count(p))
		return 0.0;
	return m_action_value[p];
}


// ebsilon贪心策略获取动作
int TDTrainer::GetAction(const Environment& env, double ebsilon)
{
	double maxv,v;
	int maxa = 0;
	int r = rand() % 10000;
	if ((double)r < (ebsilon * 10000.0))
		return rand() % env._action;
	maxv = GetValue(env, 0);
	for (int a = 1; a < env._action; a++)
	{
		v = GetValue(env, a);
		if (v > maxv)
		{
			maxv = v;
			maxa = a;
		}
	}
	return maxa;
}


// 期望SARSA时序差分算法
int TDTrainer::ExpectSARSA(Environment& env, double ebsilon, double lr, int maxstep)
{
	int all_rewards = 0;
	int r;
	int A, nA, a;
	double U;
	double proba= ebsilon /(double)Environment::_action;
	Environment myenv = env;
	//printf("New start:\n");
	//env.Print();
	for (int i = 0; !env.IsTerminated() && i != maxstep; i++)
	{
		A = GetAction(myenv, ebsilon);
		r = myenv.Step(A);
		//printf("Step %d:\n", i);
		//myenv.Print();
		if (r > 0)
			all_rewards += r;
		U = (double)r;
		nA = GetAction(myenv, ebsilon);
		for (a = 0; a < Environment::_action; a++)
		{
			if (a == nA)
			{
				U += (1.0 - ebsilon + proba) * GetValue(myenv, a);
			}
			else
			{
				U += proba * GetValue(myenv, a);
			}
		}
		UpdateActionValue(env, A, U, lr);
		env = myenv;
	}
	return all_rewards;
}


// 按照lr的学习率更新动作价值
void TDTrainer::UpdateActionValue(const Environment& env, int action, double value, double lr)
{
	std::pair<Environment, int> p(env, action);
	if (m_action_value.count(p))
	{
		m_action_value[p] += lr * (value - m_action_value[p]);
	}
	else
	{
		m_action_value[p] = lr * value;
	}
}


// 将学习结果保存下来
bool TDTrainer::Save(Recorder* pRecorder) const
{
	int n;
	int a;
	double f;
	n = -1;
	pRecorder->Write(&n, sizeof(n));
	n = (int)m_action_value.size();
	pRecorder->Write(&n, sizeof(n));
	for (std::map<std::pair<Environment, int>, double>::const_iterator it = m_action_value.begin(); it != m_action_value.end(); it++)
	{
		it->first.first.Save(pRecorder);
		a = it->first.second;
		pRecorder->Write(&a, sizeof(a));
		f = it->second;
		pRecorder->Write(&f, sizeof(f));
	}

	return true;
}


bool TDTrainer::Load(Recorder* pRecorder)
{
	bool bRet = true;
	int n, s, a;
	double f;
	unsigned long l;
	Environment env(3, 3);
	std::pair<Environment, int> p(env, 0);
	if (pRecorder->Read(&s, sizeof(s)) != sizeof(s))
		return false;
	if (s != -1)
		return false;
	if (pRecorder->Read(&n, sizeof(n)) != sizeof(n))
		return false;
	for (int i = 0; i < n; i++)
	{
		if (!env.Load(pRecorder))
		{
			bRet = false;
			break;
		}
		if (pRecorder->Read(&a, sizeof(a)) != sizeof(a))
		{
			bRet = false;
			break;
		}
		if ((l=pRecorder->Read(&f, sizeof(f))) != sizeof(f))
		{
			bRet = false;
			break;
		}
		p.first = env;
		p.second = a;
		m_action_value[p] = f;
	}
	if (!bRet)
	{
		m_action_value.clear();
	}
	return bRet;
}