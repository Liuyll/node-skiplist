#include<iostream>
using namespace std;

template<class T, class K>
class Test {
    public:
    template<class T> static void test() {
        cout<<"test"<<endl;
    };
};

int main() {
    Test<int,int>::test<int>();
    return 0;
}