/************************************************************************************************
* File: TaskSet.cpp
* Author: Stephen Thomson
* Date: 2/8/2024
* Description: This file contains the implementation of the TaskSet class. This class is used to
*              store a list of tasks and perform operations on them.
*************************************************************************************************/

#include "TaskSet.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <numeric>
#include "Task.h"
#include "TaskSet.h"


int m_numTasks = 0;
int m_numProcessors = 0;
bool m_preemptive = false;
std::vector<Task> m_tasks;

TaskSet::TaskSet()
{
    m_numTasks = 0;
    m_numProcessors = 0;
    m_tasks.clear();
    m_preemptive = false;
}

TaskSet::~TaskSet()
{
    m_numTasks = 0;
    m_numProcessors = 0;
    m_tasks.clear();
}

TaskSet::TaskSet(int numTasks, int numProcessors, bool preemptive)
{
    m_numTasks = numTasks;
    m_numProcessors = numProcessors;
    m_preemptive = preemptive;
}

TaskSet& TaskSet::operator=(const TaskSet& taskSet)
{
    if (this == &taskSet)
    {
        return *this;
    }
    m_numTasks = taskSet.m_numTasks;
    m_numProcessors = taskSet.m_numProcessors;
    m_preemptive = taskSet.m_preemptive;
    m_tasks = taskSet.m_tasks;
    return *this;
}

void TaskSet::addTask(Task task)
{
    m_tasks.push_back(task);
    if (m_numTasks < m_tasks.size()) {
        m_numTasks++;
    }
}

std::vector<Task> TaskSet::getTasks()
{
    return m_tasks;
}

void TaskSet::removeTask(int index) {
    m_tasks.erase(m_tasks.begin() + index);
}

Task TaskSet::getTask(int index) {
    return m_tasks[index];
}

void TaskSet::printTasks()
{
    std::cout << "Tasks: " << m_numTasks << std::endl;
    std::cout << "Processors: " << m_numProcessors << std::endl;
    std::cout << "Preemptive: " << (m_preemptive ? "True":"False") << std::endl;
    for (int i = 0; i < m_numTasks; i++)
    {
        // Print Task: i, then call m_tasks[i].printTask() all on the same line.
        std::cout << m_tasks[i].getName() << ", "; m_tasks[i].printTask();
    }
}

// this is caculateUtilizationRate: calculateUtilizationRate(m_tasks[i], m_tasks[i].getComputationTime());
double TaskSet::calculateUtilizationRate(Task task)
{
    return task.getComputationTime() / task.getHardDeadline();
}

double TaskSet::calculateTotalUtilizationRate()
{
    double totalUtilizationRate = 0;
    for (int i = 0; i < m_numTasks; i++)
    {
        totalUtilizationRate += calculateUtilizationRate(m_tasks[i]);
    }
    return totalUtilizationRate;
}

int TaskSet::getNumTasks()
{
    return m_numTasks;
}

void TaskSet::setNumProcessors(int numProcessors)
{
    m_numProcessors = numProcessors;
}


int TaskSet::getNumProcessors() {
    return m_numProcessors;
}
        
void TaskSet::setNumTasks(int numTasks)
{
    m_numTasks = numTasks;
}

void TaskSet::setPreemptive(bool preemptive)
{
    m_preemptive = preemptive;
}

bool TaskSet::getPreemptive()
{
    return m_preemptive;
}

