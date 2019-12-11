/*
 * img_filt.cpp
 * 1.图像高斯滤波
 * 2.对NDWI图像进行图像滚动递归引导滤波操作，消除图像中的噪声点
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
#define PATH_HDF5 "/home/zhongyong/eclipse-workspace/data/H1C_OPER_CZI_L1B_20190101T010500_00103_10.h5"
#define PATH_DataSet "/Geophysical Data"
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;

IplImage *img_filt(int nYSize, int nXSize, IplImage *pNDWI, IplImage *IMAGE, int GuassFilterSize)
{
    /*  1.图像高斯滤波
     *  2.对NDWI图像进行图像滚动递归引导滤波操作，消除图像中的噪声点
     */

	//  图像高斯滤波
//	IplImage *GAUSS = cvCreateImage(cvGetSize(pNDWI), IPL_DEPTH_32F, 1);
	cvSmooth(pNDWI, IMAGE, CV_GAUSSIAN, GuassFilterSize);

//	IMAGE=GAUSS;

    return IMAGE;
}
