#include <iostream>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv){
    double m=0, s=0;
    int n=0, element;
    vector<int>elements;
    cin>>element;
    while(not cin.eof()){
        elements.push_back(element);
        n+=1;
        cin>>element;
    }
    
    
    //Si la muestra es impar
    if(n%2!=0){
        m=elements[n/2];
    }else{
        m=(elements[n/2]+elements[n/2+1])/2;
    }

    //Number of elements is the n variable
    cout<<"Number of elements: "<<n<<endl;
    //Sorting de vector
    for(int i=0; i<sizeof(elements)-1; i++){
        for (int j=0; j<n-i-1;j++){
            if(elements[j]>elements[j+1]){
                swap(elements[j], elements[j+1]);
            }
        }
    }

    //Printear el vector ordenado
    cout<<"Los elementos ordenados son: ";
    for(int x=0; x<elements.size(); x++){
        cout<<elements[x];
    }
    //Printear la mediana
    cout<<"Median: "<<m<<endl;

    return 0;
}