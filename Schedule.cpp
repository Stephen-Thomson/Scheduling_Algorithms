/************************************************************************************************
* File: Schedule.h
* Author: Stephen Thomson & Stephen Carter & Abbey DuBois
* Date: 3/14/2024
* Description: This is the file for Schedule class.
* This class is used to store a list of tasksets and perform operations on them.
*************************************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "TaskSet.h"
#include "Task.h"
#include "Schedule.h"

using timer = std::chrono::system_clock;
using namespace std;

Schedule::Schedule()
{
    m_tasksets = vector<TaskSet>();
    m_processors = 0;
    m_lcm = 0;
}

Schedule::~Schedule()
{
    m_tasksets.clear();
}

Schedule& Schedule::operator=(const Schedule& schedule)
{
    if (this == &schedule)
    {
        return *this;
    }
    m_tasksets = schedule.m_tasksets;
    return *this;
}

void Schedule::addTaskSet(TaskSet taskset)
{
    m_tasksets.push_back(taskset);
}

void Schedule::removeTaskSet(int index)
{
    m_tasksets.erase(m_tasksets.begin() + index);
}

void Schedule::printTaskSets()
{
    for (int i = 0; i < m_tasksets.size(); i++)
    {
        cout << "Task Set " << i << endl;
        m_tasksets[i].printTasks();
    }
}

void Schedule::printTaskSet(int index)
{
    m_tasksets[index].printTasks();
}

// Reads in the task sets from a file
bool Schedule::readTaskSets(string filename)
{
    vector<TaskSet> taskSets;
    ifstream file(filename);
    if (!file)
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        if (line == "Begin")
        {
            int num_tasks;
            int num_processors;
            bool is_real_time;
            file >> num_tasks >> num_processors;
            char is_real_time_char;
            file >> is_real_time_char;
            if (is_real_time_char == 'Y')
            {
                is_real_time = true;
            }
            else
            {
                is_real_time = false;
            }
            file.ignore(); // Ignore the newline character
            TaskSet taskSet;
            taskSet.setNumProcessors(num_processors);
            taskSet.setNumTasks(num_tasks);
            taskSet.setPreemptive(is_real_time);

            for (int i = 0; i < num_tasks; ++i) {
                int count = 0;
                int start_time, computation, soft_deadline, hard_deadline, period;
                char comma;
                std::string taskName = "T" + std::to_string(i + 1);
                file >> start_time >> comma >> computation >> comma >> soft_deadline >> comma >> hard_deadline >> comma >> period;
                Task task(taskName, start_time, computation, soft_deadline, hard_deadline, period);
                taskSet.addTask(task);
            }

            taskSets.push_back(taskSet);
        }
    }
    file.close();
    m_tasksets = taskSets;
    return true;
}

// Writes the results of the algorithms to a file
void Schedule::writeResults(string filename, int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA)
{
    string schedule = "";
        // Open the file for writing
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    for (int i = 0; i < m_tasksets.size(); i++)
    {
        m_processors = m_tasksets[i].getNumProcessors();
        m_lcm = m_tasksets[i].getLCM();
        setAlgorithmsMemberVariables(EDF);
        setAlgorithmsMemberVariables(LLF);
        setAlgorithmsMemberVariables(AlgorithmA);

        EDF->setTaskSet(0, m_tasksets[i]);
        LLF->setTaskSet(0, m_tasksets[i]);
        AlgorithmA->setTaskSet(0, m_tasksets[i]);

        outFile << "* Task Set " << i+1 << endl;
        outFile << "****************************************************************************************************" << endl;
        outFile << "* Tasks: " << m_tasksets[i].getNumTasks() << endl; 
        outFile << "* Processors: " << m_tasksets[i].getNumProcessors() << endl;
        outFile << "* Utilization Rate: " << m_tasksets[i].calculateTotalUtilizationRate() << endl;
        outFile << "* USI: " << m_tasksets[i].USI() << endl;
        outFile << "* EUSI: " << m_tasksets[i].EUSI() << endl;
        outFile << "* LCM: " << m_tasksets[i].getLCM() << endl;
        outFile << "* Preemptive: " << (m_tasksets[i].getPreemptive() ? "True" : "False") << endl;
        for (int j = 0; j < m_tasksets[i].getNumTasks(); j++)
        {
            outFile << "* " << m_tasksets[i].getTask(j).getName() << ", S: " << m_tasksets[i].getTask(j).getStartTime() << ", C: " << m_tasksets[i].getTask(j).getComputationTime() << ", HD: " << m_tasksets[i].getTask(j).getHardDeadline() << ", SD: " << m_tasksets[i].getTask(j).getSoftDeadline() << ", P: " << m_tasksets[i].getTask(j).getPeriod() << endl;
        }
        outFile << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        outFile << "* EDF: " << endl;
        schedule = EDF->runEDF_Algorithm();
        outFile << "* " << schedule << endl;
        outFile << "* " << endl;
        double timeE = getAverageTimeEDF(i, numruns, EDF);
        outFile << "* Number of runs: " << numruns << endl;
        outFile << "* Average Time: " << timeE << " Microseconds" << endl;
        outFile << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        outFile << "* LLF: " << endl;
        // Instantiate LLF object
        schedule = LLF->CheckLLFSchedule();
        outFile << "* " << schedule << endl;
        double timeL = getAverageTimeLLF(i, numruns, LLF);
        outFile << "* Number of runs: " << numruns << endl;
        outFile << "* Average Time: " << timeL << " Microseconds" << endl;
        outFile << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        outFile << "* Algorithm-A: " << endl;
        schedule = AlgorithmA->algorithmASchedule();
        outFile << "* " << schedule << endl;
        double timeA = getAverageTimeAlgorithmA(i, numruns, AlgorithmA);
        outFile << "* Number of runs: " << numruns << endl;
        outFile << "* Average Time: " << timeA << " Microseconds" << endl;
        outFile << "* \n" << "****************************************************************************************************" << endl;
        outFile << "\n\n" << endl;
    }
    
    // Close the file
    outFile.close();
}

void Schedule::clearTaskSets()
{
    m_tasksets.clear();
}

int Schedule::getNumTaskSets()
{
    return m_tasksets.size();
}

TaskSet Schedule::getTaskSet(int index)
{
    return m_tasksets[index];
}

vector<TaskSet> Schedule::getTaskSets()
{
    return m_tasksets;
}

void Schedule::setTaskSet(int index, TaskSet taskset)
{
    m_tasksets[index] = taskset;
}

void Schedule::setTaskSets(vector<TaskSet> tasksets)
{
    m_tasksets = tasksets;
}

// getTimeEDF
// This functions runs the EDF algorithm on the taskset indicated, starts a timer before, and stops it after the algorithm is done
// It returns the time it took to run the algorithm
double Schedule::getTimeEDF(int index, Schedule* EDF)
{
    EDF->m_processors = m_tasksets[index].getNumProcessors();
    EDF->m_lcm = m_tasksets[index].getLCM();
    EDF->setTaskSet(0, m_tasksets[index]);

    auto start = timer::now();
    EDF->runEDF_Algorithm();
    auto end = timer::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

//getTimeLLF
//This functions runs the LLF algorithm on the taskset indicated, starts a timer before, and stops it after the algorithm is done
//It returns the time it took to run the algorithm
double Schedule::getTimeLLF(int index, Schedule* LLF)
{
    LLF->m_processors = m_tasksets[index].getNumProcessors();
    LLF->m_lcm = m_tasksets[index].getLCM();
;
    auto start = timer::now();
    LLF->CheckLLFSchedule();
    auto end = timer::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}


//getTimeAlgorithmA
//This functions runs the Algorithm A on the taskset indicated, starts a timer before, and stops it after the algorithm is done
//It returns the time it took to run the algorithm
double Schedule::getTimeAlgorithmA(int index, Schedule* AlgorithmA)
{
    AlgorithmA->m_processors = m_tasksets[index].getNumProcessors();
    AlgorithmA->m_lcm = m_tasksets[index].getLCM();
    AlgorithmA->setTaskSet(0, m_tasksets[index]);

    auto start = timer::now();
    AlgorithmA->algorithmASchedule();
    auto end = timer::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}



//getAverageTimeEDF(int numruns) Runs numruns and averages times
//This function runs the EDF algorithm on the taskset indicated numruns times, and averages the time it took to run the algorithm
//It returns the average time it took to run the algorithm
double Schedule::getAverageTimeEDF(int index, int numruns, Schedule* EDF)
{
    int sum = 0;
    for (int i = 0; i < numruns; i++)
    {
        sum += getTimeEDF(index, EDF);
    }
    return sum / numruns;
}



//getAverageTimeLLF
//This function runs the LLF algorithm on the taskset indicated numruns times, and averages the time it took to run the algorithm
//It returns the average time it took to run the algorithm
double Schedule::getAverageTimeLLF(int index, int numruns, Schedule* LLF)
{
    int sum = 0;
    for (int i = 0; i < numruns; i++)
    {
        sum += getTimeLLF(index, LLF);
    }
    return sum / numruns;
}


//getAverageTimeAlgorithmA
//This function runs the Algorithm A on the taskset indicated numruns times, and averages the time it took to run the algorithm
//It returns the average time it took to run the algorithm
double Schedule::getAverageTimeAlgorithmA(int index, int numruns, Schedule* AlgorithmA)
{
    int sum = 0;
    for (int i = 0; i < numruns; i++)
    {
        sum += getTimeAlgorithmA(index, AlgorithmA);
    }
    return sum / numruns;
}


//printScheduleEDF prints one edf schedule of index
void Schedule::printScheduleEDF(int index, int numruns, Schedule* EDF)
{
    m_processors = m_tasksets[index].getNumProcessors();
    m_lcm = m_tasksets[index].getLCM();
    setAlgorithmsMemberVariables(EDF);
    EDF->setTaskSet(0, m_tasksets[index]);

    cout << "* EDF Schedule for Task Set " << index+1 << endl;
    cout << "****************************************************************************************************" << endl;
    cout << "* Tasks: " << m_tasksets[index].getNumTasks() << endl; 
    cout << "* Processors: " << m_processors << endl;
    cout << "* Utilization Rate: " << m_tasksets[index].calculateTotalUtilizationRate() << endl;
    cout << "* USI: " << m_tasksets[index].USI() << endl;
    cout << "* EUSI: " << m_tasksets[index].EUSI() << endl;
    cout << "* LCM: " << m_lcm << endl;
    cout << "* Preemptive: " << (m_tasksets[index].getPreemptive() ? "True" : "False") << endl;
    for (int i = 0; i < m_tasksets[index].getNumTasks(); i++)
    {
        cout << "* " << m_tasksets[index].getTask(i).getName() << ", S: " << m_tasksets[index].getTask(i).getStartTime() << ", C: " << m_tasksets[index].getTask(i).getComputationTime() << ", HD: " << m_tasksets[index].getTask(i).getHardDeadline() << ", SD: " << m_tasksets[index].getTask(i).getSoftDeadline() << ", P: " << m_tasksets[index].getTask(i).getPeriod() << endl;
    }
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* EDF: " << endl;
    string schedule = EDF->runEDF_Algorithm();
    cout << "* " << schedule << endl;
    cout << "* " << endl;
    int time = getAverageTimeEDF(index, numruns, EDF);
    cout << "* Average Time: " << time << " Microseconds" << endl;
    cout << "* \n" << "****************************************************************************************************" << endl;
    cout << "\n\n" << endl;
}


//printScheduleLLF
//This function prints one LLF schedule of index
void Schedule::printScheduleLLF(int index, int numruns, Schedule* LLF)
{
    m_processors = m_tasksets[index].getNumProcessors();
    m_lcm = m_tasksets[index].getLCM();
    setAlgorithmsMemberVariables(LLF);

    LLF->setTaskSet(0, m_tasksets[index]);

    cout << "* LLF Schedule for Task Set " << index+1 << endl;
    cout << "****************************************************************************************************" << endl;
    cout << "* Tasks: " << m_tasksets[index].getNumTasks() << endl; 
    cout << "* Processors: " << m_processors<< endl;
    cout << "* Utilization Rate: " << m_tasksets[index].calculateTotalUtilizationRate() << endl;
    cout << "* USI: " << m_tasksets[index].USI() << endl;
    cout << "* EUSI: " << m_tasksets[index].EUSI() << endl;
    cout << "* LCM: " << m_lcm << endl;
    cout << "* Preemptive: " << (m_tasksets[index].getPreemptive() ? "True" : "False") << endl;
    for (int i = 0; i < m_tasksets[index].getNumTasks(); i++)
    {
        cout << "* " << m_tasksets[index].getTask(i).getName() << ", S: " << m_tasksets[index].getTask(i).getStartTime() << ", C: " << m_tasksets[index].getTask(i).getComputationTime() << ", HD: " << m_tasksets[index].getTask(i).getHardDeadline() << ", SD: " << m_tasksets[index].getTask(i).getSoftDeadline() << ", P: " << m_tasksets[index].getTask(i).getPeriod() << endl;
    }
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* LLF: " << endl;
    string schedule = LLF->CheckLLFSchedule();
    cout << "* " << schedule << endl;
    cout << "* " << endl;
    int time = getAverageTimeLLF(index, numruns, LLF);
    cout << "* Average Time: " << time << " Microseconds" << endl;
    cout << "* \n" << "****************************************************************************************************" << endl;
    cout << "\n\n" << endl;
}


//printScheduleAlgorithmA
//This function prints one Algorithm A schedule of index
void Schedule::printScheduleAlgorithmA(int index, int numruns, Schedule* AlgorithmA)
{
    m_processors = m_tasksets[index].getNumProcessors();
    m_lcm = m_tasksets[index].getLCM();
    setAlgorithmsMemberVariables(AlgorithmA);
    AlgorithmA->setTaskSet(0, m_tasksets[index]);

    cout << "* Algorithm-A Schedule for Task Set " << index+1 << endl;
    cout << "****************************************************************************************************" << endl;
    cout << "* Tasks: " << m_tasksets[index].getNumTasks() << endl; 
    cout << "* Processors: " << m_processors << endl;
    cout << "* Utilization Rate: " << m_tasksets[index].calculateTotalUtilizationRate() << endl;
    cout << "* USI: " << m_tasksets[index].USI() << endl;
    cout << "* EUSI: " << m_tasksets[index].EUSI() << endl;
    cout << "* LCM: " << m_lcm << endl;
    cout << "* Preemptive: " << (m_tasksets[index].getPreemptive() ? "True" : "False") << endl;
    for (int i = 0; i < m_tasksets[index].getNumTasks(); i++)
    {
        cout << "* " << m_tasksets[index].getTask(i).getName() << ", S: " << m_tasksets[index].getTask(i).getStartTime() << ", C: " << m_tasksets[index].getTask(i).getComputationTime() << ", HD: " << m_tasksets[index].getTask(i).getHardDeadline() << ", SD: " << m_tasksets[index].getTask(i).getSoftDeadline() << ", P: " << m_tasksets[index].getTask(i).getPeriod() << endl;
    }
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* Algorithm-A: " << endl;
    cout << "* " << AlgorithmA->algorithmASchedule() << endl;
    cout << "* " << endl;
    int time = getAverageTimeAlgorithmA(index, numruns, AlgorithmA);
    cout << "* Average Time: " << time << " Microseconds" << endl;
    cout << "* \n" << "****************************************************************************************************" << endl;
    cout << "\n\n" << endl;
}


//printSchedules prints schedules of all three algorithms for index
void Schedule::printSchedules(int index, int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA)
{
    m_processors = m_tasksets[index].getNumProcessors();
    m_lcm = m_tasksets[index].getLCM();
    setAlgorithmsMemberVariables(EDF);
    setAlgorithmsMemberVariables(LLF);
    setAlgorithmsMemberVariables(AlgorithmA);

    EDF->setTaskSet(0, m_tasksets[index]);
    LLF->setTaskSet(0, m_tasksets[index]);
    AlgorithmA->setTaskSet(0, m_tasksets[index]);

    cout << "* Task Set " << index+1 << endl;
    cout << "****************************************************************************************************" << endl;
    cout << "* Tasks: " << m_tasksets[index].getNumTasks() << endl; 
    cout << "* Processors: " << m_processors << endl;
    cout << "* Utilization Rate: " << m_tasksets[index].calculateTotalUtilizationRate() << endl;
    cout << "* USI: " << m_tasksets[index].USI() << endl;
    cout << "* EUSI: " << m_tasksets[index].EUSI() << endl;
    cout << "* LCM: " << m_lcm << endl;
    cout << "* Preemptive: " << (m_tasksets[index].getPreemptive() ? "True" : "False") << endl;
    for (int i = 0; i < m_tasksets[index].getNumTasks(); i++)
    {
        cout << "* " << m_tasksets[index].getTask(i).getName() << ", S: " << m_tasksets[index].getTask(i).getStartTime() << ", C: " << m_tasksets[index].getTask(i).getComputationTime() << ", HD: " << m_tasksets[index].getTask(i).getHardDeadline() << ", SD: " << m_tasksets[index].getTask(i).getSoftDeadline() << ", P: " << m_tasksets[index].getTask(i).getPeriod() << endl;
    }
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* EDF: " << endl;
    string schedule = EDF->runEDF_Algorithm();
    cout << "* " << schedule << endl;
    cout << "* " << endl;
    int timeE = getAverageTimeEDF(index, numruns, EDF);
    cout << "* Time: " << timeE << " Microseconds" << endl;
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* LLF: " << endl;
    schedule = LLF->CheckLLFSchedule();
    cout << "* " << schedule << endl;
    cout << "* " << endl;
    int timeL = getAverageTimeLLF(index, numruns, LLF);
    cout << "* Time: " << timeL << " Microseconds" << endl;
    cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
    cout << "* Algorithm-A: " << endl;
    cout << "* " << AlgorithmA->algorithmASchedule() << endl;
    cout << "* " << endl;
    int timeA = getAverageTimeAlgorithmA(index, numruns, AlgorithmA);
    cout << "* Time: " << timeA << " Microseconds" << endl;
    cout << "* \n" << "****************************************************************************************************" << endl;
    cout << "\n\n" << endl;
}

//printScheduleEDF prints edf schedule of all tasksets
void Schedule::printScheduleEDFAll(int numruns, Schedule * EDF)
{
    for (int i = 0; i < m_tasksets.size(); i++)
    {
        m_processors = m_tasksets[i].getNumProcessors();
        m_lcm = m_tasksets[i].getLCM();
        setAlgorithmsMemberVariables(EDF);

        EDF->setTaskSet(0, m_tasksets[i]);

        cout << "* EDF Schedule for Task Set " << i+1 << endl;
        cout << "****************************************************************************************************" << endl;
        cout << "* Tasks: " << m_tasksets[i].getNumTasks() << endl; 
        cout << "* Processors: " << m_processors << endl;
        cout << "* Utilization Rate: " << m_tasksets[i].calculateTotalUtilizationRate() << endl;
        cout << "* USI: " << m_tasksets[i].USI() << endl;
        cout << "* EUSI: " << m_tasksets[i].EUSI() << endl;
        cout << "* LCM: " << m_lcm << endl;
        cout << "* Preemptive: " << (m_tasksets[i].getPreemptive() ? "True" : "False") << endl;
        for (int j = 0; j < m_tasksets[i].getNumTasks(); j++)
        {
            cout << "* " << m_tasksets[i].getTask(j).getName() << ", S: " << m_tasksets[i].getTask(j).getStartTime() << ", C: " << m_tasksets[i].getTask(j).getComputationTime() << ", HD: " << m_tasksets[i].getTask(j).getHardDeadline() << ", SD: " << m_tasksets[i].getTask(j).getSoftDeadline() << ", P: " << m_tasksets[i].getTask(j).getPeriod() << endl;
        }
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* EDF: " << endl;
        string schedule = EDF->runEDF_Algorithm();
        cout << "* " << schedule << endl;
        cout << "* " << endl;
        int time = getAverageTimeEDF(i, numruns, EDF);
        cout << "* Average Time: " << time << " Microseconds" << endl;
        cout << "* \n" << "****************************************************************************************************" << endl;
        cout << "\n\n" << endl;
    }
}

//printScheduleLLF
//This function prints LLF schedule of all tasksets
void Schedule::printScheduleLLFAll(int numruns, Schedule* LLF)
{
    for (int i = 0; i < m_tasksets.size(); i++)
    {
        m_processors = m_tasksets[i].getNumProcessors();
        m_lcm = m_tasksets[i].getLCM();
        setAlgorithmsMemberVariables(LLF);

        LLF->setTaskSet(0, m_tasksets[i]);

        cout << "* LLF Schedule for Task Set " << i+1 << endl;
        cout << "****************************************************************************************************" << endl;
        cout << "* Tasks: " << m_tasksets[i].getNumTasks() << endl; 
        cout << "* Processors: " << m_processors << endl;
        cout << "* Utilization Rate: " << m_tasksets[i].calculateTotalUtilizationRate() << endl;
        cout << "* USI: " << m_tasksets[i].USI() << endl;
        cout << "* EUSI: " << m_tasksets[i].EUSI() << endl;
        cout << "* LCM: " << m_lcm << endl;
        cout << "* Preemptive: " << (m_tasksets[i].getPreemptive() ? "True" : "False") << endl;
        for (int j = 0; j < m_tasksets[i].getNumTasks(); j++)
        {
            cout << "* " << m_tasksets[i].getTask(j).getName() << ", S: " << m_tasksets[i].getTask(j).getStartTime() << ", C: " << m_tasksets[i].getTask(j).getComputationTime() << ", HD: " << m_tasksets[i].getTask(j).getHardDeadline() << ", SD: " << m_tasksets[i].getTask(j).getSoftDeadline() << ", P: " << m_tasksets[i].getTask(j).getPeriod() << endl;
        }
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* LLF: " << endl;
        string schedule = LLF->CheckLLFSchedule();
        cout << "* " << schedule << endl;
        cout << "* " << endl;
        int time = getAverageTimeLLF(i, numruns, LLF);
        cout << "* Average Time: " << time << " Microseconds" << endl;
        cout << "* \n" << "****************************************************************************************************" << endl;
        cout << "\n\n" << endl;
    }
}


//printScheduleAlgorithmA
//This function prints Algorithm A schedule of all tasksets
void Schedule::printScheduleAlgorithmAAll(int numruns, Schedule * AlgorithmA)
{
    for (int i = 0; i < m_tasksets.size(); i++)
    {
        m_processors = m_tasksets[i].getNumProcessors();
        m_lcm = m_tasksets[i].getLCM();
        setAlgorithmsMemberVariables(AlgorithmA);
        AlgorithmA->setTaskSet(0, m_tasksets[i]);

        cout << "* Algorithm-A Schedule for Task Set " << i+1 << endl;
        cout << "****************************************************************************************************" << endl;
        cout << "* Tasks: " << m_tasksets[i].getNumTasks() << endl; 
        cout << "* Processors: " << m_processors << endl;
        cout << "* Utilization Rate: " << m_tasksets[i].calculateTotalUtilizationRate() << endl;
        cout << "* USI: " << m_tasksets[i].USI() << endl;
        cout << "* EUSI: " << m_tasksets[i].EUSI() << endl;
        cout << "* LCM: " << m_lcm << endl;
        cout << "* Preemptive: " << (m_tasksets[i].getPreemptive() ? "True" : "False") << endl;
        for (int j = 0; j < m_tasksets[i].getNumTasks(); j++)
        {
            cout << "* " << m_tasksets[i].getTask(j).getName() << ", S: " << m_tasksets[i].getTask(j).getStartTime() << ", C: " << m_tasksets[i].getTask(j).getComputationTime() << ", HD: " << m_tasksets[i].getTask(j).getHardDeadline() << ", SD: " << m_tasksets[i].getTask(j).getSoftDeadline() << ", P: " << m_tasksets[i].getTask(j).getPeriod() << endl;
        }
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* Algorithm-A: " << endl;
        cout << "* " << AlgorithmA->algorithmASchedule() << endl;
        cout << "* " << endl;
        int time = getAverageTimeAlgorithmA(i, numruns, AlgorithmA);
        cout << "* Average Time: " << time << " Microseconds" << endl;
        cout << "* \n" << "****************************************************************************************************" << endl;
        cout << "\n\n" << endl;
    }
}


//printSchedules prints schedules of all three algorithms for all tasksets
void Schedule::printSchedulesAll(int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA)
{
    for (int i = 0; i < m_tasksets.size(); i++)
    {
        m_processors = m_tasksets[i].getNumProcessors();
        m_lcm = m_tasksets[i].getLCM();
        setAlgorithmsMemberVariables(EDF);
        setAlgorithmsMemberVariables(LLF);
        setAlgorithmsMemberVariables(AlgorithmA);
        
        EDF->setTaskSet(0, m_tasksets[i]);
        LLF->setTaskSet(0, m_tasksets[i]);
        AlgorithmA->setTaskSet(0, m_tasksets[i]);

        cout << "* Task Set " << i+1 << endl;
        cout << "****************************************************************************************************" << endl;
        cout << "* Tasks: " << m_tasksets[i].getNumTasks() << endl; 
        cout << "* Processors: " << m_processors << endl;
        cout << "* Utilization Rate: " << m_tasksets[i].calculateTotalUtilizationRate() << endl;
        cout << "* USI: " << m_tasksets[i].USI() << endl;
        cout << "* EUSI: " << m_tasksets[i].EUSI() << endl;
        cout << "* LCM: " << m_lcm << endl;
        cout << "* Preemptive: " << (m_tasksets[i].getPreemptive() ? "True" : "False") << endl;
        for (int j = 0; j < m_tasksets[i].getNumTasks(); j++)
        {
            cout << "* " << m_tasksets[i].getTask(j).getName() << ", S: " << m_tasksets[i].getTask(j).getStartTime() << ", C: " << m_tasksets[i].getTask(j).getComputationTime() << ", HD: " << m_tasksets[i].getTask(j).getHardDeadline() << ", SD: " << m_tasksets[i].getTask(j).getSoftDeadline() << ", P: " << m_tasksets[i].getTask(j).getPeriod() << endl;
        }
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* EDF: " << endl;
        string schedule = EDF->runEDF_Algorithm();
        cout << "* " << schedule << endl;
        cout << "* " << endl;
        int timeE = getAverageTimeEDF(i, numruns, EDF);
        cout << "* Average Time: " << timeE << " Microseconds" << endl;
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* LLF: " << endl;
        schedule = LLF->CheckLLFSchedule();
        cout << "* " << schedule << endl;
        cout << "* " << endl;
        int timeL = getAverageTimeLLF(i, numruns, LLF);
        cout << "* Average Time: " << timeL << " Microseconds" << endl;
        cout << "* \n" << "*---------------------------------------------------------------------------------------------------" << endl;
        cout << "* Algorithm-A: " << endl;
        cout << "* " << AlgorithmA->algorithmASchedule() << endl;
        cout << "* " << endl;
        int timeA = getAverageTimeAlgorithmA(i, numruns, AlgorithmA);
        cout << "* Average Time: " << timeA << " Microseconds" << endl;
        cout << "* \n" << "****************************************************************************************************" << endl;
        cout << "\n\n" << endl;
    }
}

//This adds in all of the instances of each task inside of the LCM
vector<Task> Schedule::addInAdditionalTasks(vector<Task> set) {
    int newStart = 0;
    int computation;
    int hardDeadline;
    int softDeadline;
    int period;
    string name;
    //Create new set with old tasks inside of it
    vector<Task> newSet = set;
    //For each task add the repeat tasks up until the lcm
    for (int i = 0; i < set.size(); i++) {
        computation = set[i].getComputationTime();
        hardDeadline = set[i].getHardDeadline();
        softDeadline = set[i].getSoftDeadline();
        period = set[i].getPeriod();
        name = set[i].getName();
        newStart = set[i].getStartTime();
        //If next instance of task fits in the lcm
        while (newStart + period < m_lcm) {
            //New start time is next period instance
            newStart += period;
            //increase both deadlines to next deadline
            softDeadline += period;
            hardDeadline += period;
            //Add next instance of task
            newSet.push_back(Task(name, newStart, computation, softDeadline, hardDeadline, period));
        }
    }
    return newSet;
}

//Configuring the vector<vector<Task>> to a readable string of task times.
//Should print out either task schedule or not feasible
/*
    EX:
        P1:T1[0,2) T3[2,6)
        P2:T2[0,4)
*/
string Schedule::configureSetToOutput(vector<vector<Task>> schedule, bool feasible) {
    string output = "";
    string task = "";
    int totalComputation = 0;
    int endOfTask = 0;
    if (feasible) {
        //For each chain/processor
        for (int i = 0; i < m_processors; i++) {
            output.append("P" + to_string(i+1) + ":");
            //For each element in the chain
            for (int j = 0; j < schedule[i].size(); j++) {
                endOfTask = totalComputation + schedule[i][j].getComputationTime();
                //Creates string of task interval. Ex) T1[0,3)
                if (schedule[i][j].getName() != "G") {
                    task = schedule[i][j].getName() + '[' + to_string(totalComputation) + ',' + to_string(endOfTask) + ") ";
                }
                else {
                    task = "-- ";
                }
                output.append(task);
                totalComputation = endOfTask;
            }
            totalComputation = 0;
            endOfTask = 0;
            //Adding new line for next processor
            output.append("\n");
        }
    }
    else {
        output.append("Not Feasible");
    }

    return output;
}

string Schedule::algorithmASchedule() { return ""; }
string Schedule::runEDF_Algorithm() { return ""; }
string Schedule::CheckLLFSchedule() { return ""; }

void Schedule::setAlgorithmsMemberVariables(Schedule* Algorithm)
{
    Algorithm->m_processors = m_processors;
    Algorithm->m_lcm = m_lcm;
}