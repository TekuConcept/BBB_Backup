#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mindgem.h"

#define D_READ  0
#define D_WRITE 1
#define DEVICE "/dev/mindgem"

/* --------------------------------------------------- *\
|                     PROGRAM INPUT                     |
\* --------------------------------------------------- */

void print_usage(char* prog) {
    printf("Usage: %s [-r|-w] HexAddress HexOffset <HexValue>\n", prog);
    printf("\t-r: Read value from memory address\n");
    printf("\t-w: Write value to memory address\n");
    exit(1);
}

int get_state(char* argv[]) {
    if (argv[1][0] != '-') print_usage(argv[0]);
    
    char cmd = argv[1][1];
    if (cmd == 'r') return D_READ;
    else if (cmd == 'w') return D_WRITE;
    else print_usage(argv[0]);
    return -1;
}

unsigned int get_hextoi(char* argv[], int idx) {
    const char* whitelist = "0123456789ABCDEFabcdefx";

    int len = 0, xidx = 0;
    char xflag = 0, cur;
    while((cur = argv[idx][len]) != '\0') {
        if(strchr(whitelist, cur) != NULL) {
            if(cur == 'x') {
                xflag++;
                xidx = len;
            }
            len++;
        }
        else print_usage(argv[0]);
    }

    if(
        (xflag > 1) ||
        (len  <= 2 && xflag) ||
        (xidx != 1 && xflag) ||
        (xidx == 1 && argv[idx][0] != '0')
    ) { print_usage(argv[0]); }

    return strtol(argv[idx], NULL, xflag?0:16);
}

int process_args(int argc, char* argv[], query_arg_t* msg) {
    if(argc < 4) print_usage(argv[0]);
    
    int state = get_state(argv);
    unsigned int addr = get_hextoi(argv, 2);
    unsigned short offset = get_hextoi(argv, 3);
    unsigned int value = 0;
    
    if(state == D_WRITE && argc < 5) print_usage(argv[0]);
    else if (state == D_WRITE) value = get_hextoi(argv, 4);
    
    msg->address = addr;
    msg->offset  = offset;
    msg->value   = value;
    
    return state;
}

/* --------------------------------------------------- *\
|                      DEVICE SETUP                     |
\* --------------------------------------------------- */

int open_device() {
    int fd = open(DEVICE, O_RDWR | O_NONBLOCK);
    
    if (fd < 0) {
        printf("%s is inaccessible at this time\n", DEVICE);
        exit(2);
    }
    
    return fd;
}

void print_err(int res) {
    if(res) {
        printf("Access error: %d\n", res);
    }
}

int main(int argc, char* argv[]) {
    query_arg_t q;
    int state = process_args(argc, argv, &q);
    
    int fd = open_device();
    
    if(state == D_READ) {
        print_err(ioctl(fd, MINDGEM_READ, &q));
        printf("0x%X+%X: %X\n", q.address, q.offset, q.value);
    }
    else {
        print_err(ioctl(fd, MINDGEM_WRITE, &q));
    }
    
    close(fd);
    return 0;
}
