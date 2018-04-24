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
#include <string>
#include <iomanip>
#include <algorithm>

#include <sstream>
#include <string>


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

string Trim(string& str)
{
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    str.erase(0,str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
}


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

// 写文件
    //ofstream outFile;
    //outFile.open("data.csv", ios::out); // 打开模式可省略
    //outFile << "name" << ',' << "age" << ',' << "hobby" << endl;
    //outFile << "Mike" << ',' << 18 << ',' << "paiting" << endl;
    //outFile << "Tom" << ',' << 25 << ',' << "football" << endl;
    //outFile << "Jack" << ',' << 21 << ',' << "music" << endl;
    //outFile.close();

    // 读文件

    //删除字符串中空格，制表符tab等无效字符
    ifstream inFile("RestA_info2.txt"); //打开文件流操作
    string line;
    string linkURL;

    if (inFile.is_open())
    {
        while (getline(inFile, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
        {
            linkURL = line.substr(0,line.find(',',0));
            cout <<"原始字符串："<< linkURL << endl; //整行输出
            /* istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
             vector<string> fields; //声明一个字符串向量
             string field;
             while (getline(sin, field, ',')) //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符
             {
                 fields.push_back(field); //将刚刚读取的字符串添加到向量fields中
             }
             string name = Trim(fields[0]); //清除掉向量fields中第一个元素的无效字符，并赋值给变量name
             string age = Trim(fields[1]); //清除掉向量fields中第二个元素的无效字符，并赋值给变量age
             string birthday = Trim(fields[2]); //清除掉向量fields中第三个元素的无效字符，并赋值给变量birthday
             cout <<"处理之后的字符串："<< name << "\t" << age << "\t" << birthday << endl;*/
        }
    }


/*
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

    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
            cout<<array[i][j];
        cout<<endl;
    }


    getchar();
    return 0;

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
*/
return 0;
}