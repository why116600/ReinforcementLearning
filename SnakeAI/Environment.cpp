#include <math.h>
#include <set>

#include "Environment.h"

Environment::Environment()
	:m_width(0)
	,m_height(0)
	,m_bDead(true)
	, m_food(-1)
	, m_map(NULL)
{

}

Environment::Environment(int width, int height)
	:m_width(width)
	,m_height(height)
	, m_bDead(false)
	, m_food(-1)
{
	m_map = new int[width * height];
	Reset();
}

Environment::Environment(const Environment& env)
	:m_width(env.m_width)
	,m_height(env.m_height)
	,m_bDead(env.m_bDead)
	, m_food(env.m_food)
{
	int len = m_width * m_height;
	m_map = new int[len];
	m_aSnake = env.m_aSnake;
	memcpy(m_map, env.m_map, sizeof(int) * len);
}

Environment::~Environment()
{
	delete []m_map;
}

bool Environment::operator < (const Environment& target) const
{
	if (m_width < target.m_width)
		return true;
	if (m_width > target.m_width)
		return false;
	if (m_height < target.m_height)
		return true;
	if (m_height > target.m_height)
		return false;
	if (m_aSnake.size() < target.m_aSnake.size())
		return true;
	if (m_aSnake.size() > target.m_aSnake.size())
		return false;

	int len = m_width * m_height;
	for (int i = 0; i < len; i++)
	{
		if (m_map[i] < target.m_map[i])
			return true;
		if (m_map[i] > target.m_map[i])
			return false;
	}
	if (m_bDead && !target.m_bDead)
		return true;

	return false;
}

Environment& Environment::operator =(const Environment& target)
{
	int len;
	m_width = target.m_width;
	m_height = target.m_height;
	m_bDead = target.m_bDead;
	m_food = target.m_food;
	len = m_width * m_height;
	if (m_map)
		delete[]m_map;
	m_map = new int[len];
	m_aSnake = target.m_aSnake;
	memcpy(m_map, target.m_map, sizeof(int) * len);
	return *this;
}

void Environment::Reset()
{
	int pos;
	int len = m_width * m_height;
	m_bDead = false;
	memset(m_map, 0, sizeof(int) * len);
	m_aSnake.clear();
	//确定蛇头
	pos = rand() % len;
	m_map[pos] = 1;
	m_aSnake.push_back(pos);
	//确定食物
	while (m_map[pos])
	{
		pos = rand() % len;
	}
	m_food = pos;
	m_map[pos] = _food;
}


// 进行一次动作，并返回奖励值
int Environment::Step(int action)
{
	int len = m_width * m_height;
	int next;
	int pos = m_aSnake[0];
	if (IsTerminated())
		return 0;
	switch (action)
	{
	case 0://up
		next = pos - m_width;
		if (next < 0)
		{
			m_bDead = true;
			return -len;
		}
		break;
	case 1://right
		if ((pos % m_width) == (m_width - 1))
		{
			m_bDead = true;
			return -len;
		}
		next = pos + 1;
		break;
	case 2://down
		next = pos + m_width;
		if (next >= len)
		{
			m_bDead = true;
			return -len;
		}
		break;
	case 3://right
		if ((pos % m_width) == 0)
		{
			m_bDead = true;
			return -len;
		}
		next = pos - 1;
		break;
	default:
		return 0;
	}
	if (m_map[next] > 0 && (m_map[next]<(int)m_aSnake.size() || m_aSnake.size() <= 2))//吃到自己了T_T
	{
		m_bDead = true;
		return -len;
	}
	if (m_map[next] == _food)//吃到食物了，要长长
	{
		m_aSnake.insert(m_aSnake.begin(), next);
		for (int i = 0; i < (int)m_aSnake.size(); i++)
		{
			pos = m_aSnake[i];
			m_map[pos] = i + 1;
		}
		if ((int)m_aSnake.size() < len)
		{
			do
			{
				pos = rand() % len;
			} while (m_map[pos]);
			m_map[pos] = _food;
			m_food = pos;
		}
		else
		{
			m_food = -1;
		}
		return 1;
	}
	memset(m_map, 0, sizeof(int) * len);
	for (int i = 0; i < (int)m_aSnake.size(); i++)
	{
		pos = m_aSnake[i];
		m_aSnake[i] = next;
		m_map[next] = i + 1;
		next = pos;
	}
	if (m_food >= 0)
		m_map[m_food] = _food;
	return 0;
}


// 当前状态是否终结了
bool Environment::IsTerminated() const
{
	// TODO: 在此处添加实现代码.
	return m_bDead || (int)m_aSnake.size()>=(m_width*m_height);
}


// 获取所有可能的初始状态环境
bool Environment::AllInitiations(std::vector<Environment>& dst, int width, int height)
{
	int len = width * height;
	if (width <= 1 || height <= 1)
		return false;
	Environment env;
	env.m_width = width;
	env.m_height = height;
	env.m_bDead = false;
	env.m_map = new int[len];
	memset(env.m_map, 0, sizeof(int) * len);
	for (int i = 0; i < len; i++)
	{
		env.m_aSnake.push_back(i);
		env.m_map[i] = 1;
		for (int j = 0; j < len; j++)
		{
			if (i == j)
				continue;
			env.m_food = j;
			env.m_map[j] = _food;
			dst.push_back(env);
			env.m_map[j] = 0;
		}
		env.m_map[i] = 0;
		env.m_aSnake.clear();
	}
	return true;
}


// 将当前状况打印出来
void Environment::Print()
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (j > 0)
				printf("\t");
			printf("%d", m_map[i * m_width + j]);
		}
		printf("\n");
	}
}


