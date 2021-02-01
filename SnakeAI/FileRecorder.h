#pragma once
#include <stdio.h>

#include "Recorder.h"

class FileRecorder :
	public Recorder
{
private:
	FILE* m_file;
public:
	FileRecorder(FILE *fp);
	~FileRecorder();
	virtual unsigned long Write(const void* pBuf, unsigned long dwSize);
	virtual unsigned long Read(void* pBuf, unsigned long dwSize);
};

