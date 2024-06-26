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
    return BF_ERROR;        \
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
    PtrHP_block_info = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
    PtrHP_info = data;
    PtrHP_info->max_records = floor((BF_BLOCK_SIZE-sizeof(HP_block_info))/sizeof(Record));
    PtrHP_info->last_blockId = 0;
    PtrHP_block_info->records = 0;
    BF_Block_SetDirty(block);
    CALL_OR_DIE(BF_UnpinBlock(block));
    CALL_OR_DIE(BF_CloseFile(fd));
    return 0;
}



HP_info* HP_OpenFile(char *fileName, int *file_desc){

    BF_Block *block;
    BF_Block_Init(&block);

    CALL_OR_DIE(BF_OpenFile(fileName, file_desc));

    CALL_OR_DIE(BF_GetBlock(*file_desc, 0, block));

    void *data = BF_Block_GetData(block);
    HP_info *info = data;

    BF_UnpinBlock(block);

    return info;
}



int HP_CloseFile(int file_desc,HP_info* hp_info ){
     CALL_OR_DIE(BF_CloseFile(file_desc));
     if(BF_CloseFile(file_desc)==BF_OK){
        return 0;
    }
    return -1;
}



int HP_InsertEntry(int file_desc,HP_info* hp_info, Record record){
    void *data;
    BF_Block *block;
    HP_block_info *blinfo;
    BF_Block_Init(&block);

    int lastblockid = hp_info->last_blockId;

    CALL_OR_DIE(BF_GetBlock(file_desc, lastblockid, block));
    data = BF_Block_GetData(block);
    blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info); 
     
    if (lastblockid == 0 || blinfo->records == hp_info->max_records) {

        blinfo->next_blockId = lastblockid + 1;
        BF_Block_SetDirty(block);
        (lastblockid == 0) ? : (BF_UnpinBlock(block));

        CALL_OR_DIE(BF_AllocateBlock(file_desc, block));
        hp_info->last_blockId = hp_info->last_blockId + 1;
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        blinfo->records = 0;
    }

    Record *rec = data;
    memcpy(&rec[blinfo->records], &record, sizeof(record));
    blinfo->records++;
    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);
    return -1;
}




int HP_GetAllEntries(int file_desc,HP_info* hp_info, int value){    
    BF_Block *block;
    BF_Block_Init(&block);
    HP_block_info *blinfo;
    void *data;
    int blocks_num = hp_info->last_blockId;

    for (int i = 1; i < blocks_num; i++) {
        BF_GetBlock(file_desc, i, block);
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        Record *rec = data;
        for (int j = 0; j < blinfo->records; j++) {
            if (rec[j].id == value) {
                printf("Found: ");
                printRecord(rec[j]);
            
            }
           
        }
         CALL_OR_DIE(BF_UnpinBlock(block));
    }
 return -1;
}




void recordBeautifier(Record *rec) {
    int change = 0;
    for (int i = 6; i < sizeof(rec->record); i++) {
        rec->record[i] = 0;

    }
    change = 0;
    for (int i = 0; i < sizeof(rec->name); i++) {
        if (change) {
            rec->name[i] = 0;
        } else if (!rec->name[i]) {
            change = 1;
        }
    }
    change = 0;
    for (int i = 0; i < sizeof(rec->surname); i++) {
        if (change) {
            rec->surname[i] = 0;
        } else if (!rec->surname[i]) {
            change = 1;
        }
    }
    change = 0;
    for (int i = 0; i < sizeof(rec->city); i++) {
        if (change) {
            rec->city[i] = 0;
        } else if (!rec->city[i]) {
            change = 1;
        }
    }

}

