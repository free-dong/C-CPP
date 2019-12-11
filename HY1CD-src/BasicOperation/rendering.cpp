/*
 * rendering.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: zhangyonghui
 */

#include "opencv2/opencv.hpp"
#include<iostream>
#include"../hy1_czi_l2_pro.hpp"
#include"rendering.hpp"
#include<cmath>
#include<math.h>
#include<algorithm>
#include<cstring>

extern "C"{
	#include"../HDF_STRUCT.h"
}

using namespace std;
using namespace cv;

/* define NaN */
typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();

#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array

/*
 * 抽样和排序(常用)
 */
float* resort(float* data, int height, int width)
{
	int numNan = 0;  //统计无效值nan的个数

	/* 抽样
	 * 每10个点抽一次样，值为第10个数的值
	 * 最后不足10个点全置为0
	 */
	int rows = floor(height / 10);
	int cols = floor(width / 10);
	float* dataSort = (float* )malloc(sizeof(float) *rows * cols);
	for(int i = 0; i < rows * cols; i++){

		if(i * 10 + 9 <= height * width){
			dataSort[i] = data[i * 10 + 9];

			if(isnan(dataSort[i]) == 1){
				dataSort[i] = -999.0;
				numNan++;
			}
		}
		else{
			break;
		}
	}

	float array[2] = {0.0, 0.0}; //存储结果

	sort(dataSort, dataSort+rows*cols);  //从小到大排序

	//索引取值
    int a = floor(numNan + (rows*cols - numNan)*0.1);
    int b = floor(numNan + (rows*cols - numNan)*0.9);
	array[0] = (float)dataSort[a];
	array[1] = (float)dataSort[b];

	xfree(dataSort);
	return array;
}

/*
 * 抽样和排序
 * NDVI: 只对[domain_left, domain_right]的值进行了排序
 */
float* resort_NDVI(float* data, int height, int width, float domain_left, float domain_right)
{
	int numNan = 0;  //统计个数

	/* 抽样
	 * 每10个点抽一次样，值为第10个数的值
	 * 最后不足10个点全置为0
	 */
	int rows = floor(height / 10);
	int cols = floor(width / 10);
	float* dataSort = (float* )malloc(sizeof(float) *rows * cols);
	for(int i = 0; i < rows * cols; i++){

		if(i * 10 + 9 <= height * width){
			dataSort[i] = data[i * 10 + 9];

			if(isnan(dataSort[i]) == 1 || dataSort[i] < domain_left || dataSort[i] > domain_right){
				dataSort[i] = -999.0;
				numNan++;
			}
		}
		else{
			break;
		}
	}

	float array[2] = {0.0, 0.0}; //存储结果

	sort(dataSort, dataSort+rows*cols);  //从小到大排序

	//索引取值
    int a = floor(numNan + (rows*cols - numNan)*0.1);
    int b = floor(numNan + (rows*cols - numNan)*0.9);
	array[0] = (float)dataSort[a];
	array[1] = (float)dataSort[b];

	xfree(dataSort);
	return array;
}

/*
 * 抽样和排序
 * NDVI: 只对[-0.35, 0)的值进行了排序
 */
//float* resort_NDVI_2(float* data, int height, int width)
//{
//	int numNan = 0;  //统计个数
//
//	/* 抽样
//	 * 每10个点抽一次样，值为第10个数的值
//	 * 最后不足10个点全置为0
//	 */
//	int rows = floor(height / 10);
//	int cols = floor(width / 10);
//	float* dataSort = (float* )malloc(sizeof(float) *rows * cols);
//	for(int i = 0; i < rows * cols; i++){
//
//		if(i * 10 + 9 <= height * width){
//			dataSort[i] = data[i * 10 + 9];
//
//			if(isnan(dataSort[i]) == 1 || dataSort[i] >= 0 || dataSort[i] < -0.35){
//				dataSort[i] = -999.0;
//				numNan++;
//			}
//		}
//		else{
//			break;
//		}
//	}
//
//	float array[2] = {0.0, 0.0}; //存储结果
//
//	sort(dataSort, dataSort+rows*cols);  //从小到大排序
//
//	//索引取值
//    int a = floor(numNan + (rows*cols - numNan)*0.1);
//    int b = floor(numNan + (rows*cols - numNan)*0.9);
//	array[0] = (float)dataSort[a];
//	array[1] = (float)dataSort[b];
//
//	xfree(dataSort);
//	return array;
//}

