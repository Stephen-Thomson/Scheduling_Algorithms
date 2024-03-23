#pragma once
//***************************************************
// Author: Erik Valadez-Ramos, Stephen Thomson
// Date: 2/15/2024
//
// Function declarations for earliest deadline first algorithm
//

#include <algorithm>
using std::min;
using std::max;
#include <numeric>
using std::lcm;
#include <vector>
using std::vector;
#include <string>
using std::string;

#include "Schedule.h"
#include "TaskSet.h"
#include "Task.h"

class EDF_Algorithm : public Schedule
{
private:
    //Members
    vector<Task> m_set;
    
    //Methods
    unsigned long long getLCM();

public:
    EDF_Algorithm();
    EDF_Algorithm(TaskSet taskSet);
    string runEDF_Algorithm();
    void setTaskSet(int index, TaskSet taskset);
};