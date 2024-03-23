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

string getFileName();

int main(int argc, char* argv[])
{
    Schedule schedule = Schedule();
    AlgorithmA algoA = AlgorithmA();
    EDF_Algorithm edf = EDF_Algorithm();
    ScheduleLLF llf = ScheduleLLF();
    int selection;
    string fileName;
    bool invalid = false;

    //Get selection on what file to read in
    do {
        invalid = false;
        cout << "What File would you like to read in for the Task Sets?" << endl;
        cout << "[1] Small task set file\n"
            << "[2] Task sets from class\n"
            << "[3] Large Task Set File\n"
            << "[4] Personal (needs to be added to Resource files)" << endl;

        cin >> selection;

        //If not valid input
        if (!cin || selection > 4 || selection <= 0) {
            cout << "Invalid Input Please try again" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            invalid = true;
        }

    } while (invalid);

    //Read in task sets from file
    switch (selection) {
        case 1:
            schedule.readTaskSets("TestTaskSet.txt");
            break;
        case 2:
            schedule.readTaskSets("TaskSets.txt");
            break;
        case 3:
            schedule.readTaskSets("RTaskSets.txt");
            break;
        case 4:
            fileName = getFileName();
            schedule.readTaskSets(fileName);
            break;
    }

    //Get selection on Algorithm To Use
    do {
        invalid = false;
        cout << "What Algorithm would you like to See for these task Sets?" << endl;
        cout << "[1] EDF\n"
            << "[2] LLF\n"
            << "[3] AlgorithmA\n"
            << "[4] All" << endl;

        cin >> selection;

        //If not valid input
        if (!cin || selection > 4 || selection <= 0) {
            cout << "Invalid Input Please try again" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            invalid = true;
        }

    } while (invalid);

    //Run the selected Algorithm for the taskSets
    switch (selection) {
    case 1:
        schedule.printScheduleEDFAll(&edf);
        break;
    case 2:
        schedule.printScheduleLLFAll(&llf);
        break;
    case 3:
        schedule.printScheduleAlgorithmAAll(&algoA);
        break;
    case 4:
        schedule.printSchedulesAll(&edf, &llf, &algoA);
        break;
    }

    return 0;
}

string getFileName() {

    string fileName;
    bool invalid;
    do {
        invalid = false;
        cout << "\nWhat is the name of your file? (Please include the .txt)" << endl;
        cin >> fileName;

        ifstream file(fileName);
        if (!file)
        {
            cout << "Error: Unable to open file " << fileName << "Please try again" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            invalid = true;
        }
        file.close();

    } while (invalid);

    return fileName;
}