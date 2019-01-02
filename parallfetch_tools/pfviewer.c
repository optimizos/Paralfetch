#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "flashfetch_fetch.h"

int main(int argc, char **argv)
{
	int fd;
	int count;
	int ret;
	paralfetch_header header;
	replay_info repinfo;
	int meta_count;
	int meta_size;
	int data_count;
	int data_size;
	

	fd = open(argv[1], O_RDONLY);

	if(fd < 0) return fd;

	count = 0;
	meta_count = 0;
	meta_size = 0;
	data_count = 0;
	data_size = 0;

	ret = read(fd, (char *)(&header), sizeof(paralfetch_header));
	if(ret < 0) return ret;

	printf("-- header --\n");
	printf("magic code : %c%c%c%c%c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3], header.magic[4], header.magic[5], header.magic[6], header.magic[7]);
	printf("version : %u\n", header.version);
	printf("total io count : %u\n", header.io_count);
	printf("uncertain io count : %u\n", header.uncertain_io_count);
	printf("uncertain io size : %u\n", header.uncertain_io_size);
	printf("harmful io count : %u\n", header.harmful_io_count);
	printf("harmful io size : %u\n", header.harmful_io_size);

	printf("dev\t\tinode\t\tstart\t\tlength\n");
	for(;;)
	{
		ret = read(fd, (char *)(&repinfo), sizeof(replay_info));
		if(ret) {
			printf("0x%-14x%-16llu%-16llu%u\n", repinfo.dev, repinfo.ino, repinfo.start, repinfo.length);
			count++;
			if(repinfo.ino == 0) {
				meta_count++;
				meta_size += (repinfo.length / 1024);
			} else {
				data_count++;
				data_size += (repinfo.length * 4);
			}
		} else break;
	}
	printf("total trace count : %d\n", count);
	printf("total meta count : %d\n", meta_count);
	printf("total meta size : %d\n", meta_size);
	printf("total data count : %d\n", data_count);
	printf("total data size : %d\n", data_size);
	close(fd);
} 
