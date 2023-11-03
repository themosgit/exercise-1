#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "hp_file.h"

//#define RECORDS_NUM 1 // you can change it if you want
//#define RECORDS_NUM 10 // you can change it if you want
//#define RECORDS_NUM 100 // you can change it if you want
#define RECORDS_NUM 1000 // you can change it if you want
//#define RECORDS_NUM 10000 // you can change it if you want
//#define RECORDS_NUM 100000 // you can change it if you want
//#define RECORDS_NUM 1000000 // you can change it if you want
//#define RECORDS_NUM 10000000 // you can change it if you want
#define FILE_NAME "data.db"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }


int main() {

    remove(FILE_NAME);

    BF_Init(LRU);
    HP_CreateFile(FILE_NAME);
    int file_desc;

    HP_info *hp_info = HP_OpenFile(FILE_NAME, &file_desc);


    Record record;
    srand(12569874);
    printf("Insert Entries\n");
    for (int id = 0; id < RECORDS_NUM; ++id) {
        record = randomRecord();
        HP_InsertEntry(file_desc, hp_info, record);
    }

    printf("RUN PrintAllEntries\n");
    int id = rand() % RECORDS_NUM;

    printf("Searching for: %d\n", id);
    HP_GetAllEntries(file_desc, hp_info, id);

    HP_CloseFile(file_desc, hp_info);
    BF_Close();

}


