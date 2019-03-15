#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <fcntl.h>

using namespace std;

int fd[2]; // file descriptor for creating unnamed pipe
// Read from fd[0], Write to fd[1]

void *reader(void *) {
  while (1) {
    char ch;
    int res;

    res = read(fd[0], &ch, 1);
    if (res != 1) {
      perror("read");
      exit(3);
    }

    printf("Reader: %c\n", ch);
    if (ch == 'Z') {
      break;
    }
  }

  pthread_exit(NULL);
}

void *writer(void *) {
  int res;
  char ch='A';

  while(1){
    res = write(fd[1], &ch, 1);
    if (res != 1) {
      perror("write");
      exit(2);
    }

    printf("Writer: %c\n", ch);
    if (ch == 'Z')
      break;

    ch++;
  }
  pthread_exit(NULL);
}

int main() {
  pthread_t tid1, tid2;
  int res;

  res = pipe(fd);
  if (res < 0) {
    perror("pipe");
    exit(1);
  }

  int rcr, rcw;

  rcr = pthread_create(&tid1, NULL, reader, NULL);
  rcw = pthread_create(&tid2, NULL, writer, NULL);

  if (rcr || rcw)  {
    cout << "Error:unable to create thread," << rcr << endl;
    exit(-1);
  }

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
}

