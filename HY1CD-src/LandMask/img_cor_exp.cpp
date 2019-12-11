/*
 * img_cor_exp.cpp
 *
 * 利用腐蚀膨胀操作对海陆分割边界进行平滑处理。
 *
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"

extern "C"{
//	#include"../BasicOperation/Read_Write.h"
//	#include"../BasicOperation/Write_LOG_FILE.h"
	#include"../HDF_STRUCT.h"
	#include"../BasicOperation/Basic_Operation.h"
}

#include"../CloudDetect/HDF_write_to_IPLImage.h"

#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;
using namespace cv;

struct HDF_DATA_INT img_cor_exp(IplImage *pBW, int height, int width)
{
	/*  腐蚀膨胀:
	 *  利用腐蚀膨胀操作对海陆分割边界进行平滑处理。
	 */

//	IplConvKernel *element = cvCreateStructuringElementEx(19,19,9,9,CV_SHAPE_ELLIPSE,NULL);
//	IplImage *dout = cvCreateImage(cvGetSize(pBW), IPL_DEPTH_8U, 1);
//	cvErode( pBW, dout, element,1 );
//	cvDilate(dout,pBW, element,1);

	// NDWI图像二值化
//	cvThreshold(pBW, pBW, 0.99, 255, CV_THRESH_BINARY);
//
//	cvSetImageROI(pBW, cvRect(0, 0, height ,width));

	//将pBW转换为结构体
	struct HDF_DATA_INT Truth_Group = IPLImage_write_to_HDF(pBW, height, width);
//	cvResetImageROI(pBW); 	//源图像用完后，清空ROI

//    cvReleaseImage(&dout);
//    cvReleaseStructuringElement(&element);
    return Truth_Group;
}
