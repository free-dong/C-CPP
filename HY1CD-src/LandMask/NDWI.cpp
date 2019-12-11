/*
 * NDWI.cpp
 *
 *  NDWI =(p(Green)-p(NIR))/(p(Green)+p(NIR))
 *  基于绿波段与近红外波段的归一化比值指数,用来提取影像中的水体信息.
 *
 *  Created on: 2018年4月1日
 *  	Author: feidong
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
//#define PATH_HDF5 "/home/zhongyong/eclipse-workspace/data/H1C_OPER_CZI_L1B_20190101T010500_00103_10.h5"
//#define PATH_DataSet "/Geophysical Data"
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;

IplImage *NDWI(int nYSize, int nXSize, struct HDF_GEODATA dataset, IplImage *pNDWI, float NDWIThreshold)
{
	/*  NDWI =(p(Green)-p(NIR))/(p(Green)+p(NIR))
	 *  基于绿波段与近红外波段的归一化比值指数,用来提取影像中的水体信息.
	 */

	//  初始化3个波段的像元值
	int green;
	int nir;
	int red;
	float tmp_NDWI = 0.0;
	float tmp_NDVI = 0.0;

	//	对每个像素进行NDWI
    for (int row = 0; row < nYSize; row++)
    {
    	for (int col = 0; col < nXSize; col++)
    	{
			green = dataset.GREEN[row*nXSize + col];
			nir = dataset.NIR[row*nXSize + col];
			red = dataset.RED[row*nXSize + col];

			tmp_NDWI = (float)(green - nir) / (green + nir);
			tmp_NDVI = (float)(nir - red)   / (nir + red);

			// NDWI和NDVI联合判断为海洋的条件
			if (tmp_NDWI>NDWIThreshold && tmp_NDVI<-0.35)
			{	//接下来img_binary根据NDWI阈值，将数据二值化为0和255
				tmp_NDWI = NDWIThreshold + 0.1;//
			}

			CV_IMAGE_ELEM(pNDWI, float, row, col) = tmp_NDWI;
    	}
    }

    //  释放内存
//    dataset.GREEN = NULL;
//    dataset.NIR = NULL;

    return pNDWI;
	}
