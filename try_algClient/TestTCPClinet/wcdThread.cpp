///////////////////////////////////////////////////////////
//  wcdCThread.cpp
//  Implementation of the Class wcdCThread
//  Created on:      12-七月-2013 19:31:49
//  Original author: ethan
///////////////////////////////////////////////////////////

#include "wcdThread.h"
#include <process.h>
#include <fstream>
using namespace std;

wcdCThread::wcdCThread():m_hThread(NULL),m_pTarget(NULL),m_nThreadID(0),m_bExitedThread(FALSE){

}

wcdCThread::wcdCThread(wcdCRunable *pTarget):\
m_hThread(NULL),m_pTarget(pTarget),\
m_nThreadID(0),m_bExitedThread(FALSE){
}

wcdCThread::~wcdCThread(){
	if(m_hThread != NULL){
		TerminateThread(m_hThread,101);
		WaitForSingleObject(m_hThread,INFINITE);
		CloseHandle(m_hThread);
	}
}

void wcdCThread::Run(){
	if(m_pTarget != NULL)
		m_pTarget->Run();
}


/**
 * 获取执行方法的指针
 */
unsigned __stdcall wcdCThread::Run0(void* param){
	wcdCThread *pThis = static_cast<wcdCThread *>(param);
	pThis->Run1();
	return 0;
}

void wcdCThread::Run1(){
	if(m_bRunOneTime)
		this->Run();
	else{
		while(!m_bExitedThread){
			this->Run();
			Sleep(10);
		}
	}
}

BOOL wcdCThread::Start(BOOL bRunOneTime){
	m_bRunOneTime = bRunOneTime;
	m_hThread = (HANDLE)_beginthreadex(NULL,0,Run0,this,0,&m_nThreadID);
	if(m_hThread != NULL){
		return TRUE;
	}

	m_hThread = NULL;
	m_nThreadID = 0;
	return FALSE;
}

/**
 * 等待线程退出。0表示一直等待。其他表示超时时间
 */
BOOL wcdCThread::Stop(unsigned long ulMillisTime){
	fstream log;
	log.open("log.txt"); 
	log.seekg(ios::end);  

	if(m_hThread == NULL)
		return TRUE;

	m_bExitedThread = TRUE;
	if(ulMillisTime == 0){
		WaitForSingleObject(m_hThread,INFINITE);
		log<<"不延迟close m_hThread"<<endl;
		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_nThreadID = 0;
		log.close();
		return TRUE;
	}
	
	DWORD waitFlag = WaitForSingleObject(m_hThread,ulMillisTime);
	if(WAIT_TIMEOUT == waitFlag || WAIT_FAILED == waitFlag)
		return FALSE;
	else{
		log<<"延迟close m_hThread"<<endl;
		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_nThreadID = 0;
		log.close();
		return TRUE;
	}

}

BOOL wcdCThread::isEqualThread(wcdCThread *pThread){
	if(NULL == pThread)
		return FALSE;

	if(pThread->m_nThreadID == GetCurrentThreadId())
		return TRUE;
	else
		return FALSE;
}