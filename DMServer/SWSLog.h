#ifndef _DMLOG_H
#define _DMLOG_H

#include <concrt.h>
#include <stdio.h>
#include <stdint.h>

const uint8_t FileNameLen = 100;

class CSingletonLog 
{  
private:  
	CSingletonLog()   //构造函数是私有的  
	{  
	}  
	CSingletonLog(const CSingletonLog &);
	CSingletonLog & operator = (const CSingletonLog &);

	FILE *fp;
	char logName[FileNameLen];
	concurrency::critical_section cs;
public:  
	static CSingletonLog & GetInstance()
	{
		static CSingletonLog instance;   //局部静态变量  
		return instance;  
	}  

	void SetLogPath(const char *path = NULL);
	void FlashLog(const char *msg, ...);
};  

#define LogIns CSingletonLog::GetInstance()


#endif //#define _DMLOG_H