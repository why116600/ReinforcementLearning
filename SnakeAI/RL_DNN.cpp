#include "RL_DNN.h"



RL_DNN::RL_DNN(int nSize)
	:m_nSize(nSize)
{
}


RL_DNN::~RL_DNN()
{
	m_nn.ClosePipe();
}


#define SNAKE_OFFSET 2
// 连接管道神经网络
bool RL_DNN::ConnectNN(LPCTSTR szPipeName)
{
	std::vector<int> shapes;
	if (!m_nn.OpenPipe(szPipeName))
		return false;
	if (!m_nn.GetInputShape(shapes))
		return false;
	if (shapes.size() != 1 || shapes[0] != (m_nSize * m_nSize + SNAKE_OFFSET))
	{
		m_nn.ClosePipe();
		return false;
	}
	return true;
}


// 关闭管道神经网络
void RL_DNN::CloseNN()
{
	m_nn.ClosePipe();
}

// 获取动作价值
bool RL_DNN::GetValue(const Environment& env, double ret[], int length)
{
	std::vector<int> aSnake;
	double inputx[20] = { 0.0 };
	int len = m_nSize * m_nSize + SNAKE_OFFSET;
	//inputx = new double[len];
	inputx[0] = env.GetFood();
	inputx[1] = env.IsDead() ? 1.0 : 0.0;
	env.GetSnake(aSnake);
	for (int i = 0; i < (len- SNAKE_OFFSET); i++)
	{
		if (i < (int)aSnake.size())
			inputx[i + SNAKE_OFFSET] = aSnake[i];
		else
			inputx[i + SNAKE_OFFSET] = -1.0;
	}
	if (!m_nn.Predict(inputx, len, ret, length))
	{
		return false;
	}
	//delete[]inputx;
	return true;
}


// 获取当前要做的动作
int RL_DNN::GetAction(const Environment& env, double ebsilon)
{
	double maxv, v;
	double values[env._action];
	int maxa = 0;
	int r = rand() % 10000;
	if ((double)r < (ebsilon * 10000.0))
		return rand() % env._action;
	if (!GetValue(env, values, env._action))
		return -1;
	maxv = values[0];
	for (int a = 1; a < env._action; a++)
	{
		v = values[a];
		if (v > maxv)
		{
			maxv = v;
			maxa = a;
		}
	}
	return maxa;
}


// 更新某动作价值
bool RL_DNN::UpdateValue(const Environment& env, int action, double value)
{
	bool bRet;
	double values[env._action];
	std::vector<int> aSnake;
	double* inputx;
	int len = m_nSize * m_nSize + SNAKE_OFFSET;
	inputx = new double[len];
	inputx[0] = action;
	inputx[1] = env.GetFood();
	inputx[2] = env.IsDead() ? 1.0 : 0.0;
	env.GetSnake(aSnake);
	for (int i = 0; i < (len- SNAKE_OFFSET); i++)
	{
		if (i < (int)aSnake.size())
			inputx[i + SNAKE_OFFSET] = aSnake[i];
		else
			inputx[i + SNAKE_OFFSET] = -1.0;
	}
	if (GetValue(env, values, env._action))
	{
		values[action] = value;
		bRet = m_nn.SendTrainData(inputx, len, values, env._action) && m_nn.StartTraining();
	}
	else
	{
		bRet = false;
	}
	delete[]inputx;
	return bRet;
}


// 使用深度网络的SARSA算法
int RL_DNN::SARSAWithDNN(Environment& env, double ebsilon)
{
	double values[env._action];
	int all_rewards = 0;
	int r;
	int A, nA, a;
	double U;
	double proba = ebsilon / (double)Environment::_action;
	Environment myenv = env;
	for (int i = 0; !env.IsTerminated(); i++)
	{
		A = GetAction(myenv, ebsilon);
		if (A < 0)
			break;
		r = myenv.Step(A);
		if (r > 0)
			all_rewards += r;
		U = (double)r;
		if (!myenv.IsTerminated())
		{
			nA = GetAction(myenv, ebsilon);
			if (!GetValue(myenv, values, env._action))
				break;
			for (a = 0; a < Environment::_action; a++)
			{
				if (a == nA)
				{
					U += (1.0 - ebsilon + proba) * values[a];
				}
				else
				{
					U += proba * values[a];
				}
			}
		}
		UpdateValue(env, A, U);
		env = myenv;
	}
	return all_rewards;
}


// 深度Q学习
int RL_DNN::DQN(Environment& env, int nPlayback, double ebsilon)
{
	int all_rewards = 0;
	int r;
	int A;
	double U;
	double values[Environment::_action];
	ExperimentItem ei(3);
	Environment myenv = env;
	while (!myenv.IsTerminated())
	{
		A = GetAction(myenv, ebsilon);
		if (A < 0)
			break;
		r = myenv.Step(A);
		ei._env1 = env;
		ei._env2 = myenv;
		ei._reward = r;
		ei._action = A;
		all_rewards += r;
		if (!m_expers.count(ei))
		{
			m_aExper.push_back(ei);
			m_expers.insert(ei);
		}
		ei = m_aExper[rand() % (int)m_aExper.size()];
		if (ei._env2.IsTerminated())
		{
			U = (double)ei._reward;
		}
		else
		{
			if (!GetValue(ei._env2, values, Environment::_action))
				break;
			U = values[0];
			for (int i = 1; i < Environment::_action; i++)
			{
				if (U < values[i])
					U = values[i];
			}
			U += (double)ei._reward;
		}
		if (!UpdateValue(ei._env1, ei._action, U))
			break;
		env = myenv;
	}
	return 0;
}
