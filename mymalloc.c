#include <stdio.h>
#include "mymalloc.h"

/*start the MyMallocStart method to set data about free block*/
void MyMallocStart(void)
{
    firstBlock->blockSize = memory_size - block_size;               //give size of memory space when remove the size for blockData
    firstBlock->nextBlock = NULL;                                   //set next of firstBlock to null
    firstBlock->prvBlock = NULL;                                    //set prv of firstBlock to null
    firstBlock->blockType = 1;                                      //set flag to free
    firstBlock->behindBlockSizes = 0;                               //set 0 since there is no block behind

    printf("\n **** My Malloc Starts **** \n");
}
/*end of MyMallocStart method*/

/*start of MyMalloc method*/
void *MyMalloc(int allocatingSize)
{
    if (firstBlock->blockSize==0)                                //Start MyMalloc if it calls for first time
    {
        MyMallocStart();
    }

    BlockDataPtr block = firstBlock->nextBlock;                 //BlockData type pointer block to hold current block

    if (block == NULL)                                          //this is true if MyMalloc calls for first time
    {
        BlockDataPtr newBlock = (void *)&malloc_memory[(block_size + allocatingSize)];          //create new block

        firstBlock->blockSize = allocatingSize;                         //set firstblock to allocting memory size
        firstBlock->blockType = 0;                                      //set flag to full
        firstBlock->nextBlock = newBlock;                               //point the firstBlock to new block
        firstBlock->prvBlock = NULL;
        firstBlock->behindBlockSizes = 0;

        newBlock->blockSize = memory_size - (block_size + allocatingSize + block_size);         //set newblock blocksize
        newBlock->blockType = 1;                                                                //set flag to full
        newBlock->nextBlock = NULL;                                                             //set nextBlock to null
        newBlock->prvBlock = firstBlock;                                                        //set prvBlock to firstBlock
        newBlock->behindBlockSizes = firstBlock->blockSize + block_size;                        //set size of behind blocks

        return(firstBlock+1);           //return the allocated memory starting location
    }
    else                                //this is true if function call is not first time
    {
        if(firstBlock->blockType == 1)
        {
            block = firstBlock;
        }
        while(block->blockType == 0 || block->blockSize < allocatingSize)               //find a block which is free and with size>= allocating size
        {
            if(block->nextBlock != NULL)
            {
                block = block->nextBlock;
            }
            else                //if nextBlock is null it is the end
            {
                printf("There is no memory free for block size \n");
                return 0;       //return null memory allocation
            }
        }

        if (block->nextBlock == NULL)
        {
            if (block->behindBlockSizes + block_size + allocatingSize <= 20000 - block_size - 1)            //check array size exceed or not
            {
                BlockDataPtr newBlock = (void *)&malloc_memory[block->behindBlockSizes + block_size + allocatingSize];          //create new block

                block->blockSize = allocatingSize;              //set current blocks blockSize to allocatingSize
                block->blockType = 0;                           //set flag to full
                block->nextBlock = newBlock;                    //set nextBlock to new block

                newBlock->blockSize = memory_size - (block->behindBlockSizes + block_size + block->blockSize + block_size);         //set new blocksize to remaining size
                newBlock->blockType = 1;                        //set flag to free
                newBlock->nextBlock = NULL;                     //set next block to null
                newBlock->prvBlock = block;                     //set prvBlock to firstBlock
                newBlock->behindBlockSizes = block->behindBlockSizes + block_size + block->blockSize;       //set behindBlock size

                return (&block->behindBlockSizes+1);            //return starting memory address of allocated memory
            }
            else                    //return error if array size exceed
            {
                printf("There is no enough memory to allocate \n");
                return 0;
            }
        }
        else if(block->blockSize == allocatingSize)
        {
            block->blockSize = allocatingSize;                  //allocate the total free size to allocating size
            block->blockType = 0;                               //set block to full

            return (&block->behindBlockSizes+1);
        }
        else if(block->blockSize > allocatingSize)
        {
            BlockDataPtr newBlock = (void *)&malloc_memory[block->behindBlockSizes + block_size + allocatingSize];              //create new block for remaining size in block when allocate

            newBlock->nextBlock = block->nextBlock;                     //set next block of new block to previous next block of block
            block->nextBlock = newBlock;
            newBlock->prvBlock = block;                                 //set prvBlock to firstBlock

            newBlock->blockSize = block->blockSize - allocatingSize - block_size;
            block->blockSize = allocatingSize;

            newBlock->blockType = 1;
            block->blockType = 0;

            newBlock->behindBlockSizes = block->behindBlockSizes + block_size + block->blockSize;

            return (&block->behindBlockSizes+1);
        }
        else
        {
            printf("No free memory /n");
            return 0;
        }
    }
}
/*end of MyMalloc method*/

