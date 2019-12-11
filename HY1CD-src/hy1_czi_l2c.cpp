/*
 * hy1_czi_l2c.cpp
 *
 *  Created on: Jul 20, 2018
 *      Author: zhongyong
 */



#include<stdlib.h>
#include"HDF_STRUCT.h"
#include"BasicOperation/Write_Data_Attr_Xml.hpp"
#include"BasicOperation/SaveJPEG.hpp"
#include"hy1_czi_l2c.hpp"
#include<math.h>
#include <iostream>
#include <exception>
using namespace std;

#include <limits>
typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();

// 叶绿素浓度反演模块
struct HDF_DATA CHL(struct HDF_Rrc_Rrs GeoData, char* L2CH5Path, char* L2CCHLPath){

	float* blue = GeoData.Rrc_BLUE;
	float* green = GeoData.Rrc_GREEN;
	float* red = GeoData.Rrc_RED;

	float Camera1[5] = {0.6171,-3.0573,2.193,-1.8324,-0.6918};
//	float Camera2[5] = {0.6389,-3.0561,2.1193,-1.5822,-0.9576};
//	float Camera3[3] = {-1.894, 0.763, 1.721};
	float Camera3[3] = {-1.903, 0.131, 1.668};

	int height = GeoData.ROWS, width = GeoData.COLUMNS;
	float* data = (float* )malloc(sizeof(float) * height * width);
	float tmp = 0.0;
	float X1, X2, total_R;
	for(int i=0;i< height * width ; i++)
	{
		if (GeoData.Rrc_BLUE >= 0)
		{//反演：判断第一个波段，若为负值，则设置为无效值

			if(green[i] != 0 && blue[i] != 0)
			{
				X1 = red[i] / green[i];
				X2 = green[i] / blue[i];
				total_R = Camera3[0] + Camera3[1] * pow(X1, 2) + Camera3[2] * pow(X2, 0.5);
				data[i] = pow (10.0,total_R);
			}
			else
			{
				data[i] = NAN_f;
			}
		}//if
		else
		{
			data[i] = NAN_f;
		}
	}//for

	struct HDF_DATA DataChl ;
	DataChl.DATASET = data;
	DataChl.RANK = 2;
	DataChl.ROWS = height;
	DataChl.COLUMNS = width;
	DataChl.Flag = 0;

	char* GroupName = "Geophysical Data";
	char* DataName = "chl_a";

	try{
//		WriteH5Data1(L2CH5Path, GroupName, DataName, DataChl, "mg/m3", "1", "0", "Chlorophyll a concentration", "Float32");
		WriteH5Data1_SSC(L2CH5Path, GroupName, DataName, DataChl, "mg/m3", 1, 0.0, "Chlorophyll a concentration", "Float32", "Chlorophyll a concentration", NAN_f, 0.01, 30.0);
	}catch(exception& e){
		DataChl.Flag = -1;
		DataChl.ErrorInfo = "Write the NDVI Failed!";
		return DataChl;
	}
	DataSaveToJPEG(data, L2CCHLPath, height, width, 1);

	return DataChl;
}

// 透明度反演模块
struct HDF_DATA SDD(struct HDF_Rrc_Rrs GeoData, char* L2CH5Path, char* L2BSDDPath){

	float* blue = GeoData.Rrc_BLUE;
	float* green = GeoData.Rrc_GREEN;

	float Camera1[5] = {-1.8973, 0.5232, -0.9855};
	float Camera2[5] = {-1.8990, 0.5264, -0.9885};
	float Camera3[4] = {-3.7230, 0.8317, -1.8965, 13.4732};

	int height = GeoData.ROWS, width = GeoData.COLUMNS;
	float* data = (float* )malloc(sizeof(float) * height * width);
	float tmp = 0.0;

	for(int i=0; i<height * width; i++)
	{
		if (GeoData.Rrc_BLUE > 0)
		{//反演：判断第一个波段，若为负值，则设置为无效值
			if(green[i]<=0)
			{
				data[i]=NAN_f;
			}
			else
			{
				tmp = float(exp(Camera3[0]+Camera3[1]*log(blue[i])+Camera3[2]*log(green[i]) + Camera3[3]*blue[i]));
				data[i] = tmp;
//				if(data[i]<100)
//					data[i]=0;
			}
		}//if
		else
		{
			data[i] = NAN_f;
		}
	}


	struct HDF_DATA DataSDD ;
	DataSDD.DATASET = data;
	DataSDD.RANK = 2;
	DataSDD.ROWS = height;
	DataSDD.COLUMNS = width;
	DataSDD.Flag = 0;

	char* GroupName = "Geophysical Data";
	char* DataName = "SDD";

	try{
//		WriteH5Data1(L2CH5Path, GroupName, DataName, DataSDD, "m", "1", "0", "Secchi disk Depth", "Float32");
//		WriteH5Data1_NDVI(L2CH5Path, GroupName, DataName, DataSDD, "m", 0.001, 0.0, "Suspended sediment concentration", "int16", "Secchi disk depth", NaN, 0.1, 200.0);
		WriteH5Data1_NDVI(L2CH5Path, GroupName, DataName, DataSDD, "m", 0.001, 0.0, "Suspended sediment concentration", "int16", "Secchi disk depth", 32767, 0.1, 200.0);
	}catch(exception& e){
		DataSDD.Flag = -1;
		DataSDD.ErrorInfo = "Write the NDVI Failed!";
		return DataSDD;
	}
	DataSaveToJPEG(data, L2BSDDPath, height, width ,1);

	return DataSDD;

}
