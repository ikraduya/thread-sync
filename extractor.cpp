#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include <iostream>
#include <string.h>

using namespace std;

char *dummybuf = "halo apakah string ini bisa dipipe secara paralel? done";

int main() {
  int fd;
  char *fifo = "/tmp/cobafifo";

  mkfifo(fifo, 0777);

  stringstream ss(dummybuf);
  char bufword[256];

  // open FIFO for writeonly
  while(ss >> bufword){
    cout << bufword << endl;
    fd = open(fifo, O_WRONLY);
    write(fd, bufword, strlen(bufword)+1);
    close(fd);
    // sleep(1);
    // getchar();
  }
  
  unlink(fifo);
  return 0;
}
