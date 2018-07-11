#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

int main(){
    string path = "../test/data/";
    string file;
    ifstream testin;
    ifstream myin;
    ofstream out;
    stringstream stream;
    string num;



    // out.open(file + "_.ans");
    // out.close();

    cout << "begin test\n";
    for(int i = 1; i <= 60; ++i){
        cout << "test " << i << '\n';
        num = to_string(i);
        file = path + num;
        


        string order = "a.exe ";
        order += file + ".s ";
        order += "<" + file + ".in>";
        order += file + "_.ans\n";

        //cout << "order name:" << order << '\n';
        try{
            system(order.data());

            testin.open(file+".ans");
            myin.open(file + "_.ans");
            if(!testin ) cerr << " test file error\n";
            if(!myin ) cerr << " my file error\n";


            string test, my;
            while(getline(testin, test)){
                getline(myin, my);
                //cout << test << '\t' << my << '\n';
                if(my != test) cout << "file:" << i << "\nstandard:\t" << test << '\n' << "my:\t\t" << my << '\n';
                //system("pause");
            }
        }catch(...){
            cout << "wrong file:" << i << '\n';
            system("pause");
        }
        testin.close();
        myin.close();
    }

    cout << "end test\n";
    return 0;
}