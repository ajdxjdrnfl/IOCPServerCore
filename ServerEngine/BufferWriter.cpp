#include "pch.h"
#include "BufferWriter.h"


BufferWriter::BufferWriter()
{

}

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 pos)
	: _buffer(buffer), _size(size), _pos(pos)
{

}

bool BufferWriter::Write(void* src, uint32 len)
{
	// TODO : reserve more memory, if freesize is small

	if (FreeSize() < len)
		return false;

	::memcpy(&_buffer[_pos], src, len);
	_pos += len;
	return true;

}