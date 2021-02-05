#include "ExperimentItem.h"



ExperimentItem::ExperimentItem(int nSize)
	:_env1(nSize,nSize)
	,_env2(nSize,nSize)
	,_reward(0)
	,_action(0)
{
}

ExperimentItem::ExperimentItem(const ExperimentItem& obj)
	:_env1(obj._env1)
	, _env2(obj._env2)
	, _reward(obj._reward)
	, _action(obj._action)
{

}

ExperimentItem::~ExperimentItem()
{
}

ExperimentItem& ExperimentItem::operator=(const ExperimentItem& target)
{
	_env1 = target._env1;
	_env2 = target._env2;
	_reward = target._reward;
	_action = target._action;
	return *this;
}

bool ExperimentItem::operator<(const ExperimentItem& target) const
{
	if (_env1 < target._env1)
		return true;
	if (target._env1 < _env1)
		return false;
	if (_env2 < target._env2)
		return true;
	if (target._env2 < _env2)
		return false;
	if (_reward < target._reward)
		return true;
	if (_reward > target._reward)
		return false;
	if (_action < target._action)
		return true;
	return false;
}