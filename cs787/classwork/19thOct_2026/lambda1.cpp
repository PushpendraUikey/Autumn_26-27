#include <iostream>
using namespace std;


int main () {



   cout << [] (int x, int y) -> int {return x+y;}  (10,20) << endl;
   [] (int x) { if(x&1) cout << "odd" << endl; else cout << "even" << endl;}  (10);
   [] (int x) { if(x&1) cout << "odd" << endl; else cout << "even" << endl;}  (11);

}
