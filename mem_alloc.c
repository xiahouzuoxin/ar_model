/*
 * FileName : mem_alloc.c
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/3/2 21:34:13
 * Brief    : 

 * Copyright (C) USTB
 */
#include "mem_alloc.h" 

OS_MEM  *OSMemFreeList;            /* Pointer to free list of memory partitions       */
OS_MEM  OSMemTbl[OS_MAX_MEM_PART]; /* Storage for memory partition manager            */

/*
 * @brief   Create a fixed-sized memory partition
 * @inputs  
 *   addr     is the starting address of the memory partition
 *   nblks    is the number of memory blocks to create from the partition.
 *   blksize  is the size (in bytes) of each block in the memory partition.
 * @outputs 
 *   perr     is a pointer to a variable containing an error message which will be set by
 *            this function to either:
 *
 *            OS_ERR_NONE              if the memory partition has been created correctly.
 *            OS_ERR_MEM_INVALID_ADDR  if you are specifying an invalid address for the memory
 *                                     storage of the partition or, the block does not align
 *                                     on a pointer boundary
 *            OS_ERR_MEM_INVALID_PART  no free partitions available
 *            OS_ERR_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
 *            OS_ERR_MEM_INVALID_SIZE  user specified an invalid block size
 *                                                   - must be greater than the size of a pointer
 *                                                   - must be able to hold an integral number of pointers*
 * @retval  
 *   != (OS_MEM *)0  is the partition was created
 *   == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
 *                   free partition is available.
 */
OS_MEM  *OSMemCreate (void     *addr,
                      uint32_t nblks,
                      uint32_t blksize,
                      uint8_t  *perr)
{
    OS_MEM    *pmem;
    uint8_t   *pblk;
    void      **plink;
    uint32_t  loops;
    uint32_t  i;

#if 0
#if OS_ARG_CHK_EN > 0u
    if (addr == (void *)0) {                          /* Must pass a valid address for the memory part.*/
        *perr = OS_ERR_MEM_INVALID_ADDR;
        return ((OS_MEM *)0);
    }
    if (((uint32_t)addr & (sizeof(void *) - 1u)) != 0u){  /* Must be pointer size aligned                */
        *perr = OS_ERR_MEM_INVALID_ADDR;
        return ((OS_MEM *)0);
    }
    if (nblks < 2u) {                                 /* Must have at least 2 blocks per partition     */
        *perr = OS_ERR_MEM_INVALID_BLKS;
        return ((OS_MEM *)0);
    }
    if (blksize < sizeof(void *)) {                   /* Must contain space for at least a pointer     */
        *perr = OS_ERR_MEM_INVALID_SIZE;
        return ((OS_MEM *)0);
    }
#endif
#endif

    pmem = OSMemFreeList;                             /* Get next free memory partition                */
    if (OSMemFreeList != (OS_MEM *)0) {               /* See if pool of free partitions was empty      */
        OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;
    }
    if (pmem == (OS_MEM *)0) {                        /* See if we have a memory partition             */
        *perr = OS_ERR_MEM_INVALID_PART;
        return ((OS_MEM *)0);
    }
    plink = (void **)addr;                            /* Create linked list of free memory blocks      */
    pblk  = (uint8_t *)addr;
    loops  = nblks - 1u;
    for (i = 0u; i < loops; i++) {
        pblk +=  blksize;                             /* Point to the FOLLOWING block                  */
       *plink = (void  *)pblk;                        /* Save pointer to NEXT block in CURRENT block   */
        plink = (void **)pblk;                        /* Position to  NEXT      block                  */
    }
    *plink              = (void *)0;                  /* Last memory block points to NULL              */
    pmem->OSMemAddr     = addr;                       /* Store start address of memory partition       */
    pmem->OSMemFreeList = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->OSMemNFree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->OSMemNBlks    = nblks;
    pmem->OSMemBlkSize  = blksize;                    /* Store block size of each memory blocks        */
    *perr               = OS_ERR_NONE;
    return (pmem);
}


/*
 * @brief   Get a memory block from a partition
 * @inputs  
 *   pmem    is a pointer to the memory partition control block
 * @outputs 
 *   perr    is a pointer to a variable containing an error message which will be set by this
 *           function to either:
 *
 *           OS_ERR_NONE             if the memory partition has been created correctly.
 *           OS_ERR_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
 *           OS_ERR_MEM_INVALID_PMEM if you passed a NULL pointer for 'pmem'
 * @retval  
 *   A pointer to a memory block if no error is detected
 *   A pointer to NULL if an error is detected 
 */
void  *OSMemGet (OS_MEM  *pmem,
                 uint8_t   *perr)
{
    void      *pblk;

#if 0
#if OS_ARG_CHK_EN > 0u
    if (pmem == (OS_MEM *)0) {                        /* Must point to a valid memory partition        */
        *perr = OS_ERR_MEM_INVALID_PMEM;
        return ((void *)0);
    }
#endif
#endif

    if (pmem->OSMemNFree > 0u) {                      /* See if there are any free memory blocks       */
        pblk                = pmem->OSMemFreeList;    /* Yes, point to next free memory block          */
        pmem->OSMemFreeList = *(void **)pblk;         /*      Adjust pointer to new free list          */
        pmem->OSMemNFree--;                           /*      One less memory block in this partition  */
        *perr = OS_ERR_NONE;                          /*      No error                                 */
        return (pblk);                                /*      Return memory block to caller            */
    }
    *perr = OS_ERR_MEM_NO_FREE_BLKS;                  /* No,  Notify caller of empty memory partition  */
    return ((void *)0);                               /*      Return NULL pointer to caller            */
}

