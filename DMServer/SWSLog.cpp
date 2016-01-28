#include <windows.h>
#include <stdint.h>
#include <time.h>
#include <sys\timeb.h>
#include "SWSLog.h"

void CSingletonLog::SetLogPath(const char *path)
{
	if (path){
		sprintf_s(logName, FileNameLen, path);
	} else {
		SYSTEMTIME tm;
		GetLocalTime(&tm);
		sprintf_s(logName, FileNameLen, "%04d-%02d-%02d.log", tm.wYear, tm.wMonth, tm.wDay);
	}
}

void CSingletonLog::FlashLog(const char *msg, ...)
{
	if (!msg)
	{
		return;
	}
	va_list args;

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char LogTime[100];
	sprintf_s(LogTime, "%02d-%02d %02d:%02d:%02d:%03d ", tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	
	cs.lock();
	fopen_s(&fp, logName, "a");
	if (!fp)
	{
		return;
	}
	fprintf_s(fp, LogTime);
	va_start(args, msg);
	vfprintf_s(fp, msg, args);
	va_end(args);
//	fprintf_s(fp, "\n");
	fflush(fp);
	fclose(fp);
	fp = NULL;
	cs.unlock();
}