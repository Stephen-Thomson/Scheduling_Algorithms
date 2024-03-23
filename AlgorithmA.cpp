/************************************************************************************************
* File: AlgorithmA.cpp
* Author: Abbey DuBois, Stephen Thomson
* Date: 3/14/2024
* Description: File for Algorithm A where functions are defined
*************************************************************************************************/

#include "AlgorithmA.h"
#include "TOR.h"

//Constructor takes the initial task set
//Will Sort the set, find lcm and create the TOR list
AlgorithmA::AlgorithmA() {
    m_processors = 0;
    //This is to add in all of tasks needed to hit the lcm time unit into m_set
    m_set = vector<Task>();
    m_gap = 0;
}

//Constructor takes the initial task set
//Will Sort the set, find lcm and create the TOR list
AlgorithmA::AlgorithmA(TaskSet taskSet) {
    //This is to add in all of tasks needed to hit the lcm time unit into m_set
    m_set = addInAdditionalTasks(taskSet.getTasks());
    //Sorts m_set
    sortTaskSetChronologically();
    //Creates TOR list and assigns it to m_torList
    createTORList();
}

//Basic psuedo code Implementation of Algorithm A
//1: Check if feasible through USI
//2: Need to sort tasks in task set chronologically d-c primary d as secondary
//3: Set the begining of the chains (T1..T2...T(USI)) and remove them from task set as they are placed
//4: In while loop where it's feasible and non empty
    /*
        Choose task where Computation of Task + Computation of Chain is less than Deadline of T
        If chain exists
            add task to chain
            Remove from Task Set
        Else
            Choose chain where last in chain can swap with current task (Will be from list given from TOR)
            If chain exists
                Take out last in chain
                Add in Task
                Add in last in chain again
                remove task from set
            Else
                Set is infeasible
    */
    //5: Return Chains to main
string AlgorithmA::algorithmASchedule() {
    bool feasible = true;
    bool added = false;
    m_gap = 0;
    vector<int>chainComputation(m_processors);
    vector<int>sortedProcessors(m_processors);
    vector<vector<Task>> schedule(m_processors);
    int computation;
    int deadline;
    int start;
    int index;

    //Set the beginning of the chains
    //Erase the set tasks from m_set
    for (int i = 0; i < m_processors; i++) {
        schedule[i].push_back(m_set[0]);
        chainComputation[i] = m_set[0].getComputationTime();
        m_set.erase(m_set.begin());
    }

    //feasible and non empty
    while (m_set.size() != 0 && feasible) {
        computation = m_set[0].getComputationTime();
        deadline = m_set[0].getHardDeadline();
        start = m_set[0].getStartTime();

        sortedProcessors = sortProcessors(chainComputation, start);
        added = false;

        for (int i = 0; i < m_processors && !added; i++) {
            index = sortedProcessors[i];

            //See if you can add the current task to the current chain without swapping
            m_gap = 0;

            //If the start time is after totalComputationTime calculate the gap needed
            if (start > chainComputation[index])
            {
                m_gap = start - chainComputation[index];
            }

            //Can the task fit before it's deadline?
            if (m_gap + chainComputation[index] + computation <= deadline) {

                //If there is a gap add that into computation time and add gap to schedule
                if (m_gap > 0) {
                    //Add gap to the scedule
                    schedule[index].push_back(Task("G", chainComputation[index], m_gap, 0, chainComputation[index] + m_gap, 0));
                    chainComputation[index] += m_gap;
                }
                //Add it's computation Time
                chainComputation[index] += computation;
                //Add Task to Chain
                schedule[index].push_back(m_set[0]);
                //Erase Task from set
                m_set.erase(m_set.begin());
                added = true;
            }
            //See if you can add the current Task to current chain with swapping
            else if (inTORList(m_set[0], schedule[index].back(), chainComputation[index])) {

                Task potentialGap = Task("", 0, 0, 0, 0, 0);
                int size = schedule[index].size();
                if (size - 2 >= 0) {
                    //Get the second to last task in the chain
                    potentialGap = schedule[index][schedule[index].size() - 2];
                }
                //Save last task
                Task swappedTask = schedule[index].back();
                //Delete task from chain
                schedule[index].pop_back();

                //Was the second to last a gap?
                if (potentialGap.getName() == "G") {
                    //Is there a possibility of getting rid or shortening gap?
                    if (start < potentialGap.getHardDeadline()) {
                        //Can you get rid of gap completely?
                        if (start <= potentialGap.getStartTime()) {
                            //Delete gaps computation from the chain
                            chainComputation[index] -= schedule[index].back().getComputationTime();
                            //Delete Gap from chain
                            schedule[index].pop_back();
                        }
                        //Getting rid of gap partially
                        else {
                            //Getting the new gap between the start times of the tasks
                            m_gap = start - potentialGap.getStartTime();
                            //Delete old gaps computation from the chain
                            chainComputation[index] -= schedule[index].back().getComputationTime();
                            //Delete old gap from chain
                            schedule[index].pop_back();
                            //Add new gap to the scedule
                            schedule[index].push_back(Task("G", chainComputation[index], m_gap, 0, chainComputation[index] + m_gap, 0));
                        }
                    }
                }

                //Add current Task then saved task back into chain
                schedule[index].push_back(m_set[0]);
                schedule[index].push_back(swappedTask);

                //Add current Tasks computation to chain
                chainComputation[index] += computation;
                //Erase Task from set
                m_set.erase(m_set.begin());
                added = true;
            }
        }

        if (!added) {
            feasible = false;
        }

        //Double checking that all's good and tasks meet deadline
        for (int i = 0; i < m_processors; i++) {
            if (chainComputation[i] > schedule[i].back().getHardDeadline()) {
                feasible = false;
            }
        }
    }

    return configureSetToOutput(schedule, feasible);
}

