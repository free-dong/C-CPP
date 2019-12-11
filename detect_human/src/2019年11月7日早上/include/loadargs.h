//
#ifndef _LOADARGS_H_
#define _LOADARGS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
typedef struct arguments
{
	string seq;							//输出文件夹名
	string videopath;					//视频路径
	
	string modelCFG;					//模型cfg路径
	string modelWeights;				//模型权重路径
	string classNamesFile;				//标签文件
		
	string httpd_ip;					//httpd_ip
	string httpd_port;					//httpd_port
	string redis_host;					//redis_host
	int redis_port;						//redis_port
	int threshold;						//检测范围大小	
	int thre_flag;						//入侵区域范围
	bool display;						//显示视频
	int frame_skip;						//跳帧数
	string pt_interval;					//post 间隔时间
	int pt_interval_i;					//post 间隔秒数
	int videofps;						//视频帧率
	//四边形：：左上，右上，左下，右下
	string point_A;
	string point_B;
	string point_C;
	string point_D;
	
 /* mode = 1 只将图片保存到本地
	mode = 2 将图片信息和编码二进制post到web地址
	mode = 3 将图片信息保存到web地址，将图片编码二进制保存到redis 
*/
	int mode;							
	bool use_crow;						//使用crow监听httpd请求
	int crow_port;						//crow端口
	int expire_time;
	//bool showline;						//画出围栏线段
	//bool showarea;						//画出检测范围
	//bool showrect;						//画出人体的框
	arguments()
	{
		pt_interval_i = 10;
		seq = "video";
		mode = 2;
	}
}arguments;

void loadargs(arguments* args);

#endif // !_LOADARGS_H_

