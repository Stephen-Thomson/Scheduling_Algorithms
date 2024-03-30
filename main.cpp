/************************************************************************************************
* File: main.cpp
* Author: Stephen Thomson & Stephen Carter & Abbey DuBois
* Date: 3/14/2024
* Description: This is the main file for running the project
*************************************************************************************************/

#include <iostream>
#include "Schedule.h"
#include "AlgorithmA.h"
#include "edf_algorithm.h"
#include "ScheduleLLF.h"

string getFileName()
{
    string fileName;
    cout << "Please enter the name of the file you would like to read in: ";
    cin >> fileName;
    return fileName;
}

string getWriteFileName()
{
    string fileName;
    cout << "Please enter the name of the file you would like to write to: ";
    cin >> fileName;
    return fileName;
}

int main(int argc, char* argv[])
{
    // Create objects and variables
    Schedule schedule = Schedule();
    AlgorithmA algoA = AlgorithmA();
    EDF_Algorithm edf = EDF_Algorithm();
    ScheduleLLF llf = ScheduleLLF();
    int selection;
    string fileName;
    string writeFileName;
    bool invalid = true;
    bool exit = false;
    int printWrite = 0;
    int numRuns = 0;
    int chosenTaskSet = 0;
    int chosenAlgorithm = 0;


    // While not exit:
    while(!exit)
    {
        while(invalid)
        {
            // Selection of what file to read or enter own text file or exit.
            cout << "What File would you like to read in for the Task Sets?" << endl;
            cout << "[1] Small task set file\n"
                << "[2] Task sets from class\n"
                << "[3] Large Task Set File\n"
                << "[4] Personal (needs to be added to Resource files)\n"
                << "[5] Exit\n";
            cin >> selection;

            switch(selection)
            {
                case 1:
                    fileName = "TestTaskSet.txt";
                    invalid = false;
                    break;
                case 2:
                    fileName = "TaskSets.txt";
                    invalid = false;
                    break;
                case 3:
                    fileName = "RTaskSets.txt";
                    invalid = false;
                    break;
                case 4:
                    fileName = getFileName();
                    invalid = false;
                    break;
                case 5:
                    exit = true;
                    invalid = false;
                    break;
                default:
                    cout << "Invalid selection. Please try again." << endl;
                    break;
            }
        }

        if(exit)
        {
            break;
        }

        // Read in the file, if it fails, return to beginning
        if(!schedule.readTaskSets(fileName))
        {
            cout << "File read failed. Please try again." << endl;
            invalid = true;
            continue;
        }

        invalid = true;
        // Print or Write file
        while(invalid)
        {
            cout << "Would you like to print the Task Sets or write the results to a file?" << endl;
            cout << "[1] Print Task Sets\n"
                << "[2] Write Results\n"
                << "[3] Exit\n";
            cin >> selection;

            switch(selection)
            {
                case 1:
                    printWrite = 1;
                    invalid = false;
                    break;
                case 2:
                    printWrite = 2;
                    writeFileName = getWriteFileName();
                    invalid = false;
                    break;
                case 3:
                    invalid = false;
                    exit = true;
                    break;
                default:
                    cout << "Invalid selection. Please try again." << endl;
                    break;
            }
        }

        if(exit)
        {
            break;
        }

        invalid = true;
        // Choose a specific task set or all of them
        // Select specific task or set to 0 for all
        // Check to make sure the task set exists
        while(invalid)
        {
            cout << "Which Task Set would you like to run?" << endl;
            cout << "Enter 0 for all Task Sets" << endl;
            cin >> chosenTaskSet;

            if(chosenTaskSet < 0 || chosenTaskSet > schedule.getNumTaskSets())
            {
                cout << "Invalid Task Set. Please try again." << endl;
            }
            else
            {
                invalid = false;
            }
        }

        invalid = true;
        // Choose a specific Algorithm or all of them
        // Select specific algorithm or set to 0 for all
        while(invalid)
        {
            cout << "Which Algorithm would you like to run?" << endl;
            cout << "[1] EDF\n"
                << "[2] LLF\n"
                << "[3] Algorithm A\n"
                << "[4] All\n";
            cin >> chosenAlgorithm;

            if(chosenAlgorithm < 1 || chosenAlgorithm > 4)
            {
                cout << "Invalid Algorithm. Please try again." << endl;
            }
            else
            {
                invalid = false;
            }
        }

        invalid = true;
        // Single run or multiple runs with average time
        // If average, how many runs
        while(invalid)
        {
            cout << "Would you like to run a single run or multiple runs with average time?" << endl;
            cout << "[1] Single Run\n"
                << "[2] Multiple Runs\n";
            cin >> selection;

            if(selection < 1 || selection > 2)
            {
                cout << "Invalid selection. Please try again." << endl;
            }
            else if (selection == 2)
            {
                cout << "How many runs would you like to run?" << endl;
                cin >> numRuns;
                invalid = false;
            }
            else
            {
                numRuns = 1;
                invalid = false;
            }
        }

        // Run Program
        if (printWrite == 1)
        {
            if (chosenTaskSet > 0)
            {
                if (chosenAlgorithm == 1)
                {
                    schedule.printScheduleEDF(chosenTaskSet-1, numRuns, &edf);
                }
                else if (chosenAlgorithm == 2)
                {
                    schedule.printScheduleLLF(chosenTaskSet-1, numRuns, &llf);
                }
                else if (chosenAlgorithm == 3)
                {
                    schedule.printScheduleAlgorithmA(chosenTaskSet-1, numRuns, &algoA);
                }
                else
                {
                    schedule.printSchedules(chosenTaskSet-1, numRuns, &edf, &llf, &algoA);
                }
            }
            else
            {
                if (chosenAlgorithm == 1)
                {
                    schedule.printScheduleEDFAll(numRuns, &edf);
                }
                else if (chosenAlgorithm == 2)
                {
                    schedule.printScheduleLLFAll(numRuns, &llf);
                }
                else if (chosenAlgorithm == 3)
                {
                    schedule.printScheduleAlgorithmAAll(numRuns, &algoA);
                }
                else
                {
                    schedule.printSchedulesAll(numRuns, &edf, &llf, &algoA);
                }
            }
        }
        else if (printWrite == 2)
        {
            schedule.writeResults(writeFileName, numRuns, &edf, &llf, &algoA);
        }

        invalid = true;

        // Ask if run again or exit
        while (invalid)
        {
            cout << "Would you like to run the program again?" << endl;
            cout << "[1] Yes\n"
                << "[2] No\n";
            cin >> selection;

            if (selection < 1 || selection > 2)
            {
                cout << "Invalid selection. Please try again." << endl;
            }
            else
            {
                invalid = false;
            }
        }
        
        if (selection == 2)
        {
            exit = true;
        }
        else
        {
            invalid = true;
            schedule.clearTaskSets();
            continue;
        }
    }
 
    return 0;
}