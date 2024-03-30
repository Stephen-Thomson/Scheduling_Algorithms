#ifndef TOR_H
#define TOR_H

/************************************************************************************************
* File: TOR.h
* Author: Abbey DuBois
* Date: 3/14/2024
* Description: This file contains the TOR class which is used to store information about a TOR
*             in the system.
*************************************************************************************************/

#include <iostream>
#include "Task.h"

class TOR {
private:
	Task m_T1;
	Task m_T2;
	int m_lowerEnd;
	int m_higherEnd;

public:
	TOR();
	TOR(Task T1, Task T2, int lowerEnd, int higherEnd);
	Task getT1();
	Task getT2();
	int getLowerEnd();
	int getHigherEnd();
	void setT1(Task T1);
	void setT2(Task T2);
	void setLowerEnd(int lowerEnd);
	void setHigherEnd(int higherEnd);
};

#endif