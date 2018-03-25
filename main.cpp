#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <cmath>

using namespace std;


enum STATUS { DE, DR, SR, PR }; // DE is the default
//Three modes: driving, searching for parking, parking

struct TruckPropStru
{

    double speed;           //Driving speed
    double StartT;          //Time entering the highway
    double BP1;             //Time when driver decides to take the short rest
    double RestShort;       //Duration of short break
    double BP2;             //Time when driver decides to take the long rest
    double RestLong;        //Duration of long break
    double BufferTime;      //Buffer time
    STATUS stu;

//start here tomorrow #############//double arrival;     //arrival time
// entrance/exit, enter from reset area
};


struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the observation point
    int Anum;           //number of trucks already park here
    int Snum[24];       //number of trucks taking SHORT rest
                        //eg. Snum[3] store the number of trucks parked at RestArea from [3,4)
                        // if truck parks from 1.5 to 3.5, then Sunm[1],[2],[3] all plus 1.
    int Lnum[24];       //number of trucks taking LONG rest
    int location;       //rest area location
};


struct RegulationStru
{
    int MaxWS;   //Maximum working hours until interruption by short break regulation
    int MaxWL;    //Maximum working hours until interruption by long break regulation

};


/*################################  Main Function   ##################################*/

int main() {
    //generate random numbers
    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    default_random_engine e;                            // 定义一个随机数引擎
    /*float a = 8*u(e);*/

    /*Parameters*/
    int i =0;                               // Iterate for Truck combined with n
    int j =0;                               // Iterate for RestArea combined with m
    int L = 1000;                            //Total simulation distance unit in mile
    int n = 10;                           //number of trucks to simulate
    int m = 20;                              // number of rest area

    int a = 0;                           // store the RestArea number of SHORT rest
    int b = 0;                           // store the RestArea number of LONG rest
    int s1 = 0;                         // store the entering time of SHORT and LONG rest
    int s2 = 0;                         // store the leaving time number of SHORT LONG rest, in 1 hour interval

    int k = 24;                             // parameter to generate start time,a day is 24h
    int Violation = 0;                      //potential number of trucks violate the regulation
    int t = 0;                           // time point for print out
    vector<double> LDT = {0};               //Legal driving time
    /*initialization*/
    /* Creat rest area*/
    double Spacing = 1;                     // Rest area spacing interval
    int TimeInv = 1;                        // Time interval
    Spacing = L/m;

    RestAreaStru RestArea[m] ={{0}};

    for ( j = 0; j < m ; j++)
    {
        /*Truck.stu.push_back(DR);*/
        RestArea[j] = {j,0,{},{},(j)*L/m};

        /* in the future it can be set*/

        cout<<"Id  "<<RestArea[j].id<<endl;
        cout<<"Location  "<<RestArea[j].location<<endl;
    }

    RegulationStru Reg = {8,11};     // USDOT HOS Regulation

    TruckPropStru Truck[n] = {{0,0,0,0,0,0,0,DE}};

    for ( i = 0; i < n ; i++)
    {
        //Truck[i].WorkTime = k*u(e);
        Truck[i].speed = 70;  //assume speed is 70 mph
        Truck[i].StartT = k*u(e); //Arrival function, in the future u(e) can be replaced by traffic flow function
        Truck[i].BP1 = Truck[i].StartT + 7*u(e)+1; // ku(e) is the first part driving time
        // allocate the rest area to decide the tru BP1
        // truck cannot park randomly
        a = int(floor((Truck[i].BP1 - Truck[i].StartT) * Truck[i].speed / Spacing));

        //update BP1 value
        Truck[i].BP1 = Truck[i].StartT + RestArea[a].location / Truck[i].speed;
        // short rest time
        Truck[i].RestShort = 1*u(e)+0.5;// rest time distribution
        //similar to BP1

        s1 = int(floor(Truck[i].BP1)) % 24;//Time truck enters the RestArea[a], round down
        s2 = int(ceil(Truck[i].BP1 + Truck[i].RestShort)) %24;//Time truck leaves the RestArea[a], round up

        while((s1 < s2) || (s1 == s2) ) {
            RestArea[a].Snum[s1] = RestArea[a].Snum[s1] + 1;
            s1 ++;
        }



        Truck[i].BP2 = Truck[i].BP1 + Truck[i].RestShort + min((Reg.MaxWL - Truck[i].BP1 + Truck[i].StartT )*u(e) +0.5
                , 7*u(e)+1);
        // the latter is the same as driving time function in the first part
        // truck driver driving time can be less than the regulation


        b = a + int(floor((Truck[i].BP2 - Truck[i].BP1 - Truck[i].RestShort ) * Truck[i].speed / Spacing));

        Truck[i].BP2 = Truck[i].BP1 + Truck[i].RestShort + RestArea[b].location/ Truck[i].speed;
        Truck[i].RestLong = 12*u(e)+0.5;

        s1 = int(floor(Truck[i].BP2)) % 24;//Time truck enters the RestArea[a], round down
        s2 = int(ceil(Truck[i].BP2 + Truck[i].RestLong)) %24;//Time truck leaves the RestArea[a]

        while((s1 < s2) || (s1 == s2) ) {
            RestArea[b].Lnum[s1] = RestArea[b].Lnum[s1] + 1;
            s1 ++;
        }

        Truck[i].BufferTime = 0;

        //Truck.stu[i] = DE;
        // print results
        cout<<"Truck "<<i<<endl;
        cout<< Truck[i].StartT<<endl;
        cout<< Truck[i].BP1<<endl;
        cout<< Truck[i].RestShort<<endl;
        cout<<a<<endl;
        cout<< Truck[i].BP2<<endl;
        cout<<b<<endl;
        cout<< Truck[i].RestLong<<endl;

    }

    for( j = 0; j <m ; j++)
    {
        for ( t = 0; t<2; t++)
        {
            cout << " Number of trucks in short rest in rest area " << j<<"at time "<<t << " is " << RestArea[j].Snum[t] << endl;
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
    }

    return 0;
}

