#include <iostream>
using namespace std; 

int main(){
    int a,b;
    int s=0;
    
    cout<<"Introduce a: "<<endl;
    cin>>a;
    
    cout<<"Introduce b: "<<endl;
    cin>>b;

    if(a>=0 and b>=0){
        cout<<"La suma de "<<a<<"y "<<b<<" es: "<<a+b;
        return 0
    }else{
        cout<<"Repite el procedimiento"<<endl;
        main();
    }
}