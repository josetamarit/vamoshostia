#include <iostream>
#include <Eigen/Dense>
using namespace std;
using Eigen::VectorXd;

int main(){
    double m=0, input;
    int n=0;
    VectorXd elements(1);
    cout<< "Dame un número (0 para acabar): "<<endl;
    cin>>input;

    while (input != 0) {
        elements.conservativeResize(n+1);
        elements(n) = input;
        n++;
        cout << "Dame un número (0 para acabar): " << endl;
        cin >> input;
    }

    sort(elements.data(), elements.data() + elements.size());
    int middle = elements.size()/2;
    if (n % 2 == 0) { 
        cout << "Par " << endl;
        m = double(elements(middle-1) + elements(middle))/2.0;
    } else {
        cout << "Impar " << endl;
        m = elements(middle);
    }

    cout << "Median: " << m << endl;
    return 0;
}