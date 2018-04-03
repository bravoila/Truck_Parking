#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

enum STATUS { DE, DR, SR, PR }; // DE is the default
//Three modes: driving, searching for parking, parking

struct TruckPropStru
{
    double speed;           //Driving speed
    double DRbefore;        //Driving time before entering the highway
    double StartT;          //Time entering the highway
    double BP1;             //Time when driver decides to take the short rest
    int RS;                  // Rest area number for short rest
    double RestShort;       //Duration of short break
    double BP2;             //Time when driver decides to take the long rest
    int RL;                  // Rest area number for short rest
    double RestLong;        //Duration of long break
    double BufferTime;      //Buffer time
    STATUS stu;

//start here tomorrow #############//double arrival;     //arrival time
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

struct EnterExitStru
{
    double dist;  //enterance/exit distance to point 0
    int n1;       //number of trucks entering or exiting
};

int PreferS(int farest)        // preference,return the preferred parking number, can summerize from data
// consider service and legal driving hour left by far
{
    int temp = farest;                      // store the preferred restarea in the iteration
    double Score[20] = {6};      //score of each rest area
    for(int num = 1; num < farest; num++)
    {
        if(Score[num] > Score[farest]) {
            temp = num;
        }
    }
    return temp;
}

int PreferL(int local, int farest)        // preference,return the preferred parking number, can summerize from data
// truck just drive out from rest area local
// consider service and legal driving hour left by far
{
    int temp = farest;                      // store the preferred restarea in the iteration
    double Score[20] = {6};      //Score[m] m value is the same, score of each rest area
    for(int num = local + 1; num < farest; num++)
    {
        if(Score[num] > Score[farest]) {
            temp = num;
        }
    }
    return temp;
}

/*################################  Truck2Rest Function   ##################################*/

// for future modification RestArea[m]!!!
// The function works for 24 hour
void Truck2Rest(TruckPropStru* Truck, double legal, double Spacing, double eti, RestAreaStru RestArea[20])
{

    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    default_random_engine e;                            // 定义一个随机数引擎
    /*float a = 8*u(e);*/
    //std::normal_distribution<double> distribution(5.0,2.0);   //normal distribution
    std::lognormal_distribution<double> lgn1(2,0.5);  // Log-normal distribution,use for arrival
    std::lognormal_distribution<double> lgn2(0.05,0.2);  // Log-normal distribution,use for short rest time

    //https://www.medcalc.org/manual/log-normal_distribution_functions.php
    std::poisson_distribution<int> pos(2.3);   //Poisson distribution

    std::normal_distribution<double> nor1(6,0.7);   //Normal distribution, use for driving time
    std::normal_distribution<double> nor2(2,0.7);   //Normal distribution, use for driving time
    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html

    int a = 0;                       // store the RestArea number of SHORT rest
    int b = 0;                       // store the RestArea number of LONG rest
    int s1 = 0;                      // store the entering time of SHORT and LONG rest
    int s2 = 0;                      // store the leaving time number of SHORT LONG rest, in 1 hour interval
    RegulationStru Reg = {8,11};     // USDOT HOS Regulation

    Truck->BP1 = Truck->StartT +  legal;

    // allocate the rest area to decide the tru BP1
    // truck cannot park randomly, a is actually the last RestArea number
    // the driver can park in order to obey HOS
    a =  int(floor( (legal * Truck->speed + eti)/ Spacing)) ; //farest rest area the trucker can reach legally

    //consider preference here or another function in the header file
    //the driver can park at the place he prefer in [0,a], choose the preferred RestArea
    //if the variance of the preference is below threshold, then choose the farest RestArea (a)
    //update BP1 value
    //similar to BP1
    //a is nearest RestArea
    Truck->RS = PreferS(a);     //rest area for short rest

    s1 = int(floor(Truck->BP1)) % 24;//Time truck enters the RestArea[a], round down
    s2 = int(ceil(Truck->BP1 + Truck->RestShort)) %24;//Time the truck leave the rest area, round up
    Truck->BP1 = Truck->StartT + RestArea[a].location / Truck->speed;
    // short rest time
    //Truck->RestShort = lgn2(e);// rest time distribution truck leaves the RestArea[a], round up

    //number of short term parking static
    while((s1 < s2) || (s1 == s2) ) {
        RestArea[a].Snum[s1] = RestArea[a].Snum[s1] + 1;
        s1 ++;
    }
    Truck->BP2 = Truck->BP1 + Truck->RestShort + min((Reg.MaxWL - Truck->BP1 + Truck->StartT ), nor2(e));
    // the latter is the same as driving time function in the first part
    // truck driver driving time can be less than the regulation

    b = a + int(floor((Truck->BP2 - Truck->BP1 - Truck->RestShort ) * Truck->speed / Spacing));
    //farest rest area trucker can reach
    Truck->RL = PreferL(a,b);
    Truck->BP2 = Truck->BP1 + Truck->RestShort + RestArea[b].location/ Truck->speed;
    //Truck->RestLong = 4 + 12*u(e)+0.5;

    s1 = int(floor(Truck->BP2)) % 24;//Time truck enters the RestArea[b], round down
    s2 = int(ceil(Truck->BP2 + Truck->RestLong)) %24;//Time truck leaves the RestArea[b]

    while((s1 < s2) || (s1 == s2) ) {
        RestArea[b].Lnum[s1] = RestArea[b].Lnum[s1] + 1;
        s1 ++;
    }
}

/*################################  Main Function   ##################################*/
int main() {
    ofstream outFile;
    //generate random numbers
    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    default_random_engine e;                            // 定义一个随机数引擎
    /*float a = 8*u(e);*/
    //std::normal_distribution<double> distribution(5.0,2.0);   //normal distribution
    std::lognormal_distribution<double> lgn1(2,0.5);  // Log-normal distribution,use for arrival
    std::lognormal_distribution<double> lgn2(0.05,0.2);  // Log-normal distribution,use for short rest time

    //https://www.medcalc.org/manual/log-normal_distribution_functions.php
    std::poisson_distribution<int> pos(2.3);   //Poisson distribution

    std::normal_distribution<double> nor1(6,0.7);   //Normal distribution, use for driving time
    std::normal_distribution<double> nor2(2,0.7);   //Normal distribution, use for driving time
    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html

    /*Parameters*/
    int i =0;                               // Iterate for Truck combined with n
    int j =0;                               // Iterate for RestArea combined with m
    int l =0;                               // Random Iterator
    int L = 1000;                           //Total simulation distance unit in mile
    int n = 10000;                          //number of trucks to simulate entering from point 0
    int tn = n;                             // total number of trucks to simulate



    int m = 20;                             // number of rest area
    int et = 5;                             // number of entrance
    double eti = 0;                          // initial entry point ( distant to the 0 point)

    int k = 24;                             // parameter to generate start time,a day is 24h
    int Violation = 0;                      //?potential number of trucks violate the regulation
    int t = 0;                              // time point for print out
    double legal = 0;                       // record legal driving time
    vector<double> LDH = {};               //Legal driving time
    /*initialization*/
    /* Creat rest area*/

    double Spacing = double(L/m);            // Rest area spacing interval
    int TimeInv = 1;                        // Time interval

    // USDOT HOS Regulation
    RegulationStru Reg = {8,11};
    // rest area
    RestAreaStru RestArea[m] ={{0}};

    for ( j = 0; j < m ; j++)
    {
        /*Truck.stu.push_back(DR);*/
        RestArea[j] = {j,0,{},{},(j)*L/m};
        /* in the future it can be set*/
    }
    // entrance{distance to point 0,number of trucks entering}
    struct EnterExitStru Enter[et] = {
            {200,1000},
            {400,2000},
            {600,600},
            {700,100},
            {900,100},
    };

    // get total number of trucks
    for ( l = 0; l< et; l ++)
    {
        tn = tn + Enter[l].n1; //total number of trucks simulated ( including enter);
    }

    // print total number of trucks, for check purpose
    cout<<tn<<endl;


    TruckPropStru Truck[tn] = {{0,0,0,0,0,0,0,0,0,0,DE}};

    // trucks start from the entering point of highway
    for ( i = 0 ; i < n; i++)
    {
        //Truck[i].WorkTime = k*u(e);
        Truck[i].speed = 70;  //assume speed is 70 mph

        /*################################  Set distributions   ##################################*/
        Truck[i].DRbefore = 3*u(e) ;// Driving time before entering the highway
        Truck[i].StartT = abs(lgn1(e)); //Arrival function at entrance
                                        //in the future u(e) can be replaced by traffic flow function
        legal = min((Reg.MaxWS - Truck[i].DRbefore ), nor1(e));// nor1(e) is the first part driving time
        LDH.push_back(legal);          // add to LDH vector
        // short and long rest time
        Truck[i].RestShort = lgn2(e);// rest time distribution truck leaves the RestArea[a], round up
        Truck[i].RestLong = 4 + 12*u(e)+0.5;

        Truck2Rest(&Truck[i], legal, Spacing, eti, RestArea);

/*
        Truck[i].BufferTime = 0;
        //Truck.stu[i] = DE;
        // print results test
        cout<<"Truck "<<i<<endl;
        cout<< Truck[i].StartT<<endl;
        cout<< Truck[i].BP1<<endl;
        cout<< Truck[i].RestShort<<endl;
        cout<< Truck[i].RS<<endl;
        cout<< Truck[i].BP2<<endl;
        cout<< Truck[i].RL<<endl;
        cout<< Truck[i].RestLong<<endl;
*/
    }

    // trucks start from the rest area after long rest(re-enter from rest area)
/*
        //Truck[i].WorkTime = k*u(e);
        for ( i = 0; i < n ; i++) {
            Truck[i].speed = 70;  //assume speed is 70 mph

            //################################  Set distributions   ##################################
            Truck[i].StartT = abs(lgn1(e)); //Arrival function, in the future u(e) can be replaced by traffic flow function
            Truck[i].DRbefore = 3 * u(e);// Driving time before entering the highway
            legal = min((Reg.MaxWS - Truck[i].DRbefore ), nor1(e));// nor1(e) is the first part driving time
            LDH.push_back(legal);          // add to LDH vector
        }

*/

    // trucks enter the highway
    for ( l = 0; l < et; l ++)
    {

        for ( i = n; i < Enter[l].n1; i++)
        {
            //Truck[i].WorkTime = k*u(e);
            Truck[i].speed = 70;  //assume speed is 70 mph

            /*################################  Set distributions   ##################################*/
            Truck[i].DRbefore = 3*u(e) ;// Driving time before entering the highway
            Truck[i].StartT = abs(lgn1(e)); //Arrival function, in the future u(e) can be replaced by traffic flow function

            legal = min((Reg.MaxWS - Truck[i].DRbefore ), nor1(e));// nor1(e) is the first part driving time
            LDH.push_back(legal);          // add to LDH vector

            // short and long rest time
            Truck[i].RestShort = lgn2(e);// rest time distribution truck leaves the RestArea[a], round up
            Truck[i].RestLong = 4 + 12*u(e)+0.5;

            Truck2Rest(&Truck[i], legal, Spacing, Enter[l].dist, RestArea);
        }
    }

    // trucks leaving the highway
    // re-entr after long rest
    // circle simulation
    // tomorrow






    // output to txt file
    // output truck file
    outFile.open("Truck.txt");
     outFile << std::setprecision(2) << std::fixed; // keep two decimals
    //print title
    outFile <<"Truck"<<std::right<<setw(7)<<"Num"<<std::right<<setw(10)<<"StartT"<<std::right<<setw(8)\
            <<"BP1"<<std::right<<setw(13)<<"ShortRest"<<std::right<<setw(15)<<"FirstParking"<<std::right<<setw(8)\
            <<"BP2"<<std::right<<setw(18)<<"SecondParking"<<std::right<<setw(10)<<"LongRest"<<std::right<<setw(8)<<"\n";
    for( i = 0; i < tn; i++)
    {
        outFile <<"Truck "<<std::right<<setw(5)<<i<<std::right<<setw(10)<< Truck[i].StartT <<std::right<<setw(10)\
                <<Truck[i].BP1<<std::right<<setw(10)<< Truck[i].RestShort<<std::right<<setw(10)<<Truck[i].RS<<std::right<<setw(15)\
                <<Truck[i].BP2<<std::right<<setw(12)<<Truck[i].RL<<std::right<<setw(15)<<Truck[i].RestLong<<endl;
    }
    outFile.close();

    //output RestArea
    outFile.open("RestArea.txt");
    outFile << " Number of trucks in short rest \n" <<endl;
    outFile << "RestArea"<<std::right<<setw(10)<<"Time"<<std::right<<setw(10)<<"Number of turcks"<<std::right<<setw(10)<<endl;

    for( j = 0; j < m ; j++)
    {
        outFile <<"Rest Area " << j<<"\n"<< endl;

        for ( t = 0; t<25; t++)
        {
            outFile <<std::right<<setw(20)<<t <<std::right<<setw(10)<< RestArea[j].Snum[t] << endl;
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
    }

    outFile.close();
    return 0;
}

