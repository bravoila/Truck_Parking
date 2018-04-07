#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the observation point
    double location;       //rest area location

    //!!change here in the truck parking code
    int Snum[24];       //number of trucks taking SHORT rest
    //eg. Snum[3] store the number of trucks parked at RestArea from [3,4)
    // if truck parks from 1.5 to 3.5, then Sunm[1],[2],[3] all plus 1.
    int Lnum[24];       //number of trucks taking LONG rest

};


int main() {

    // read rest area info
/*
    int rnum = 0;
    int m = 20;
    int i = 0;

    RestAreaStru RestArea[m];

    ifstream infile("RestA_info.txt", ios::in);
    if(!infile)
    {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    while(!infile.eof() && rnum < m)
    {
        infile >> RestArea[rnum].id >> RestArea[rnum].location;
        for( i = 0 ; i< 24 ; i++)
        {
            infile >> RestArea[rnum].Snum[i] ;
        }
        for( i = 0 ; i< 24 ; i++)
        {
            infile >>  RestArea[rnum].Lnum[i];
        }
        rnum++;

    }

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
    infile.close();

    */

    int m =5;
    int it = m -1 ;                         //iterater
    int a = 0;                       // store the RestArea number of SHORT rest
    int b = 0;                       // store the RestArea number of LONG rest
    int s1 = 0;                      // store the entering time of SHORT and LONG rest
    int s2 = 0;                      // store the leaving time number of SHORT LONG rest, in 1 hour interval

    int speed = 60;
    double legal = 0.5;
    double eti = 100;
    double RestArea[m] = {40,40,40,40,50};
    // allocate the rest area to decide the tru BP1
    // truck cannot park randomly, a is actually the last RestArea number
    // the driver can park in order to obey HOS

    while(int(floor((speed * legal + eti)/ RestArea[it])) == 0)
    {
        it--;
    }

    a = it;
    cout<<a<<endl;

    return 0;
}