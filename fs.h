#ifndef FS_H
#define FS_H

#include "stdint.h"

#define RSFS_MAGIC         0x52534653
#define BLOCK_SIZE         4096
#define MAX_FILES          64
#define MAX_FILENAME       32
#define SUPERBLOCK_BLOCK   0
#define BITMAP_BLOCK       1
#define ROOTDIR_BLOCK      2
#define FIRST_DATA_BLOCK   3

typedef struct {
    char name[MAX_FILENAME];
    uint32_t size;
    uint32_t first_block;
    uint8_t flags;
} __attribute__((packed)) dir_entry_t;

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t free_blocks;
    uint32_t root_dir_entries;
    char volume_name[32];
} __attribute__((packed)) superblock_t;

void fs_init(uint32_t device_base);
void fs_format();
int fs_create(const char *filename);
int fs_write(const char *filename, const uint8_t *data, uint32_t size);
int fs_read(const char *filename, uint8_t *buffer, uint32_t size);
void fs_list();

#endif