//C++应用中调用YOLOv3（darknet）进行目标检�?
//参考：https://blog.csdn.net/weixin_33860450/article/details/84890877

#include<iostream>
#include<DetectHuman.h>

#include "toolfuncs.h"
#include "my_crow.h"
//#include <thread> 
//#include <X11/Xlib.h>

#include <unistd.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <stdio.h>
#include <string>
#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"
 
using namespace std;
using namespace cv;

 
int main()
{
	//结构体：参数
	arguments args;
	//读取参数
	loadargs(&args);

	if (args.use_crow == 0)
	{
		// 进行目标入侵检测和追踪
		detectHuman(args);

		// Note: time counted here is of tracking procedure, while the running speed bottleneck is opening and parsing detectionFile.
		//cout << "Total Tracking took: " << total_time << " for " << total_frames << " frames or " << ((double)total_frames / (double)total_time) << " FPS" << endl;
	}
	else
	{
		crow::App<ExampleMiddleware> app;
		CROW_ROUTE(app, "/")
			.methods("POST"_method)
			([&args](const crow::request& req)
				{
					auto x = crow::json::load(req.body);
					if (!x)
						return crow::response(400);

					loadJsonArgs(&args, x);

					// 进行目标入侵检测和追踪
					detectHuman(args);

					{
						string cfgpath = "../models/" + args.seq + "decmodel.cfg";
						string wpath = "../models/" + args.seq + "decmodel.w";
						string npath = "../models/" + args.seq + "decclass.cls";
						remove(cfgpath.c_str());
						remove(wpath.c_str());
						remove(npath.c_str());
					}
					// Note: time counted here is of tracking procedure, while the running speed bottleneck is opening and parsing detectionFile.
					//cout << "Total Tracking took: " << total_time << " for " << total_frames << " frames or " << ((double)total_frames / (double)total_time) << " FPS" << endl;
					_exit(0);
				}	
			);

		app.loglevel(crow::LogLevel::DEBUG);
		app.loglevel(crow::LogLevel::Warning);
		cout << "Httpd port opened." << endl;
		app.port(args.crow_port)
			.multithreaded()
			.run();
	}
	return 0;
}
 
