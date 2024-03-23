#pragma once
/************************************************************************************************
* File: AlgorithmA.h
* Author: Abbey DuBois, Stephen Thomson
* Date: 3/14/2024
* Description: File for Algorithm A where class is defined
*************************************************************************************************/

#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <tuple>
#include <string>
#include "TaskSet.h"
#include "TOR.h"
#include "Schedule.h"

using std::vector;
using std::pair;
using std::tuple;
using std::string;
using std::swap;
using std::get;
using std::make_tuple;
using std::lcm;
using std::to_string;

class AlgorithmA: public Schedule {
private:
	int m_gap;
	vector<Task> m_set;
	vector<TOR> m_torList;
	void createTORList();
	void sortTaskSetChronologically();
	vector<int> sortProcessors(vector<int> chainComputation, int start);
	bool canSwap(Task task1, Task task2);
	bool inTORList(Task currentTask, Task lastTask, int chainComputation);
public:
	AlgorithmA();
	AlgorithmA(TaskSet taskSet);
	string algorithmASchedule();
	void setTaskSet(int index, TaskSet taskset);
};