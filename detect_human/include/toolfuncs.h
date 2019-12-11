#ifndef __TOOLFUNCS_H__
#define __TOOLFUNCS_H__
#include <unistd.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include "opencv2/core/core.hpp"
// #include <Track.h>
#include <math.h>
#include <set>
#include <string.h>
#include <fstream>
#include <iterator>
#include <sys/stat.h>
#include <vector>
// #include "KalmanTracker.h"
// #include "loadargs.h"
// #include "b64.h"
// #include "vigenere.h"
// #include "crow/json.h"

using namespace std;
using namespace cv;

double GetIOU(Rect_<int> bb_test, Rect_<int> bb_gt);


#endif