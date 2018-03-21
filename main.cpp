#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <random>

using namespace std;

enum STATUS { DR, SR, PR };
//Three modes: driving, searching for parking, parking

struct TruckProp
{
    STATUS stu;
    double time;        // legal drving time left
    double speed;       //speed
};

struct RestArea
{
    int num;            //number of trucks already park here
    int location;       //rest area location
};




int main() {
    //generate random numbers
    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    default_random_engine e;                            // 定义一个随机数引擎
    float a = 10*u(e);

    /*Parameters*/
    int L = 0;                                          //legal driving distance remaining


    std::cout << "Hello, World!" << a<<std::endl;
    return 0;
}

