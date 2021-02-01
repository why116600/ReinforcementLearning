#include <vector>
#include <set>

#include "FileRecorder.h"
#include "Environment.h"
#include "RLIteration.h"


RLIteration::RLIteration(int nSize)
	:m_nSize(nSize)
{
}


RLIteration::~RLIteration()
{
}


void RLIteration::SetValue(const Environment& env, double v)
{
	std::pair<double, int> p;
	if (m_value_policy.count(env))
	{
		m_value_policy[env].first = v;
		return;
	}
	p.first = v;
	p.second = 0;
	m_value_policy[env] = p;
}

void RLIteration::SetAction(const Environment& env, int a)
{
	std::pair<double, int> p;
	if (m_value_policy.count(env))
	{
		m_value_policy[env].second = a;
		return;
	}
	p.first = 0.0;
	p.second = a;
	m_value_policy[env] = p;

}

// 获取目标环境当前状态的状态价值
double RLIteration::GetValue(const Environment& env)
{
	if (m_value_policy.count(env))
		return m_value_policy[env].first;
	return 0.0;
}


// 根据当前状态获取要做的动作
int RLIteration::GetAction(const Environment& env)
{
	if (env.IsTerminated())
		return -1;
	if (m_value_policy.count(env))
		return m_value_policy[env].second;
	return 0;
}


// 有模型的价值迭代
void RLIteration::InterationWithModel(int nIter,double fTolerance)
{
	std::vector<Environment> first_env;
	std::vector<Environment> env_queue;
	std::vector<Environment> next_env;
	std::set<Environment> envs;
	double reward;
	double value,maxvalue,maxr;
	int maxa;
	double maxerror, e;
	Environment::AllInitiations(first_env, m_nSize, m_nSize);
	for (int i = 0; i != nIter; i++)
	{
		maxerror = 0.0;
		env_queue = first_env;
		envs.clear();
		for (int j = 0; j < (int)first_env.size(); j++)
			envs.insert(first_env[j]);
		//用队列进行广度优先搜索，从而遍历所有可能的状态
		while (env_queue.size() > 0)
		{
			maxa = -1;
			maxvalue = 0.0;
			//printf("current state:\n");
			//env_queue[0].Print();
			for (int a = 0; a < Environment::_action; a++)
			{
				next_env.clear();
				reward = (double)env_queue[0].StepAll(a, next_env);
				value = reward;
				for (int j = 0; j < (int)next_env.size(); j++)
				{
					value += GetValue(next_env[j]) / (double)next_env.size();
					if (!next_env[j].IsTerminated() && !envs.count(next_env[j]))
					{
						//printf("Insert a new state:\n");
						//next_env[j].Print();
						env_queue.push_back(next_env[j]);
						envs.insert(next_env[j]);
					}
				}
				if (maxa<0 || value>maxvalue)
				{
					maxa = a;
					maxvalue = value;
					maxr = reward;
				}
				else if (value == maxvalue && reward > maxr)
				{
					maxr = reward;
					maxa = a;
				}
			}
			value = GetValue(env_queue[0]);
			e = abs(value - maxvalue);
			if (e > maxerror)
				maxerror = e;
			SetValue(env_queue[0], maxvalue);
			SetAction(env_queue[0], maxa);
			env_queue.erase(env_queue.begin());
		}
		printf("Iteration %d completed!\n", i);
		if (maxerror <= fTolerance)
			break;
	}
	int ra;
	//更新一遍策略
	for (std::map<Environment, std::pair<double, int>>::iterator it = m_value_policy.begin(); it != m_value_policy.end(); it++)
	{
		ra = -1;
		maxa = -1;
		maxvalue = 0.0;
		for (int a = 0; a < Environment::_action; a++)
		{
			next_env.clear();
			reward = (double)it->first.StepAll(a, next_env);
			if (reward > 0)
				ra = a;
			value = reward;
			for (int j = 0; j < (int)next_env.size(); j++)
			{
				value += GetValue(next_env[j]) / (double)next_env.size();
			}
			if (maxa<0 || value>maxvalue)
			{
				maxr = reward;
				maxa = a;
				maxvalue = value;
			}
			else if (value == maxvalue && reward>maxr)
			{
				maxr = reward;
				maxa = a;
			}
		}
		it->second.second = maxa;
	}
}


// 将学习结果保存下来
bool RLIteration::Save(Recorder* pRecorder) const
{
	int n;
	double f;
	pRecorder->Write(&m_nSize, sizeof(m_nSize));
	n = (int)m_value_policy.size();
	pRecorder->Write(&n, sizeof(n));
	for (std::map<Environment, std::pair<double, int>>::const_iterator it = m_value_policy.begin(); it != m_value_policy.end(); it++)
	{
		f = it->second.first;
		n = it->second.second;
		pRecorder->Write(&n, sizeof(n));
		pRecorder->Write(&f, sizeof(f));
		it->first.Save(pRecorder);
	}

	return true;
}


bool RLIteration::Load(Recorder* pRecorder)
{
	bool bRet = true;
	int n, s, a;
	double f;
	std::pair<double, int> p;
	if (pRecorder->Read(&s, sizeof(s)) != sizeof(s))
		return false;
	if (pRecorder->Read(&n, sizeof(n)) != sizeof(n))
		return false;
	Environment env(s, s);
	for (int i = 0; i < n; i++)
	{
		if (pRecorder->Read(&a, sizeof(a)) != sizeof(a))
		{
			bRet = false;
			break;
		}
		if (pRecorder->Read(&f, sizeof(f)) != sizeof(f))
		{
			bRet = false;
			break;
		}
		if (!env.Load(pRecorder))
		{
			bRet = false;
			break;
		}
		p.first = f;
		p.second = a;
		m_value_policy[env] = p;
	}
	if (bRet)
	{
		m_nSize = s;
	}
	else
	{
		m_value_policy.clear();
	}
	return bRet;
}
