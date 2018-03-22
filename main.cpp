#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <cmath>

using namespace std;


enum STATUS { DR, SR, PR };
//Three modes: driving, searching for parking, parking

struct TruckPropStru
{
    STATUS stu;
    double time;        // legal driving time left
    double speed;       //speed
    //double arrival;     //arrival time
};

struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the observation point
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
    int i =0;                               // Iterate for Truck combined with n
    int j =0;                               // Iterate for RestArea combined with m
    int L = 600;                            //Total simulation distance unit in mile
    int n = 10000;                          //number of trucks to simulate
    int m = 5;                              // number of rest area
    int k = 8;                              // parameter to generate legal drving time
    int Violation = 0;                      //potential number of trucks violate the regulation
    double t = 0;                           // Time eclipsed

    /*initialization*/
    /* Creat rest area*/
    RestAreaStru RestArea[m] ={{0}};

    for ( j = 0; j < m ; j++)
    {
        /*Truck.stu.push_back(DR);*/
        RestArea[j] = {j,0,(j)*L/m};

        /* in the future it can be set*/

        cout<<"Id  "<<RestArea[j].id<<endl;
        cout<<"Location  "<<RestArea[j].location<<endl;
    }
   TruckPropStru Truck[n] = {{DR,0,0}};

    for ( i = 0; i < n ; i++)
    {
        //Truck.stu[i] = DR;
        Truck[i].time = k*u(e);
        Truck[i].speed = 70;  //assume speed is 70 mph

        cout<< "Truck number "<<i<<"; Time "<<Truck[i].time<<endl;
        //cout<<"Speed  "<<Truck[i].speed<<endl;

    }
    //i =0;
    //a = floor(Truck.time.at(i) * Truck.speed.at(i) / (L/m));
    //cout<<a<<endl;


    for ( i = 0; i<n ; i++)
    {
        // calculate rest area id for truck i to park
        a = int(floor(Truck[i].time * Truck[i].speed / (L/m))) ;
        // the number of trucks in the rest area
        RestArea[a].num = RestArea[a].num + 1;
        //potential number of trucks violate the regulation
        Violation = 0;
    }

    for( j = 0; j <m ; j++)
    {
        cout << " Number of trucks in rest area " <<j<<" is "<<RestArea[j].num<< endl;
    }



    return 0;
}

