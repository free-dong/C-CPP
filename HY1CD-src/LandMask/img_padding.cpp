/*
 * img_padding.cpp
 *
 * 填充陆地和海洋区域内的空洞。
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

IplImage *img_padding(IplImage *pBW)
{
	/*
	 *  填充陆地和海洋区域内的空洞。
	 */

	CvSeq *pContour = NULL;
	CvSeq *pConInner = NULL;
	CvMemStorage *pStorage = cvCreateMemStorage(0);

	// 填充海洋中的岛屿
	cvFindContours(pBW, pStorage, &pContour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (; pContour != 0; pContour = pContour->h_next)
	{
		cvDrawContours(pBW, pContour, CvScalar(1), CvScalar(1), -1, CV_FILLED);
	}

	// 填充陆地中的水塘
	cvFindContours(pBW, pStorage, &pContour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int nMaxContour = 0;
	for (; pContour != 0; pContour = pContour->h_next)
	{
		double length = cvArcLength(pContour);
		if (length > nMaxContour) nMaxContour = length;
	}

	cvFindContours(pBW, pStorage, &pContour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (; pContour != 0; pContour = pContour->h_next)
	{
		double length = cvArcLength(pContour);
		if (length < nMaxContour)
		{
			cvDrawContours(pBW, pContour, CvScalar(0), CvScalar(0), -1, CV_FILLED);
		}
	}

	// 填充图像二值化
	cvThreshold(pBW, pBW, 0.99, 255, CV_THRESH_BINARY);

	cvSetImageROI(pBW, cvRect(0, 0, 10000 ,10000));
	cvResetImageROI(pBW); 	//源图像用完后，清空ROI

	//free
	cvReleaseMemStorage(& pStorage);

    return pBW;
	}
