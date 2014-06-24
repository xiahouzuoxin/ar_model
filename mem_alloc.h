/*
 * FileName : Drivers\mem_alloc.h
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/3/2 21:14:30
 * Brief    : External SDRAM Managment

 * Copyright (C) USTB
 */
#ifndef _MEM_ALLOC_H
#define _MEM_ALLOC_H
#ifdef __cplusplus
  extern "C" {
#endif

#include "Config.h"

typedef struct os_mem {       /* MEMORY CONTROL BLOCK                               */
    void     *OSMemAddr;      /* Pointer to beginning of memory partition           */
    void     *OSMemFreeList;  /* Pointer to list of free memory blocks              */
    uint32_t OSMemBlkSize;    /* Size (in bytes) of each block of memory            */
    uint32_t OSMemNBlks;      /* Total number of blocks in this partition           */
    uint32_t OSMemNFree;      /* Number of memory blocks remaining in this partition*/
} OS_MEM;

typedef struct os_mem_data {
    void      *OSAddr;        /* Pointer to the beginning address of the memory partition     */
    void      *OSFreeList;    /* Pointer to the beginning of the free list of memory blocks   */
    uint32_t  OSBlkSize;      /* Size (in bytes) of each memory block                         */
    uint32_t  OSNBlks;        /* Total number of blocks in the partition                      */
    uint32_t  OSNFree;        /* Number of memory blocks free                                 */
    uint32_t  OSNUsed;        /* Number of memory blocks used                                 */
} OS_MEM_DATA;


#define OS_MAX_MEM_PART               100u

/* ERROR CODES */
#define OS_ERR_NONE                     0u
#define OS_ERR_MEM_INVALID_PART        90u
#define OS_ERR_MEM_INVALID_BLKS        91u
#define OS_ERR_MEM_INVALID_SIZE        92u
#define OS_ERR_MEM_NO_FREE_BLKS        93u
#define OS_ERR_MEM_FULL                94u
#define OS_ERR_MEM_INVALID_PBLK        95u
#define OS_ERR_MEM_INVALID_PMEM        96u
#define OS_ERR_MEM_INVALID_PDATA       97u
#define OS_ERR_MEM_INVALID_ADDR        98u
#define OS_ERR_MEM_NAME_TOO_LONG       99u


/* External Functions */
extern OS_MEM  *OSMemCreate (void     *addr,
                      uint32_t nblks,
                      uint32_t blksize,
                      uint8_t  *perr);

extern void  *OSMemGet (OS_MEM  *pmem,
                 uint8_t   *perr);

extern uint8_t  OSMemPut (OS_MEM  *pmem,
                 void    *pblk);

extern uint8_t  OSMemQuery (OS_MEM       *pmem,
                   OS_MEM_DATA  *p_mem_data);

extern void  OS_MemInit (void);

extern void  OS_MemClr (uint8_t  *pdest,
                 uint16_t  size);

extern void  OS_MemCopy (uint8_t  *pdest,
                  uint8_t  *psrc,
                  uint16_t  size);

/* External Variables */
extern OS_MEM  *OSMemFreeList;            /* Pointer to free list of memory partitions       */
extern OS_MEM  OSMemTbl[OS_MAX_MEM_PART]; /* Storage for memory partition manager            */

#ifdef __cplusplus
  }
#endif /* extern "C" */
#endif

