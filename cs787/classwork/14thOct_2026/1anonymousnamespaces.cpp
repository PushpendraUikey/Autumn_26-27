#include <iostream>
using namespace std;

namespace {

 int p, q;

}



int main () {


 p = 10;
 q = 20;

 cout << p+q << endl;


}

// where is the using? --> already unlocked inside the file
// unnamed/anonymous namespace are by default unlocked inside the file,
// named namespace are by default locked inside the file. You need to have a 'using' keyword.