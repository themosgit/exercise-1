#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/bf.h"
#include "../include/hp_file.h"
#include "../include/record.h"

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

int HP_CreateFile(char *fileName) {
    int fd;
    void *data;
    BF_Block *block;
    BF_Block_Init(&block);
    BF_CreateFile(fileName);
    BF_OpenFile(fileName, &fd);
    BF_AllocateBlock(fd, block);

    data = BF_Block_GetData(block);
    HP_info *PtrHP_info;
//    HP_block_info *PtrHP_block_info;
    PtrHP_info = data;
//    PtrHP_block_info = data + BF_BLOCK_SIZE - sizeof(HP_info);
    PtrHP_info->max_records = (BF_BLOCK_SIZE - sizeof(HP_block_info)) / sizeof(Record);
    PtrHP_info->last_blockId = (BF_Block *) 0;
//    PtrHP_block_info->records = 0;
//    PtrHP_block_info->next_blockId = NULL;
    BF_Block_SetDirty(block);
    CALL_OR_DIE(BF_UnpinBlock(block));
    CALL_OR_DIE(BF_CloseFile(fd));
    //CALL_OR_DIE(BF_Close());
    return 0;
}

HP_info *HP_OpenFile(char *fileName, int *file_desc) {
    //BF_OpenFile(fileName,file_desc);

    BF_Block *block;
    BF_Block_Init(&block);

    CALL_OR_DIE(BF_OpenFile(fileName, file_desc));

    CALL_OR_DIE(BF_GetBlock(*file_desc, 0, block));

    return (HP_info *) BF_Block_GetData(block);
}


int HP_CloseFile(int file_desc, HP_info *hp_info) {
    BF_ErrorCode a = BF_CloseFile(file_desc);
//    if(BF_CloseFile(file_desc)==BF_OK){
//        return 0;
//    }
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

int HP_InsertEntry(int file_desc, HP_info *hp_info, Record record) {
    BF_Block *block;
    BF_Block_Init(&block);
    void *data;
    int blocks_num;
    BF_GetBlockCounter(file_desc, &blocks_num);
    int lastblockid = (int) hp_info->last_blockId;
    HP_block_info *blinfo;

    if (lastblockid == 0) {
        hp_info->last_blockId = (BF_Block *) 1;
        BF_AllocateBlock(file_desc, block);
        lastblockid = 1;
    }
    BF_GetBlock(file_desc, lastblockid, block);
    data = BF_Block_GetData(block);
    blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
    if (blinfo->records == hp_info->max_records) {
        //dhmiourgia neou block
        BF_UnpinBlock(block);
        hp_info->last_blockId = (BF_Block *) ((int) (hp_info->last_blockId) + 1);
        BF_ErrorCode asdf = BF_AllocateBlock(file_desc, block);
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        Record *rec = data;
        memcpy(&rec[0], &record, sizeof(record));
        blinfo->records = 1;
        BF_Block_SetDirty(block);
        BF_ErrorCode as = BF_UnpinBlock(block);
    } else {
        //eisagwgh eggrafhs sto yparxon block
        Record *rec = data;
        memcpy(&rec[blinfo->records], &record, sizeof(record));
        blinfo->records++;
        BF_Block_SetDirty(block);
        BF_ErrorCode as = BF_UnpinBlock(block);
    }

    return -1;
}

int HP_InsertEntrybu2(int file_desc, HP_info *hp_info, Record record) {
    BF_Block *block;
    BF_Block_Init(&block);
    void *data;
    int blocks_num;
    BF_GetBlockCounter(file_desc, &blocks_num);
    int lastblockid = (int) hp_info->last_blockId;
    HP_block_info *blinfo;

    if (lastblockid == 0) {
        hp_info->last_blockId = (BF_Block *) 1;
        BF_AllocateBlock(file_desc, block);
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        Record *rec = data;
        memcpy(&rec[0], &record, sizeof(record));
        blinfo->records = 1;
        BF_Block_SetDirty(block);
        BF_UnpinBlock(block);
    } else {
        BF_GetBlock(file_desc, lastblockid, block);
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        if (blinfo->records == hp_info->max_records) {
            //dhmiourgia neou block
            hp_info->last_blockId = (BF_Block *) ((int) (hp_info->last_blockId) + 1);
            BF_AllocateBlock(file_desc, block);
            data = BF_Block_GetData(block);
            blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
            Record *rec = data;
            memcpy(&rec[0], &record, sizeof(record));
            blinfo->records = 1;
            BF_Block_SetDirty(block);
            BF_UnpinBlock(block);
        } else {
            //eisagwgh eggrafhs sto yparxon block
            Record *rec = data;
            memcpy(&rec[blinfo->records], &record, sizeof(record));
            blinfo->records++;
            BF_Block_SetDirty(block);
            BF_UnpinBlock(block);
        }
    }

    return -1;
}

int HP_InsertEntrybu(int file_desc, HP_info *hp_info, Record record) {
    BF_Block *block = hp_info->last_blockId;
    void *data = BF_Block_GetData(block);
    HP_block_info *blockInfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
    printf("kati:%d\n", blockInfo->records);

    if (blockInfo->records < hp_info->max_records) {
        Record *rec = data;
        rec[blockInfo->records] = record;
        blockInfo->records++;
        BF_Block_SetDirty(block);
        CALL_OR_DIE(BF_UnpinBlock(block));
    } else {
        BF_Block *block2;
        CALL_OR_DIE(BF_AllocateBlock(file_desc, block2));
        void *data2 = BF_Block_GetData(block2);
        blockInfo->next_blockId = data2;
        HP_block_info *blockInfo2 = data2 + BF_BLOCK_SIZE - sizeof(HP_block_info);
        blockInfo2->records = 1;
        Record *rec = data2;
        rec[0] = record;
        BF_Block_SetDirty(block);
        BF_Block_SetDirty(block2);
        CALL_OR_DIE(BF_UnpinBlock(block));
        CALL_OR_DIE(BF_UnpinBlock(block2));
    }

    return -1;
}

int HP_GetAllEntries(int file_desc, HP_info *hp_info, int value) {
    BF_Block *block;
    BF_Block_Init(&block);
    void *data;
    int blocks_num;
    BF_GetBlockCounter(file_desc, &blocks_num);

    for (int i = 1; i < blocks_num; i++) {
        BF_GetBlock(file_desc, i, block);
        data = BF_Block_GetData(block);
        Record *rec = data;
        //printf("block %d\n",i);
        for (int j = 0; j < hp_info->max_records; j++) {
            //printf("den vrethike:");
            //printRecord(rec[j]);
            if (rec[j].id == value) {
                printf("vrethike:\n");
                printRecord(rec[j]);
            }
        }

    }

    return -1;
}

