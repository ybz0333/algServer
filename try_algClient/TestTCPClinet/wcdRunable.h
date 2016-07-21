///////////////////////////////////////////////////////////
//  wcdCRunable.h
//  Implementation of the Class wcdCRunable
//  Created on:      12-七月-2013 19:31:36
//  Original author: ethan
///////////////////////////////////////////////////////////

#if !defined(EA_AB3B2D24_25C5_4fae_AF98_88ABABBFDCE7__INCLUDED_)
#define EA_AB3B2D24_25C5_4fae_AF98_88ABABBFDCE7__INCLUDED_

/**
 * Thread 类的调用类，通过该类，可以实现线程的调用
 */
class wcdCRunable
{
public:
	virtual void Run() = 0;

};
#endif // !defined(EA_AB3B2D24_25C5_4fae_AF98_88ABABBFDCE7__INCLUDED_)
