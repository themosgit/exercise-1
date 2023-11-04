#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define CHECK_AND_RETURN(call)\
{                             \
  BF_ErrorCode code = call;   \
  if (code != BF_OK) {        \
    BF_PrintError(code);      \
    return -1;                \
  }                           \
}

/*H CHECK_AND_RETURN dhmiourgh8hke gia na kanei auto pou zhtaei h askhsh, htoi an apotyxei to call
 * na epistrefei -1, anti gia HP_ERROR pou epistrfei h CALL_BF. Opote oles oi synarthseis BF pou epistrefoun
 * error code, kalountai me CHECK_AND_RETURN anti gia CALL_BF.
 * */


int HP_CreateFile(char *fileName) {
    int fd;
    void *data;
    BF_Block *block;
    BF_Block_Init(&block);
    CHECK_AND_RETURN(BF_CreateFile(fileName));
    CHECK_AND_RETURN(BF_OpenFile(fileName, &fd));
    CHECK_AND_RETURN(BF_AllocateBlock(fd, block));
    //dhmiourgei ena block to opoio periexei ena HP_info sthn arxh kai tpt allo
    data = BF_Block_GetData(block);
    HP_info *PtrHP_info = data;
    PtrHP_info->max_records = (BF_BLOCK_SIZE - sizeof(HP_block_info)) / sizeof(Record);
    PtrHP_info->last_blockId = 0;
    BF_Block_SetDirty(block);
    CHECK_AND_RETURN(BF_UnpinBlock(block));
    CHECK_AND_RETURN(BF_CloseFile(fd));
    BF_Block_Destroy(&block);
    //an ta CHECK_AND_RETURN apotyxoun
    //epistrefetai -1
    //diaforetika 0
    return 0;
}

HP_info *HP_OpenFile(char *fileName, int *file_desc) {
    BF_Block *block;
    BF_Block_Init(&block);
    BF_OpenFile(fileName, file_desc);
    BF_GetBlock(*file_desc, 0, block);
    HP_info *hpinfo = (HP_info *) BF_Block_GetData(block);
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);
    return hpinfo;
}


int HP_CloseFile(int file_desc, HP_info *hp_info) {
    CHECK_AND_RETURN(BF_CloseFile(file_desc));
    //epistrefei -1 an kati den paei kala mesw tou CHECK_AND_RETURN
    //diaforetika 0
    return 0;
}

void recordBeautifier(Record *rec) {
    int change;
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
    recordBeautifier(&record);
    void *data;
    BF_Block *block;
    BF_Block_Init(&block);

    BF_GetBlock(file_desc,0,block);
    HP_info *zerodata = (HP_info *) BF_Block_GetData(block);
    BF_Block_SetDirty(block);
    BF_UnpinBlock(block);

    if (zerodata->last_blockId == 0) {
        zerodata->last_blockId =  1;
        BF_Block_SetDirty(block);
        BF_AllocateBlock(file_desc, block);
    }

    int lastblockid = zerodata->last_blockId;
    HP_block_info *blinfo;

    BF_GetBlock(file_desc, zerodata->last_blockId, block);
    data = BF_Block_GetData(block);
    blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
    if (blinfo->records == zerodata->max_records) {
        //dhmiourgia neou block
        CHECK_AND_RETURN(BF_UnpinBlock(block));
        zerodata->last_blockId = zerodata->last_blockId + 1;
        lastblockid=zerodata->last_blockId;
        CHECK_AND_RETURN(BF_AllocateBlock(file_desc, block));
        data = BF_Block_GetData(block);
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        Record *rec = data;
        memcpy(&rec[0], &record, sizeof(record));
        blinfo->records = 1;
        BF_Block_SetDirty(block);
        CHECK_AND_RETURN(BF_UnpinBlock(block));


    } else {
        //eisagwgh eggrafhs sto yparxon block
        Record *rec = data;
        memcpy(&rec[blinfo->records], &record, sizeof(record));
        blinfo->records++;
        BF_Block_SetDirty(block);
        CHECK_AND_RETURN(BF_UnpinBlock(block));
    }
    //an yparxei kapoio provlhma epistrefetai -1 mesw twn CHECK_AND_RETURN
    //diaforetika epistrefetai to id tou block sto opoio egine h eggrafh tou record
    BF_Block_Destroy(&block);

    return lastblockid;
}

int HP_GetAllEntries(int file_desc, HP_info *hp_info, int value) {
    BF_Block *block;
    BF_Block_Init(&block);
    void *data;
    int blocks_num;
    CHECK_AND_RETURN(BF_GetBlockCounter(file_desc, &blocks_num));
    HP_block_info *blinfo;
    for (int i = 1; i < blocks_num; i++) {
        CHECK_AND_RETURN(BF_GetBlock(file_desc, i, block));
        data = BF_Block_GetData(block);
        Record *rec = data;
        blinfo = data + BF_BLOCK_SIZE - sizeof(HP_block_info);
        for (int j = 0; j < blinfo->records; j++) {
            if (rec[j].id == value) {
                printRecord(rec[j]);
            }
        }
        CHECK_AND_RETURN(BF_UnpinBlock(block));
    }
    BF_Block_Destroy(&block);
    return blocks_num;
}



