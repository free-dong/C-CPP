/*
 * img_binary.cpp
 *
 * 对NDWI图像二值化（陆地为0，海洋为1）。
 *
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
extern "C"{
	#include"../HDF_STRUCT.h"
	#include"../BasicOperation/Basic_Operation.h"
}

#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;

IplImage *img_binary(IplImage *IMAGE, IplImage *pBW, float NDWIThreshold)
{
	/*
	 *  对NDWI图像二值化（陆地为0，海洋为1）。
	 */
	//NDWI图像二值化
	cvThreshold(IMAGE, pBW, NDWIThreshold, 255, CV_THRESH_BINARY);

    return pBW;
	}
