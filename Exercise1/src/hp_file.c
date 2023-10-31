#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h" 

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {         \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}
#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int HP_CreateFile(char *fileName){
    // int fd;
    // void* data;
    // BF_Block *block;
    // BF_Block_Init(&block);
    // CALL_OR_DIE(BF_Init(LRU));
    // CALL_OR_DIE(BF_CreateFile(*fileName,".db"))
    // CALL_OR_DIE(BF_OpenFile(*fileName,".db", &fd));
    // CALL_OR_DIE(BF_AllocateBlock(fd, block));
    // data = BF_Block_GetData(block);
    // struct HP_Info *PtrHP_info;
    // struct HP_block_info *PtrHP_block_info;
    // *PtrHP_info = data;
    // *PtrHP_block_info = data + 504/sizeof(int);
    // PtrHP_info->max_records = 2;
    // PtrHP_info->last_blockId = &block;
    // PtrHP_block_info->records = 0;
    // PtrHP_block_info->next_blockId = NULL;
    // BF_Block_SetDirty(block);
    // CALL_OR_DIE(BF_UnpinBlock(block));
    // CALL_OR_DIE(BF_CloseFile(fd));
    // CALL_OR_DIE(BF_Close());
    // return 0;

    int fd;
    void* data;
    BF_Block *block;
    BF_Block_Init(&block);
    CALL_OR_DIE(BF_CreateFile(fileName));
    CALL_OR_DIE(BF_OpenFile(fileName, &fd));
    CALL_OR_DIE(BF_AllocateBlock(fd, block));
    data = BF_Block_GetData(block);
    HP_info *PtrHP_info;
    HP_block_info *PtrHP_block_info;
    PtrHP_info = data;
    PtrHP_block_info = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
    PtrHP_info->max_records = floor((BF_BLOCK_SIZE-sizeof(HP_block_info)-sizeof(HP_info))/sizeof(Record));
    PtrHP_info->last_blockId = block;
    PtrHP_block_info->records = 0;
    PtrHP_block_info->next_blockId = NULL;
    BF_Block_SetDirty(block);
    CALL_OR_DIE(BF_UnpinBlock(block));
    CALL_OR_DIE(BF_CloseFile(fd));
    CALL_OR_DIE(BF_Close());
    printf("test\n");
    return 0;
}

HP_info* HP_OpenFile(char *fileName, int *file_desc){
    HP_info* hpInfo;    
    return hpInfo;
}


int HP_CloseFile(int file_desc,HP_info* hp_info ){
}

int HP_InsertEntry(int file_desc,HP_info* hp_info, Record record){
    return -1;
}

int HP_GetAllEntries(int file_desc,HP_info* hp_info, int value){    
    return -1;
}

