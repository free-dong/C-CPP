/*
 * HDF_write_to_IPLImage.cpp
 *
 *  Created on: 2018年4月2日
 *      Author: zhangyonghui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include "HDF_write_to_IPLImage.h"
extern "C"{
	#include"../HDF_STRUCT.h"
}
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array
using namespace std;
using namespace cv;

/*
 * 定义两个函数HDF_write_to_IPLImage()和IPLImage_write_to_HDF()，
 * 分别实现结构体的数据集和IplImage类型的数据集的相互转换，
 * 便于之后的各种openCV操作。
 *
 * 使用宏CV_IMAGE_ELEM()
 */

IplImage *HDF_write_to_IPLImage(IplImage* img_rgb, int height, int width, struct HDF_GEODATA dataset)
{
	//初始化数组指针point，用于从各波段的数据集向img_rgb中各通道中传递数值，便于下一步的HSV转换;
	float* point_BLUE = (float* )malloc(sizeof(float) * height * width);
	float* point_GREEN = (float* )malloc(sizeof(float) * height * width);
	float* point_RED = (float* )malloc(sizeof(float) * height * width);

//	float max = 1022.0;
	//get max number
	float max_blue = dataset.BLUE[0];
	float max_green = dataset.GREEN[0];
	float max_red = dataset.RED[0];
	for(int i = 0; i < height * width; i++)
	{
		if(dataset.BLUE[i] >= max_blue)
		{
			max_blue = dataset.BLUE[i];
		}
		if(dataset.GREEN[i] >= max_green)
		{
			max_green = dataset.GREEN[i];
		}
		if(dataset.RED[i] >= max_red)
		{
			max_red = dataset.RED[i];
		}
	}

    for(int i = 0; i < height * width; i++)
    {
    	point_BLUE[i] = (float)(dataset.BLUE[i] / max_blue * 255);
    	point_GREEN[i] = (float)(dataset.GREEN[i] / max_green * 255);
    	point_RED[i] = (float)(dataset.RED[i] / max_red * 255);//归一化到0-255,并用上面初始化的指针指向归一化后的Band的数据集;
    }

    //将结构体中的数据写入到IplImage中
	for(int i = 0; i < height; i++)
	{
		for(int j=0; j< width; j++)
		{
			CV_IMAGE_ELEM(img_rgb, uchar, i, 3 * j + 2) = point_RED[i * width + j];//写到第三个通道，红;
			CV_IMAGE_ELEM(img_rgb, uchar, i, 3 * j + 1) = point_GREEN[i * width + j];//写到第二个通道，绿;
			CV_IMAGE_ELEM(img_rgb, uchar, i, 3 * j) = point_BLUE[i * width + j];//写到第一个通道，蓝;

		}
	}

    xfree(point_BLUE);
    xfree(point_GREEN);
    xfree(point_RED);//释放内存;

	return img_rgb;
}

struct HDF_DATA_INT IPLImage_write_to_HDF(IplImage* ipl_data, int height, int width)
{
	//初始化结构体Truth_Group
	struct HDF_DATA_INT Truth_Group;

	//定义指针point,用于传值
	int* point = (int* )malloc(sizeof(int) * height * width);

    //用指针point指向ipl_data
	for(int i = 0; i < height; i++)
	{
		for(int j=0; j< width; j++)
		{
			point[i * width + j] = CV_IMAGE_ELEM(ipl_data, uchar, i, j);//写到第三个通道，红
		}
	}

	//将指针point指向的值以及其他信息写入Truth_Group，
	Truth_Group.DATASET = point;
	Truth_Group.RANK = 2;
	Truth_Group.Flag = 0;
	Truth_Group.ROWS = height;
	Truth_Group.COLUMNS = width;

	return Truth_Group;
}
