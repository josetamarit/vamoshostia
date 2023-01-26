#include <iostream>
#include <istream>

using namespace std;

int main(){
	int numero;
	long suma;
	cout<<"Introduce un número"<<endl;
	cin>>numero;
	suma= (numero*(numero+1))/2;
	
	cout<<"La suma de todos los números hasta "<<numero<<" es "<<suma<<endl;
	return 0;
}
