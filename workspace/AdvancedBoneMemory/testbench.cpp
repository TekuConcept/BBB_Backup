#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <stdint.h>

#include "mindgem.h"

#define DEVICE "/dev/mindgem"

int main() {
	int fd;
	char ch, io_buf[32]; // write_buf[12], read_buf[12];

	fd = open(DEVICE, O_RDWR | O_NONBLOCK);

	if(fd < 0)
	{
		printf("file %s either does not exist or has been locked by another process\n", DEVICE);
		return -1;
	}



	query_arg_t q;
	q.address = 0x44E00000;
	q.offset  = 0x0044;
	q.value   = 0x00000002;
	fprintf(stderr, "Write: 0x%X+%X: %X\n", q.address, q.offset, q.value);
	if(ioctl(fd, MINDGEM_WRITE, &q))
	{
		perror("run ioctl read");
	}



	q.offset  = 0x0000;
	q.value   = 0x00000000;
	fprintf(stderr, "Read: 0x%X+%X: %X\n", q.address, q.offset, q.value);
	if(ioctl(fd, MINDGEM_READ, &q))
	{
		perror("run ioctl read");
	}
	else
	{
		fprintf(stderr, "Result: 0x%X+%X: %X\n", q.address, q.offset, q.value);
	}

	close(fd);
}
