///////////////////////////////////////////////////////////
//  wcdCThreadSync.cpp
//  Implementation of the Class wcdCThreadSync
//  Created on:      12-七月-2013 19:30:53
//  Original author: ethan
///////////////////////////////////////////////////////////

#include "wcdThreadSync.h"


wcdCThreadSync::wcdCThreadSync(){
	InitializeCriticalSection(&m_csThread);
}



wcdCThreadSync::~wcdCThreadSync(){
	DeleteCriticalSection(&m_csThread);
}





void wcdCThreadSync::Lock(){
	EnterCriticalSection(&m_csThread);
}


void wcdCThreadSync::Unlock(){
	LeaveCriticalSection(&m_csThread);
}