/*
 * 分段线性拉伸算法
 *
 * 设置三段函数
 * 定义域分别是[x_start, x1), [x1, x2], (x2, x_end]
 * 值域分别是[0, 10), [10, 245],(245, 255]
 */
float* dividedLinearStrength(float* data, int height, int width, float x1, float x2, float y1, float y2, float x_start, float x_end)
{
	// 防止数据错误导致程序出错
	if(x1 < x_start){
		x1 = x_start + 0.00001;
	}
	if(x2 > x_end){
		x2 = x_end - 0.00001;
	}

	// 计算三段线性函数的斜率和截距
	float k1 =  (float)(y1 / (x1 - x_start));
	float b1 = (float)(y1 - k1 * x1);

	float k2 = (float)((y2 - y1) / (x2 - x1));
	float b2 =(float)(y1 - k2 * x1);

	float k3 = (float)((255.0 - y2) / (x_end - x2));
	float b3 =(float)(y2 - k3 * x2);

	float* data_strength = (float* )malloc(sizeof(float) *height * width);

	// 分段线性拉伸
	for(int i = 0; i < height * width; i++){

		if(isnan(data[i]) == 1) {   // Nan's value

			data_strength[i] = data[i];
		}
		else{
			if(data[i] >= x_start && data[i] <= x_end)
			{
				if(data[i] < x1){
					data_strength[i] = data[i] * k1 + b1;
				}
				else if(data[i] > x2){
					data_strength[i] = data[i] * k3 + b3;
				}
				else{
					data_strength[i] = data[i] * k2 + b2;
				}
			}
			else{
				data_strength[i] = NAN_f;
			}
		}
	}

	return data_strength;
}

//float* dividedLinearStrength_NDVI(float* data, int height, int width, float x1, float x2, float y1, float y2, float x_start, float x_end)
//{
//	/*
//	 * NDVI的值的范围是[x_start, x_end]
//	 * 只对[0, 1]的值进行拉伸
//	 * [-1, 0)的值和Nan的值不变
//	 * 其他的值设为Nan
//	 */
//	if(x1 < 0){
//		x1 = 0 + 0.00001;
//	}
//	if(x2 > x_end){
//		x2 = x_end - 0.000001;
//	}
//	float k1 =  (float)(y1 / (x1 - 0));
//	float b1 = (float)(y1 - k1 * x1);
//
//	float k2 = (float)((y2 - y1) / (x2 - x1));
//	float b2 =(float)(y1 - k2 * x1);
//
//	float k3 = (float)((255.0 - y2) / (x_end - x2));
//	float b3 =(float)(y2 - k3 * x2);
//
//	float* data_ndvi = (float* )malloc(sizeof(float) *height * width);
//
//	for(int i = 0; i < height * width; i++){
//
//		if(isnan(data[i]) == 1) {
//			// Nan's value
//			data_ndvi[i] = data[i];
//		}
//		else if(data[i] >= x_start && data[i] < 0){
//			// [x_start, 0)的值
//			data_ndvi[i] = data[i];
//		}
//		else if(data[i] >= 0 && data[i] <= x_end){
//			// [0, x_end]的值
//			if(data[i] < x1){
//				data_ndvi[i] = data[i] * k1 + b1;
//			}
//			else if(data[i] > x2){
//				data_ndvi[i] = data[i] * k3 + b3;
//			}
//			else{
//				data_ndvi[i] = data[i] * k2 + b2;
//			}
//		}
//		else{
//			// 其他的值设为Nan
//			data_ndvi[i] = NAN_f;
//		}
//	}
//
//	return data_ndvi;
//}
//float* dividedLinearStrength_NDVI_2(float* data, int height, int width){
//
//	/*
//	 * 只对[-0.35, 0)的值进行拉伸
//	 */
//	float k = (float)(255.0 / 0.35);
//	float b = (float)(255.0);
//	float* data_ndvi = (float* )malloc(sizeof(float) *height * width);
//	for(int i = 0; i < height * width; i++){
//
//		if(isnan(data[i]) == 1) {
//			// Nan's value
//			data_ndvi[i] = data[i];
//		}
//		else if(data[i] >= -0.35 && data[i] < 0){
//			// [-0.35, 0)的值
//			data_ndvi[i] = data[i] * k + b;
//		}
//		else{
//			// 其他的值设为Nan
//			data_ndvi[i] = NAN_f;
//		}
//	}
//
//	return data_ndvi;
//}