//Checks if there is an opportunity to swap between the two tasks,
//If there is an opportunity, adds it to a list of swappable Tasks
void AlgorithmA::createTORList() {
    //For TOR
    //1: TOR is Empty Set
    //2: For each task in the Task set (Passed in)
        /*
            If the Deadline of task - computation of task > 0
                For each other task in the set (Comparing all other tasks to the one your looking at)
                    If the deadline of new task is less than the current time unit
                        If the computation time of original task + computation time of new task is less than or equal to deadline of original task
                            Add the available Swap to the TOR List
        */
    int lowerEnd;
    int higherEnd;
    int C1;
    int C2;
    int D1;
    int D2;

    //Look through each task in the task set
    for (int i = 0; i < m_set.size(); i++) {
        C1 = m_set[i].getComputationTime();
        D1 = m_set[i].getHardDeadline();
        //Check if there is wiggle room between the computation and deadline of the set
        if (D1 - C1 > 0) {
            //Start comparing current task to all other tasks in the set
            for (int j = i + 1; j < m_set.size() - 1; j++) {
                C2 = m_set[j].getComputationTime();
                D2 = m_set[j].getHardDeadline();

                //Does the new task end faster than current? Does the current task not start after the new task?
                if (D2 < D1 && m_set[i].getStartTime() < D2) {
                    //Making it one above the lower bounds so it is not equal to D2
                    lowerEnd = D2 - (C1 + C2) + 1;
                    higherEnd = D1 - (C1 + C2);

                    //Are the time + computations greater than deadline 2 and lessthan or equal to deadline 2?
                    if (lowerEnd + C1 + C2 > D2 && lowerEnd + C1 + C2 <= D1) {
                        if (higherEnd + C1 + C2 > D2 && higherEnd + C1 + C2 <= D1) {
                            m_torList.push_back(TOR(m_set[i], m_set[j], lowerEnd, higherEnd));
                        }
                    }
                }
            }
        }
    }
};

//Sorts the Tasks chronologically with a flagged bubble sort
void AlgorithmA::sortTaskSetChronologically() {
    bool swapped = true;
    int size = m_set.size() - 1;

    for (int i = 0; i < size && swapped == true; i++) {
        swapped = false;
        for (int j = 0; j < size - i; j++) {
            if (canSwap(m_set[j], m_set[j + 1])) {
                swap(m_set[j], m_set[j + 1]);
                swapped = true;
            }
        }
    }
}

