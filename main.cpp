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
    int Lnum[24];           //number of trucks taking LONG rest
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
    int a = 0;// for test purposes
    /*Parameters*/
    int i =0;                               // Iterate for Truck combined with n
    int j =0;                               // Iterate for RestArea combined with m
    int L = 1000;                            //Total simulation distance unit in mile
    int n = 1000;                             //number of trucks to simulate
    int m = 20;                              // number of rest area
    int k = 24;                             // parameter to generate start time,a day is 24h
    int Violation = 0;                      //potential number of trucks violate the regulation
    double t = 0;                           // Time eclipsed
    vector<double> LDT = {0};               //Legal driving time
    int timenow = 0;
    double LegalTimeLeft = 0;               // Legal driving time left
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

    double LDTT = Reg.MaxWS;

    for ( i = 0; i < n ; i++)
    {
        //Truck[i].WorkTime = k*u(e);
        Truck[i].speed = 70;  //assume speed is 70 mph
        Truck[i].StartT = k*u(e); // in the future u(e) can be repleced by traffic flow function
        Truck[i].BP1 = Truck[i].StartT + 7*u(e)+1; // ku(e) is the first part driving time
        // allocate the rest area to decide the tru BP1
        // truck cannot park randomly
        a = int(floor((Truck[i].BP1 - Truck[i].StartT) * Truck[i].speed / Spacing));
        cout<< a<<endl;
        //update BP1 value
        Truck[i].BP1 = Truck[i].StartT + RestArea[a].location / Truck[i].speed;
        // short rest time
        Truck[i].RestShort = 1*u(e)+0.5;
        //similar to BP1
        Truck[i].BP2 = Truck[i].BP1 + Truck[i].RestShort + (Reg.MaxWL - Truck[i].BP1 + Truck[i].StartT )*u(e) +0.5;
        a = a + int(floor((Truck[i].BP2 - Truck[i].BP1 - Truck[i].RestShort ) * Truck[i].speed / Spacing));
        cout<< a<<endl;
        Truck[i].BP2 = Truck[i].BP1 + Truck[i].RestShort + RestArea[a].location/ Truck[i].speed;
        Truck[i].RestLong = 12*u(e)+0.5;
        Truck[i].BufferTime = 0;

        //Truck.stu[i] = DE;
        cout<< "Truck number "<<i<<"; Time "<<Truck[i].BP2<<endl;//cout<<"Speed  "<<Truck[i].speed<<endl;


    }





    //i =0;
    //a = floor(Truck.time.at(i) * Truck.speed.at(i) / (L/m));
    //cout<<a<<endl;
    // set simulation parameter ( time of obsevation)



        for ( i = 0; i<n ; i++)
    {
        // if timenow is inside


        if((timenow  > Truck[i].BP1) && (timenow  < Truck[i].BP1 + Truck[i].RestShort )){
            LegalTimeLeft = Truck[i].BP1 - Truck[i].StartT;
            // assume truck parks at the farthest RestArea he can reach
            a = int(floor(LegalTimeLeft * Truck[i].speed / Spacing));
            //Short rest
            RestArea[a].Snum[timenow] = RestArea[a].Snum[timenow] + 1;
            cout<<RestArea[a].Snum[timenow]<<endl;
        }

        else if((timenow  > Truck[i].BP2) && (timenow  < Truck[i].BP2 + Truck[i].RestLong)) {
            LegalTimeLeft = Truck[i].BP2 - Truck[i].StartT - Truck[i].RestShort;
            a = int(floor(LegalTimeLeft * Truck[i].speed / Spacing));
            Truck[i].BP2 = Truck[i].StartT + LegalTimeLeft;
            //long rest
            RestArea[a].Lnum[timenow] = RestArea[a].Lnum[timenow] + 1;
            cout<<RestArea[a].Lnum[timenow]<<endl;

        }

    }

    for( j = 0; j <m ; j++)
    {
        for (int t = 0; t<25; t++)
        cout <<t<< " Number of trucks in rest area " <<j<<" is "<<RestArea[j].Snum[t]<< endl;
    }

        // if not

/*
            //short rest
        if((timenow  > Truck[i].BP1) && (timenow  < Truck[i].BP1 + Truck[i].RestShort )){

            // assume truck parks at the farthest RestArea he can reach
            a = int(floor((timenow - Truck[i].StartT) * Truck[i].speed / Spacing));
            // Truck first short rest break point
            Truck[i].BP1 = Truck[i].StartT + LegalTimeLeft;
            Truck[i].BP2 = Truck[i].StartT + Reg.MaxWL - (Truck[i].BP1 + Truck[i].RestShort);
            //Short rest
            RestArea[a].Snum = RestArea[a].Snum + 1;
        }

        else if((timenow  > Truck[i].BP1 + Truck[i].RestShort) && (timenow  < Truck[i].BP2)){
            LegalTimeLeft = Reg.MaxWL-(timenow - Truck[i].StartT);
            a = int(floor(LegalTimeLeft * Truck[i].speed / Spacing));
            Truck[i].BP2 = Truck[i].StartT + LegalTimeLeft;

        }


        if((timenow  > Truck[i].BP1) && (timenow  < Truck[i].BP1 + Truck[i].RestShort )){

            // assume truck parks at the farthest RestArea he can reach
            a = int(floor((timenow - Truck[i].StartT) * Truck[i].speed / Spacing));
            // Truck first short rest break point
            Truck[i].BP1 = Truck[i].StartT + LegalTimeLeft;
            Truck[i].BP2 = Truck[i].StartT + Reg.MaxWL - (Truck[i].BP1 + Truck[i].RestShort);
            //Short rest
            RestArea[a].Snum = RestArea[a].Snum + 1;
        }

        else if((timenow  > Truck[i].BP1 + Truck[i].RestShort) && (timenow  < Truck[i].BP2)){
            LegalTimeLeft = Reg.MaxWL-(timenow - Truck[i].StartT);
            a = int(floor(LegalTimeLeft * Truck[i].speed / Spacing));
            Truck[i].BP2 = Truck[i].StartT + LegalTimeLeft;

        }


        //Truck[i].WorkTime + Truck[i].RestShort * (Truck[i].WorkTime - Reg.MaxWShort) +
        //LongRestNum = Truck[i].RestLong *(Truck[i].WorkTime -Reg.MaxWLong);
        //Decide to take short rest


        // calculate rest area id for truck i to park
        a = int(floor(timenow * Truck[i].speed / (L/m))) ;
        // the number of trucks in the rest area
        RestArea[a].num = RestArea[a].num + 1;
        //potential number of trucks violate the regulation
        Violation = 0;
    }

    for( j = 0; j <m ; j++)
    {
        cout << " Number of trucks in rest area " <<j<<" is "<<RestArea[j].num<< endl;
    }
*/

    return 0;
}