/*
 * Rendering:
 *           悬浮物泥沙浓度 SSC
 *           叶绿素浓度 CHL
 *           海水透明度 SDD
 */
void Rendering(struct HDF_DATA gray_image, const char* path, const char* flag, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData, CGEN_ExecutionHandler *_poExHand)
{
	const char* SSC_name = "SSC";
	const char* CHL_name = "CHL";
	const char* SDD_name = "SDD";

	/* Get image size */
	int height = gray_image.ROWS;
	int width = gray_image.COLUMNS;

	/* Initialization RGB image of rendering */
	IplImage* rendering_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);  //为了渲染

	/*
	 * SSC
	 */
	if(strcmp(flag, SSC_name) == 0){

		float* array = resort(gray_image.DATASET, height, width);
		float* rendata =  dividedLinearStrength(gray_image.DATASET, height, width, array[0], array[1], 10, 245, 0.1, 1000);

		/* Pixel by pixel rendering */
		for(int i = 0; i < height; i++){
			for(int j = 0; j< width; j++){

				if(CloudDetectData.DATASET[i * width + j] == 1){  // Cloud
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;                                           //R
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;                                           //G
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;                                               //B
				}
				else if(LandMaskData.DATASET[i * width + j] == 1){  // Land

					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 128;
				}
				else if(isnan(rendata[i * width + j]) == 1){  // Nan's
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] >= 0 && rendata[i * width + j] <= 63.75)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 4 * rendata[i * width + j];
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
				}
				else if(rendata[i * width + j] > 63.75 && rendata[i * width + j] <= 127.5)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) =255 - (int)(4 * (rendata[i * width + j] - 63.75));
				}
				else if(rendata[i * width + j] > 127.5 && rendata[i * width + j] <= 191.25)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255 - (int)(4 * (rendata[i * width + j] - 127.5));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] > 191.25 && rendata[i * width + j] <= 255)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255 - (int)(4 * (rendata[i * width + j] - 191.25));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else
				{
					_poExHand->sendInfoMessage("The SSC value is not between 0 and 255!", CLOG_NO_PRODUCT);
				}
			}
		}
		xfree(rendata);
	}
	/*
	 * CHL
	 */
	else if(strcmp(flag, CHL_name) == 0){

		float* array = resort(gray_image.DATASET, height, width);
		float* rendata =  dividedLinearStrength(gray_image.DATASET, height, width, array[0], array[1], 10, 245, 0.01, 30);

		/* Pixel by pixel rendering */
		for(int i = 0; i < height; i++){
			for(int j = 0; j< width; j++){

				if(CloudDetectData.DATASET[i * width + j] == 1){
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;   //R
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;   //G
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;       //B
				}
				else if(LandMaskData.DATASET[i * width + j] == 1){
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 128;
				}
				else if(isnan(rendata[i * width + j]) == 1){
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] >= 0 && rendata[i * width + j] <= 85)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 3 * rendata[i * width + j];
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] > 85 && rendata[i * width + j] <= 170)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255 - (int)(3 * (rendata[i * width + j] - 85));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] > 170 && rendata[i * width + j] <= 255)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255 - (int)(3 * (rendata[i * width + j] - 170));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else
				{
					_poExHand->sendInfoMessage("The CHL value is not between 0 and 255!", CLOG_NO_PRODUCT);
				}
			}
		}
		xfree(rendata);
	}
	/*
	 * SDD
	 */
	else if(strcmp(flag, SDD_name) == 0){

		float* array = resort(gray_image.DATASET, height, width);
		float* rendata =  dividedLinearStrength(gray_image.DATASET, height, width, array[0], array[1], 10, 245, 0.1, 30);

		/* Pixel by pixel rendering */
		for(int i = 0; i < height; i++){
			for(int j = 0; j< width; j++){

				if(CloudDetectData.DATASET[i * width + j] == 1){  //Cloud
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;                                           //R
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;                                           //G
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;                                               //B
				}
				else if(LandMaskData.DATASET[i * width + j] == 1){  //Land

					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 128;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 128;
				}
				else if(isnan(rendata[i * width + j]) == 1){  //Nan
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] >= 0 && rendata[i * width + j] <= 63.75)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 4 * rendata[i * width + j];
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
				}
				else if(rendata[i * width + j] > 63.75 && rendata[i * width + j] <= 127.5)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) =255 - (int)(4 * (rendata[i * width + j] - 63.75));
				}
				else if(rendata[i * width + j] > 127.5 && rendata[i * width + j] <= 191.25)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255 - (int)(4 * (rendata[i * width + j] - 127.5));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else if(rendata[i * width + j] > 191.25 && rendata[i * width + j] <= 255)
				{
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255 - (int)(4 * (rendata[i * width + j] - 191.25));
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else
				{
					_poExHand->sendInfoMessage("The SDD value is not between 0 and 255!", CLOG_NO_PRODUCT);
				}
			}
		}
		xfree(rendata);
	}
	else{
		_poExHand->sendInfoMessage("Unable to render the SSC's or CHL's or SDD's image!", CLOG_NO_PRODUCT);
	}

	/* 抽样 */
	int rows = height / 4;
	int cols = width / 4;
	IplImage* rendering_image_chouyang = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);
	cvResize(rendering_image,rendering_image_chouyang,CV_INTER_LINEAR);

	/* 滤波 */
	cvSmooth(rendering_image_chouyang, rendering_image_chouyang, CV_BLUR, 3);

	/* Save rendering image */
	cvSaveImage(path, rendering_image_chouyang);

	//free
    cvReleaseImage(&rendering_image);

}


