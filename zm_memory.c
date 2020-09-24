#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "zm_memory.h"


typedef struct blockInfo_t
{
	unsigned int IsUsed  : 1;
	unsigned int IsFirst : 1;
	unsigned int useBlockNum : 14;
	void *address;
}blockInfo_t;


typedef struct memPoolInfo_t
{
	unsigned int blockNum;
	unsigned int blockSurplus;
	unsigned int blockUseNum;
	blockInfo_t block[BLOCK_NUMBER];
}memPoolInfo_t;



static unsigned char *memoryPool;
static memPoolInfo_t memoryPoolInfo;



/*****************************************************************
* DESCRIPTION: zm_memoryInit
*     
* INPUTS:
*     null
* OUTPUTS:
*     null
* NOTE:
*     null
*****************************************************************/
int zm_memoryInit(void)
{
	printf("memory initialize\r\n");
	memoryPool = (unsigned char *)malloc(MEMORY_POLL_SIZE);
	if(memoryPool == NULL)
	{
		printf("memory pool request faild!\r\n");
		return 0;
	}
	memoryPoolInfo.blockNum = BLOCK_NUMBER;
	memoryPoolInfo.blockSurplus = BLOCK_NUMBER;
	memoryPoolInfo.blockUseNum = 0;
	memset(memoryPool, 0, MEMORY_POLL_SIZE);
	for(unsigned int cnt = 0; cnt < BLOCK_NUMBER; cnt++)
	{
		memoryPoolInfo.block[cnt].useBlockNum = 0;
		memoryPoolInfo.block[cnt].IsFirst = false;
		memoryPoolInfo.block[cnt].IsUsed = false;
		memoryPoolInfo.block[cnt].address = (void *)&memoryPool[cnt*BLOCK_SIZE];
		//printf("#");
	}
	printf("     OK!\r\n");
	return 1;
}

/*****************************************************************
* DESCRIPTION: zm_malloc
*     
* INPUTS:
*     null
* OUTPUTS:
*     null
* NOTE:
*     null
*****************************************************************/
void *zm_malloc(size_t size)
{
	unsigned int blockNum = (size%BLOCK_SIZE)?(size/BLOCK_SIZE + 1):(size/BLOCK_SIZE);
	unsigned int findBlockNum = 0;
	unsigned int firstBlock;
	unsigned int cnt = 0;
	
	if(memoryPool == NULL || size == 0 || blockNum > memoryPoolInfo.blockSurplus)
	{
		return NULL;
	}
	//printf("malloc size : %d,use block : %d\r\n", size, blockNum);
	for(cnt = 0; cnt < BLOCK_NUMBER; cnt++)
	{
		if(memoryPoolInfo.block[cnt].IsUsed == false)
		{
			if(findBlockNum == 0)
			{
				firstBlock = cnt;
			}
			if(++findBlockNum >= blockNum)
			{
				break;
			}
		}
		else
		{
			findBlockNum = 0;
		}
	}
	if(findBlockNum >= blockNum)
	{
		memoryPoolInfo.block[firstBlock].IsUsed = true;
		memoryPoolInfo.block[firstBlock].IsFirst = true;
		memoryPoolInfo.block[firstBlock].useBlockNum = blockNum;
		//memset(memoryPoolInfo.block[firstBlock].address, 0, BLOCK_SIZE*blockNum);
		memset(memoryPoolInfo.block[firstBlock].address, 0, BLOCK_SIZE);
		for(cnt = 1; cnt < blockNum; cnt++)
		{
			memoryPoolInfo.block[firstBlock+cnt].IsUsed = true;
			memoryPoolInfo.block[firstBlock+cnt].IsFirst = false;
			memoryPoolInfo.block[firstBlock+cnt].useBlockNum = 0;
			memset(memoryPoolInfo.block[firstBlock+cnt].address, 0, BLOCK_SIZE);
		}
		memoryPoolInfo.blockUseNum += blockNum;
		memoryPoolInfo.blockSurplus -= blockNum;
		//printf("malloc success!\r\n");
		//printf("surplus block : %d--%0.1f%%\r\n", memoryPoolInfo.blockSurplus, (float)memoryPoolInfo.blockSurplus/(float)memoryPoolInfo.blockNum*100);
		return memoryPoolInfo.block[firstBlock].address;
	}
	else
	{
		return NULL;
	}
}
/*****************************************************************
* DESCRIPTION: zm_free
*     
* INPUTS:
*     null
* OUTPUTS:
*     null
* NOTE:
*     null
*****************************************************************/
void zm_free(void *_Block)
{
	if(memoryPool && _Block)
	{
		for(unsigned int cnt = 0; cnt < BLOCK_NUMBER; cnt++)
		{
			if(memoryPoolInfo.block[cnt].address == _Block && memoryPoolInfo.block[cnt].IsFirst == true)
			{
				unsigned int blockNum = memoryPoolInfo.block[cnt].useBlockNum;
				memoryPoolInfo.blockUseNum -= memoryPoolInfo.block[cnt].useBlockNum;
				memoryPoolInfo.blockSurplus += memoryPoolInfo.block[cnt].useBlockNum;
				//printf("free block : %d\r\n", memoryPoolInfo.block[cnt].useBlockNum);
				//printf("used block : %d\r\n", memoryPoolInfo.blockUseNum);
				//printf("surplus block : %d--%0.1f%%\r\n", memoryPoolInfo.blockSurplus, (float)memoryPoolInfo.blockSurplus/(float)memoryPoolInfo.blockNum*100);
				for(unsigned int i = 0; i < blockNum; i++)
				{
					memoryPoolInfo.block[cnt+i].IsFirst = false;
					memoryPoolInfo.block[cnt+i].IsUsed = false;
					memoryPoolInfo.block[cnt+i].useBlockNum = 0;
				}
				break;
			}
		}
	}
}

/*****************************************************************
* DESCRIPTION: zm_clearMemPool
*     
* INPUTS:
*     null
* OUTPUTS:
*     null
* NOTE:
*     null
*****************************************************************/
void zm_clearMemPool(void)
{
	for(unsigned int cnt = 0; cnt < BLOCK_NUMBER; cnt++)
	{
		if(memoryPoolInfo.block[cnt].IsUsed == false)
		{
			memoryPoolInfo.block[cnt].IsFirst = false;
			memoryPoolInfo.block[cnt].useBlockNum = 0;
			memset(memoryPoolInfo.block[cnt].address, 0, BLOCK_SIZE);
		}
	}
}


