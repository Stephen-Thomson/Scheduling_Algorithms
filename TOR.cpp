/************************************************************************************************
* File: TOR.cpp
* Author: Abbey DuBois
* Date: 3/14/2024
* Description: This file contains the implementation of the TOR class
*************************************************************************************************/

#include "TOR.h"

TOR::TOR() {
	m_lowerEnd = 0;
	m_higherEnd = 0;
}

TOR::TOR(Task T1, Task T2, int lowerEnd, int higherEnd) {
	m_T1 = T1;
	m_T2 = T2;
	m_lowerEnd = lowerEnd;
	m_higherEnd = higherEnd;
}

void TOR::setT1(Task T1) {
	m_T1 = T1;
}
void TOR::setT2(Task T2) {
	m_T2 = T2;
}
void TOR::setLowerEnd(int lowerEnd) {
	m_lowerEnd = lowerEnd;
}
void TOR::setHigherEnd(int higherEnd) {
	m_higherEnd = higherEnd;
}

Task TOR::getT1() {
	return m_T1;
}
Task TOR::getT2() {
	return m_T2;
}
int TOR::getLowerEnd() {
	return m_lowerEnd;
}
int TOR::getHigherEnd() {
	return m_higherEnd;
}