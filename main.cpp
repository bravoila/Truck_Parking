#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

enum STATUS { DE, DR, SR, PR }; // DE is the default
//Three modes: driving, searching for parking, parking

struct TruckPropStru
{
    double speed;           //Driving speed
    double DRbefore;        //Driving time before entering the highway
    double StartT;          //Time entering the highway
    double BP1;             //Time when driver decides to take the short rest

    //consider combine BP1, BP2 to BP[i]
    //consider combine RestShort, RestLong to Rest[i]
    //consider combine RS,RL to R[i]
    //then re-enter and multi resting is possible
    // end condition: truck driving out of observing segment

    int RS;                  // Rest area number for short rest
    double RestShort;       //Duration of short break
    double BP2;             //Time when driver decides to take the long rest
    int RL;                  // Rest area number for short rest
    double RestLong;        //Duration of long break
    double Entryd;              //entry distance (distance to point 0) default value =0
    double Exitd;              //exit distance (distance to point 0) default value =L
    STATUS stu;
};

struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the observation point
    double location;    //rest area location
    int Snum[24];       // number of trucks taking SHORT rest
                        // eg. Snum[3] store the number of trucks parked at RestArea from [3,4)
                        // if truck parks from 1.5 to 3.5, then Sunm[1],[2],[3] all plus 1.
    int Lnum[24];       //number of trucks taking LONG rest

};

struct RegulationStru
{
    double MaxWS;   //Maximum working hours until interruption by short break regulation
    double MaxWL;    //Maximum working hours until interruption by long break regulation

};
//partical OD

