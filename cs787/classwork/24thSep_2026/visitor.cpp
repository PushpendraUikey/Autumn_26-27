//fix it
#include <iostream>
using namespace std;
class A;
class Visitor {
	public:
		virtual void visit(A*)=0;
		virtual int getv(A*);
};
class A {
 friend class Visitor;
 int v;
	public: 
		A(int x) {v=x;}
		void visitme(Visitor *v) {
			v->visit(this);
		}
};
int Visitor::getv(A* ap) { return ap->v;}

class Visitor1 : public Visitor {
	public:
		virtual void visit(A *ap) {
			cout << "visitor 1 " << getv(ap) << endl;
		}
};

class Visitor2 : public Visitor {
	public:
		virtual void visit(A *ap) {
			cout << "visitor 2 " << getv(ap) << endl;
		}
};

int main () {
	A *ap1= new A(10);
	A *ap2= new A(20);
	Visitor *v1, *v2;
	v1=new Visitor1();
	v2=new Visitor2();
	ap1->visitme(v1);
	ap2->visitme(v2);
}
