#include <stdio.h>

#include "TDTrainer.h"



TDTrainer::TDTrainer()
{
}


TDTrainer::~TDTrainer()
{
}


// 获取动作价值
double TDTrainer::GetValue(const Environment& env, int action, int index)
{
	std::pair<Environment, int> p(env,action);
	p.second += index * Environment::_action;
	if (!m_action_value.count(p))
		return 0.0;
	return m_action_value[p];
}


// ebsilon贪心策略获取动作
int TDTrainer::GetAction(const Environment& env, double ebsilon, int index)
{
	double maxv,v;
	int maxa = 0;
	int r = rand() % 10000;
	if ((double)r < (ebsilon * 10000.0))
		return rand() % env._action;
	maxv = GetValue(env, 0, index);
	for (int a = 1; a < env._action; a++)
	{
		v = GetValue(env, a, index);
		if (v > maxv)
		{
			maxv = v;
			maxa = a;
		}
	}
	return maxa;
}

// 使用多组动作价值共同决定动作
int TDTrainer::GetWholeAction(const Environment& env, double ebsilon, int nWhole)
{
	double maxv, v;
	int maxa = 0;
	int r = rand() % 10000;
	if ((double)r < (ebsilon * 10000.0))
		return rand() % env._action;
	maxv = 0.0;
	for (int i = 0; i < nWhole; i++)
		maxv += GetValue(env, 0, i);
	for (int a = 1; a < env._action; a++)
	{
		v = 0.0;
		for (int i = 0; i < nWhole; i++)
			v += GetValue(env, a, i);
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
void TDTrainer::UpdateActionValue(const Environment& env, int action, double value, double lr, int index)
{
	std::pair<Environment, int> p(env, action);
	p.second += index * Environment::_action;
	if (m_action_value.count(p))
	{
		m_action_value[p] += lr * (value - m_action_value[p]);
	}
	else
	{
		m_action_value[p] = lr * value;
	}
}

// 计算当前某状态-动作对的访问次数
int TDTrainer::HitSA(const Environment& env, int action)
{
	int ret;
	std::pair<Environment, int> p(env, action);
	if (!m_hit_time.count(p))
	{
		ret = 1;
	}
	else
	{
		ret = m_hit_time[p] + 1;
	}
	m_hit_time[p] = ret;
	return ret;
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

// Q学习
int TDTrainer::QLearn(Environment& env, double ebsilon, double lr)
{
	int all_rewards = 0;
	int r;
	int A, a;
	double U, maxv, v;
	double proba = ebsilon / (double)Environment::_action;
	Environment myenv = env;
	//printf("New start:\n");
	//env.Print();
	while(!env.IsTerminated())
	{
		A = GetAction(myenv, ebsilon);
		r = myenv.Step(A);
		//printf("Step %d:\n", i);
		//myenv.Print();
		if (r > 0)
			all_rewards += r;
		U = (double)r;
		maxv = GetValue(myenv, 0);
		for (a = 1; a < Environment::_action; a++)
		{
			v = GetValue(myenv, a);
			if (v > maxv)
				maxv = v;
		}
		U += maxv;
		UpdateActionValue(env, A, U, lr);
		env = myenv;
	}
	return all_rewards;
}


// 双重Q学习
int TDTrainer::DoubleQLearn(Environment& env, double ebsilon, double lr)
{
	int all_rewards = 0;
	int r;
	int A, a, nA;
	int index;
	double U, maxv, v;
	Environment myenv = env;
	while (!env.IsTerminated())
	{
		A = GetWholeAction(myenv, ebsilon, 2);
		r = myenv.Step(A);
		if (r > 0)
			all_rewards += r;
		U = (double)r;
		index = rand() % 2;
		nA = GetAction(myenv, 0.0, index);
		U += GetValue(myenv, nA, 1 - index);
		UpdateActionValue(env, A, U, lr, index);
		env = myenv;
	}
	return all_rewards;
}




// 蒙特卡罗
int TDTrainer::MonteCarlo(Environment& env, double ebsilon, double lr, int maxstep)
{
	std::vector<Environment> envs;
	std::vector<int> actions, rewards;
	int all_rewards = 0;
	int bingo = 0;
	int r, a;
	double c;
	for (int i = 0; !env.IsTerminated() && i != maxstep; i++)
	{
		envs.push_back(env);
		a = GetAction(env, ebsilon);
		r = env.Step(a);
		if (r > 0)
			bingo += r;
		actions.push_back(a);
		rewards.push_back(r);
	}
	for (int i = (int)envs.size() - 1; i >= 0; i--)
	{
		all_rewards += rewards[i];
		c = (double)HitSA(envs[i], actions[i]);
		UpdateActionValue(envs[i], actions[i], (double)all_rewards / c, lr);
	}
	return bingo;
}


