#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define DEVICE "/dev/mindgem"

int main() {
	int fd;
	char ch, io_buf[32];

	fd = open(DEVICE, O_RDWR | O_NONBLOCK);

	if(fd < 0)
	{
		printf("file %s either does not exist or has been locked by another process\n", DEVICE);
		return -1;
	}



	// Test Read Functionality using CM_PER[0x00]
	io_buf[0] = 'r';
	io_buf[1] = 0x44; // 0x44E00000
	io_buf[2] = 0xE0;
	io_buf[3] = 0x00;
	io_buf[4] = 0x00;
	io_buf[5] = 0x00; // 0x0000
	io_buf[6] = 0x00;
	fprintf(stderr, "IO_BUF: | %c%X%X%X%X%X%X |\n", io_buf[0], io_buf[1],
		io_buf[2], io_buf[3], io_buf[4], io_buf[5], io_buf[6]);
	write(fd, io_buf, sizeof(io_buf));

	fprintf(stderr, "reading from device\n");
	read(fd, io_buf, sizeof(io_buf));

	uint32_t result = (io_buf[0]<<24) | (io_buf[1]<<16) | (io_buf[2]<<8) | io_buf[3];
	printf("Splice: 0x%X\n", result);



	// Test Write Functionality using CM_PER[0x44]
	io_buf[0] = 'w';
	io_buf[1] = 0x44; // 0x44E00000
	io_buf[2] = 0xE0;
	io_buf[3] = 0x00;
	io_buf[4] = 0x00;
	io_buf[5] = 0x00; // 0x0044
	io_buf[6] = 0x44;
	io_buf[7] = 0x00; // 0x00000000
	io_buf[8] = 0x00;
	io_buf[9] = 0x00;
	io_buf[10]= 0x00;
	fprintf(stderr, "IO_BUF: | %c%X%X%X%X%X%X |\n", io_buf[0], io_buf[1],
		io_buf[2], io_buf[3], io_buf[4], io_buf[5], io_buf[6]);
	write(fd, io_buf, sizeof(io_buf));

	fprintf(stderr, "reading from device\n");
	read(fd, io_buf, sizeof(io_buf));

	result = (io_buf[0]<<24) | (io_buf[1]<<16) | (io_buf[2]<<8) | io_buf[3];
	printf("Splice: 0x%X\n", result);



	close(fd);
}
