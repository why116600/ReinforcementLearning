#pragma once

#include "Environment.h"

class ExperimentItem
{
public:
	Environment _env1, _env2;
	int _reward, _action;
public:
	ExperimentItem(int nSize);
	ExperimentItem(const ExperimentItem& obj);
	~ExperimentItem();

	ExperimentItem& operator=(const ExperimentItem& target);
	bool operator<(const ExperimentItem& target) const;
};

