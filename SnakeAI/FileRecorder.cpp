#include "FileRecorder.h"



FileRecorder::FileRecorder(FILE* fp)
	:m_file(fp)
{
}


FileRecorder::~FileRecorder()
{
}


unsigned long FileRecorder::Write(const void* pBuf, unsigned long dwSize)
{
	return (unsigned long)fwrite(pBuf, 1, dwSize, m_file);
}


unsigned long FileRecorder::Read(void* pBuf, unsigned long dwSize)
{
	return (unsigned long)fread(pBuf, 1, dwSize, m_file);
}