int TaskSet::EUSI()
{
    //s = start time
    //d = hard deadline
    //c = computation time    

    // Step 1, calculate Requested Execution Time (Ri) for each task
    // Create an int vector to store the Ri for each task
    vector<int> Ri;

    // Make computations for each task
    // Start Loop i to numtasks
    // int Ri1Value = 0
    // Start Loop j to less than equal to i
    // Ri1Value += cj
    // End Loop j
    // Set Ri[i] to Ri1Value;
    // End Loop i

    for (int i = 0; i < m_numTasks; i++)
    {
        int Ri1Value = 0;
        for (int j = 0; j <= i; j++)
        {
            Ri1Value += m_tasks[j].getComputationTime();
        }
        Ri.push_back(Ri1Value);
    }

    // Start Loop i to numtasks
    // Create int riValue and set to Ri[i]
    // Start Loop j at i+1 to numtasks
    // if dj<(di+cj)
    // 	Add to riValue(cj+di-dj)
    // End Loop j
    // 	Update Ri[i] to riValue

    for (int i = 0; i < m_numTasks; i++)
    {
        int riValue = Ri[i];
        for (int j = i + 1; j < m_numTasks; j++)
        {
            if (m_tasks[j].getHardDeadline() < (m_tasks[i].getHardDeadline() + m_tasks[j].getComputationTime()))
            {
                riValue += (m_tasks[j].getComputationTime() + m_tasks[i].getHardDeadline() - m_tasks[j].getHardDeadline());
            }
        }
        Ri[i] = riValue;
    }

    // Step 2, calculate Available Execution Time (Ai) for each task
    // Create an int vector to store the Ai for each task
    vector<int> Ai;

    // Make computations for each task
    // Start Loop i to numTasks
    // int Ai1Value = 0
    // Start Loop j start 0 to numtasks
    // if dj<=si
    // 	Ai1Value = Ai[i] + cj
    // End Loop j
    // Set Ai[i] to Ai1Value

    for (int i = 0; i < m_numTasks; i++)
    {
        int Ai1Value = 0;
        Ai.push_back(Ai1Value);
        for (int j = 0; j < m_numTasks; j++)
        {
            if (m_tasks[j].getHardDeadline() <= m_tasks[i].getStartTime())
            {
                Ai1Value = Ai[i] + m_tasks[j].getComputationTime();
            }
        }
        Ai[i] = Ai1Value;
    }

    // Start Loop i to numtasks
    // Create int aiValue and set to Ai[i]
    // Start Loop j at 0 to numtasks
    // if dj>si and sj<si
    // 	aiValue += (minimum value of (cj or (si-sj)))
    // End Loop j
    // Set Ai[i] to aiValue
    // End Loop i

    for (int i = 0; i < m_numTasks; i++)
    {
        int aiValue = Ai[i];
        for (int j = 0; j < m_numTasks; j++)
        {
            if (m_tasks[j].getHardDeadline() > m_tasks[i].getStartTime() && m_tasks[j].getStartTime() < m_tasks[i].getStartTime())
            {
                aiValue += min(m_tasks[j].getComputationTime(), (m_tasks[i].getStartTime() - m_tasks[j].getStartTime()));
            }
        }
        Ai[i] = aiValue;
    }

    // Step 3, calculate the EUSI for each task
    // Create an int vector to store the EUSI for each task
    vector<int> EUSI;
    // Make computations for each task

    //Start Loop i start 0 to numtasks
    // Create vector to hold value of each loop
    //vector<int> EUSIValue;
    // Start Loop j start 0 to numtasks
    // Calculate (Ri[i] - Ai[j])/(di-sj) and push absolute value into EUSIValue
    // End Loop j
    // Insert into EUSI[i] the Maximum value (Highest Value) in EUSIValue)
    // End Loop i

    for (int i = 0; i < m_numTasks; i++)
    {
        int EUSIValue = 0;
        for (int j = 0; j < m_numTasks; j++)
        {
            if ((m_tasks[i].getHardDeadline() - m_tasks[j].getStartTime()) != 0)
            {
                int calcVal = (ceil((Ri[i] - Ai[j]) / static_cast<double>(m_tasks[i].getHardDeadline() - m_tasks[j].getStartTime())));
                if (EUSIValue < calcVal)
                {
                    EUSIValue = calcVal;
                }
            }
        }
        EUSI.push_back(EUSIValue);
    }

    // Step 4, find the maximum EUSI value
    // Got through vector to find the maximum value
    int maxEUSI = EUSI[0];
    for (int i = 1; i < m_numTasks; i++)
    {
        if (EUSI[i] > maxEUSI)
        {
            maxEUSI = EUSI[i];
        }
    }

    // Step 5, return the maximum EUSI value
    return maxEUSI;
}

int TaskSet::USI()
{
    // int vector to hold USI calculations for each task
    vector<int> USICalculations;
    // int USI
    int USI = 0;
    // Begin Loop i from 0 to m_numTasks
    for (int i = 0; i < m_numTasks; i++)
    {
        // int to hold total
        int total = 0;
        // Begin Loop j from 0 to i
        for (int j = 0; j <= i; j++)
        {
            // total += task j computation time
            total += m_tasks[j].getComputationTime();
        }
        // USI = total/task i hard deadline rounded up to nearest integer
        USI = ceil(static_cast<double>(total) / m_tasks[i].getHardDeadline());
        // push USI into int vector
        USICalculations.push_back(USI);
        // set USI to 0
        USI = 0;
    }

    int finalUSI = 0;
    // Go through vector of USI calculations and find biggest number
    for (int i = 0; i < USICalculations.size(); i++)
    {
        if (USICalculations[i] > finalUSI)
        {
            finalUSI = USICalculations[i];
        }
    }
    // Return the highest int in vector.
    return finalUSI;
}

bool TaskSet::checkProcessors()
{
    int USIValue = USI();
    int EUSIValue = EUSI();
    if (USIValue <= m_numProcessors && EUSIValue <= m_numProcessors)
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned long long TaskSet::getLCM()
{
    unsigned long long leastcm = m_tasks[0].getPeriod();
    for (int i = 1; i < m_numTasks; i++)
    {
        leastcm = lcm(leastcm, m_tasks[i].getPeriod());
    }
    if (leastcm > 18446744073709551615 || leastcm < 0)
    {
        leastcm = 18446744073709551615;
    }
    return leastcm;
}