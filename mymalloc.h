#define memory_size 20000
#define block_size 20

char malloc_memory[memory_size];            //array for memory

/*self referential structure blockData to keep information about the block*/
struct blockData{
    int blockSize;                          //size of the block
    struct blockData *nextBlock;            //pointer to next block of type blockdata
    struct blockData *prvBlock;             //pointer to previous block of type blockdata
    int blockType;                          //a flag to keep track of block is free or not => if 1 it is free, else 0
    int behindBlockSizes;                   //keep track of memory size of each block in behind
};
/*end of structure*/

/*synonym for struct blockData*/
typedef struct blockData BlockData;

/*synonym for BlockData */
typedef BlockData *BlockDataPtr;

/* Method Signatures */
void MyMallocStart(void);
void *MyMalloc(int allocatingSize);
void MyFree(void *memoryPtr);
void MyContinuousNext(BlockDataPtr currentBlock);
void MyContinuousPrv(BlockDataPtr currentBlock);

BlockDataPtr firstBlock = (void *)malloc_memory;
