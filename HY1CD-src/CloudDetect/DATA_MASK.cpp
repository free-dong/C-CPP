/*
 * DATA_MASK.cpp
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"DATA_MASK.h"
extern "C"{
	#include"../HDF_STRUCT.h"
}

using namespace std;
using namespace cv;

/**
 * 将得到的阴影二值图膨胀2个像元，与云二值图合并。
 *
 * 利用合并的二值图对L1B数据进行数据掩膜。
 */

IplImage *CLOUD_SHADOW_MERGE(IplImage *R_shadow, IplImage *B_cloud, int height, int width)
{
    //膨胀2个单元;
    IplConvKernel *element = cvCreateStructuringElementEx(19, 19, 9, 9, CV_SHAPE_ELLIPSE, NULL);
    IplImage *R_shadow_expand = cvCreateImage(cvGetSize(R_shadow), IPL_DEPTH_8U, 1);
    cvDilate( R_shadow, R_shadow_expand, element, 0);

    for(int i = 0; i < height; i++)
    {
    	for(int j = 0; j < width; j++)
    	{
    		if((int)CV_IMAGE_ELEM(B_cloud, uchar, i, j) != (int)CV_IMAGE_ELEM(R_shadow_expand, uchar, i, j))
    		{
    			//两个二值图中，找到对应位置的值不同的像素点，然后将云二值图B_cloud中对应位置的值设为255，返回B_cloud;
    			CV_IMAGE_ELEM(B_cloud, uchar, i, j) = 255;
    		}
    	}
    }

    //将合并后的二值图B_cloud中的0和255分别置换成1和0
    for(int i = 0; i < height; i++)
    {
    	for(int j = 0; j < width; j++)
    	{
    		if((int)CV_IMAGE_ELEM(B_cloud, uchar, i, j) == 255)
    		{
    			CV_IMAGE_ELEM(B_cloud, uchar, i, j) = 1;
    		}
    		else
    		{
    			CV_IMAGE_ELEM(B_cloud, uchar, i, j) = 0;
    		}
    	}
    }
    //free
    cvReleaseImage(&R_shadow_expand);
    cvReleaseStructuringElement(&element);
	return B_cloud;
}

struct HDF_GEODATA DATA_MASK(struct HDF_GEODATA DataSet, struct HDF_DATA_INT TruthValueGraph)
{
	int height = TruthValueGraph.ROWS;
	int width = TruthValueGraph.COLUMNS;

    //用真值图TrueValueGraph对原始数据DataSet的4各波段金喜各掩膜
	for(int i = 0; i < height * width; i++)
	{
			DataSet.BLUE[i] *= TruthValueGraph.DATASET[i];
			DataSet.GREEN[i] *= TruthValueGraph.DATASET[i];
			DataSet.RED[i] *= TruthValueGraph.DATASET[i];
			DataSet.NIR[i] *= TruthValueGraph.DATASET[i];
	}

	return DataSet;
}
