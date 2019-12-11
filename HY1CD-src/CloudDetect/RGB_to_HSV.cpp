/*
 * RGB_to_HSV.cpp
 *
 *  Created on: 2018年4月24日
 *      Author: zhangyonghui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"RGB_to_HSV.h"

using namespace std;
using namespace cv;

/*
 * 定义函数RGB_to_HSV()，实现图像从RGB空间到HSV空间的转换
 * 利用openCV库函数：cvCvtColor()
 */

IplImage *RGB_to_HSV(IplImage* img_hsv, IplImage *img_rgb, int height, int width)
{
//	IplImage* img_hsv = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    cvCvtColor(img_rgb, img_hsv, CV_BGR2HSV);
	return img_hsv;
}
