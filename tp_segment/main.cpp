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
//default_random_engine e((unsigned  long) time(nullptr));                           //don't use it anymore,use the following
random_device rd;// 定义一个随机数引擎,
mt19937 e(rd());
uniform_real_distribution<double> u(0, 1);

const double L = 100.0;                       //Total simulation distance unit in mile
const int MaxCy = 1;                          // max cycle number run in the simulation
const int TT = MaxCy*24;                     // total simulation time, 16 = L/v

//Three modes: driving, searching for parking, parking

//To do
// Occupied rate( consider capacity)
// Documentation
// Header file

struct TruckPropStru
{
    double speed;               //Driving speed
    double DRbefore;            //Driving time before entering the highway
    double StartT;              //Time entering the highway

    vector<double> BP;          //Time when driver decides to take rest
    vector<int> RN;             // Rest area number for trucker's rest
    vector<double> RestTime;    // Duration of break

    //consider combine BP1, BP2 to BP[i]
    //consider combine RestShort, RestLong to Rest[i]
    //consider combine RS,RL to R[i]
    //then re-enter and multi resting is possible
    // end condition: truck driving out of observing segment
    //
    vector<double> Entryd;              //entry distance (distance to point 0) default value =0
    vector<double> Exitd;              //exit distance (distance to point 0) default value =L
};

