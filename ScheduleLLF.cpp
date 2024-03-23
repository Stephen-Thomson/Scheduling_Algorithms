/****************************************
Author: Zane Omelchuck
Filename: ScheduleLLF.cpp
*****************************************/

#include "ScheduleLLF.h"

int PartitionLaxity(std::vector<Task>& v, int start, int end)
{
    int pivot = end;
    int j = start;
    for (int i = start; i < end; ++i) {
        if (v[i].getLaxity() < v[pivot].getLaxity()) {
            swap(v[i], v[j]);
            ++j;
        }
    }
    swap(v[j], v[pivot]);
    return j;
}

int PartitionDeadline(std::vector<Task>& v, int start, int end)
{
    int pivot = end;
    int j = start;
    for (int i = start; i < end; ++i) {
        if (v[i].getHardDeadline() < v[pivot].getHardDeadline()) {
            swap(v[i], v[j]);
            ++j;
        }
    }
    swap(v[j], v[pivot]);
    return j;
}

void Quicksort(std::vector<Task>& v, int start, int end)
{
    if (start < end) 
    {
        int p = PartitionDeadline(v, start, end);
        Quicksort(v, start, p - 1);
        Quicksort(v, p + 1, end);
        p = PartitionLaxity(v, start, end);
        Quicksort(v, start, p - 1);
        Quicksort(v, p + 1, end);
    }

}

bool ScheduleLLF::CaclualteLaxity(std::vector<Task>& tasks, int time)
{
    for (int i = 0; i < tasks.size(); i++)
    {
        //(deadline * period)-(time+computation)
        int laxity = (tasks[i].getPeriod() * tasks[i].getCurrPeriod()) - 
            (time + tasks[i].getComputationTime());
        tasks[i].setLaxity(laxity);
        
        //check to make sure all tasks are before deadline
        if (tasks[i].getPeriod() * tasks[i].getCurrPeriod() < time)
            return false;

        //cout << tasks[i].getName() << " Laxity:" << tasks[i].getLaxity() << " Period:" << tasks[i].getCurrPeriod() << " -> time: " << time << endl;
        if (laxity < 0)
            return false;
    }   

    return true;
}

ScheduleLLF::ScheduleLLF(TaskSet taskSet)
{
    m_numTasks = taskSet.getNumTasks();
    m_processors = taskSet.getNumProcessors();
    m_taskSet = taskSet;
    m_lcm = 1;
}
ScheduleLLF::ScheduleLLF()
{
    m_numTasks = 0;
    m_processors = 0;
    m_taskSet = TaskSet();
    m_lcm = 0;
}

string ScheduleLLF::CheckLLFSchedule()
{
    Task sleepTask = { "G" };
    std::vector<int> time;
    std::vector<Task> tasks = m_taskSet.getTasks();
    int tasks_size = tasks.size() - 1;
    std::vector<int> period_tracker;
    vector<vector<Task>> schedule(m_processors);

    //initilize multipprosessing times
    for (int i = 0; i < m_processors; i++)
        time.push_back(0);

    //cant have a negative lcm
    if(m_lcm < 1)
        return configureSetToOutput(schedule, false);

    while (1)
    {
        //for each processor
        for (int ii = 0; ii < m_processors; ii++)
        {
            bool isLaxGreaterThenZero = CaclualteLaxity(tasks, time[ii]);
            if (!isLaxGreaterThenZero)
                return configureSetToOutput(schedule, false);

            Quicksort(tasks, 0, tasks_size);
            //cout << "__+" << tasks[0].getName() << " Laxity:" << tasks[0].getLaxity() << endl;

            if (tasks[0].getCurrPeriod() > 1)
            {
                //if time is before next period, wait till next period
                if (tasks[0].getPeriod() * (tasks[0].getCurrPeriod() - 1) > time[ii])
                { 
                    //adds remining time before period then runs task
                    int restTime = tasks[0].getPeriod() * (tasks[0].getCurrPeriod() - 1);
                    sleepTask.setComputationTime(restTime - time[ii]);
                    time[ii] = (restTime) + tasks[0].getComputationTime();
                    tasks[0].incramentCurrPeriod();
                    schedule[ii].push_back(sleepTask);
                    schedule[ii].push_back(tasks[0]);
                }
                //if task is in proper period
                else if (tasks[0].getPeriod() * tasks[0].getCurrPeriod() >= time[ii])
                {
                    time[ii] = time[ii] + tasks[0].getComputationTime();
                    tasks[0].incramentCurrPeriod();
                    schedule[ii].push_back(tasks[0]);
                }
            }
            //account for start times
            else
            {
                if (tasks[0].getStartTime() > time[ii])
                {
                    sleepTask.setComputationTime(tasks[0].getStartTime());
                    schedule[ii].push_back(sleepTask);
                    time[ii] = tasks[0].getStartTime();
                }
                    

                time[ii] = time[ii] + tasks[0].getComputationTime();
                tasks[0].incramentCurrPeriod();
                schedule[ii].push_back(tasks[0]);
            }

            if (time[ii] > m_lcm)
            {
                //cout << "Ending Time: " << time[ii] << endl;
                return configureSetToOutput(schedule, false);
            }
            if (time[ii] == m_lcm)
            {
                for (int j = 0; j < tasks.size(); j++)
                {
                    //not all tasks were ran
                    if (tasks[j].getCurrPeriod() == 1)
                        return configureSetToOutput(schedule, false);
                }

                return configureSetToOutput(schedule, true);
            }
        }
    }

    return configureSetToOutput(schedule, false);;
}

void ScheduleLLF::setTaskSet(int index, TaskSet taskSet) {
    m_taskSet = taskSet;
    m_numTasks = taskSet.getNumTasks();
}