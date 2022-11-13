
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "archiver.h"


/*internal*/
uint32_t ushortWriteFile(ARCHIVE *arch, char * file);//returns file size
void vWriteIndex(ARCHIVE *arch);



uint32_t ushortWriteFile(ARCHIVE *arch, char * file)
{
    FILE *in;
    in = fopen(file, "rb");
    uint32_t file_size;

    char buff[BLOCK_SIZE];

    while(fread(buff, 1, BLOCK_SIZE, in)){
        fwrite(buff, 1, BLOCK_SIZE, arch->f);
    }

    fseek(in, 0, SEEK_END);
    file_size = ftell(in);

    fclose(in);

    return file_size;
}

void vWriteIndex(ARCHIVE *arch)
{
    long pos_of_index = ftell(arch->f);

    fwrite(arch->index, sizeof(INDEX_ENTRY), (arch->current_index), arch->f);
    fwrite(&(pos_of_index), sizeof(pos_of_index), 1, arch->f);

}

void ARCHIVE_vInit(ARCHIVE *arch, char * archive_name)
{
    memset(arch, 0, sizeof(ARCHIVE));
    arch->f = fopen(archive_name, "ab+");
}

void ARCHIVE_vDeinit(ARCHIVE *arch)
{
    fclose(arch->f);
}

void ARCHIVE_printIndex(ARCHIVE *arch)
{
    uint8_t i = 0;
    while(strcmp(arch->index[i].file_name, "")){
        printf("%s %u %u\n", arch->index[i].file_name,( arch->index[i].file_size) / 1024,arch->index[i].file_position);
        i++;
    }
}

//write index content to file
void ARCHIVE_vCommit(ARCHIVE *arch)
{
    for(uint8_t i = 0; i < arch->current_index; i++){
        arch->index[i].file_position = arch->current_position;
        arch->index[i].file_size = ushortWriteFile(arch, arch->index[i].file_name);
        strcpy(arch->index[i].file_name, arch->index[i].file_name);
        arch->current_position += arch->index[i].file_size;
    }

    vWriteIndex(arch);
}

void ARCHIVE_vAddFile(ARCHIVE *arch, char *file)
{
    uint8_t index_index = arch->current_index;
    arch->current_index++;
    strcpy(arch->index[index_index].file_name, file);
}

/*load index and reset counters*/
void ARCHIVE_vLoadArchive(ARCHIVE *arch)
{
    long index_start = 0;
    long file_end = 0;

    fseek(arch->f, -sizeof(uint32_t), SEEK_END);
    file_end = ftell(arch->f);
    fread(&index_start, sizeof(long), 1, arch->f);

    fseek(arch->f, index_start, SEEK_SET);


    fread(arch->index, sizeof(INDEX_ENTRY), (file_end - index_start)/sizeof(INDEX_ENTRY), arch->f);

    arch->current_index = 0;
    arch->current_position = 0;

}

char *ARCHIVE_cGetFileBytes(ARCHIVE *arch, char *file_name, unsigned int *file_size)
{
    uint32_t position = 0;
    uint32_t size = 0;
    char *buff = NULL;

    for(uint8_t i = 0; strcmp(arch->index[i].file_name, ""); i++){
           if(!strcmp(arch->index[i].file_name, file_name)){
            position = arch->index[i].file_position;
            size = arch->index[i].file_size;

            fseek(arch->f, position, SEEK_SET);

            buff = malloc(size);
            fread(buff, size, sizeof(char), arch->f);
            break;
           }
    }

    if(file_size) *file_size = size;

    return buff;
}
