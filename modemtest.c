#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int open_port(char *ttydev) {
	int fd;
	struct termios options;

	fd = open(ttydev, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		return -1;
	}

	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &options);

	/* set raw input, 1 second timeout */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	options.c_cc[VMIN]  = 0;
	options.c_cc[VTIME] = 10;

	/* 8N1 */
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	/* 9600 */
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	/* set the options */
	tcsetattr(fd, TCSANOW, &options);
	return (fd);
}

int init_modem(int fd) {
	char buffer[255];
	char *bufptr;
	int  nbytes;
	int  tries;

	for (tries = 0; tries < 3; tries ++) {
		if (write(fd, "AT\r", 3) < 3) {
			continue;
		}
		bufptr = buffer;
		while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0) {
			bufptr += nbytes;
			if (bufptr[-1] == '\n' || bufptr[-1] == '\r') {
				break;
			}
			*bufptr = '\0';

			if (strncmp(buffer, "OK", 2) == 0) {
				return (0);
			}

			return (-1);
		}
	}
}

int main(int argc, char *argv[]) {
	int fd;

	if (argc <= 1) {
		printf("Missing Device\n");
	}

	if (((fd = open_port(argv[1])) == -1) ||
	    (!init_modem(fd))) {
		printf("Device %s Failed\n", argv[1]);
		return -1;
	};

	printf("Device %s Is Ok\n", argv[1]);
	return 0;
}
