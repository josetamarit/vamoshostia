#include <iostream>
#include <vector>
using namespace std; 

int main() {
	double m = 0, s = 0;
	int n = 0, input;
	vector<int> elements;
	cout << "Dame un número ó 0 para acabar: " << endl;
	cin >> input;
	
	while( not cin.eof() ) { 
		elements.push_back(input);
		n++;
		cout << "Dame un número ó 0 para acabar: " << endl;
		cin >> input;
	}
	for ( int i=0; i< n; i++ ) {
		s+= elements[i];	
	}
	m = s/n; 
	cout << "media aritmética: " << m << endl;
	return 0;
}