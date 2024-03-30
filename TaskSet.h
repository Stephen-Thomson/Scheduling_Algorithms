/************************************************************************
 * TaskSet.h
 * Author: Stephen Thomson
 * Date: 2/8/2024
 * Description: This file contains the declaration of the TaskSet class. This class
 *             is used to store a list of tasks and perform operations on them.
 * *********************************************************************/

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Task.h"

using std::string;
using std::vector;

class TaskSet
{
    private:
        int m_numTasks;
        int m_numProcessors;
        bool m_preemptive;
        std::vector<Task> m_tasks;

    public:
        TaskSet();

        ~TaskSet();

        TaskSet(int numTasks, int numProcessors, bool premptive);

        TaskSet& operator=(const TaskSet& taskSet);

        Task getTask(int index);

        vector<Task> getTasks();

        void addTask(Task task);

        int getNumTasks();

        int getNumProcessors();
        
        double calculateUtilizationRate(Task task);

        double calculateTotalUtilizationRate();

        void setNumProcessors(int numProcessors);

        void setNumTasks(int numTasks);

        void removeTask(int index);

        void printTasks();

        void setPreemptive(bool premptive);

        bool getPreemptive();

        int EUSI();

        int USI();

        bool checkProcessors();

        unsigned long long getLCM();
};