//Sorts the Lowest computation processors with a flagged bubble sort
vector<int> AlgorithmA::sortProcessors(vector<int> chainComputation, int start) {
    vector<int>indexes;
    vector<int>tempComputation = chainComputation;
    int index = 0;
    vector<int>::iterator iter;

    while (tempComputation.size() > 0) {
        index = 0;
        //Find lowest element in array
        for (int i = 1; i < tempComputation.size(); i++) {
            index = tempComputation[index] < tempComputation[i] ? index : i;
        }
        //Find where element is in chaincompuation
        iter = find(chainComputation.begin(), chainComputation.end(), tempComputation[index]);
        //Push that index into indexes
        indexes.push_back(iter - chainComputation.begin());
        //Take out element in tempComputation
        tempComputation.erase(tempComputation.begin() + index);
    }


    ////Setting the initial indexes
    //for (int i = 0; i < chainComputation.size(); i++) {
    //    indexes[i] = i;
    //}

    //for (int i = 0; i < chainComputation.size(); i++) {
    //    swapped = false;
    //    for (int j = i; j < chainComputation.size() - 1 && swapped != true; j++) {

    //        //How long is it until the task can start?
    //        diff1 = start - tempComputation[j] < 0 ? 0 : start - tempComputation[j];
    //        diff2 = start - tempComputation[j+1] < 0 ? 0 : start - tempComputation[j+1];

    //        //Swap if it takes longer for task to start on this processor
    //        //Else take the one with the lowest computation
    //        if (diff1 > diff2) {
    //            swap(indexes[j], indexes[j + 1]);
    //            swap(tempComputation[j], tempComputation[j + 1]);
    //            swapped = true;
    //        }
    //        else if (diff1 == diff2 && tempComputation[j] > tempComputation[j + 1]) {
    //            swap(indexes[j], indexes[j + 1]);
    //            swap(tempComputation[j], tempComputation[j + 1]);
    //            swapped = true;
    //        }
    //    }
    //}
    return indexes;
}

//Checks if two tasks can swap
//First checks between the tasks the deadline - computation for both (lowest goes first)
//Then checks between the tasks dedline
bool AlgorithmA::canSwap(Task task1, Task task2) {
    bool swap = false;
    int firstLaxity = task1.getHardDeadline() - task1.getComputationTime();
    int secondLaxity = task2.getHardDeadline() - task2.getComputationTime();

    if (task1.getStartTime() > task2.getStartTime()) {
        swap = true;
    }
    else if (task1.getStartTime() == task2.getStartTime()) {
        if (firstLaxity > secondLaxity) {
            swap = true;
        }
        else if (firstLaxity == secondLaxity && task1.getHardDeadline() > task2.getHardDeadline()) {
            swap = true;
        }
    }

    return swap;
}

//Checks if the two tasks are in the TOR list and if they have availability to swap
//m_torList <current Task, new Task, time that they can swap>
bool AlgorithmA::inTORList(Task currentTask, Task lastTask, int chainComputation) {
    //Potential start of the task (Does not take into consideration of gap)
    int start = chainComputation - lastTask.getComputationTime();
    bool canSwap = false;
    string firstTask;
    string secondTask;
    int lowerEnd;
    int higherEnd;

    //For each element in the list check if both tasks are inside of the tuple and if they could swap due to the time unit
    for (int i = 0; i < m_torList.size(); i++) {
        firstTask = m_torList[i].getT1().getName();
        secondTask = m_torList[i].getT2().getName();
        lowerEnd = m_torList[i].getLowerEnd();
        higherEnd = m_torList[i].getHigherEnd();

        //Does the start time fall between the swapping range of the tasks?
        if (start >= lowerEnd && start <= higherEnd) {
            //Do the names line up with the tasks?
            if (firstTask == currentTask.getName() && secondTask == lastTask.getName()) {
                canSwap = true;
            }
            else if (secondTask == currentTask.getName() && firstTask == lastTask.getName()) {
                canSwap = true;
            }
        }
    }
    return canSwap;
}

void AlgorithmA::setTaskSet(int index, TaskSet taskSet) {
    m_set = addInAdditionalTasks(taskSet.getTasks());
    sortTaskSetChronologically();
    m_torList.clear();
    //Creates TOR list and assigns it to m_torList
    createTORList();

}