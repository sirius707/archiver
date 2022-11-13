#ifndef ARCHIVER_H_INCLUDED
#define ARCHIVER_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define BLOCK_SIZE 200
#define FILE_NAME_SIZE 50
#define FILE_LIST_SIZE 50

typedef struct ARCHIVE ARCHIVE;
typedef struct INDEX_ENTRY INDEX_ENTRY;

struct INDEX_ENTRY{
    uint32_t file_size;
    uint32_t file_position;//position at archive
    char file_name[FILE_NAME_SIZE];
};

struct ARCHIVE{
    FILE *f;

    uint32_t current_position;

    uint8_t current_index;
    //char **file_index;//array of strings
    INDEX_ENTRY index[FILE_LIST_SIZE];
};

/*interface*/
void ARCHIVE_vInit(ARCHIVE *arch, char * archive_name);
void ARCHIVE_vDeinit(ARCHIVE *arch);
void ARCHIVE_vCommit(ARCHIVE *arch);
void ARCHIVE_vAddFile(ARCHIVE *arch, char *file);
void ARCHIVE_printIndex(ARCHIVE *arch);
void ARCHIVE_vLoadArchive(ARCHIVE *arch);

/*file_size to retrive size of file opened, if not going to use NULL must be assigned*/
char *ARCHIVE_cGetFileBytes(ARCHIVE *arch, char *file_name, unsigned int *file_size);


#endif // ARCHIVER_H_INCLUDED
