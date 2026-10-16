#include <iostream>
using namespace std;
void h();
void f(void);
static int x=100;
//can be rewritten as:
// namespace { int x=100;}
namespace {int y=10;}
int z=1000;
void f(void) { 
	x++; y=x+1; z++; 
	cout << x << " " << y << " " << z << "\n";
}
void g() {
	y++;
}
int main () {
 f();g(); h();
 cout << x << "  " << y << " " << z << endl;
}


/*
Un-named namespaces have a 'static' behavior. They also have an internal linkage, just like 'static' keyword does to 
a variable. 
Objects declared inside an un-named namespace have a static storage duration and are initialized only once at the beginning
of the program. They are destroyed at the end of the program.

They have internal linkage, which means they are only visible within the translation unit (source file)
they are defined in. 

namespace {
    int x = 10;
}
	is translated to by compiler as:
namespace _UNIQUE_NAMESPACE_IDENTIFIER_FOR_THIS_FILE_ {
	int x = 10;
}
usingn _UNIQUE_NAMESPACE_IDENTIFIER_FOR_THIS_FILE_;

This is completely unique to the current source file and no other file can ever link against it.

un-named namespaces are preferred over static variables because static can't be applied to everything.
You CAN'T make 
	static struct
	static class
	static function

But all these can be achieved using un-named namespaces.
*/