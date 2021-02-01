#pragma once
class Recorder
{
public:
	Recorder();
	~Recorder();
	virtual unsigned long Write(const void* pBuf, unsigned long dwSize);
	virtual unsigned long Read(void* pBuf, unsigned long dwSize);
};

