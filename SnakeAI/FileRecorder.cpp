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
	char* buffer = (char*)pBuf;
	size_t s=fread(pBuf, 1, dwSize, m_file);
	while (s < dwSize)
	{
		s += fread(buffer + s, 1, dwSize - s, m_file);
		/*long p = ftell(m_file);
		fseek(m_file, 0, SEEK_END);
		long q = ftell(m_file);
		fseek(m_file, p, SEEK_SET);*/
	}
	return (unsigned long)s;
}
