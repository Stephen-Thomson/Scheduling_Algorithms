/************************************************************************
 * Task.cpp
 * Author: Stephen Thomson
 * Date: 2/8/2024
 * Description: This file contains the implementation of the Task class
 * 
 * *********************************************************************/

#include <iostream>
#include <string>
#include "Task.h"

using namespace std;

Task::Task(string name, int startTime, int computationTime, int softDeadline, int hardDeadline, int period)
{
    m_name = name;
    m_startTime = startTime;
    m_computationTime = computationTime;
    m_hardDeadline = hardDeadline;
    m_softDeadline = softDeadline;
    m_period = period;
    m_currPeriod = 1;
    m_laxity = hardDeadline - startTime - computationTime;
}

Task& Task::operator=(const Task& task)
{
    if (this == &task)
    {
        return *this;
    }
    m_name = task.m_name;
    m_startTime = task.m_startTime;
    m_computationTime = task.m_computationTime;
    m_hardDeadline = task.m_hardDeadline;
    m_softDeadline = task.m_softDeadline;
    m_period = task.m_period;
    m_currPeriod = task.m_currPeriod;
    m_laxity = task.m_laxity;
    return *this;
}

int Task::getStartTime(){
    return m_startTime;
}

void Task::setStartTime(int startTime){
    m_startTime = startTime;
}

int Task::getComputationTime(){
    return m_computationTime;
}

void Task::setComputationTime(int computationTime){
    m_computationTime = computationTime;
}

int Task::getHardDeadline(){
    return m_hardDeadline;
}

void Task::setHardDeadline(int hardDeadline){
    m_hardDeadline = hardDeadline;
}

int Task::getSoftDeadline(){
    return m_softDeadline;
}

void Task::setSoftDeadline(int softDeadline){
    m_softDeadline = softDeadline;
}

int Task::getPeriod(){
    return m_period;
}

void Task::setPeriod(int period){
    m_period = period;
}

void Task::printTask(){
    std::cout << "* " << m_name << ", S: " << m_startTime << ", C: " << m_computationTime << ", HD: " << m_hardDeadline << ", SD: " << m_softDeadline << ", P: " << m_period << std::endl;
    
}

void Task::setName(string name){
    m_name = name;
}

string Task::getName(){
    return m_name;
}

void Task::setLaxity(int laxity)
{
    m_laxity = laxity;
}

int Task::getLaxity()
{
    return m_laxity;
}

void Task::incramentCurrPeriod()
{
    m_currPeriod++;
}

int Task::getCurrPeriod()
{
    return m_currPeriod;
}