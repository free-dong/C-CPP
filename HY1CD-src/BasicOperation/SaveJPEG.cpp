/*
 * SaveJPEG.cpp
 *
 *  Created on: Jul 12, 2018
 *      Author: zhongyong
 */
#include<opencv2/opencv.hpp>
#include"../HDF_STRUCT.h"
using namespace cv;

//flag == 1 is * 255
void DataSaveToJPEG(float* Data, char* OutputJPEGPath, int Row, int Column, int flag){

	IplImage* grayImage = cvCreateImage(cvSize(Column , Row), IPL_DEPTH_8U, 1);
	int coefficient = 1;
	if(flag == 1){
		coefficient = 255;
	}

	//将数组中的数据写入到IplImage中
	for(int i = 0; i < Row; i++)
	{
		for(int j=0; j< Column; j++)
		{
			if (Data[i * Column + j] != 0){
	//			CV_IMAGE_ELEM(grayImage, uchar, i, j) = Data[i * Column + j] * coefficient ;
				CV_IMAGE_ELEM(grayImage, uchar, i, j) = (float)(1 / (1 + exp(-Data[i * Column + j])) * 255);
			}
			else{
				CV_IMAGE_ELEM(grayImage, uchar, i, j) = Data[i * Column + j];
			}

		}
	}

	cvSaveImage(OutputJPEGPath, grayImage);
	cvReleaseImage(&grayImage);
}

void NDVISaveToJPEG(float* Data, char* OutputJPEGPath, int Row, int Column, int flag){

	IplImage* grayImage = cvCreateImage(cvSize(Column , Row), IPL_DEPTH_8U, 1);
	int coefficient = 1;
	if(flag == 1){
		coefficient = 255;
	}

	//将数组中的数据写入到IplImage中
	for(int i = 0; i < Row; i++)
	{
		for(int j=0; j< Column; j++)
		{
			if (Data[i * Column + j] != 0){
				CV_IMAGE_ELEM(grayImage, uchar, i, j) = Data[i * Column + j] * coefficient ;
//				CV_IMAGE_ELEM(grayImage, uchar, i, j) = (float)(1 / (1 + exp(-Data[i * Column + j])) * 255);
			}
			else{
				CV_IMAGE_ELEM(grayImage, uchar, i, j) = Data[i * Column + j];
			}

		}
	}

	cvSaveImage(OutputJPEGPath, grayImage);
	cvReleaseImage(&grayImage);
}

void HDFDataSaveToJPEG(struct HDF_DATA Data, char* OutputJPEGPath, int flag){

	int Row = Data.ROWS;
	int Column= Data.COLUMNS;
	int coefficient = 1;
	if(flag == 1){
		coefficient = 255;
	}

	IplImage* grayImage = cvCreateImage(cvSize(Column , Row), IPL_DEPTH_8U, 1);

	//将数组中的数据写入到IplImage中
	for(int i = 0; i < Row; i++)
	{
		for(int j=0; j< Column; j++)
		{
			CV_IMAGE_ELEM(grayImage, uchar, i, j) = Data.DATASET[i * Column + j] * coefficient ;
		}
	}

	cvSaveImage(OutputJPEGPath, grayImage);
	cvReleaseImage(&grayImage);


}
