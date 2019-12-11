//C++应用中调用YOLOv3（darknet）进行目标检�?
//参考：https://blog.csdn.net/weixin_33860450/article/details/84890877

#include<iostream>

#include<Track.h>

 
using namespace std;
using namespace cv;

 
int main()
{
	//结构体：参数
	arguments args;
	//读取参数
	loadargs(&args);

    // 进行目标入侵检测和追踪
	TTrack(args);
	
	return 0;
}
 
