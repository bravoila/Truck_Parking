#ifndef WANGHEADER_FILE
#define WANGHEADER_FILE

// the entire header file content is here.
// This header file is used to define global variables
//blablabla 
#include <vector>
std::vector <double> WangCurrent, QClearance, ArrivalRate, CurrentRate;  //WangCurrent is used to save the lane efficiency-equivalent for the current phase used in Signal-b paper
												//QClearance is to record the queue clearance time for a lane in the time forward (e.g. immediate NEXT cycle
												//ArrivalRate is for the per lane average arrival rate for the duration of control
												//CurrentRate is for the rate around the (expected queue clearance time point)
double  WangCycle; //approximate signal cycle time to use to estimate lane queues
double simulationPeriod;
#endif

//Note: the lane index starts from zero that increases clockwise. The lane zero is the lane for southbound traffic (e.g. from north) to turn right