/*start of MyContinuousNext method*/
void MyContinuousNext(BlockDataPtr currentBlock)            //to add free blocks together when a free block is present next to the current one
{
    do
    {
        currentBlock->blockSize = currentBlock->blockSize + currentBlock->nextBlock->blockSize + block_size;
        currentBlock->nextBlock = currentBlock->nextBlock->nextBlock;
        currentBlock->nextBlock->prvBlock = currentBlock;
        if (currentBlock->nextBlock == NULL)
        {
            break;
        }
    }
    while (currentBlock->nextBlock->blockType == 1);
}
/*end of MyContinuousNext method*/

/*start of MyContinuousPrv method*/
void MyContinuousPrv(BlockDataPtr currentBlock)             //to add free blocks together when there is blocks previous to current block
{
    do
    {
        currentBlock->prvBlock->blockSize = currentBlock->prvBlock->blockSize + currentBlock->blockSize + block_size;
        currentBlock->prvBlock->nextBlock = currentBlock->nextBlock;
        currentBlock->nextBlock->prvBlock = currentBlock->prvBlock;
        currentBlock = currentBlock->prvBlock;
        if (currentBlock->prvBlock == NULL)
        {
            break;
        }
    }
    while (currentBlock->prvBlock->blockType == 1);
}
/*end of MyContinuousPrv method*/

/*start of MyFree method*/
void MyFree(void *memoryPtr)
{
    if ( (memoryPtr > (void *)(&malloc_memory)) && (memoryPtr < (void *)(&malloc_memory + 1)))
    {
        *(int *)(memoryPtr-8) = 1;
        BlockDataPtr currentBlock = memoryPtr-20;

        currentBlock->blockSize = *(int *)(memoryPtr-20);
        currentBlock->nextBlock = (void *)*(int *)(memoryPtr-16);
        currentBlock->prvBlock = (void *)*(int *)(memoryPtr-12);
        currentBlock->blockType = *(int *)(memoryPtr-8);
        currentBlock->behindBlockSizes = *(int *)(memoryPtr-4);

        if (currentBlock->prvBlock != NULL && currentBlock->prvBlock->blockType == 1)
        {
            MyContinuousPrv(currentBlock);
        }
        else if (currentBlock->nextBlock != NULL && currentBlock->nextBlock->blockType == 1)
        {
            MyContinuousNext(currentBlock);
        }
    }
    else
    {
        printf("This is not a valid pointer\n");
    }
}
/*end of MyFree method*/

int main()
{
    printf("%d\n",&malloc_memory);

    int *a = MyMalloc(5000);
    printf("Allocating Address 1 for 5000 memory: %d \n\n",a);
    int *b = MyMalloc(2000);
    printf("Allocating Address 2 for 2000 memory: %d \n\n",b);
    int *c = MyMalloc(8000);
    printf("Allocating Address 3 for 8000 memory: %d \n\n",c);
    int *d = MyMalloc(4800);
    printf("Allocating Address 4 for 4800 memory: %d \n\n",d);
    int *e = MyMalloc(500);
    printf("Allocating Address 5 for 500 memory: %d \n\n",e);

    MyFree(c);
    MyFree(b);
    MyFree(d);

    int *f = MyMalloc(6000);
    printf("Allocating Address 6 for 6000 memory 2: %d \n\n",f);

    MyFree(a);

    printf("Allocating Address 6 for 6000 memory 2: %d \n\n",MyMalloc(10000));
    return 0;
}
