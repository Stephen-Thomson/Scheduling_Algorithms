#pragma once
/************************************************************************************************
* File: Schedule.h
* Author: Stephen Thomson & Stephen Carter & Abbey DuBois
* Date: 3/14/2024
* Description: This is the header file for the Schedule class.
* This class is used to store a list of tasksets and perform operations on them.
*************************************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "TaskSet.h"

class Schedule
{
    private:
        vector<TaskSet> m_tasksets;

    public:
        Schedule();

        ~Schedule();

        Schedule& operator=(const Schedule& schedule);

        void addTaskSet(TaskSet taskset);

        void removeTaskSet(int index);

        void printTaskSets();

        void printTaskSet(int index);

        bool readTaskSets(string filename);

        void writeResults(string filename, int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA);

        void clearTaskSets();

        int getNumTaskSets();

        TaskSet getTaskSet(int index);

        vector<TaskSet> getTaskSets();

        virtual void setTaskSet(int index, TaskSet taskset);

        void setTaskSets(vector<TaskSet> tasksets);

        double getTimeEDF(int index, Schedule* EDF);

        double getTimeLLF(int index, Schedule* LLF);

        double getTimeAlgorithmA(int index, Schedule* AlgorithmA);

        double getAverageTimeEDF(int index, int numruns, Schedule* EDF);

        double getAverageTimeLLF(int index, int numruns, Schedule* LLF);

        double getAverageTimeAlgorithmA(int index, int numruns, Schedule* AlgorithmA);

        void printScheduleEDF(int index, int numruns, Schedule* EDF);

        void printScheduleLLF(int index, int numruns,  Schedule* LLF);

        void printScheduleAlgorithmA(int index, int numruns,  Schedule* AlgorithmA);

        void printSchedules(int index, int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA);

        void printScheduleEDFAll(int numruns, Schedule * EDF);

        void printScheduleLLFAll(int numruns, Schedule* LLF);

        void printScheduleAlgorithmAAll(int numruns, Schedule * AlgorithmA);

        void printSchedulesAll(int numruns, Schedule* EDF, Schedule* LLF, Schedule* AlgorithmA);

        virtual string algorithmASchedule();
        virtual string runEDF_Algorithm();
        void setAlgorithmsMemberVariables(Schedule* Algorithm);
        virtual string CheckLLFSchedule();

    protected:
        unsigned long long m_lcm;
        int m_processors;
        vector<Task> addInAdditionalTasks(vector<Task> set);
        string configureSetToOutput(vector<vector<Task>> schedule, bool feasible);
};