// 将采取动作后所有可能的结果输出
int Environment::StepAll(int action, std::vector<Environment>& envs) const
{
	Environment env=*this;
	int len = m_width * m_height;
	int next;
	int pos = m_aSnake[0];
	if (IsTerminated())
		return 0;
	switch (action)
	{
	case 0://up
		next = pos - m_width;
		if (next < 0)
		{
			env.m_bDead = true;
			envs.push_back(env);
			return -len;
		}
		break;
	case 1://right
		if ((pos % m_width) == (m_width - 1))
		{
			env.m_bDead = true;
			envs.push_back(env);
			return -len;
		}
		next = pos + 1;
		break;
	case 2://down
		next = pos + m_width;
		if (next >= len)
		{
			env.m_bDead = true;
			envs.push_back(env);
			return -len;
		}
		break;
	case 3://right
		if ((pos % m_width) == 0)
		{
			env.m_bDead = true;
			envs.push_back(env);
			return -len;
		}
		next = pos - 1;
		break;
	default:
		return 0;
	}
	if (m_map[next] > 0 && (m_map[next] < (int)m_aSnake.size() || m_aSnake.size()<=2))//吃到自己了T_T
	{
		env.m_bDead = true;
		envs.push_back(env);
		return -len;
	}
	if (m_map[next] == _food)//吃到食物了，要长长
	{
		env.m_aSnake.insert(env.m_aSnake.begin(), next);
		for (int i = 0; i < (int)env.m_aSnake.size(); i++)
		{
			pos = env.m_aSnake[i];
			env.m_map[pos] = i + 1;
		}
		if ((int)env.m_aSnake.size() < len)
		{
			for (int i = 0; i < len; i++)
			{
				if (env.m_map[i] != 0)
					continue;
				env.m_food = i;
				env.m_map[i] = _food;
				envs.push_back(env);
				env.m_map[i] = 0;
			}
		}
		else
		{
			env.m_food = -1;
			envs.push_back(env);
		}
		return 1;
	}
	memset(env.m_map, 0, sizeof(int) * len);
	for (int i = 0; i < (int)env.m_aSnake.size(); i++)
	{
		pos = env.m_aSnake[i];
		env.m_aSnake[i] = next;
		env.m_map[next] = i + 1;
		next = pos;
	}
	if (env.m_food >= 0)
		env.m_map[env.m_food] = _food;
	envs.push_back(env);
	return 0;
}


// 保存环境数据
bool Environment::Save(Recorder* pRecorder) const
{
	int n,m;
	pRecorder->Write(&m_width, sizeof(m_width));
	pRecorder->Write(&m_height, sizeof(m_height));
	pRecorder->Write(&m_food, sizeof(m_food));
	m = m_bDead ? 1 : 0;
	pRecorder->Write(&m, sizeof(m));
	n = (int)m_aSnake.size();
	pRecorder->Write(&n, sizeof(n));
	for (int i = 0; i < n; i++)
	{
		m = m_aSnake[i];
		pRecorder->Write(&m, sizeof(m));
	}

	return true;
}


// 加载环境数据
bool Environment::Load(Recorder* pRecorder)
{
	int n, m, width, height,f,d;
	int len;
	int* smap;
	bool bRet = true;
	std::vector<int> snake;
	if (pRecorder->Read(&width, sizeof(width)) != sizeof(width))
		return false;
	if (pRecorder->Read(&height, sizeof(height)) != sizeof(height))
		return false;
	if (width <= 1 || height <= 1)
		return false;
	len = width * height;
	if (pRecorder->Read(&f, sizeof(f)) != sizeof(f))
		return false;
	if (f < 0 || f >= len)
		return false;
	if (pRecorder->Read(&d, sizeof(d)) != sizeof(d))
		return false;
	if (d != 0 && d != 1)
		return false;
	if (pRecorder->Read(&n, sizeof(n)) != sizeof(n))
		return false;
	smap = new int[len];
	memset(smap, 0, sizeof(int) * len);
	for (int i = 0; i < n; i++)
	{
		if (pRecorder->Read(&m, sizeof(m)) != sizeof(m))
		{
			bRet = false;
			break;
		}
		if (m < 0 || m >= len || smap[m])
		{
			bRet = false;
			break;
		}
		snake.push_back(m);
		smap[m] = i + 1;
	}
	if (bRet)
	{
		m_width = width;
		m_height = m_height;
		m_food = f;
		m_aSnake = snake;
		m_bDead = d != 0;
		smap[f] = _food;
		memcpy(m_map, smap, sizeof(int) * len);
	}
	delete[]smap;
	return bRet;
}


// 获取所有可能的特定长度的蛇的环境
bool Environment::AllSpecificSnake(std::vector<Environment>& dst, int width, int height, int length)
{
	std::vector<Environment> queue;
	std::vector<Environment> next;
	std::set<Environment> met;
	int len = width * height;
	if (length<1 || length>len)
		return false;
	if (!AllInitiations(queue, width, height))
		return false;
	if (length == 1)
	{
		dst = queue;
		return true;
	}
	for (size_t i = 0; i < queue.size(); i++)
		met.insert(queue[i]);
	//广度优先搜索
	while (queue.size() > 0)
	{
		for (int a = 0; a < _action; a++)
		{
			queue[0].StepAll(a, next);
		}
		for (size_t i = 0; i < next.size(); i++)
		{
			if (met.count(next[i]) || next[i].IsTerminated())
				continue;
			met.insert(next[i]);
			if ((int)next[i].m_aSnake.size() == length)
			{
				dst.push_back(next[i]);
				continue;
			}
			queue.push_back(next[i]);
		}
		next.clear();
		queue.erase(queue.begin());
	}
	return true;
}
