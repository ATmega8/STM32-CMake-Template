#include "circularbuffer.h"

CircularBufferTypeDef* CircularBuffer_Create(int len, size_t size)
{
	CircularBufferTypeDef* pcbuf;
	pcbuf = (CircularBufferTypeDef*)malloc(sizeof(CircularBufferTypeDef));

	CircularBuffer_SetSize(pcbuf, size);

	CircularBuffer_SetLength(pcbuf, len);

	CircularBuffer_SetMemoryPointer(pcbuf, malloc(size*len));

	CircularBuffer_SetHeadPosition(pcbuf, 0);

	CircularBuffer_SetTailPosition(pcbuf, 0);

	CircularBuffer_SetStatus(pcbuf, CircularBuffer_Empty);

	return pcbuf;
}

static inline uint32_t CircularBuffer_Unused(CircularBufferTypeDef* pcbuf)
{
	uint32_t n;

	n =	((CircularBuffer_Length(pcbuf) - (CircularBuffer_HeadPosition(pcbuf) -
				CircularBuffer_TailPosition(pcbuf)))%CircularBuffer_Length(pcbuf));
	return n;
}

inline int CircularBuffer_Length(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->length;
}

inline void CircularBuffer_SetLength(CircularBufferTypeDef* pcbuf, int length)
{
	pcbuf->length = length;
}

inline int CircularBuffer_Size(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->size;
}

inline void CircularBuffer_SetSize(CircularBufferTypeDef* pcbuf, int size)
{
	pcbuf->size = size;
}

inline int CircularBuffer_HeadPosition(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->headPosition;
}

inline void CircularBuffer_SetHeadPosition(CircularBufferTypeDef* pcbuf, int headPosition)
{
	pcbuf->headPosition = headPosition;
}

inline int CircularBuffer_TailPosition(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->tailPosition;
}

inline void CircularBuffer_SetTailPosition(CircularBufferTypeDef* pcbuf, int tailPosition)
{
	pcbuf->tailPosition = tailPosition;
}

inline CircularBufferStatusTypeDef CircularBuffer_Status(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->status;
}

inline void CircularBuffer_SetStatus(CircularBufferTypeDef* pcbuf, CircularBufferStatusTypeDef status)
{
	pcbuf->status = status;
}

inline int* CircularBuffer_MemoryPointer(CircularBufferTypeDef* pcbuf)
{
	return pcbuf->memoryPointer;
}

inline void CircularBuffer_SetMemoryPointer(CircularBufferTypeDef* pcbuf, void* memoryPointer)
{
	pcbuf->memoryPointer = (int*)memoryPointer;
}

int CircularBuffer_Write(CircularBufferTypeDef* pcbuf, void* data, int len)
{
	uint32_t offset, n;
	int unused;

	/*检查环形缓冲区状态*/
	if(CircularBuffer_Status(pcbuf) == CircularBuffer_Full)
	{
		printf("%s %d: Circular Buffer Has Full!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/*计算实际写入长度*/
	if(CircularBuffer_Status(pcbuf) == CircularBuffer_Empty)
	{
		n = min(len, CircularBuffer_Length(pcbuf));
		unused = 10;
	}
	else
	{
		unused = CircularBuffer_Unused(pcbuf);
		n = min(len, unused);
	}

	/*计算偏移量*/
	offset =min(CircularBuffer_Length(pcbuf) - CircularBuffer_HeadPosition(pcbuf), \
			n); 

	/*写入写指针至缓冲区末尾方向*/
	memcpy(CircularBuffer_MemoryPointer(pcbuf) + \
			CircularBuffer_HeadPosition(pcbuf)*(CircularBuffer_Size(pcbuf) >> 2), \
			data, \
			offset*CircularBuffer_Size(pcbuf));


	/*写入缓冲区开头至读指针方向*/
	memcpy(CircularBuffer_MemoryPointer(pcbuf), \
			data + offset*(CircularBuffer_Size(pcbuf) >> 2),
			(n - offset)*CircularBuffer_Size(pcbuf));

	/*更新写指针及缓冲区状态*/
	if((int)(unused - n) <= 0)
	{
		CircularBuffer_SetHeadPosition(pcbuf, CircularBuffer_TailPosition(pcbuf));
		CircularBuffer_SetStatus(pcbuf, CircularBuffer_Full);
	}
	else
	{
		CircularBuffer_SetHeadPosition(pcbuf, \
				(CircularBuffer_HeadPosition(pcbuf) + n)%CircularBuffer_Length(pcbuf));
		CircularBuffer_SetStatus(pcbuf, CircularBuffer_NoEmpty);
	}

	return n;
}

int CircularBuffer_Read(CircularBufferTypeDef* pcbuf, void* data, int len)
{
	uint32_t offset, n;
	int unused;

	/*检查环形缓冲区状态*/
	if(CircularBuffer_Status(pcbuf) == CircularBuffer_Empty)
	{
		printf("%s %d: Circular Buffer Has Empty!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/*计算实际读出长度*/
	if(CircularBuffer_Status(pcbuf) == CircularBuffer_Full)
	{
		n = min(len, CircularBuffer_Length(pcbuf));
		unused = 0;
	}
	else
	{
		unused = CircularBuffer_Unused(pcbuf);
	    n = min(len, 
				(CircularBuffer_Length(pcbuf) - unused));

	}

	/*计算偏移量*/
	offset = min(n, (CircularBuffer_Length(pcbuf) - CircularBuffer_TailPosition(pcbuf)));

	/*读出读指针至缓冲区末尾方向*/
	memcpy(data, \
			CircularBuffer_MemoryPointer(pcbuf) + \
			CircularBuffer_TailPosition(pcbuf)*(CircularBuffer_Size(pcbuf) >> 2), \
			CircularBuffer_Size(pcbuf)*offset);


	/*读出缓冲区开头至写指针方向*/
	memcpy(data + offset*(CircularBuffer_Size(pcbuf) >> 2), \
			CircularBuffer_MemoryPointer(pcbuf), \
			(n - offset)*CircularBuffer_Size(pcbuf));

	/*更新读指针及缓冲区状态*/
	if((unused  + n) < CircularBuffer_Length(pcbuf))
	{
		CircularBuffer_SetTailPosition(pcbuf, \
				  (CircularBuffer_TailPosition(pcbuf) + n)%CircularBuffer_Length(pcbuf));

		CircularBuffer_SetStatus(pcbuf, CircularBuffer_NoEmpty);	
	}
	else 
	{
		CircularBuffer_SetTailPosition(pcbuf, CircularBuffer_HeadPosition(pcbuf));
		CircularBuffer_SetStatus(pcbuf, CircularBuffer_Empty);
	}

	return n;
}

