/*
 * Land_Mask.cpp
 *
 *  Created on: 2018年3月15日
 *      Author: feidong
 */


#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"NDWI.h"
#include"img_filt.h"
#include"img_binary.h"
#include"img_padding.h"
#include"img_cor_exp.h"

extern "C"{
	#include"../HDF_STRUCT.h"
	#include"../BasicOperation/Basic_Operation.h"
}
#include"../CloudDetect/HDF_write_to_IPLImage.h"
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;

using namespace cv;
struct HDF_DATA_INT LandMask(struct HDF_GEODATA DataSet, int GuassFilterSize, float NDWIThreshold)
{
	//  获取波段的长、宽
	int nYSize = DataSet.ROWS;
	int nXSize = DataSet.COLUMNS;

//	//添加图例------------------
//	Mat img1, img2, img, mask;
//	img1 = imread("land_result_1.jpg");//image
//	img2 = imread("img3.jpg");//图例
//	//定义一个Mat类型并设置ROI区域
//	Mat imageROI = img1(Rect(img1.cols - img2.cols, img1.rows - img2.rows, img2.cols, img2.rows));
//	//加载掩码
//	mask = imread("img3.jpg");
//	//将掩码复制到ROI
//	img2.copyTo(imageROI, mask);
//	imwrite("图例out-img.jpg",img1);

	// **************************************************进行NDWI计算************************************************

	/*  NDWI =(p(Green)-p(NIR))/(p(Green)+p(NIR))
	 *  基于绿波段与近红外波段的归一化比值指数,用来提取影像中的水体信息.
	 */

	IplImage *pNDWI = cvCreateImage(cvSize(nXSize, nYSize), IPL_DEPTH_32F, 1);
	pNDWI = NDWI(nYSize, nXSize, DataSet, pNDWI, NDWIThreshold);

//	对NDWI图像二值化（陆地为0，海洋为1）。
//	IplImage *test = cvCreateImage(cvGetSize(pNDWI), IPL_DEPTH_32F, 1);
//	cvThreshold(pNDWI, test, 0.25, 255, CV_THRESH_BINARY);
//	//save image
//	cvSaveImage("./Landmask.jpg", test);
	//***************************************************图像滤波***************************************************

	/*  图像高斯滤波
     *  对NDWI图像进行图像滚动递归引导滤波操作，消除图像中的噪声点
     */

	IplImage *IMAGE = cvCreateImage(cvGetSize(pNDWI), IPL_DEPTH_32F, 1);
	IMAGE = img_filt(nYSize, nXSize, pNDWI, IMAGE, GuassFilterSize);

	// *************************************************NDWI图像二值化************************************************

	/*
	 *  对NDWI图像二值化（陆地为0，海洋为1）。
	 */

	IplImage *pBW = cvCreateImage(cvGetSize(IMAGE), IPL_DEPTH_8U, 1);
	pBW = img_binary(IMAGE, pBW, NDWIThreshold);

	// *************************************************填充陆地和海洋区域内的空洞**************************************

	/*
	 *  填充陆地和海洋区域内的空洞。
	 *  容易出现大片的错误（把海填充为陆地），故现在没使用
	 */

	//pBW = img_padding(pBW);
	//save image
//	cvSaveImage("./Landmask-2.jpg", pBW);
	// *************************************************腐蚀膨胀*****************************************************

	/*  腐蚀膨胀:
	 *  利用腐蚀膨胀操作对海陆分割边界进行平滑处理。
	 */

	struct HDF_DATA_INT TruthValueGraph = img_cor_exp(pBW, nYSize, nXSize);

	for(int i = 0; i < nYSize * nXSize; i++)
	{
		if(TruthValueGraph.DATASET[i] == 255)
		{
			TruthValueGraph.DATASET[i] = 0;
		}
		else if(TruthValueGraph.DATASET[i] == 0)
		{
			TruthValueGraph.DATASET[i] = 1;
		}
	}


	// *************************************************Land mask start***********************************************

	//数据掩膜
//	for(int i = 0; i < nYSize * nXSize; i++)
//	{
//			DataSet.BLUE[i] *= TruthValueGraph.DATASET[i];
//			DataSet.GREEN[i] *= TruthValueGraph.DATASET[i];
//			DataSet.RED[i] *= TruthValueGraph.DATASET[i];
//			DataSet.NIR[i] *= TruthValueGraph.DATASET[i];
//	}

	//save image
	IplImage* land_result_1 = cvCreateImage(cvGetSize(pBW), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;
	HDF_write_to_IPLImage(land_result_1, nYSize, nXSize, DataSet);
//	cvSaveImage("./land_result_1.jpg", pBW);
    cvReleaseImage(&pNDWI);
    cvReleaseImage(&IMAGE);
    cvReleaseImage(&pBW);

	return TruthValueGraph;
	// *************************************************Land mask end***********************************************
}