/*
 * @brief   Free a memory block to a partition
 * @inputs  
 *   pmem    is a pointer to the memory partition control block
 *   pblk    is a pointer to the memory block being released.
 * @outputs 
 * @retval  
 *   OS_ERR_NONE              if the memory block was inserted into the partition
 *   OS_ERR_MEM_FULL          if you are returning a memory block to an already FULL memory
 *                            partition (You freed more blocks than you allocated!)
 *   OS_ERR_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
 *   OS_ERR_MEM_INVALID_PBLK  if you passed a NULL pointer for the block to release. 
 */
uint8_t  OSMemPut (OS_MEM  *pmem,
                 void    *pblk)
{
#if 0
#if OS_ARG_CHK_EN > 0u
    if (pmem == (OS_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (OS_ERR_MEM_INVALID_PMEM);
    }
    if (pblk == (void *)0) {                     /* Must release a valid block                         */
        return (OS_ERR_MEM_INVALID_PBLK);
    }
#endif
#endif

    if (pmem->OSMemNFree >= pmem->OSMemNBlks) {  /* Make sure all blocks not already returned          */
        return (OS_ERR_MEM_FULL);
    }
    *(void **)pblk      = pmem->OSMemFreeList;   /* Insert released block into free block list         */
    pmem->OSMemFreeList = pblk;
    pmem->OSMemNFree++;                          /* One more memory block in this partition            */
    return (OS_ERR_NONE);                        /* Notify caller that memory block was released       */
}


/*
 * @brief   
 *   This function is used to determine the number of free memory blocks and the number of
 *   used memory blocks from a memory partition.
 * @inputs  
 *   pmem        is a pointer to the memory partition control block
 *   p_mem_data  is a pointer to a structure that will contain information about the memory
 *               partition.
 * @outputs 
 * @retval  
 *   OS_ERR_NONE               if no errors were found.
 *   OS_ERR_MEM_INVALID_PMEM   if you passed a NULL pointer for 'pmem'
 *   OS_ERR_MEM_INVALID_PDATA  if you passed a NULL pointer to the data recipient.
 */
uint8_t  OSMemQuery (OS_MEM       *pmem,
                   OS_MEM_DATA  *p_mem_data)
{
#if 0
#if OS_ARG_CHK_EN > 0u
    if (pmem == (OS_MEM *)0) {                   /* Must point to a valid memory partition             */
        return (OS_ERR_MEM_INVALID_PMEM);
    }
    if (p_mem_data == (OS_MEM_DATA *)0) {        /* Must release a valid storage area for the data     */
        return (OS_ERR_MEM_INVALID_PDATA);
    }
#endif
#endif

    p_mem_data->OSAddr     = pmem->OSMemAddr;
    p_mem_data->OSFreeList = pmem->OSMemFreeList;
    p_mem_data->OSBlkSize  = pmem->OSMemBlkSize;
    p_mem_data->OSNBlks    = pmem->OSMemNBlks;
    p_mem_data->OSNFree    = pmem->OSMemNFree;
    p_mem_data->OSNUsed    = p_mem_data->OSNBlks - p_mem_data->OSNFree;
    return (OS_ERR_NONE);
}


/*
 * @brief   initialize the memory partition manager. must be called when system power on.
 * @inputs  None
 * @outputs None
 * @retval  None
 */
void  OS_MemInit (void)
{
#if OS_MAX_MEM_PART == 1u
    OS_MemClr((uint8_t *)&OSMemTbl[0], sizeof(OSMemTbl)); /* Clear the memory partition table          */
    OSMemFreeList               = (OS_MEM *)&OSMemTbl[0]; /* Point to beginning of free list           */
#endif

#if OS_MAX_MEM_PART >= 2u
    OS_MEM  *pmem;
    uint16_t   i;

    OS_MemClr((uint8_t *)&OSMemTbl[0], sizeof(OSMemTbl)); /* Clear the memory partition table          */
    for (i = 0u; i < (OS_MAX_MEM_PART - 1u); i++) {       /* Init. list of free memory partitions      */
        pmem                = &OSMemTbl[i];               /* Point to memory control block (MCB)       */
        pmem->OSMemFreeList = (void *)&OSMemTbl[i + 1u];  /* Chain list of free partitions             */
    }
    pmem                = &OSMemTbl[i];
    pmem->OSMemFreeList = (void *)0;                      /* Initialize last node                      */

    OSMemFreeList   = &OSMemTbl[0];                       /* Point to beginning of free list           */
#endif
}


/*
 * @brief   clear a contiguous block of RAM.
 * @inputs  
 *   pdest    is the start of the RAM to clear (i.e. write 0x00 to)
 *   size     is the number of bytes to clear.
 * @outputs 
 * @retval  
 */
void  OS_MemClr (uint8_t  *pdest,
                 uint16_t  size)
{
    while (size > 0u) {
        *pdest++ = (uint8_t)0;
        size--;
    }
}

/*
 * @brief   copy a block of memory from one location to another.
 * @inputs  
 *   pdest    is a pointer to the 'destination' memory block
 *   size     is the number of bytes to copy.
 *   psrc     is a pointer to the 'source'      memory block
 * @outputs 
 * @retval  
 */
void  OS_MemCopy (uint8_t  *pdest,
                  uint8_t  *psrc,
                  uint16_t  size)
{
    while (size > 0u) {
        *pdest++ = *psrc++;
        size--;
    }
}

/* END OF FILE */

