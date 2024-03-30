/****************************************
Author: Zane Omelchuck
Filename: ScheduleLLF.h
*****************************************/

#pragma once
#include <iostream>
#include <numeric>
#include "TaskSet.h"
#include "Schedule.h"

using namespace std;


class ScheduleLLF: public Schedule
{
    private:
        int m_numTasks;
        TaskSet m_taskSet;
    public:
        ScheduleLLF(TaskSet taskSet);
        ScheduleLLF();
        string CheckLLFSchedule();
        void setTaskSet(int index, TaskSet taskSet);
        bool CaclualteLaxity(std::vector<Task>& tasks, int time);
};