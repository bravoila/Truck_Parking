#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <math.h>

using namespace std;


enum STATUS { DR, SR, PR };
//Three modes: driving, searching for parking, parking

struct TruckPropStru
{
    STATUS stu;
    double time;        // legal drving time left
    double speed;       //speed
};

struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the boservation point.
                                // UNit in meter
    int num;            //number of trucks already park here
    int location;       //rest area location
};

double LostTime = 0;            // Driver's lost time

/*################################  Main Function   ##################################*/

int main() {
    //generate random numbers
    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    default_random_engine e;                            // 定义一个随机数引擎
    /*float a = 8*u(e);*/
    int a = 0;// for test purposes
    /*Parameters*/
    int i =0;
    int j = 0;
    int ij =0;
    int L = 100;                              //Total simulation distance unit in mile
    int n = 100;                             //number of trucks to simulate
    int m = 5;                              // number of rest area
    int k = 8;                              // parameter to generate legal drving time
    double t = 0;                           // Time eclipsed

    /*initialization*/
    /* Creat rest area*/
    RestAreaStru RestArea[m] ={{0}};

    for ( i = 0; i<m ; i++)
    {
        /*Truck.stu.push_back(DR);*/
        RestArea.id[i] = i;
        RestArea.num[i] = 0;
        /* in the future it can be set*/
        RestArea.location[i] = (i)*L/m;
        //cout<<"Id  "<<RestArea.id.at(i)<<endl;
        //cout<<"Location  "<<RestArea.location.at(i)<<endl;
    }
/*    TruckPropStru Truck[m] = {{DR},{0}};

    for ( i = 0; i<n ; i++)
    {
        /*Truck.stu.push_back(DR);*/
        Truck.time.push_back(k*u(e));
        Truck.speed.push_back(70);  //assume speed is 70 mph

        cout<< "Truck number "<<i<<"; Time "<<Truck.time[i]<<endl;
        cout<<"Speed  "<<Truck.speed[i]<<endl;

    }
    //i =0;
    //a = floor(Truck.time.at(i) * Truck.speed.at(i) / (L/m));
    //cout<<a<<endl;


    for ( i =0; i< n ; i++)
    {
        // calculate rest area id for truck i to park
        a = int(floor(Truck.time[i] * Truck.speed[i] / (L/m))) ;
        // the number of trucks in the rest area
        RestArea.num[a] = RestArea.num[a] + 1;
        cout << " add one car in rest area :" <<a<<" "<<RestArea.num[a]<< endl;
        cout<<i<<endl;
    }
*/
    return 0;
}

