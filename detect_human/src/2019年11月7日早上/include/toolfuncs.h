#ifndef __TOOLFUNCS_H__
#define __TOOLFUNCS_H__
#include <unistd.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include "opencv2/core/core.hpp"
#include <Track.h>
#include <math.h>
#include <set>
#include <string.h>
#include <fstream>
#include <iterator>
#include <sys/stat.h>
#include "KalmanTracker.h"
#include "loadargs.h"
#include "b64.h"
#include "vigenere.h"
//#include "crow/json.h"

typedef struct Points
{
	cv::Point A;
	cv::Point B;
	cv::Point C;
	cv::Point D;
	bool flag;
	Points()
	{
		flag = 0;
	}
}Points;

extern set<int>flags;
bool across(int id);
double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt);
float get_color(int c, int x, int max);
Points inside(Point C, Point A, Point B, int d);

double distance(Point center, Point line_a, Point line_b, int thre);
void makedir(string dirpath);

int encrypt(string path, string outputpath);
int decrypt(string path, string outputpath);
//int loadJsonArgs(arguments* args, crow::json::rvalue x);
void pltRect(Points quad_gt, Mat frame);

//判断两线段相交
bool intersection(const vector<Point> & line1, const vector<Point> &line2);
//判断线段与矩形相交
bool rect_line_intersection(const vector<Point> & line,const Rect & targetRect);
//判断四边形与矩形相交
bool dete_intersection(Points quad_gt, const Rect & targetRect);

Point getPoint(string args_Point);
#endif