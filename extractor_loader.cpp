#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <string.h>

#define T_NUM 1

using namespace std;

struct thread_data {
  int thread_id;
  char* data;
};

int g_ct = 0;

// Global vars
pthread_mutex_t *mutexes;
pthread_cond_t *cond_vars;
char **fifo;

char lain[] = "huh";

void *extractor(void *data) {
  struct thread_data *t_data = (struct thread_data *) data;


  pthread_mutex_lock(&mutexes[t_data->thread_id]);
  cout << "extractor: " << t_data->data << " " << t_data->thread_id << endl;
  cout << "cs extractor " << fifo[t_data->thread_id] << endl;
  // critical section
  int fd;
  try {
    fd = open(fifo[t_data->thread_id], O_WRONLY);
  } catch (...) {
    printf("errro");
  }

  cout << "cs extractor 2" << endl;
  write(fd, (char *)t_data->data, sizeof(t_data->data)+1);
  close(fd);
  pthread_mutex_unlock(&mutexes[t_data->thread_id]);
  pthread_cond_signal(&cond_vars[t_data->thread_id]);

  cout << "extractor: " << t_data->thread_id << " selesai" << endl;
  
  pthread_exit(NULL);
}

void *loader(void * tid) {
  int t_id = *(long*)tid;


  pthread_mutex_lock(&mutexes[t_id]);
  cout << "loader: " << t_id << endl;
  cout << "cs loader" << endl;
  pthread_cond_wait(&cond_vars[t_id], &mutexes[t_id]);
  // critical section
  int fd = open(fifo[t_id], O_RDONLY);
  char buf[4096];
  read(fd, (void *)buf, sizeof(buf));
  printf("Buffer from thread: %d\ndata: %s\n", t_id, buf);
  close(fd);
  pthread_mutex_unlock(&mutexes[t_id]);

  cout << "loader: " << t_id << " selesai" << endl;

  pthread_exit(NULL);
}

string alpha[] = {
  "A", "B", "C", "D", "E"
};


int main() {
  struct thread_data dummy_workload[T_NUM];
  // initialize dummy workload
  for (int i=0; i<T_NUM; i++) {
    dummy_workload[i].thread_id = i;// untuk extractor dulu
    dummy_workload[i].data = new char [4096];
    strcpy(dummy_workload[i].data, (alpha[i%5]).c_str());
    cout << dummy_workload[i].data << endl;
  }

  // initialize mutex and cond lock
  mutexes = new pthread_mutex_t[T_NUM];
  cond_vars = new pthread_cond_t[T_NUM];
  for (int i=0; i<T_NUM; ++i) {
    // int ri1 = pthread_mutex_init(&mutexes[i], NULL);
    mutexes[i] = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    // if (ri1) {
    //   printf("Mutex can't be created : %d\n", i);
    // }
    pthread_cond_init(&cond_vars[i], NULL);
  }

  int fd;
  string fifo_default = "/tmp/fifothread";

  pthread_t *ethread;
  pthread_t *lthread;

  // create named pipes
  fifo = new char*[T_NUM];
  for (int i=0; i<T_NUM; ++i) {
    fifo[i] = new char[4096];
    strcpy(fifo[i],(fifo_default + "_" + to_string(i)).c_str());  // assign name to pipe
    mkfifo(fifo[i], 0777);
  }

  // create pthreads
  int itr;
  ethread = new pthread_t[T_NUM];
  lthread = new pthread_t[T_NUM];
  for (itr=0; itr<T_NUM; ++itr) {
    int rc1 = pthread_create(&ethread[itr], NULL, extractor, (void *)&dummy_workload[itr]);
    if (rc1) {
      printf("Extractor thread can't be created : %d [%s]\n", itr, strerror(rc1));
    }
    int rc2 = pthread_create(&lthread[itr], NULL, loader, (void *)&dummy_workload[itr].thread_id);
    if (rc2) {
      printf("Loader thread can't be created : %d [%s]\n", itr, strerror(rc2));
    }
  }

  // wait for all process termination
  for (int i=0; i<T_NUM; ++i) {
    pthread_join(ethread[i], NULL);
    pthread_join(lthread[i], NULL);
  }
  cout << "sini process selesai" << endl;

  // unlink all named pipe
  for (int i=0; i<T_NUM; ++i) {
    unlink(fifo[i]);
  }

  for (int i=0; i<T_NUM; ++i) {
    pthread_mutex_destroy(&mutexes[i]);
    delete[] dummy_workload[i].data;
    delete[] fifo[i];
  }

  delete[] fifo;
  delete[] lthread;
  delete[] ethread;
  delete[] mutexes;
  delete[] cond_vars;

  pthread_exit(NULL);
  return 0;
}

