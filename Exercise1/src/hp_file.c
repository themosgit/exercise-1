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

int HP_InsertEntry(int file_desc, HP_info *hp_info, Record record) {
    BF_Block *block=hp_info->last_blockId;
    void* data=BF_Block_GetData(block);
    HP_block_info *blockInfo=data+BF_BLOCK_SIZE-sizeof(HP_block_info);
    //printf("kati:%p\n",hp_info->max_records);

    if(blockInfo->records<hp_info->max_records){
        Record *rec=data;
        rec[blockInfo->records]=record;
        blockInfo->records++;
        BF_Block_SetDirty(block);
        CALL_OR_DIE(BF_UnpinBlock(block));
    }else{
        BF_Block *block2;
        CALL_OR_DIE(BF_AllocateBlock(file_desc, block2));
        void* data2 = BF_Block_GetData(block2);
        blockInfo->next_blockId=data2;
        HP_block_info *blockInfo2=data2+BF_BLOCK_SIZE-sizeof(HP_block_info);
        blockInfo2->records=1;
        Record *rec=data2;
        rec[0]=record;
        BF_Block_SetDirty(block);
        BF_Block_SetDirty(block2);
        CALL_OR_DIE(BF_UnpinBlock(block));
        CALL_OR_DIE(BF_UnpinBlock(block2));
    }

    return -1;
}

int HP_GetAllEntries(int file_desc,HP_info* hp_info, int value){    
    return -1;
}