/*
 * Rendering:
 *           植被覆盖指数 NDVI
 */
void Rendering_NDVI(float* data, const char* path, struct HDF_DATA_INT CloudDetectData, struct HDF_GEODATA GeoData)
{
	/* Get image size */
	int height = CloudDetectData.ROWS;
	int width = CloudDetectData.COLUMNS;

	/*
	 * 计算NDWI
	 * 用于辅助判断
	 */
	float* Nir = GeoData.NIR;
	float* Green = GeoData.GREEN;
	float* NDWI = (float* )malloc(sizeof(float) * height * width);
	for(int i = 0; i < height * width; i++){
		if((Green[i] + Nir[i]) != 0){
			NDWI[i] = float( (Green[i] - Nir[i]) / (Green[i] + Nir[i]) );
		}
		else{
			NDWI[i] = NAN_f;
		}
	}

	/* Initialization RGB image of rendering */
	IplImage* rendering_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	// 只对[0, 1]的值进行了排序和拉伸
	float* array_ndvi = resort_NDVI(data, height, width, 0, 1);
	float* rendata =  dividedLinearStrength(data, height, width, array_ndvi[0], array_ndvi[1], 10, 245, 0, 1);

	// 只对[-0.35, 0]的值进行了排序和拉伸
	float* array_ndvi_2 = resort_NDVI(data, height, width, -0.35, 0);
	float* rendata_2 = dividedLinearStrength(data, height, width, array_ndvi_2[0], array_ndvi_2[1], 10, 245, -0.35, 0);

	/* Pixel by pixel rendering */
	for(int i = 0; i < height; i++){
		for(int j = 0; j< width; j++){

			if(CloudDetectData.DATASET[i * width + j] == 1)  {  //  cloud
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;          //R
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;          //G
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;              //B
			}
			else if(isnan(data[i * width + j]) == 1){  // Nan's value
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
			}
			else if(data[i * width + j] <= -0.35 || data[i * width + j] > 1){  // no vegetation
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
				}
			else if(data[i * width + j] < 0 && data[i * width + j] > -0.35){

				if(NDWI[i * width + j] < 0.4){
					/*
					 * 黄色渐变
					 * 暗黄：255 153   0
					 * 橘黄：255 204   0
					 * 黄色：255 255   0
					 */
					if(rendata_2[i * width + j] >= 0 && rendata_2[i * width + j] <= 255){
						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 153 + (int)(rendata_2[i * width + j] * 102 / 255);
						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
					}
					else{
//						cout<<"The NDVI's rendata_2 Color strip value is not between 0 and 255!"<<endl;
//						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
//						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
//						CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
					}
				}
				else{
					// Sea
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
				}
			}
			else{
				/*
				 * 绿色渐变
				 * 浅绿：204 255 204
				 * 鲜绿：0   255   0
				 * 绿： 0   128   0
				 * 深绿：0   51    0
				 */
				if(rendata[i * width + j] >= 0 && rendata[i * width + j] < 51){
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 204 - (int)(4 * rendata[i * width + j]);
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 204 - (int)(4 * rendata[i * width + j]);
				}
				else if(rendata[i * width + j] >= 51 && rendata[i * width + j] <= 255){
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 306 - rendata[i * width + j];
					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
				}
				else{
//					cout<<"The NDVI's rendata Color strip value is not between 0 and 255!"<<endl;
//					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
//					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
//					CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
				}
			}
		}
	}

	/* 抽样 */
	int rows = height / 4;
	int cols = width / 4;
	IplImage* rendering_image_chouyang = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);
	cvResize(rendering_image,rendering_image_chouyang,CV_INTER_LINEAR);

	/* 滤波 */
	cvSmooth(rendering_image_chouyang, rendering_image_chouyang, CV_BLUR, 3);

	/* Save rendering image */
	cvSaveImage(path, rendering_image_chouyang);

	//free
    cvReleaseImage(&rendering_image);

    xfree(rendata);
	xfree(rendata_2);
    xfree(NDWI);
}