struct EnterExitStru
{
    double Etd;     //enterance distance to point 0
    double Exd;     //exit distance to point 0
    int num;        //number of trucks entering or exiting
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

// The function works for 24 hour

// To do:
// add capacity consideration, violation,etc
// add perference of rest area
// simulate circular of segment of highway?

void Truck2Rest(struct TruckPropStru *Truck, struct RestAreaStru RestArea[],vector<double> &REE,int m)
{
    default_random_engine e;                            // 定义一个随机数引擎
    std::normal_distribution<double> nor1(6,0.7);   //Normal distribution, use for driving time
    std::normal_distribution<double> nor2(3,0.7);   //Normal distribution, use for driving time

    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html

    int it = m - 1;                  //iterator
    int a = 0;                       // store the RestArea number of SHORT rest
    int b = 0;                       // store the RestArea number of LONG rest
    int s1 = 0;                      // store the entering time of SHORT and LONG rest
    int s2 = 0;                      // store the leaving time number of SHORT LONG rest, in 1 hour interval
    double legal = 0;                   // record legal driving time
    double loc1;                  //for calculation
    RegulationStru Reg = {8.0,11.0};     // USDOT HOS Regulation

    //find the nearest rest area before location of exit, determine the range
    while(int(floor((Truck->Exitd)/ RestArea[it].location)) == 0)
    {
        it--;
    }

    m = it;// reset m value for b calculation

    if(Truck->DRbefore < Reg.MaxWS) {
        legal = min((Reg.MaxWS - Truck->DRbefore ), nor1(e));// nor1(e) is the first part driving time
        Truck->BP1 = Truck->StartT + legal;

        // allocate the rest area to decide the truck BP1
        // truck cannot park randomly, a is actually the last RestArea number
        // the driver can park in order to obey HOS

        // find the parking for short rest
        while (int(floor((Truck->speed * legal + Truck->Entryd) / RestArea[it].location)) == 0) {
            it--;
        }

        a = it;
        cout << a << endl;
        // when
        if (a == m - 1 || a < 0 || RestArea[a].location < Truck->Entryd || RestArea[a].location == Truck->Entryd) {
            cout << "q" << endl;
            Truck->RS = 66;//error code for outside the highway section for the first part
            return;
            // if the truck is beyond the observed segment (a = m-1)
            // or before the entry point(RestArea[a].location < eti||RestArea[a].location == eti),
            // or the truck cannot even reach the nearset rest area( a= -1\ a <0), then quit the function
        }
        //consider preference here or another function in the header file
        //the driver can park at the place he prefer in [0,a], choose the preferred RestArea
        //if the variance of the preference is below threshold, then choose the farest RestArea (a)
        //update BP1 value
        //similar to BP1
        //a is nearest RestArea
        Truck->RS = PreferS(a);     //rest area for short rest

        s1 = int(floor(Truck->BP1));//Time truck enters the RestArea[a], round down
        s2 = int(ceil(Truck->BP1 + Truck->RestShort));//Time the truck leave the rest area, round up
        if (s1 > 24 || s2 > 24) {
            Truck->RS = 77;
            return;
        }
        Truck->BP1 = Truck->StartT + (RestArea[a].location - Truck->Entryd) / Truck->speed;
        // short rest time
        //Truck->RestShort = lgn2(e); rest time distribution truck leaves the RestArea[a], round up

        //number of short term parking statistics
        while ((s1 < s2) || (s1 == s2)) {
            RestArea[a].Snum[s1] = RestArea[a].Snum[s1] + 1;
            s1++;
        }
        Truck->BP2 = Truck->BP1 + Truck->RestShort + min((Reg.MaxWL - Truck->BP1 + Truck->StartT ), nor2(e));
        // the latter is the same as driving time function in the first part
        // truck driver driving time can be less than the regulation
        loc1 = RestArea[a].location;

    }else{// when the truck only have long rest in the observed section
        legal = min((Reg.MaxWL - Truck->DRbefore ), nor1(e));// nor1(e) is the first part driving time
        Truck->BP1 = Truck->StartT;
        Truck->RS = 55;// code for only observing the second part
        Truck->RestShort = 0;
        Truck->BP2 = Truck->StartT + legal;
        a = 55;// for continuing the following code
        loc1 = Truck->Entryd;
    }

    it = m ; //reset it value
    while(int(floor((loc1 + (Truck->BP2 - Truck->BP1 - Truck->RestShort)* Truck->speed)/ RestArea[it].location)) == 0)
    {//test whether the truck has left the segment
        it--;
    }
    b = it;
    cout<<b<<endl;
    // when outside the segment
    if(b == m-1 || b == a || RestArea[b].location < Truck->Entryd)
    {
        Truck->RL = 88;// code for outside the highway section for the second part
        cout<<"qq"<<endl;
        return;
        // if the truck is beyond the observed segment (b = m-1)
        // or the truck cannot even reach the nearset rest area( b == a), then quit the function
    }
    Truck->RL = PreferL(a,b);
    Truck->BP2 = Truck->BP1 + Truck->RestShort + ((RestArea[b].location - RestArea[a].location )/ Truck->speed);
    //Truck->RestLong = 4 + 12*u(e)+0.5;

    s1 = int(floor(Truck->BP2));//Time truck enters the RestArea[b], round down
    s2 = int(ceil(Truck->BP2 + Truck->RestLong));//Time truck leaves the RestArea[b]
    if(s1 > 24)
    {
        Truck->RL = 99;//code for truck leave the segment
        return;
    }

    while((s1 < s2) || (s1 == s2) ) {
        RestArea[b].Lnum[s1] = RestArea[b].Lnum[s1] + 1;
        s1 ++;
    }
    //save leaving time (re-enter after long rest)###############################################
    REE.push_back(s2);


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
    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html

    /*Parameters*/
    int i = 0;                               // Iterate for Truck combined with n
    int j = 0;                               // Iterate for RestArea combined with m
    int l = 0;                               // Random Iterator
    double L = 1000.0;                       //Total simulation distance unit in mile
    int n = 10000;                           //number of trucks to simulate entering from point 0
                                             // WARNING: the code cannot run the simulation above 100,000.(total number)
    int tn = n;                              // total number of trucks to simulate
    int m = 20;                              // number of rest area
    int et = 21;                             // total number of combination of entrance and exit
                        // (assume 5 entr , 5 exit, plus initial entry and final exit) C7_2 = 7x6/(2x1)=21
    int k = 24;                              // parameter to generate start time,a day is 24h
    int Violation = 0;                       //?potential number of trucks violate the regulation
    int t = 0;                               // time point for print out
    vector<double> LDH = {};                 //Legal driving time
    vector<double> REE = {};                 //save leaving time (re-enter point for each truck after long rest)
    //initialization
    // Create rest area

    // USDOT HOS Regulation
    RegulationStru Reg = {8.0,11.0};

    /*
    RestAreaStru RestArea[m] ={{0,0.0,{0},{0}}};
    for ( j = 0; j < m ; j++)
    {
        //Truck.stu.push_back(DR);
        RestArea[j] = {j,double(j)*L/m,{},{}};
        // in the future it can be set
    }
    */
    // input rest area info
    RestAreaStru RestArea[m];

    ifstream infile_r("RestA_info.txt", ios::in);
    if(!infile_r)
    {
        cout << "Error: opening RestArea file fail" << endl;
        exit(1);
    }
    while(!infile_r.eof() && j < m)
    {
        infile_r >> RestArea[j].id >> RestArea[j].location;
        for( i = 0 ; i< 24 ; i++)
        {
            infile_r >> RestArea[j].Snum[i] ;
        }
        for( i = 0 ; i< 24 ; i++)
        {
            infile_r >>  RestArea[j].Lnum[i];
        }
        j++;

    }
/*print
    for( int j = 0; j < m ; j++)
    {
        cout <<"Rest Area " << j<<"\n"<< endl;

        cout<<RestArea[j].id<<"\t"<<RestArea[j].location<<endl;

        for ( int t = 0; t<24; t++)
        {
            cout << RestArea[j].Snum[t] <<"\t";
        }
        for ( int tt = 0; tt<24; tt++)
        {
            cout << RestArea[j].Lnum[tt] <<"\t";
        }

        cout<<"\n"<<endl;
    }
    */
    infile_r.close();



    // entrance{distance to point 0,number of trucks entering}
    struct EnterExitStru POD[et];
    /*= {
            {0.0,100,100},{0.0,200,100},{0.0,300,100},
            {0.0,400,100},{0.0,500,100},{0.0,1000,100},
            {100.0,200,100},{100.0,300,100},{100.0,400,100},//double Etd,double Exd int num
            {100.0,500,100},{100.0,1000,100},{200.0,300,100},
            {200.0,400,100},{200.0,500,100},{200.0,1000,100},
            {300.0,400,100},{300.0,500,100},{300.0,1000,100},
            {400.0,500,100},{400.0,1000,100},{500.0,1000,100},
            };
    */
    // input partical OD info
    j = 0; //reset j value
    ifstream infile_p("POD_info.txt", ios::in);
    if(!infile_p)
    {
        cout << "Error: opening POD file fail" << endl;
        exit(1);
    }
    while(!infile_p.eof() && j < et)
    {
        infile_p >> POD[j].Etd >> POD[j].Exd >> POD[j].num;
        j++;
    }

    for( j = 0; j < et ; j++) {
        cout << "POD" << j << "\n";
        cout << POD[j].Etd <<"\t"<< POD[j].Exd <<"\t"<< POD[j].num<<endl;
    }
    infile_p.close();


    // get total number of trucks
    for ( l = 0; l< et; l ++)
    {
        cout<<"POD"<<l<<"\t"<<POD[l].num<<endl; //total number of trucks simulated ( including enter);
        tn = tn + POD[l].num; //total number of trucks simulated ( including enter);
    }

    TruckPropStru Truck[tn] = {{0.0,0.0,0.0,0.0,0,0.0,0.0,0,0.0,0.0,L,DE}};
    // trucks start from the entering point of highway and drive through the whole section
    for ( i = 0 ; i < n; i++)
    {
        //Truck[i].WorkTime = k*u(e);
        Truck[i].speed = 60;  //assume speed is 60 mph
        //################################  Set distributions   ##################################
        Truck[i].DRbefore = 11*u(e) ;// Driving time before entering the highway
        Truck[i].StartT = abs(lgn1(e)); //Arrival function at entrance
                                        //in the future u(e) can be replaced by traffic flow function
        // short and long rest time
        Truck[i].RestShort = lgn2(e);// rest time distribution truck leaves the RestArea[a], round up
        Truck[i].RestLong = 4 + 12*u(e)+0.5;
        Truck[i].Entryd = 0;
        Truck[i].Exitd = L;

        Truck2Rest(&(Truck[i]), RestArea,REE,m);
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

    // partial OD

    for ( l = 0; l < et; l ++)
    {
        cout<<l<<endl;
        cout<<"================="<<endl;

        for ( i = n; i < n + POD[l].num; i++)
        {
            //Truck[i].WorkTime = k*u(e);
            Truck[i].speed = 70;  //assume speed is 70 mph
            //################################  Set distributions   ##################################//
            Truck[i].DRbefore = 3*u(e) ;// Driving time before entering the highway
            Truck[i].StartT = abs(lgn1(e)); //Arrival function, in the future u(e) can be replaced by traffic flow function

            // short and long rest time
            Truck[i].RestShort = lgn2(e);// rest time distribution truck leaves the RestArea[a], round up
            Truck[i].RestLong = 12*u(e)+0.5;
            Truck[i].Entryd = POD[l].Etd;
            Truck[i].Exitd = POD[l].Exd;

            Truck2Rest(&(Truck[i]), RestArea, REE,m);
            cout<<i<<endl;
        }
        n = n + POD[l].num ;
        cout<<"n = "<<n<<endl;
    }
    //##################################################################################################

    // re-entr after long rest
    // continue here
    // under construction


    // output to txt file
    // output truck file
    outFile.open("Truck.txt");
    outFile << std::setprecision(2) << std::fixed; // keep two decimals
    //print title
    outFile <<"Truck"<<std::right<<setw(7)<<"Num"<<std::right<<setw(10)<<"StartT"<<std::right<<setw(8)\
            <<"BP1"<<std::right<<setw(13)<<"ShortRest"<<std::right<<setw(15)<<"FirstParking"<<std::right<<setw(8)\
            <<"BP2"<<std::right<<setw(18)<<"SecondParking"<<std::right<<setw(10)<<"LongRest"<<std::right<<setw(8)\
            <<"Entry"<<std::right<<setw(8)<<"Exit"<<"\n";
    for( i = 0; i < tn; i++)
    {
        outFile <<"Truck "<<std::right<<setw(5)<<i<<std::right<<setw(10)<< Truck[i].StartT <<std::right<<setw(10)\
                <<Truck[i].BP1<<std::right<<setw(10)<< Truck[i].RestShort<<std::right<<setw(10)<<Truck[i].RS<<std::right<<setw(15)\
                <<Truck[i].BP2<<std::right<<setw(12)<<Truck[i].RL<<std::right<<setw(15)<<Truck[i].RestLong<<std::right<<setw(10)\
                <<Truck[i].Entryd<<std::right<<setw(10)<<Truck[i].Exitd<<endl;
    }
    outFile.close();

    //output RestArea
    outFile.open("RestArea.txt");
    outFile << " Number of trucks in short rest \n" <<endl;
    outFile << "RestArea"<<std::right<<setw(10)<<"Time"<<std::right<<setw(10)<<"Number of turcks"<<std::right<<setw(10)<<endl;

    for( j = 0; j < m ; j++)
    {
        outFile <<"Rest Area " << j<<"\n"<< endl;
        for ( t = 0; t<24; t++)
        {
            outFile <<std::right<<setw(20)<<t <<std::right<<setw(10)<< RestArea[j].Snum[t] << endl;
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
    }
    outFile.close();
    return 0;
}

