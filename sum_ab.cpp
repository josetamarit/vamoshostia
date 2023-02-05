#include <iostream>
#include <cassert>
using namespace std;

int main() {
int a,b, suma=0;
cout << "Introduce  a: ";
cin >> a;
cout << "Introduce  b: ";
cin >> b;

while ( a<1 ){
cout << "Introduce  a: ";
cin >> a;
}
while ( b<a ){
cout << "Introduce  b: ";
cin >> b;
}
for (int I=a; I<=b; I++) {
	suma+=I;
	}

cout << "La suma es  " << suma << std::endl;
return 0;
}