/*
 * Rendering:
 *           大气校正结果 Rrc's and Rrs's
 */
void Rendering_Rrs(float* data, const char* path, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData, const char* flag, CGEN_ExecutionHandler *_poExHand)
{
	const char* Rrc_name = "Rrc";
	const char* Rrs_name = "Rrs";

	/* Get image size */
	int height = CloudDetectData.ROWS;
	int width = CloudDetectData.COLUMNS;

	/* Save path for rendering images */
	char rendering_path[256];
	strcpy(rendering_path, path);

	/* Initialization RGB image of rendering */
	IplImage* rendering_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	float* rendata;
	/*
	 * Rrc's
	 */
	if(strcmp(flag, Rrc_name) == 0){
		float* array = resort(data, height, width);
		rendata =  dividedLinearStrength(data, height, width, array[0], array[1], 10, 245, -0.05, 0.4);
	}
	/*
	 * Rrs's
	 */
	else if(strcmp(flag, Rrs_name) == 0){
		float* array = resort(data, height, width);
		rendata =  dividedLinearStrength(data, height, width, array[0], array[1], 10, 245, -0.015, 0.115);
	}
	else{
		_poExHand->sendInfoMessage("Unable to render the Rrc's or Rrs's image!", CLOG_NO_PRODUCT);
	}

	/* Pixel by pixel rendering */
	for(int i = 0; i < height; i++){
		for(int j = 0; j< width; j++){

			if(CloudDetectData.DATASET[i * width + j] == 1){  // Cloud
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;                                           //R
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;                                           //G
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;                                               //B
			}
			else if(LandMaskData.DATASET[i * width + j] == 1){  // Land

				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 128;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 128;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 128;
			}
			else if(isnan(rendata[i * width + j]) == 1){  // Nan's
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
			}
			else if(rendata[i * width + j] >= 0 && rendata[i * width + j] <= 63.75)
			{
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 4 * rendata[i * width + j];
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 255;
			}
			else if(rendata[i * width + j] > 63.75 && rendata[i * width + j] <= 127.5)
			{
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 0;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) =255 - (int)(4 * (rendata[i * width + j] - 63.75));
			}
			else if(rendata[i * width + j] > 127.5 && rendata[i * width + j] <= 191.25)
			{
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255 - (int)(4 * (rendata[i * width + j] - 127.5));
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
			}
			else if(rendata[i * width + j] > 191.25 && rendata[i * width + j] <= 255)
			{
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 2) = 255;
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j + 1) = 255 - (int)(4 * (rendata[i * width + j] - 191.25));
				CV_IMAGE_ELEM(rendering_image, uchar, i, 3 * j) = 0;
			}
			else
			{
				_poExHand->sendInfoMessage("The Rrc's or Rrs's value is not between 0 and 255!", CLOG_NO_PRODUCT);
			}
		}
	}

	/* 抽样 */
	int rows = height / 4;
	int cols = width / 4;
	IplImage* rendering_image_chouyang = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);
	cvResize(rendering_image,rendering_image_chouyang,CV_INTER_LINEAR);

	/* 滤波 */
	cvSmooth(rendering_image_chouyang, rendering_image_chouyang, CV_BLUR, 3);

	/* Save rendering image */
	cvSaveImage(path, rendering_image_chouyang);

	//free
    cvReleaseImage(&rendering_image);
    xfree(rendata);

}


