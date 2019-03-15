#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string.h>

using namespace std;

int main() {
  int fd;
  char *fifo = "/tmp/cobafifo";

  mkfifo(fifo, 0777);

  char bufword[256];
  while (1) {
    // open fifo for readonly
    fd = open(fifo, O_RDONLY);

    // read from fifo
    read(fd, bufword, sizeof(bufword));

    printf("Buffer loaded: %s\n", bufword);
    close(fd);

    if (strcmp(bufword, "done") == 0) {
      break;
    }
  }

  cout << "DONE" << endl;

  return 0;
}
