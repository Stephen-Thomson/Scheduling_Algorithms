//***************************************************
// Author: Erik Valadez-Ramos, Stephen Thomson
// Date: 2/15/2024
//
// Function definitions for earliest deadline first algorithm
//
#include "edf_algorithm.h"
EDF_Algorithm::EDF_Algorithm()
{
	m_set = vector<Task>(0);
	m_processors = 0;
	m_lcm = 0;
}

EDF_Algorithm::EDF_Algorithm(TaskSet taskSet)
{
	m_set = taskSet.getTasks();
	m_processors = taskSet.getNumProcessors();
	m_lcm = getLCM();
}

unsigned long long EDF_Algorithm::getLCM()
{
	unsigned long long LCM = m_set[0].getPeriod();

	for (Task task : m_set)
	{
		LCM = lcm(LCM, task.getPeriod());
	}
	if (LCM > 500 || LCM < 0)
    {
        LCM = 500;
    }

	return LCM;
}

string EDF_Algorithm::runEDF_Algorithm()
{
	// Make list of full periodic tasks.
	if (m_lcm > 0)
		addInAdditionalTasks(m_set);

	// Sort m_set by earliest deadline first
	// Flagged Bubble Sort
	bool sorted = false;
	int numTasks = m_set.size();

	for (int ii = 0; ii < numTasks - 1 && !sorted; ii++)
	{
		sorted = true;
		for (int jj = 0; jj < numTasks - ii - 1; jj++)
		{
			if (m_set[jj].getHardDeadline() + m_set[jj].getStartTime() >
				m_set[jj + 1].getHardDeadline() + m_set[jj + 1].getStartTime())
			{
				swap(m_set[jj], m_set[jj + 1]);
				sorted = false;
			}
		}
	}

	// Can we run EDF-Algorithm?
	int computation_counter = 0, processor_minimum = 0;
	bool feasible = true;
	vector<vector<Task>> processors(m_processors);
	vector<int> processor_time(m_processors);

	for (int ii = 0; feasible && ii < numTasks; ii++)
	{
		// Give the first task to a processor
		Task task = m_set[ii];

		int gap = 0;

		// Is there a gap in between tasks?
		if (computation_counter < task.getStartTime())
		{
			// Can another task start now?
			bool done = false;
			int earliest = ii;
			for (int jj = ii + 1; !done && jj < numTasks; jj++)
			{
				if (m_set[earliest].getStartTime() > m_set[jj].getStartTime())
					earliest = jj;
				if (computation_counter >= m_set[earliest].getStartTime())
					done = true;
			}

			// A swap needs to be done if another task can execute now.
			while (earliest != ii)
			{
				swap(m_set[earliest], m_set[earliest - 1]);
				earliest--;
			}

			task = m_set[ii];

			gap = task.getStartTime() - computation_counter;
			if (gap > 0)
			{
				processors[processor_minimum].push_back(Task("G", computation_counter, gap));
				computation_counter += gap;
			}
		}

		//Does the task meet its deadline?
		if (computation_counter + task.getComputationTime() <= task.getHardDeadline())
		{
			processors[processor_minimum].push_back(task);
			processor_time[processor_minimum] = computation_counter + task.getComputationTime();
		}
		else
			feasible = false;


		//Update next processor index and computation counter.
		if (processor_time.size() > 1)
		{
			int lowest_time = processor_time[processor_minimum];
			for (int jj = 0; jj < processor_time.size(); jj++)
			{
				// Set counter to minimum of processors
				lowest_time = min(lowest_time, processor_time[jj]);
				if (processor_time[jj] < processor_time[processor_minimum])
					processor_minimum = jj;
			}

			computation_counter = lowest_time;
		}
		else
			computation_counter += task.getComputationTime();
	}

	if (!feasible)
		return configureSetToOutput(processors, feasible);

	if (m_lcm > 0)
	{
		// Final check to ensure all tasks were completed within the proper window.
		int largest_cpu_val = processor_time[0];
		for (int jj = 1; jj < processor_time.size(); jj++)
		{
			// Set counter to minimum of processors
			largest_cpu_val = max(largest_cpu_val, processor_time[jj]);
		}

		if (largest_cpu_val > m_lcm)
			return configureSetToOutput(processors, !feasible);
	}

	return configureSetToOutput(processors, feasible);
}

void EDF_Algorithm::setTaskSet(int index, TaskSet taskSet) {
	m_set = addInAdditionalTasks(taskSet.getTasks());
}