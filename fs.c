#include "fs.h"
#include "string.h"
#include "kheap.h"

static superblock_t superblock;
static dir_entry_t root_dir[MAX_FILES];
static uint8_t block_bitmap[BLOCK_SIZE];

static uint8_t *fake_disk = (uint8_t*)0x200000; // 2 MB offset for disk image

static void read_block(uint32_t block_num, uint8_t *buffer) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 512; j++) {
            buffer[i*512 + j] = fake_disk[block_num * 4096 + i*512 + j];
        }
    }
}

static void write_block(uint32_t block_num, uint8_t *buffer) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 512; j++) {
            fake_disk[block_num * 4096 + i*512 + j] = buffer[i*512 + j];
        }
    }
}

void fs_init(uint32_t device_base) {
    (void)device_base;
    read_block(SUPERBLOCK_BLOCK, (uint8_t*)&superblock);
    if (superblock.magic != RSFS_MAGIC) {
        fs_format();
    } else {
        read_block(ROOTDIR_BLOCK, (uint8_t*)root_dir);
        read_block(BITMAP_BLOCK, block_bitmap);
    }
}

void fs_format() {
    superblock.magic = RSFS_MAGIC;
    superblock.total_blocks = 65536;
    superblock.free_blocks = superblock.total_blocks - FIRST_DATA_BLOCK;
    superblock.root_dir_entries = 0;
    strcpy(superblock.volume_name, "RSFS Volume");
    for (int i = 0; i < MAX_FILES; i++) {
        root_dir[i].name[0] = '\0';
        root_dir[i].size = 0;
        root_dir[i].first_block = 0;
        root_dir[i].flags = 0;
    }
    for (int i = 0; i < BLOCK_SIZE * 8; i++) {
        if (i < FIRST_DATA_BLOCK) {
            block_bitmap[i / 8] |= (1 << (i % 8));
        } else {
            block_bitmap[i / 8] &= ~(1 << (i % 8));
        }
    }
    write_block(SUPERBLOCK_BLOCK, (uint8_t*)&superblock);
    write_block(ROOTDIR_BLOCK, (uint8_t*)root_dir);
    write_block(BITMAP_BLOCK, block_bitmap);
}

int fs_create(const char *filename) {
    int slot = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_dir[i].name[0] == '\0') { slot = i; break; }
    }
    if (slot == -1) return -1;
    int block = -1;
    for (int i = FIRST_DATA_BLOCK; i < superblock.total_blocks; i++) {
        if (!(block_bitmap[i / 8] & (1 << (i % 8)))) {
            block = i; break;
        }
    }
    if (block == -1) return -1;
    block_bitmap[block / 8] |= (1 << (block % 8));
    superblock.free_blocks--;
    strcpy(root_dir[slot].name, filename);
    root_dir[slot].size = 0;
    root_dir[slot].first_block = block;
    root_dir[slot].flags = 0;
    superblock.root_dir_entries++;
    write_block(ROOTDIR_BLOCK, (uint8_t*)root_dir);
    write_block(BITMAP_BLOCK, block_bitmap);
    write_block(SUPERBLOCK_BLOCK, (uint8_t*)&superblock);
    return 0;
}

int fs_write(const char *filename, const uint8_t *data, uint32_t size) {
    dir_entry_t *file = NULL;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(root_dir[i].name, filename) == 0) {
            file = &root_dir[i]; break;
        }
    }
    if (!file) return -1;
    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint32_t current_block = file->first_block;
    uint32_t offset = 0;
    for (uint32_t b = 0; b < blocks_needed; b++) {
        uint8_t block_buffer[BLOCK_SIZE] = {0};
        uint32_t copy_size = (size - offset > BLOCK_SIZE) ? BLOCK_SIZE : (size - offset);
        memcpy(block_buffer, data + offset, copy_size);
        write_block(current_block, block_buffer);
        offset += copy_size;
        if (b < blocks_needed - 1) {
            int next_block = -1;
            for (int i = FIRST_DATA_BLOCK; i < superblock.total_blocks; i++) {
                if (!(block_bitmap[i / 8] & (1 << (i % 8))) && i != current_block) {
                    next_block = i; break;
                }
            }
            if (next_block == -1) return offset;
            block_bitmap[next_block / 8] |= (1 << (next_block % 8));
            superblock.free_blocks--;
            current_block = next_block;
        }
    }
    file->size = size;
    write_block(ROOTDIR_BLOCK, (uint8_t*)root_dir);
    write_block(BITMAP_BLOCK, block_bitmap);
    write_block(SUPERBLOCK_BLOCK, (uint8_t*)&superblock);
    return size;
}

int fs_read(const char *filename, uint8_t *buffer, uint32_t size) {
    dir_entry_t *file = NULL;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(root_dir[i].name, filename) == 0) {
            file = &root_dir[i]; break;
        }
    }
    if (!file) return -1;
    uint32_t to_read = (size < file->size) ? size : file->size;
    uint32_t blocks_needed = (to_read + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint32_t current_block = file->first_block;
    uint32_t offset = 0;
    for (uint32_t b = 0; b < blocks_needed; b++) {
        uint8_t block_buffer[BLOCK_SIZE];
        read_block(current_block, block_buffer);
        uint32_t copy_size = (to_read - offset > BLOCK_SIZE) ? BLOCK_SIZE : (to_read - offset);
        memcpy(buffer + offset, block_buffer, copy_size);
        offset += copy_size;
        current_block++; // упрощение: блоки идут подряд
    }
    return to_read;
}

void fs_list() {
    // stub
}