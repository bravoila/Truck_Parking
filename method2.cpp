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

struct TruckPropStru {

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
struct Position {
    double Tau ;            //Time at [position] [x] for a truck entering the highway at time s
    double Sigma ;          //Highway entrance [time] for a car truck be in [position] [x] at time t

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

double integral(double(*f)(double x), double a, double b, int n) {
    double step = (b - a) / n;  // width of each small rectangle
    double area = 0.0;  // signed area
    for (int i = 0; i < n; i ++) {
        area += f(a + (i + 0.5) * step) * step; // sum up each small rectangle
    }
    return area;
}


double fun1(double x)       //标准正态分布 for test purposes
{
    const int u=0;
    const int b=1;
    const double Pi=3.14159;
    double coefficient=1/(sqrt(2*Pi)*b);
    double index=-((x-u)*(x-u)/(2*b*b));
    return coefficient*exp(index);
}

double EQfun(double x)       //标准正态分布 for test purposes
{
    const int u=0;
    const int b=1;
    const double Pi=3.14159;
    double coefficient=1/(sqrt(2*Pi)*b);
    double index=-((x-u)*(x-u)/(2*b*b));
    return coefficient*exp(index);
}



//Driver  is  driving  until  some  randomtime T0 to start finding place for parking

//Near  the  driving  time  limit,  truck  driver  begins  to  search  for  available parking space
//along  the  high  in  order  to  obey  the  Hour  of  Service(HOS) regulation.
// The searching process takes T1 to finish


//When the truck driver finds the rest area, he leaves the highway and start to rest.


int main() {

    std::exponential_distribution<double> nexp(3.5); // for T0
    // negative expectational lambda*e^(-lambda x) x>0
    // for T1
    int x =1;
    Position PosX[x];
    double Q = 0;
    double H = 0;
    double CEnter = 0;
    double CExit = 0;
    double time = 1;
    int N = 10000;      //interval
    double inf = std::numeric_limits<double>::infinity(); //????
    Q = integral(EQfun, sigma[t], time, N);
    H = integral(EHfun, -inf, sigma[t], N);
    Q = integral(fun1, -inf, sigma[t], N);
    Q = integral(fun1, -inf, sigma[t], N);


    cout.precision(7);
    cout << integral(fun1, -99, 99, 10000);

    return 0;
}