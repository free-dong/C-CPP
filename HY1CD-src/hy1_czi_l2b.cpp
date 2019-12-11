/*
 * hy1_czi_l2b.cpp
 *
 *  Created on: Jul 20, 2018
 *      Author: zhongyong
 */
#include<stdlib.h>
#include"HDF_STRUCT.h"
#include"BasicOperation/Write_Data_Attr_Xml.hpp"
#include"BasicOperation/SaveJPEG.hpp"
#include"hy1_czi_l2b.hpp"
#include <iostream>
#include <exception>
using namespace std;

#include <limits>

#include<cmath>

typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();
// NDVI---------------------------------------------------

struct HDF_DATA NDVI(struct HDF_GEODATA GeoData, struct HDF_DATA_INT CloudDetectData, float NDVIThreshold, char* L2BH5Path, char* L2BH5ThumbPath, float NDWIThreshold){

//	float  a = NAN_f;
//	if(isnan(a) == 1)
//	{
//		cout<<"ok!"<<endl;
//	}
	float* Red = GeoData.RED;
	float* Nir = GeoData.NIR;
	float* Green = GeoData.GREEN;

	int height = GeoData.ROWS, width = GeoData.COLUMNS;
	float* data = (float* )malloc(sizeof(float) * height * width);
	float tmp = 0.0;
	float tmp_NDWI = 0.0;
	for(int i = 0; i < height * width; i++){
		//若此点不为云，进行NDVI计算
		if(CloudDetectData.DATASET[i] != 1)//云 1，其他 0
		{
			//NDVI公式计算
			if((Red[i] + Nir[i]) != 0)
			{
				data[i]  = float( (Nir[i]   - Red[i]) / (Red[i] + Nir[i]) );
				tmp_NDWI = float( (Green[i] - Nir[i]) / (Green[i] + Nir[i]) );
				if (data[i]<-0.35 && tmp_NDWI<NDWIThreshold)
				{
					data[i] = NAN_f;//
				}
			}else
			{
				data[i] = NAN_f;//以后需要改
			}
			//通过与阈值进行比较来赋值
//					if(tmp < NDVIThreshold)
//					{
//						data[i] = NAN_f;
//					}else
//					{
//						data[i] = tmp;
//					}
		}
		//若此点为云，赋值为NaN
		else if(CloudDetectData.DATASET[i] == 1 || data[i] < -1 || data[i] > 1)
		{
			data[i] = NAN_f;
		}
		else
		{
			cout<<"Cloud detection results are wrong！"<<endl;
		}
	}

	struct HDF_DATA DataNDVI ;
	DataNDVI.DATASET = data;
	DataNDVI.RANK = 2;
	DataNDVI.ROWS = height;
	DataNDVI.COLUMNS = width;
	DataNDVI.Flag = 0;

	char* GroupName = "Geophysical Data";
	char* DataName = "NDVI";

	try{
		WriteH5Data1_NDVI(L2BH5Path, GroupName, DataName, DataNDVI, "none", 0.001, 0.0, "Normalized difference vegetation index", "Float32", "Normalized difference vegetation index", 32767, -1.0, 1.0);
	}catch(exception& e){
		DataNDVI.Flag = -1;
		DataNDVI.ErrorInfo = "Write the NDVI DataSet Failed!";
		return DataNDVI;
	}

//	try{
//		NDVISaveToJPEG(data, L2BH5ThumbPath, height, width,1);
//	}catch(exception& e){
//		DataNDVI.Flag = -1;
//		DataNDVI.ErrorInfo = "Write the NDVI JPEG Failed!";
//		return DataNDVI;
//	}

	return DataNDVI;

}

////那个属性需要修改
//void WriteAttribute(struct HDF_Attribute_Data AttrData, char* L2BH5Path){
////	AttrData.
//
//}

//悬浮物浓度反演---------------------------------------------------
struct HDF_DATA SSC(struct HDF_Rrc_Rrs GeoData, char* L2BH5Path, char* L2BH5ThumbPath){

//	float* Blue = GeoData.Rrc_BLUE;//460
	float* Green = GeoData.Rrc_GREEN;//560
	float* Red = GeoData.Rrc_RED;//650
	float* Nir = GeoData.Rrc_NIR;//825
	int row = GeoData.ROWS, col = GeoData.COLUMNS;

//	float a = -1.47443, b = 1.96736 , c = 0.32141 , d = 3.98565; //Camera parameterrs
//	float a = -0.64151, b = 8.75934 , c = 3.52131;
	float a = 0.3315, b = 220.3146 , c = 36.8262;

	float* data = (float* )malloc(sizeof(float) * row * col);
	float tmp = 0.0;
	int i;
	for(i = 0; i < row * col; i++)
	{
		if (GeoData.Rrc_BLUE >= 0){//反演：判断第一个波段，若为负值，则设置为无效值
			if(Green[i] != 0)
			{
//				tmp = float(a + b * Nir[i] + c * Red[i] / Green[i]);
				tmp = float(a + b * Red[i] * Red[i] + c * Red[i]);
				data[i] = pow(10, tmp);
			}
			else
			{
				data[i] = NAN_f;
			}

		}
		else{
			data[i] = NAN_f;
		}
	}

	struct HDF_DATA DataSSC ;
	DataSSC.DATASET = data;
	DataSSC.RANK = 2;
	DataSSC.ROWS = row;
	DataSSC.COLUMNS = col;
	DataSSC.Flag = 0;
	DataSSC.ErrorInfo = "none";

	char* GroupName = "Geophysical Data";
	char* DataName = "SSC";

	try{
//		WriteH5Data1(L2BH5Path, GroupName, DataName, DataSSC, "mg/l", "1", "0", "Suspended sediment concentration", "Float32");
//		WriteH5Data1_SSC(L2BH5Path, GroupName, DataName, DataSSC, "mg/l", 0.001, 0.0, "Suspended sediment concentration", "Float32", "Suspended sediment concentration", NAN_f, 0.1, 200.0);
//		WriteH5Data1_SSC(L2BH5Path, GroupName, DataName, DataSSC, "mg/l", 1, 0.0, "Suspended sediment concentration", "Float32", "Suspended sediment concentration", NAN_f, 0.1, 200.0);
		WriteH5Data1_SSC(L2BH5Path, GroupName, DataName, DataSSC, "mg/L", 1, 0.0, "Suspended sediment concentration", "Float32", "Suspended sediment concentration", NAN_f, 0.1, 200.0);
	}catch(exception& e){
		DataSSC.Flag = -1;
		DataSSC.ErrorInfo = "Write the SSC DataSet Failed!";
		return DataSSC;
	}
//	try{
//		DataSaveToJPEG(data, L2BH5ThumbPath, row, col,1);
//	}catch(exception& e){
//		DataSSC.Flag = -1;
//		DataSSC.ErrorInfo = "Write the SSC JPEG Failed!";
//		return DataSSC;
//	}
	return DataSSC;

}

