#include <iostream>
using namespace std;
int Sumv3(int num){
    int sum = 0;
    for(int i= 1; i <= num; i++){
        sum = sum+i;
    }
    return sum;
}
int main()
{
    int num, sum= 0;
    do{
        std::cout << "Introduce número mayor que 0: "<<endl;
        std::cin >> num;
    }
    while(num < 1);
    sum = Sumv3(num);
    std::cout<< "La suma entre 1 y " << num << " es: " << sum<<endl;
	return 0;
}