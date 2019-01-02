//#include <linux/types.h>

/*
 * METADATA (ino == 0)
 * dev = device number
 * ino = 0
 * start = logical block number (block offset in the partition)
 * length = the number of bytes to read
 *
 * DATA (ino != 0)
 * dev = device number
 * ino = inode number
 * start = file offset in page size unit (PAGE_CACHE_SIZE = PAGE_SIZE)
 * length = the number of pages to read
 */

#ifndef FLASHFETCH_FETCH_H
#define FLASHFETCH_FETCH_H

#define MAGIC_LENGTH	8
#define NAME_LEN		255

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned int udev_t;
//typedef unsigned long long uino_t;
typedef unsigned int uino_t;

typedef struct __replay_info {
	udev_t dev;
	uino_t ino;
	u64 start;
	u32 length;
} __attribute__((packed)) replay_info;

typedef struct __paralfetch_header {
	char magic[MAGIC_LENGTH];
	u32 version;
	u16 io_count;
	u16 private;
	u16 uncertain_io_count;
	u16 uncertain_io_size;
	u16 harmful_io_count;
	u16 harmful_io_size;
	replay_info trace_data[0];
} paralfetch_header;

#endif
