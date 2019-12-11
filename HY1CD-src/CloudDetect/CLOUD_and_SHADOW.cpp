/*
 * CLOUD_and_SHAOW.cpp
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */


#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"CLOUD_and_SHADOW.h"
#include"HDF_write_to_IPLImage.h"
extern "C"{
	#include"../BasicOperation/Basic_Operation.h"
	#include"../HDF_STRUCT.h"
}

using namespace std;
using namespace cv;

IplImage *CLOUD(IplImage *new_RGB, IplImage *B_cloud, int height, int width, float cloud_threshold, int median_filter_window)
{
    /*利用转换后的B通道，通过设置阈值提取云信息，得到二值图，云为1,其他为0*/

    //分离出B通道;
    cvSplit(new_RGB, B_cloud, 0, 0, 0);

    //去噪;
    //中值滤波;
    cvSmooth(B_cloud, B_cloud, CV_MEDIAN, median_filter_window);

    //二值化0, 255;
    cvThreshold(B_cloud, B_cloud, cloud_threshold, 255, CV_THRESH_BINARY);

	return B_cloud;
}

IplImage *SHADOW(IplImage *new_RGB, IplImage *R_shadow, int height, int width, float shadow_threshold)
{
    cvSplit(new_RGB, 0, 0, R_shadow, 0);

    cvThreshold(R_shadow, R_shadow, shadow_threshold, 255, CV_THRESH_BINARY);

	return R_shadow;
}
