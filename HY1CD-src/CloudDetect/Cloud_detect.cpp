/*
 * Cloud_detect.cpp
 *
 *  Created on: 2018年3月15日
 *      Author: zhangyonghui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"HDF_write_to_IPLImage.h"
#include"DATA_MASK.h"
#include"RGB_to_HSV.h"
#include"HSV_to_RGB.h"
#include"CLOUD_and_SHADOW.h"

extern "C"{
	#include"../BasicOperation/Basic_Operation.h"
	#include"../HDF_STRUCT.h"
}
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array
using namespace std;
using namespace cv;

//主函数;
struct HDF_DATA_INT CloudDetect(struct HDF_GEODATA DataSet, float cloud_threshold, float shadow_threshold, int median_filter_window)
{
//*********************************************云检测开始*****************************************//

	//获取数据集维度，并创建IplImage类型的图像数据img_rgb;
	int height = DataSet.ROWS;
	int width = DataSet.COLUMNS;

	IplImage* img_rgb = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;

	//将各band中的数据写入img_rgb中
	img_rgb = HDF_write_to_IPLImage(img_rgb, height, width, DataSet);

	//save image
	//cvSaveImage("./InputH5.jpg", img_rgb);

//	//添加图例------------------
//	Mat img1, img2, img, mask;
//	//IplImage 转 Mat
//	img1=cvarrToMat(img_rgb);
////	img1 = imread("land_result_1.jpg");//image
//	img2 = imread("ColorBar.jpg");//图例
//	//定义一个Mat类型并设置ROI区域
//	Mat imageROI = img1(Rect(img1.cols - img2.cols, img1.rows - img2.rows, img2.cols, img2.rows));
//	//加载掩码
//	mask = imread("ColorBar.jpg");
//	//将掩码复制到ROI
//	img2.copyTo(imageROI, mask);
////	imwrite("图例InputH5.jpg",img1);
//	//Mat 转 IplImage
//	//设置中间变量
//	IplImage img1_temp = (IplImage)img1;
//	IplImage* img_rgb_result = &img1_temp;
//	cvSaveImage("./ColorBar-InputH5.jpg", img_rgb_result);

//**********************************************HSV转换*****************************************//

    //HSV转换:RGB_to_HSV
    IplImage* img_hsv = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    img_hsv = RGB_to_HSV(img_hsv, img_rgb, height, width);


//******************************************HSV逆变换*******************************************//

    //HSV逆变换:HSV_to_RGB
    IplImage* new_RGB = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;
    new_RGB = HSV_to_RGB(new_RGB, img_hsv, img_rgb, height, width);


//******************************************云提取**********************************************//

    IplImage* B_cloud = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    B_cloud = CLOUD(new_RGB, B_cloud, height, width, cloud_threshold, median_filter_window);


//******************************************阴影提取********************************************//

    IplImage* R_shadow = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    R_shadow = SHADOW(new_RGB, R_shadow, height, width, shadow_threshold);


//******************************************数据掩膜*******************************************//

    //先阴影膨胀,然后云和阴影的二值图合并为CS_truthValueGraph
//    IplImage* CS_truthValueGraph = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
//    CS_truthValueGraph = CLOUD_SHADOW_MERGE(CS_truthValueGraph, R_shadow, B_cloud, height, width);
    B_cloud = CLOUD_SHADOW_MERGE(R_shadow, B_cloud, height, width);
    //数据转换，将CS_truthValueGraph从IplImage转到结构体TrueValueGraph
    struct HDF_DATA_INT TruthValueGraph = IPLImage_write_to_HDF(B_cloud, height, width);

    //数据掩膜
//    DATA_MASK(DataSet, TruthValueGraph);

    //save image
//	IplImage* cloud_result = cvCreateImage(cvGetSize(img_rgb), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;
//	IplImage* cloud_result = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;
//	HDF_write_to_IPLImage(cloud_result, height, width, DataSet);
//	cvSaveImage("./cloud_result.jpg", cloud_result);

//**************************************云检测结束*********************************************//
    // free
//    cvReleaseImage(&CS_truthValueGraph);
    cvReleaseImage(&R_shadow);
//    cvReleaseImage(&img_rgb);
    cvReleaseImage(&new_RGB);
    cvReleaseImage(&B_cloud);

	return TruthValueGraph;
}

