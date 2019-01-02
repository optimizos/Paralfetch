#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define EI_PAD 8

#define MAX_FILE_NAME 255

void help(char *filename)
{
	printf("-- flashfetch usage --\n\n");
	printf("$%s <type> <file path>\n\n", filename);
	printf("-- type --\n");
	printf("-e\tenable default flashfetch for selected binary\n");
	printf("-d\tdisable flashfetch for selected binary\n");
	printf("-r\trequest for generating prefetch information file\n");
	printf("-x\tdisable flashfetch for selected binary and also delete correspondent prefetch information file (.pf)\n");
	printf("-f\trequest for refining prefetch information file\n");
	printf("-s\tset to synchronous prefetch mode for selected binary\n");
	printf("-a\tset to asynchronous prefetch mode for selected binary\n");
}

int extract_filename(char *filepath, char *filename)
{
	int i, name_pos;

	if(filepath == NULL || filename == NULL) return -EINVAL;

	memset(filename, '\0', sizeof(filename));

	for(i=0, name_pos=0; i<strlen(filepath); i++)
	{
		if(filepath[i] == '/') name_pos = i+1;
	}

	strcpy(filename, &(filepath[name_pos]));
	return 0;
}

#define ELF_BINARY			0
#define DESKTOP_ICON1		1

int ftcheck(char *optarg, char *exec_path)
{
	FILE *fp;
	int ret;
	char line_buffer[256];

	fp = fopen(optarg, "r");

	if(fp == NULL) return -1;

	ret = fread(line_buffer, 4, 1, fp);
	if(ret != 1) return -1;

	if(line_buffer[0] == 0x7f && line_buffer[1] == 0x45 && line_buffer[2] == 0x4c && line_buffer[3] == 0x46) {
		fclose(fp);
		return ELF_BINARY;
	}

	rewind(fp);

	fgets(line_buffer, sizeof(line_buffer), fp);

	if(strcmp(line_buffer, "#!/usr/bin/env xdg-open\n")==0 || strcmp(line_buffer, "[Desktop Entry]\n")==0) {
		while(fgets(line_buffer, sizeof(line_buffer), fp))
		{
			if(strncmp(line_buffer, "Exec=", 5)==0) {
//				fprintf(stderr, "%s", line_buffer);
				sscanf(line_buffer, "Exec=%s", exec_path);
#ifdef DEBUG
				fprintf(stderr, "parsed Exec=%s\n", exec_path);
#endif
				fclose(fp);
				return DESKTOP_ICON1;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int fd;
	int opt;
	char val;
	extern char *optarg;
	extern int optind;
	int ret;
	char filename[MAX_FILE_NAME+1];
	char pf_path[MAX_FILE_NAME+1];
	char exec_path[MAX_FILE_NAME+1];
	
	if(argc < 3 || (argc % 2) != 1) {
		help(argv[0]);
		return -EINVAL;
	}

	while((opt = getopt(argc, argv, "e:d:r:x:f:s:a:")) != -1) {
		switch(opt) {
			case 'e':
				ret = ftcheck(optarg, exec_path);
				if(ret == ELF_BINARY) fd = open(optarg, O_RDWR);
				else if(ret == DESKTOP_ICON1) fd = open(exec_path, O_RDWR);

				if(fd < 0) perror("open()");
				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}
				ret = read(fd, &val, 1);
				if(ret != 1) {
					perror("read()");
					return ret;
				}
				ret = extract_filename(optarg, filename);
				if(ret < 0) {
					fprintf(stderr, "can't extract filename from filepath!\n");
					return -EINVAL;
				}
				memset(pf_path, '\0', sizeof(pf_path));
				sprintf(pf_path, "/flashfetch/%s.pf", filename);

				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}

				if(access(pf_path, F_OK) == 0) {
					val = 1;
				} else {
					val = 1;
				}

				ret = write(fd, &val, 1);
				if(ret != 1) {
					perror("write()");
					return ret;
				}

				break;
			case 'd':
				ret = ftcheck(optarg, exec_path);
				if(ret == ELF_BINARY) fd = open(optarg, O_RDWR);
				else if(ret == DESKTOP_ICON1) fd = open(exec_path, O_RDWR);

				if(fd < 0) perror("open()");
				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}

				val = 0;

				ret = write(fd, &val, 1);
				if(ret != 1) {
					perror("write()");
					return ret;
				}

				break;
			case 'r':
				ret = ftcheck(optarg, exec_path);

				if(ret == ELF_BINARY) {
					fd = open(optarg, O_RDWR);
#ifdef DEBUG
					fprintf(stderr, "ELF_BINARY\n");
#endif
				} else if(ret == DESKTOP_ICON1) {
					fd = open(exec_path, O_RDWR);
#ifdef DEBUG
					fprintf(stderr, "DESKTOP_ICON1 (exec path:%s)\n", exec_path);
#endif
				}
				
				if(fd < 0) perror("open()");
				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}

				val = 3;

				ret = write(fd, &val, 1);
				if(ret != 1) {
					perror("write()");
					return ret;
				}

				ret = extract_filename(optarg, filename);
				if(ret < 0) {
					fprintf(stderr, "can't extract filename from filepath!\n");
					return -EINVAL;
				}
				memset(pf_path, '\0', sizeof(pf_path));
				sprintf(pf_path, "/flashfetch/%s.pf", filename);

				if(access(pf_path, F_OK) == 0) {
					ret = unlink(pf_path);
					if(ret) {
						fprintf(stderr, "can't unlink prefetch file (%s)\n", pf_path);
					}
				}

				break;
			case 'x':
				ret = ftcheck(optarg, exec_path);
				if(ret == ELF_BINARY) fd = open(optarg, O_RDWR);
				else if(ret == DESKTOP_ICON1) fd = open(exec_path, O_RDWR);

				if(fd < 0) perror("open()");
				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}

				val = 0;

				ret = write(fd, &val, 1);
				if(ret != 1) {
					perror("write()");
					return ret;
				}

				ret = extract_filename(optarg, filename);
				if(ret < 0) {
					fprintf(stderr, "can't extract filename from filepath!\n");
					return -EINVAL;
				}
				memset(pf_path, '\0', sizeof(pf_path));
				sprintf(pf_path, "/flashfetch/%s.pf", filename);

				if(access(pf_path, F_OK) == 0) {
					ret = unlink(pf_path);
					if(ret) {
						fprintf(stderr, "can't unlink prefetch file (%s)\n", pf_path);
					}
				}

				break;
			case 'f':
			case 's':
			case 'a':
				ret = ftcheck(optarg, exec_path);
				if(ret == ELF_BINARY) fd = open(optarg, O_RDWR);
				else if(ret == DESKTOP_ICON1) fd = open(exec_path, O_RDWR);

				if(fd < 0) perror("open()");
				ret = lseek(fd, EI_PAD, SEEK_SET);
				if(ret != EI_PAD) {
					perror("lseek()");
					return ret;
				}

				if(opt == 'f') val = 4;
				else if(opt == 's') val = 2;
				else if(opt == 'a') val = 1;

				ret = write(fd, &val, 1);
				if(ret != 1) {
					perror("write()");
					return ret;
				}

				break;
		}
	}
} 
