#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string.h>

using namespace std;

int main() {
  int fd;
  char *myfifo = "/tmp/myfifo";

  // create the FIFO (named pipe)
  mkfifo(myfifo, 0666);

  char buf1[80], buf2[80];
  while (1) {
    // open fifo for readonly
    fd = open(myfifo, O_RDONLY);

    // read from fifo
    read(fd, buf1, sizeof(buf1));

    printf("User1: %s\n", buf1);
    close(fd);


    // // open FIFO for writeonly
    // fd = open(myfifo, O_WRONLY);

    // // takes input from user
    // fgets(buf2, 80, stdin);

    // //write input to named pipe
    // write(fd, buf2, strlen(buf2)+1);
    // close(fd);
  }
  
  unlink(myfifo);

  return 0;
}