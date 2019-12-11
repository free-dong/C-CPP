//
#include "Track.h"
#include "redis.h"
//#include "my_curl.h"
#include "base64.h"

#include<fstream>
#include<opencv2/opencv.hpp>
#include "opencv2/video/video.hpp"
#include<darknet.h>
#include<improcess.h>
#include<toolfuncs.h>


using namespace std;
using namespace cv;

 
// *********************************************
//进行目标检测（eg. yolo）+目标追踪+区域入侵检测
// *********************************************
int TTrack(arguments args)
{
	//读取模型文件
    string cfgfile = args.modelCFG;
    string weightfile = args.modelWeights;
    //参数设置
    float thresh=0.7;
    float nms=0.35;
    int classes=80;
 
    //加载网络模型
    network *net=load_network((char*)cfgfile.c_str(),(char*)weightfile.c_str(),0);
    set_batch_network(net, 1);
    //读取视频
    VideoCapture capture(args.videopath);
	
	// 0. randomly generate colors, only for display
	RNG rng(0xFFFFFFFF);
	Scalar_<int> randColor[CNUM];
	for (int i = 0; i < CNUM; i++)
		rng.fill(randColor[i], RNG::UNIFORM, 0, 256);
    
	Mat frame;
    Mat rgbImg;
 
    vector<string> classNamesVec;
    //标签文件 eg. coco.names 
    ifstream classNamesFile(args.classNamesFile);
 
    if (classNamesFile.is_open()){
        string className = "";
        while (getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }
 
	cout << "Load models done." << endl;
 
	//**********************************************
	//	定义一些参数和变量，用于目标检测和追踪算法
	//检测目标标签
	bool flag_detect = false; //是否检测到目标
	string savePath = "save_images";
	makedir(savePath);
	//定义集合：用来监测是否有新的目标
	set<int> set_target;
	
	String detectLabel = "person";
	int fi = 0;
	// 3. update across frames
	int max_age = 1;
	int min_hits = 3;
	double iouThreshold = 0.3;
	vector<KalmanTracker> trackers;
	
	// tracking id relies on this, so we have to reset it in each seq.
	KalmanTracker::kf_count = 0; 

	// variables used in the for-loop
	vector<Rect_<float>> predictedBoxes;
	vector<vector<double>> iouMatrix;
	vector<int> assignment;
	set<int> unmatchedDetections;
	set<int> unmatchedTrajectories;
	set<int> allItems;
	set<int> matchedItems;
	vector<cv::Point> matchedPairs;
	vector<TrackingBox> frameTrackingResult;
	unsigned int trkNum = 0;
	unsigned int detNum = 0;

	double cycle_time = 0.0;
	double start_time = 0.0;
	double rt_fps = 0.0;
	double rt_fps_t = 0.0;
	bool park_s = 0;
	bool park_t = 0;
	vector<int>old_classNames;
	vector<int>new_classNames;
	vector<Postframe>postframes;
	//end**********************************************
	
	//开始处理每一帧
    bool stop=false;
    while(!stop){
        // if (!capture.read(frame)){
            // printf("fail to read.\n");
            // return 0;
        // }
		//跳帧
		for (int i = 0; i < args.frame_skip; i++)
		{	if (!capture.read(frame))
			{
				// for (auto it = postframes.begin(); it != postframes.end();)
				// {	//网络
					// vector<uchar> data_encode;
					// imencode(".jpg", (*it).frame, data_encode);
					// string str_data(data_encode.begin(), data_encode.end());
					// str_data = base64_encode((char*)str_data.c_str(), str_data.size());
					// curl_post_data(args.httpd_ip, args.httpd_port, (*it).picname, (*it).time, (*it).frametime, str_data);
					// it = postframes.erase(it);
				// }
				break;
			}
		}
		
		// 时间
		start_time = (double)getTickCount();
		fi += 1;
		
		timer my_time;
		my_time.add((int)(fi * (args.frame_skip + 1) / args.videofps));
		string h = to_string(my_time.h);
		string m = to_string(my_time.m);
		string s = to_string(my_time.s);
		if (h.size() == 1)
			h = "0" + h;
		if (m.size() == 1)
			m = "0" + m;
		if (s.size() == 1)
			s = "0" + s;

		int time_s = 3600 * my_time.h + 60 * my_time.m + my_time.s;
		time_t t = time(0);
		char time_c[30];
		strftime(time_c, sizeof(time_c), "%H:%M:%S", localtime(&t));
		string time(time_c);
		string frametime = h + ":" + m + ":" + s;		
		
		//**********************************************
		//		开始进行目标检测
		//转换成RGB
        cvtColor(frame, rgbImg, cv::COLOR_BGR2RGB);
        float* srcImg;
        size_t srcSize=rgbImg.rows*rgbImg.cols*3*sizeof(float);
        srcImg=(float*)malloc(srcSize);
        //将图像转为yolo形式
        imgConvert(rgbImg,srcImg);
        float* resizeImg;
        size_t resizeSize=net->w*net->h*3*sizeof(float);
        resizeImg=(float*)malloc(resizeSize);
        //缩放图像
        imgResize(srcImg,resizeImg,frame.cols,frame.rows,net->w,net->h);
        //网络推理
        network_predict(net,resizeImg);
        int nboxes=0;
		//获取预多个测框
        detection *dets=get_network_boxes(net,rgbImg.cols,rgbImg.rows,thresh,0.5,0,1,&nboxes);
		//进行 非极大值抑制
        if(nms){
            do_nms_sort(dets,nboxes,classes,nms);
        }
 
        vector<cv::Rect>boxes;
        boxes.clear();
        vector<int>classNames;
		//根据阈值选择预测框
        for (int i = 0; i < nboxes; i++){
            bool flag=0;
            int className;
            for(int j=0;j<classes;j++){
                if(dets[i].prob[j]>thresh){
                    if(!flag){
                        flag=1;
                        className=j;
                    }
                }
            }
            if(flag){
                int left = (dets[i].bbox.x - dets[i].bbox.w / 2.)*frame.cols;
                int right = (dets[i].bbox.x + dets[i].bbox.w / 2.)*frame.cols;
                int top = (dets[i].bbox.y - dets[i].bbox.h / 2.)*frame.rows;
                int bot = (dets[i].bbox.y + dets[i].bbox.h / 2.)*frame.rows;
 
                if (left < 0)
                    left = 0;
                if (right > frame.cols - 1)
                    right = frame.cols - 1;
                if (top < 0)
                    top = 0;
                if (bot > frame.rows - 1)
                    bot = frame.rows - 1;
 
                Rect box(left, top, fabs(left - right), fabs(top - bot));
                boxes.push_back(box);
                classNames.push_back(className);
            }
        }
		if (trackers.size())
			new_classNames = classNames;
		else
			new_classNames = old_classNames;		
        free_detections(dets, nboxes);
 
 		vector<TrackingBox> detFrameData;
		detFrameData.clear();
		for (int i = 0; i < boxes.size(); i++)
		{
			TrackingBox tb;
			tb.frame = fi;
			tb.id = -1;
			tb.box = Rect_<float>(Point_<float>(boxes[i].tl().x, boxes[i].tl().y), Point_<float>(boxes[i].tl().x + boxes[i].width, boxes[i].tl().y + boxes[i].height));
			detFrameData.push_back(tb);
		}
		if (trackers.size() == 0) // the first frame met
		{
			// initialize kalman trackers using first detections.
			for (unsigned int i = 0; i < boxes.size(); i++)
			{
				KalmanTracker trk = KalmanTracker(detFrameData[i].box);
				trackers.push_back(trk);
			}
			// output the first frame detections
			for (unsigned int id = 0; id < boxes.size(); id++)
				TrackingBox tb = detFrameData[id];
			continue;
		}
 
 		///////////////////////////////////////
		// 3.1. get predicted locations from existing trackers.
		predictedBoxes.clear();

		for (auto it = trackers.begin(); it != trackers.end();)
		{
			Rect_<float> pBox = (*it).predict();
			if (pBox.x >= 0 && pBox.y >= 0)
			{
				predictedBoxes.push_back(pBox);
				it++;
			}
			else
			{
				it = trackers.erase(it);
			}
		}
		///////////////////////////////////////
		// 3.2. associate detections to tracked object (both represented as bounding boxes)
		trkNum = predictedBoxes.size();
		detNum = detFrameData.size();

		iouMatrix.clear();
		iouMatrix.resize(trkNum, vector<double>(detNum, 0));

		for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
		{
			for (unsigned int j = 0; j < detNum; j++)
			{
				// use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
				iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detFrameData[j].box);
			}
		}

		// solve the assignment problem using hungarian algorithm.
		// the resulting assignment is [track(prediction) : detection], with len=preNum
		HungarianAlgorithm HungAlgo;
		assignment.clear();
		HungAlgo.Solve(iouMatrix, assignment);

		// find matches, unmatched_detections and unmatched_predictions
		unmatchedTrajectories.clear();
		unmatchedDetections.clear();
		allItems.clear();
		matchedItems.clear();

		if (detNum > trkNum) //	there are unmatched detections
		{
			for (unsigned int n = 0; n < detNum; n++)
				allItems.insert(n);

			for (unsigned int i = 0; i < trkNum; ++i)
				matchedItems.insert(assignment[i]);

			set_difference(allItems.begin(), allItems.end(),
				matchedItems.begin(), matchedItems.end(),
				insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
		}
		else if (detNum < trkNum) // there are unmatched trajectory/predictions
		{
			for (unsigned int i = 0; i < trkNum; ++i)
				if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
					unmatchedTrajectories.insert(i);
		}
		else
			;

		// filter out matched with low IOU
		matchedPairs.clear();
		for (unsigned int i = 0; i < trkNum; ++i)
		{
			if (assignment[i] == -1) // pass over invalid values
				continue;
			if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
			{
				unmatchedTrajectories.insert(i);
				unmatchedDetections.insert(assignment[i]);
			}
			else
				matchedPairs.push_back(Point(i, assignment[i]));
		}

		///////////////////////////////////////
		// 3.3. updating trackers

		// update matched trackers with assigned detections.
		// each prediction is corresponding to a tracker
		int detIdx, trkIdx;
		for (unsigned int i = 0; i < matchedPairs.size(); i++)
		{
			trkIdx = matchedPairs[i].x;
			detIdx = matchedPairs[i].y;
			trackers[trkIdx].update(detFrameData[detIdx].box);
			new_classNames[trkIdx] = classNames[detIdx];
		}

		for (auto umd : unmatchedDetections)
		{
			KalmanTracker tracker = KalmanTracker(detFrameData[umd].box);
			trackers.push_back(tracker);
			new_classNames.push_back(classNames[umd]);
		}
		old_classNames = new_classNames;
		frameTrackingResult.clear();
		int count_deadt = 0;
		int count_discB = 0;
		vector<int>discardedBoxes;
		int discnum = 0;
		//frameTrackingResult & labels
		for (auto it = trackers.begin(); it != trackers.end();)
		{
			if (((*it).m_time_since_update < 1) &&
				((*it).m_hit_streak >= min_hits || fi <= min_hits))
			{
				TrackingBox res;
				res.box = (*it).get_state();
				res.id = (*it).m_id + 1;
				res.frame = fi;
				frameTrackingResult.push_back(res);
				it++;
				count_discB++;
			}
			else
			{
				discnum = count_discB + count_deadt;
				discardedBoxes.push_back(discnum);
				it++;
				count_discB++;
			}
			// remove dead tracklet
			if (it != trackers.end() && (*it).m_time_since_update > max_age)
			{
				discnum = count_discB + count_deadt;
				discardedBoxes.push_back(discnum);
				it = trackers.erase(it);
				count_deadt += 1;
			}
		}
		int flag = 0;
		//labels
		for (auto num : discardedBoxes)
		{
			num = num - flag;
			int numofcNs = 0;
			for (auto it = new_classNames.begin(); it != new_classNames.end();)
			{
				if (numofcNs == num)
				{
					it = new_classNames.erase(it);
					flag++;
					break;
				}
				else
				{
					it++;
					numofcNs++;
				}
			}
		}
		
		//*******************************************************************
 		//1.添加矩形框（区域入侵检测）
		// Rect rect_gt(10, 200, 600, 200);//（x, y, a, b）左上坐标（x,y）和矩形的长(b)宽(a)
		// cv::rectangle(frame, rect_gt, Scalar(0, 0, 255),2, LINE_8,0);
		// String rectName = "Regional detection - person";
		// putText(frame, rectName, Point(20, 220), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));
 		
		//2.添加四边形（区域入侵检测）
		Points quad_gt;
		quad_gt.A = getPoint(args.point_A);//左上
		quad_gt.B = getPoint(args.point_B);//右上
		quad_gt.C = getPoint(args.point_C);//左下
		quad_gt.D = getPoint(args.point_D);//右下
		// 绘制四边形
		pltRect(quad_gt, frame);

		vector<String>labels;
		vector<Speed>speedofBox;
		int i = 0;
		//Centers & Speed
		for (auto tb : frameTrackingResult)
		{
			String label = String(classNamesVec[new_classNames[i]]);
			labels.push_back(label);

			//判断是否有入侵，并在入侵区域 显示 目标检测框
			if (dete_intersection(quad_gt, tb.box) && (label == detectLabel))
				//若检测到入侵目标 且 目标为“detectLabel”
			{	
				//显示 目标检测框
				cv::rectangle(frame, tb.box, randColor[tb.id % CNUM], 2, 8, 0);
				//显示 监测目标类别文字
				int baseLine = 0;
				Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
				putText(frame, label, tb.box.tl(), FONT_HERSHEY_SIMPLEX, 0.5, randColor[tb.id % CNUM]);
				putText(frame, to_string(tb.id), Point(tb.box.x, tb.box.y + labelSize.height), 
					FONT_HERSHEY_SIMPLEX, 0.5, randColor[tb.id % CNUM]);
					
				//更新集合：用来监测是否有新的目标
				if (set_target.count(tb.id) == 0)
				{
					flag_detect = true;
					set_target.insert(tb.id);
				}
			}
			i = i + 1;
		}

		char str[10];
		sprintf(str, "%.2f", rt_fps);
		string fpsString("FPS:");
		fpsString += str;
		char fnum[10];
		sprintf(fnum, "%d", fi);
		string framenum("Frame number:");
		framenum += fnum;
		if (!args.display)
			putText(frame, framenum, cv::Point(5, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		else // read image, draw results and show them
		{
			putText(frame, fpsString, Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(107, 211, 170));
			putText(frame, framenum, Point(5, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(107, 211, 170));
			if (frame.empty())
				continue;
			
			cv::namedWindow(args.seq);
			imshow(args.seq, frame);
			if (waitKey(10) == 27)
				break;
		}
		if (flag_detect)
		{	//若检测到入侵目标
			//保存图像
			string saveName = savePath + "/frame-" + fnum + ".jpg";
			imwrite(saveName, frame);
			//重置检测flag
			flag_detect = false;
		}
		
		/* if (time_s % args.pt_interval_i == 0)
		{
			for (auto it = postframes.begin(); it != postframes.end();)
			{
				vector<uchar> data_encode;
				imencode(".jpg", (*it).frame, data_encode);
				string str_data(data_encode.begin(), data_encode.end());
				str_data = base64_encode((char*)str_data.c_str(), str_data.size());
				curl_post_data(args.httpd_ip, args.httpd_port, (*it).picname, (*it).time, (*it).frametime, str_data);
				it = postframes.erase(it);
			}
		} */
		free(srcImg);
		free(resizeImg);
		rt_fps_t = ((double)getTickCount() - start_time);
		rt_fps = getTickFrequency() / rt_fps_t;
		cycle_time = ((double)getTickCount() - start_time);
		double total_time;
		total_time += cycle_time / getTickFrequency();
	}
	//delete r; //redis
	if (args.display)
		cv::destroyWindow(args.seq);
	free_network(net);
	capture.release();
	return 0;
}