struct RestAreaStru
{
    int id;             // Rest area's ID , stands for the distance from the observation point
    double location;    //rest area location
    int Snum[TT];       // number of trucks taking SHORT rest
                        // eg. Snum[3] store the number of trucks parked at RestArea from [3,4)
                        // if truck parks from 1.5 to 3.5, then Sunm[1],[2],[3] all plus 1.
    int Lnum[TT];       //number of trucks taking LONG rest

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
//Calculate the location between each rest area, from rest area i to rest are j
double DistRR(int i,int j,struct RestAreaStru RestArea[])
{
    if(j > i ){
        return RestArea[j].location - RestArea[i].location;
    }else if(RestArea[j].location == RestArea[i].location) {
        return 0.0001;
    }else{
        return L + RestArea[j].location - RestArea[i].location;
    }
}

double DistER(double i,int j,struct RestAreaStru RestArea[])
{
    if(RestArea[j].location > i ){
        return RestArea[j].location - i;
    }else if(RestArea[j].location == i){
        return 0;
    }else{
        return L + RestArea[j].location - i;
    }

}

double DrivingTime(double a)
{
    if(a < 0 ){
        DrivingTime(abs(a)) ;
    }else if(a > 7.99){
        DrivingTime(abs(a - 7.99));
    }else{
        return a;
    };
}

double Arrival() {//custom arrival distribution
        //plot https://www.desmos.com/calculator/7kmkfmqtgp

        int xmin = 0;
        int xmax = 24;
        double x = (xmax - xmin) * u(e) + xmin;
        double mean1 = 8;
        double sigma1 = 1;
        double mean2 = 14;
        double sigma2 = 3.5;
        double k1 = 0.5;
        double k2 = 0.5;
        double fx = k1 / (pow((2 * M_PI), 0.5) * sigma1) * exp(-(pow((x - mean1), 2) / (2 * sigma1 * sigma1))) +
                    k2 / (pow((2 * M_PI), 0.5) * sigma2) * exp(-(pow((x - mean2), 2) / (2 * sigma2 * sigma2)));
        double ymax = 0.2;//see the plot, max y value
        double ymin = 0;
        double Y = (ymax - ymin) * u(e) - ymin;
        if (Y <= fx && x <=24.0 && x > 0.0) {
            std::cout<<x<<std::endl;
            return x;
        }else{
            Arrival();// in case to return random number to main function
        }
    }

//CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
//Truck2RestC is used to simulate circular, start from one rest area to another.
// function for circluar highway (loop)
// from rest area to rest area
void Truck2RestS(struct TruckPropStru *Truck, struct RestAreaStru RestArea[],vector<double> &REE,int m)
{
    //default_random_engine e((unsigned  long) time(NULL));// 定义一个随机数引擎
    std::random_device rd;
    std::mt19937 e(rd());
    //https://gaomf.cn/2017/03/22/C++_Random/

    normal_distribution<double> nor1(7.5,15);   //Normal distribution, use for first driving time
    normal_distribution<double> nor2(2.5,15);   //Normal distribution, use for second driving time

    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html
    m = 20;
    int it = m - 1;                  //iterator
    int a = 0;                       // store the RestArea number of SHORT rest
    int b = 0;                       // store the RestArea number of LONG rest
    int s1 = 0;                      // store the entering time of SHORT and LONG rest
    int s2 = 0;                      // store the leaving time number of SHORT LONG rest, in 1 hour interval
    double rem = 0;                     // remaining distance after deduction circle
    double legal = 0;                   // record legal driving time
    RegulationStru Reg = {8.0,11.0};     // USDOT HOS Regulation

    while(Truck->Exitd.back() > RestArea[it].location)
    {
        it = it -1;
    }
    m = it + 1;
    if(Truck->DRbefore < Reg.MaxWS) {
        legal = min((Reg.MaxWS - Truck->DRbefore ), DrivingTime(nor1(e)));// nor1(e) is the first part driving time
        Truck->BP.push_back(Truck->StartT + legal);
        // allocate the rest area to decide the truck BP1
        // truck cannot park randomly, a is actually the last RestArea number
        // the driver can park in order to obey HOS
        // find the parking for short rest

        //check if the truck will be driven out of the observing segment

        if((Truck->speed * legal) >  DistER(Truck->Entryd.back(),it,RestArea)){
            Truck->BP.clear();//error code for the truck, the data point will be dropped
            Truck->RN.clear();
            Truck->RestTime.clear();
            Truck->Entryd.clear();
            Truck->Exitd.clear();
            return;
        }

        while ((Truck->speed * legal) < DistER(Truck->Entryd.back(),it,RestArea))
        {
            it --;
        };

        a = it;
        cout << "a = "<<a << endl;

        //consider preference here or another function in the header file
        //the driver can park at the place he prefer in [0,a], choose the preferred RestArea
        //if the variance of the preference is below threshold, then choose the farest RestArea (a)
        //update BP1 value
        //similar to BP1
        //a is nearest RestArea

        Truck->RN.push_back(PreferS(a));     //rest area for short rest
        Truck->BP.back() = Truck->StartT + (DistER(Truck->Entryd.back(),a,RestArea)) / Truck->speed;
        // short rest time
        s1 = int(floor(Truck->BP.back())) ;//Time truck enters the RestArea[a], round down
        s2 = int(ceil(Truck->BP.back() + Truck->RestTime.at(Truck->RestTime.size()-2)));//Time the truck leave the rest area, round up

        //number of short term parking statistics
        while ((s1 < s2) || (s1 == s2)) {
            RestArea[a].Snum[s1] = RestArea[a].Snum[s1] + 1;
            s1++;
        }

        //for next rest part(long rest) ####################
        Truck->BP.push_back(Truck->BP.back() + Truck->RestTime.at(Truck->RestTime.size()-2) + min((Reg.MaxWL+ Truck->StartT - Truck->BP.back()), DrivingTime(nor2(e))));
        // the latter is the same as driving time function in the first part
        // truck driver driving time can be less than the regulation

    }else{// when the truck only have long rest in the observed section
        legal = min((Reg.MaxWL - Truck->DRbefore ), DrivingTime(nor1(e)));// nor1(e) is the first part driving time
        // if the truck drives beyond observing highway segment
        if((Truck->speed * legal) >  DistER(Truck->Entryd.back(),it,RestArea)){
            Truck->BP.clear();//error code for the truck, the data point will be dropped
            Truck->RN.clear();
            Truck->RestTime.clear();
            Truck->Entryd.clear();
            Truck->Exitd.clear();
            return;
        }

        Truck->BP.push_back(Truck->StartT);
        Truck->RN.push_back(-1);// code for only observing the second part
        Truck->RestTime.at(Truck->RestTime.size()-2) = 0;
        Truck->BP.push_back(Truck->StartT + legal);

        while(Truck->Entryd.back() > RestArea[a].location){
            a++;//find the nearest rest area at far end.
        }
    }

    rem = (Truck->BP.back() - Truck->BP.at(Truck->BP.size()-2)  - Truck->RestTime.at(Truck->RestTime.size()-2))* Truck->speed;
    if(rem >  DistRR(a,m -1,RestArea)){//here it = m-1
        Truck->BP.pop_back();//error code for the truck, the data point will be dropped
        return;
    }

    it = a + 1;

    while(rem > DistRR(a,it,RestArea))
    {//test whether the truck has left the segment
        it = (it + 1) % m ;
        cout<<rem<<" " <<DistRR(a,it,RestArea)<<endl;
    }

    if(it == a + 1){
        b = a;
    }else{
        b = it -1;
    }

    cout<<"b = "<<b<<endl;
    Truck->RN.push_back(PreferL(a,b));
    cout<<"prefer"<<PreferL(a,b)<<endl;

    Truck->BP.back() = Truck->BP.at(Truck->BP.size()-2) + Truck->RestTime.at(Truck->RestTime.size()-2)  + DistRR(a,b,RestArea) / Truck->speed;

    s1 = int(floor(Truck->BP.back()));//Time truck enters the RestArea[b], round down
    s2 = int(ceil(Truck->BP.back() + Truck->RestTime.back()));//Time truck leaves the RestArea[b]
    while((s1 < s2) || (s1 == s2) ) {
        RestArea[b].Lnum[s1] = RestArea[b].Lnum[s1] + 1;
        s1 ++;
        //consider change Lnum to vector;!!!!!!!!!!!!!!!!1
    }

    REE.push_back(Truck->BP.back() + Truck->RestTime.back());
    //save leaving time (re-enter after long rest)###############################################
}

/*################################  Main Function   ##################################*/
int main() {
    ofstream outFile;
    //generate random numbers
    uniform_real_distribution<double> u(0, 1);          // 定义一个范围为0~1的浮点数分布类型
    random_device rd;// 定义一个随机数引擎
    mt19937 e(rd());
    //std::normal_distribution<double> distribution(5.0,2.0);   //normal distribution
    lognormal_distribution<double> lgn2(0.1,0.1);  // Log-normal distribution,use for short rest time
    //https://www.medcalc.org/manual/log-normal_distribution_functions.php
    //https://homepage.divms.uiowa.edu/~mbognar/applets/normal.html

    /*Parameters*/
    int i = 0;                               // Iterate for Truck combined with n
    int j = 0;                               // Iterate for RestArea combined with m
    int l = 0;                               // Random Iterator
    int count = 0;                      // for the count, run times of truck
    int n = 10000;                           //number of trucks to simulate entering from point 0
                                             // WARNING: the code cannot run the simulation above 100,000.(total number)
    int tn = n;                              // total number of trucks to simulate
    int m = 20;                              // number of rest area
    int et = 21;                             // total number of combination of entrance and exit
                        // (assume 5 entr , 5 exit, plus initial entry and final exit) C7_2 = 7x6/(2x1)=21
    int t = 0;                               // time point for print out
    vector<double> LDH = {};                 //Legal driving time
    vector<double> REE = {};                 //save leaving time (re-enter point for each truck after long rest)
    //initialization
    double test=0;

    RestAreaStru RestArea[m] = {0,0.0,{0},{0}};

    ifstream infile_r("RestA_info3.txt",ios::in);
    if(!infile_r)
    {
        cout << "Error: opening RestArea file fail" << endl;
        exit(1);
    }
    while(!infile_r.eof() && j < m)
    {
        infile_r >> RestArea[j].id >> RestArea[j].location;

        //read previous rest truck
//        for( i = 0 ; i< 24 ; i++)
//        {
//            infile_r >> RestArea[j].Snum[i] ;
//        }
//        for( i = 0 ; i< 24 ; i++)
//        {
//            infile_r >>  RestArea[j].Lnum[i];
//        }
        j++;

    }

    infile_r.close();

    //###################### POD ###############################
 // POD is not needed in circular simulation
    // entrance{distance to point 0,number of trucks entering}
    struct EnterExitStru POD[et];
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
    //output check
    for( j = 0; j < et ; j++) {
        cout << "POD" << j << "\n";
        cout << POD[j].Etd <<"\t"<< POD[j].Exd <<"\t"<< POD[j].num<<endl;
    }
    infile_p.close();


    // get total number of trucks,may be deleted later
    for ( l = 0; l< et; l ++)
    {
        cout<<"POD"<<l<<"\t"<<POD[l].num<<endl; //total number of trucks simulated ( including enter);
        tn = tn + POD[l].num; //total number of trucks simulated ( including enter);
    }
    //###############################################


    outFile.open("Truck.csv",ios::out);
    outFile << std::setprecision(2) << std::fixed; // keep two decimals
    //print title
    outFile <<"TruckNum"<<","<<"StartT"<<","\
            <<"BP1"<<","<<"ShortRest"<<","<<"FirstParking"<<","\
            <<"BP2"<<","<<"LongRest"<<","<<"SecondParking"<<","\
            <<"Entry"<<","<<"Exit"<<"\n";

    TruckPropStru Truck;

    // trucks start from the entering point of highway and drive through the whole section
    for ( i = 0 ; i < n; i++)
    {
        //initialization
        m = 20;
        Truck.speed = 65;  //assume speed is 65 mph
        //################################  Set distributions   ##################################
        Truck.DRbefore = 11*u(e) ;// Driving time before entering the highway
        Truck.StartT = Arrival(); //Arrival function at entrance. In the future it can be replaced by traffic flow function
        // short and long rest time
        Truck.RestTime.push_back(lgn2(e));// rest time distribution truck leaves the RestArea[a], round up
        Truck.RestTime.push_back(11*u(e)+0.5);
        Truck.Entryd.push_back(0.1);
        Truck.Exitd.push_back(RestArea[m-1].location);
        outFile <<i<<","<< Truck.StartT <<",";
        Truck2RestS(&Truck, RestArea,REE,m);//function
        if( Truck.BP.size() != 0){
            Truck.Exitd.pop_back();
            Truck.Exitd.push_back(RestArea[Truck.RN.back()].location);
            cout<<"!!!!!!!!!!!!!SIZE"<<Truck.BP.size()<<endl;

            for (unsigned it = 0; it < Truck.BP.size(); it++)
            {
                outFile<<Truck.BP.at(it)<<","<< Truck.RestTime.at(it)<<","<<Truck.RN.at(it)<<",";
            }
            outFile << Truck.Entryd.back() << "," << Truck.Exitd.back()<<",";
        }
        outFile<<endl;

        //clear and reset for next loop
        Truck.BP.clear();          //Time when driver decides to take rest
        Truck.RN.clear();             // Rest area number for trucker's rest
        Truck.RestTime.clear();    // Duration of break
        Truck.Entryd.clear();
        Truck.Exitd.clear();
        REE.clear();
    }

    outFile<<"Partial OD \n"<<endl;

    // ######################### partial OD ########################################
   for ( l = 0; l < et; l ++)
    {
        cout<<l<<endl;
        cout<<"================="<<endl;

        for ( i = n ; i <  n + POD[l].num ; i++)
        {
            //initialization
            m = 20;
            Truck.speed = 65;  //assume speed is 65 mph
            //################################  Set distributions   ##################################
            Truck.DRbefore = 11*u(e) ;// Driving time before entering the highway
            Truck.StartT = Arrival(); //Arrival function at entrance. In the future it can be replaced by traffic flow function
            // short and long rest time
            Truck.RestTime.push_back(lgn2(e));// rest time distribution truck leaves the RestArea[a], round up
            Truck.RestTime.push_back(11*u(e)+0.5);
            Truck.Entryd.push_back(POD[l].Etd);
            Truck.Exitd.push_back(POD[l].Exd);

            outFile <<i<<","<< Truck.StartT <<",";
            Truck2RestS(&Truck, RestArea,REE,m);//function
            if( Truck.BP.size() != 0){
                Truck.Exitd.pop_back();
                Truck.Exitd.push_back(RestArea[Truck.RN.back()].location);
                for (unsigned it = 0; it < Truck.BP.size(); it++)
                {
                    outFile<<Truck.BP.at(it)<<","<< Truck.RestTime.at(it)<<","<<Truck.RN.at(it)<<",";
                }
                outFile << Truck.Entryd.back() << "," << Truck.Exitd.back()<<",";
            }
            outFile<<endl;

            //clear and reset for next loop
            Truck.BP.clear();          //Time when driver decides to take rest
            Truck.RN.clear();             // Rest area number for trucker's rest
            Truck.RestTime.clear();    // Duration of break
            Truck.Entryd.clear();
            Truck.Exitd.clear();
            REE.clear();
        }

        n = n + POD[l].num ;
        cout<<"n = "<<n<<endl;
    }
    //##################################################################################################

    outFile.close();

    //output RestArea
    outFile.open("RestArea.csv");
    outFile << " Number of trucks in short rest \n" <<endl;
    outFile << "RestArea"<<","<<"Time"<<","<<"Number of trucks"<<endl;

    for( j = 0; j < m ; j++)
    {
        outFile <<"RestArea"<< j<<"\n";
        outFile<<"Time ";
        for ( t = 0; t <TT; t++)
        {
            outFile <<t<<"," ;
            if(t % 24 == 0 && t !=0){
                outFile << " day "<< int(floor(t/24))<<",";
            }
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
        outFile <<endl;
        outFile<<"Snum ";
        for ( t = 0; t <TT; t++)
        {
            outFile <<RestArea[j].Snum[t]<<"," ;
            if(t % 24 == 0 && t !=0){
                outFile << " day "<< int(floor(t/24))<<",";
            }
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
        outFile <<endl;

        outFile<<"Lnum ";
        for ( t = 0; t <TT; t++)
        {
            outFile <<RestArea[j].Lnum[t]<<"," ;
            if(t % 24 == 0 && t !=0){
                outFile << " day "<< int(floor(t/24))<<",";
            }
            //cout << t << " Number of trucks in long rest in rest area " << j << " is " << RestArea[j].Lnum[t] << endl;
        }
        outFile <<endl;
    }
    outFile.close();
    return 0;
}

