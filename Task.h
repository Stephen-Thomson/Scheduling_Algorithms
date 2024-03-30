#pragma once
/************************************************************************
 * Task.h
 * Author: Stephen Thomson
 * Date: 2/8/2024
 * Description: This file contains the Task class which is used to store
 *             information about a task in the system.
 * *********************************************************************/

#include <iostream>
#include <string>

using namespace std;

class Task 
{
    private:
        string m_name;
        int m_startTime;
        int m_computationTime;
        int m_hardDeadline;
        int m_softDeadline;
        int m_period;
        int m_currPeriod;
        int m_laxity;

    public:
        Task(string name = "", int startTime = 0, int computationTime = 0, int softDeadline = 0, int hardDeadline = 0, int period = 0);

        Task& operator=(const Task& task);

        int getStartTime();

        void setStartTime(int startTime);

        int getComputationTime();

        void setComputationTime(int computationTime);

        int getHardDeadline();

        void setHardDeadline(int hardDeadline);

        int getSoftDeadline();

        void setSoftDeadline(int softDeadline);

        int getPeriod();

        void setPeriod(int period);

        void printTask();

        void setName(string name);

        string getName();

        void setLaxity(int laxity);

        int getLaxity();

        void incramentCurrPeriod();

        int getCurrPeriod();
};
