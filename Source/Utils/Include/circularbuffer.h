#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define max(a,b) \
	({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
	_a > _b ? _a : _b; })

#define min(a,b) \
	({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
	_a < _b ? _a : _b; })

typedef enum
{
	CircularBuffer_Empty,
	CircularBuffer_NoEmpty,
	CircularBuffer_Full
} CircularBufferStatusTypeDef;

typedef struct
{
	size_t   size;
	uint32_t length;
	void*    memoryPointer;
	uint32_t headPosition;
	uint32_t tailPosition;
	CircularBufferStatusTypeDef status;
} CircularBufferTypeDef;

CircularBufferTypeDef* CircularBuffer_Create(int len, size_t size);

int  CircularBuffer_Write(CircularBufferTypeDef* pcbuf, void* data, int len);
int  CircularBuffer_Read(CircularBufferTypeDef* pcbuf, void* data, int len);

int  CircularBuffer_Size(CircularBufferTypeDef* pcbuf);
int  CircularBuffer_HeadPosition(CircularBufferTypeDef* pcbuf);
int  CircularBuffer_TailPosition(CircularBufferTypeDef* pcbuf);
int* CircularBuffer_MemoryPointer(CircularBufferTypeDef* pcbuf);
int  CircularBuffer_Length(CircularBufferTypeDef* pcbuf);
CircularBufferStatusTypeDef CircularBuffer_Status(CircularBufferTypeDef* pcbuf);

void CircularBuffer_SetSize(CircularBufferTypeDef* pcbuf, int size);
void CircularBuffer_SetHeadPosition(CircularBufferTypeDef* pcbuf, int headPosition);
void CircularBuffer_SetTailPosition(CircularBufferTypeDef* pcbuf, int tailPosition);
void CircularBuffer_SetStatus(CircularBufferTypeDef* pcbuf, CircularBufferStatusTypeDef status);
void CircularBuffer_SetMemoryPointer(CircularBufferTypeDef* pcbuf, void* memoryPointer);
void CircularBuffer_SetLength(CircularBufferTypeDef* pcbuf, int length);

uint32_t CircularBuffer_Unused(CircularBufferTypeDef* pcbuf);

