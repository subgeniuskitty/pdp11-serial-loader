/*
 * © 2017 Aaron Taylor <ataylor at subgeniuskitty dot com>
 * See LICENSE file for copyright and license details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define VER_MAJOR 1
#define VER_MINOR 0

void print_usage( char ** argv ) {
    printf( "PDP-11 Serial Loader %d.%d (www.subgeniuskitty.com)\n"
            "Usage: %s [option] ...\n"
            "Options:\n"
            "  -h          Help (prints this message)\n"
            "  -i <string> Path to input file (raw binary image)\n"
            "  -o <string> Path to serial port (/dev/cuau0 for first port on FreeBSD)\n"
            , VER_MAJOR, VER_MINOR, argv[0]
    );
}

int main( int argc, char ** argv ) {
    /*
     * Variable declarations
     */
    int serial_fd;
    FILE * binary_fd;
    uint16_t bytes = 0;
    struct termios options;
    char * device, * binary_file;

    /*
     * Process command line arguments
     */
    int c;
    while((c = getopt(argc,argv,"hi:o:")) != -1) {
        switch(c) {
            case 'h':
                print_usage(argv);
                exit(EXIT_SUCCESS);
                break;
            case 'i':
                binary_file = optarg;
                break;
            case 'o':
                device = optarg;
                break;
            default:
                break;
        }
    }

    /*
     * Process the binary image file
     */
    if((binary_fd = fopen(binary_file,"r")) < 0) {
        fprintf(stderr, "Failed to open %s\n", binary_file);
        exit(EXIT_FAILURE);
    }

    fseek(binary_fd, 0L, SEEK_END);
    bytes = (uint16_t) ftell(binary_fd);
    rewind(binary_fd);

    uint16_t * buffer = malloc(bytes);
    fread(buffer, 1, bytes, binary_fd);
    fclose(binary_fd);

    /*
     * Open and configure the serial port
     */
    if((serial_fd = open(device, O_RDWR | O_NOCTTY)) < 0) {
        fprintf(stderr, "Failed to open %s\n", device);
        exit(EXIT_FAILURE);
    }

    tcgetattr(serial_fd, &options);

    options.c_cflag &= ~CSIZE;  /* Mask the character size bits */
    options.c_cflag |= CS8;     /* Select 8 data bits */
    options.c_cflag &= ~PARENB; /* Select no parity */
    options.c_cflag &= ~CSTOPB; /* Select 1 stop bit */
    options.c_oflag &= ~OPOST;  /* Select raw output */
    options.c_cflag |= CLOCAL;  /* CLOCAL -> Succeed on open() even w/o connection */
    options.c_cflag |= CREAD;   /* CREAD  -> Enable receiver */

    tcsetattr(serial_fd, TCSANOW, &options); /* TCSANOW -> The change occurs immediately */

    cfsetspeed(&options, B9600);

    /*
     * Sanity check with user
     */
    uint16_t checksum = 0;
    uint16_t words = bytes / 2;
    for(int i = 0; i < words; i++) checksum += buffer[i];
    printf("Checksum: %o\n", checksum);
    printf("Bytes (for R1): %o\n", bytes);
    printf("Press ENTER to begin transmission.");
    int k;
    if((k = getchar()) != 012) { /* Only continue on ENTER (LF -> 012) */
        close(serial_fd);
        exit(EXIT_FAILURE);
    }

    /*
     * Begin transfer
     */
    write(serial_fd, buffer, bytes);

    /*
     * Clean up and GTFO
     */
    free(buffer);
    close(serial_fd);

    exit(EXIT_SUCCESS);
}
