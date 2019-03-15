#include <sstream>
#include <iostream>
using namespace std;

int main() {
  stringstream s("co ba in i di break sama sstream");
  string word;

  int ct = 0;
  while (s >> word) {
    cout << word << endl;
    ct++;
  }

  cout << "Number of words = " << ct << endl;
  return 0;
}