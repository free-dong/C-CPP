/*
 * Atmospheric_Correction.cpp
 *
 *  Created on: 2018年5月16日
 *  	Author: zhangyonghui AND feidong
 */
//#include "hy1_czi_l2a_pro.hpp"
//#include"CGEN_IProcessing.hpp"
//#include "CGEN_ExceptionsHandler.hpp"
//#include "CLOG_EnumsLOG.hpp"
#include "../hy1_czi_l2_pro.hpp"
#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include <cmath>
#include"TableSearch.h"
#include"Linear_Interpolation.h"
#include"read_txt.h"
#include"read_Aerosol_and_scattering.h"
extern "C"{
	#include"../BasicOperation/Read_Data_Attr_Xml.h"
	#include"../HDF_STRUCT.h"
	#include"../BasicOperation/Basic_Operation.h"
}
#include"omp.h"

#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array
#include <exception>
using namespace std;
using namespace cv;

typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();

/*
 * Judge whether memory application is successful
 */

// 判断内存是否占用，如果占用（指针为null），则输出错误信息
void judgeMemoryApplication(float* point, CGEN_ExecutionHandler *_poExHand, char* pointName)
{
	char name_failed[512]; char name_success[512];
	strcpy(name_failed, pointName); strcat(name_failed, "'s memory application failed!");

	if(point == NULL)
	{
		_poExHand->sendInfoMessage(name_failed, CLOG_NO_PRODUCT);
		exit(0);
	}
}

struct HDF_Rrc_Rrs Atmospheric_Correction(struct HDF_GEODATA DataSet, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData, char* AuxDataPathName, char* NCEPDataName, int ASCMethod, char* PATH_HDF5, CGEN_ExecutionHandler *_poExHand)
{
//******************************************************read data******************************************************

	int height = DataSet.ROWS;
	int width = DataSet.COLUMNS;

	/*
	 * Initial Rrs and Rrc's Struct;
	 * TODO:2018-10-19
	 */
	struct HDF_Rrc_Rrs Rrc_Rrs_data;
	Rrc_Rrs_data.Flag = 0;
	Rrc_Rrs_data.AngleFlag = 0;
	Rrc_Rrs_data.LatLonFlag = 0;

	Rrc_Rrs_data.RANK = 2;
	Rrc_Rrs_data.ROWS = DataSet.ROWS;
	Rrc_Rrs_data.COLUMNS = DataSet.COLUMNS;

	//Rrc's
	Rrc_Rrs_data.Rrc_BLUE = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);    judgeMemoryApplication(Rrc_Rrs_data.Rrc_BLUE,_poExHand,"Rrc_BLUE");
	Rrc_Rrs_data.Rrc_GREEN = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);   judgeMemoryApplication(Rrc_Rrs_data.Rrc_GREEN,_poExHand,"Rrc_GREEN");
	Rrc_Rrs_data.Rrc_NIR = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);     judgeMemoryApplication(Rrc_Rrs_data.Rrc_NIR,_poExHand,"Rrc_NIR");
	Rrc_Rrs_data.Rrc_RED = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);     judgeMemoryApplication(Rrc_Rrs_data.Rrc_RED,_poExHand,"Rrc_RED");

	//Rrs's
	Rrc_Rrs_data.Rrs_BLUE = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);    judgeMemoryApplication(Rrc_Rrs_data.Rrs_BLUE,_poExHand,"Rrs_BLUE");
	Rrc_Rrs_data.Rrs_GREEN = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);   judgeMemoryApplication(Rrc_Rrs_data.Rrs_GREEN,_poExHand,"Rrs_GREEN");
	Rrc_Rrs_data.Rrs_NIR = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);     judgeMemoryApplication(Rrc_Rrs_data.Rrs_NIR,_poExHand,"Rrs_NIR");
	Rrc_Rrs_data.Rrs_RED = (float* )malloc(sizeof(float) * Rrc_Rrs_data.ROWS * Rrc_Rrs_data.COLUMNS);     judgeMemoryApplication(Rrc_Rrs_data.Rrs_RED,_poExHand,"Rrs_RED");

	//read H5 Attribute
	struct HDF_Attribute_Data Attribute = ReadH5Attribute(PATH_HDF5);

	//get RangeBeginDate Attribute
	char RangeBeginDate[32];    strcpy(RangeBeginDate, Attribute.RangeBeginDate);

	//get Date
	int Date = atoi(RangeBeginDate);  //example --- 20160101 153 -> 20160101

	//get month
	int month;
	if((int)((Date % 10000) / 1000) == 0){
		month = (int)((Date % 1000) / 100);
	}
	else{
		month = 10 + (int)((Date % 1000) / 100);
	}

//	cout<<"month: "<<month<<endl;
	char month_char[10] = "";
	sprintf(month_char, "%d", month);

    /*
     * read NCEP hdf4 data
     *
     * select --- 1, NCEP real-time data;
     *            2, NCEP climate data;
     *            3, COCTS data
     */

	float*** h4_NCEP;
	if (ASCMethod == 1)
		{
			_poExHand->sendInfoMessage("Read realtime data.", CLOG_NO_PRODUCT);
			h4_NCEP = hdf4Read_Realtime(NCEPDataName);
		}
	else if (ASCMethod == 2)
	{
		_poExHand->sendInfoMessage("Read climate data.", CLOG_NO_PRODUCT);

		struct HDF_hdf4Read_Climatedata h4_NCEP_HDF = hdf4Read_Climatedata(NCEPDataName, month_char);
		h4_NCEP = h4_NCEP_HDF.hdf4Read_Climatedata;

		if(h4_NCEP_HDF.Flag < 0)
		{

//			_poExHand->sendInfoMessage(h4_NCEP_HDF.ErrorInfo, CLOG_NO_PRODUCT);
	    	Rrc_Rrs_data.ClimateFlag = -3;
	    	Rrc_Rrs_data.ErrorInfo = h4_NCEP_HDF.ErrorInfo;
	    	return Rrc_Rrs_data;
		}

	}
	else
	{
//		cout<<"ASCMethod = 3"<<endl;
	}
//	printf("h4[0][0][0]: %f\n", h4_NCEP[0][0][0]);
//	printf("h4[0][0][1]: %f\n", h4_NCEP[0][0][1]);
//	printf("h4[0][2][3]: %f\n", h4_NCEP[0][2][3]);
//	printf("h4[2][225][1194]: %f\n", h4_NCEP[2][225][1194]);
//	exit(0);


	if(h4_NCEP != NULL)
	{
		_poExHand->sendInfoMessage("Hdf4 data read successfully.", CLOG_NO_PRODUCT);
	}
	else
	{
		_poExHand->sendInfoMessage("Hdf4 data read failed!", CLOG_NO_PRODUCT);
		exit(0);
	}


	/*
	 * folder path:
	 *
	 * 1. Rayleigh --- (1).Mid latitude summer
	 *                 (2).Mid latitude winter
	 *                 (3).Tropic
	 *                 (4).US1962
	 *
	 * 2. Gas absorption --- (1).ozone
	 *                       (2).steam
	 *                       (3).oxygen
	 */

	//Rayleigh --- (1).Mid latitude summer
	char PATH_LUT_MS_B1[256];
	strcpy(PATH_LUT_MS_B1,AuxDataPathName);
	strcat(PATH_LUT_MS_B1,"/Rayleigh/Mid latitude summer/LUT_MS_B1.txt");
	char PATH_LUT_MS_B2[256];
	strcpy(PATH_LUT_MS_B2,AuxDataPathName);
	strcat(PATH_LUT_MS_B2,"/Rayleigh/Mid latitude summer/LUT_MS_B2.txt");
	char PATH_LUT_MS_B3[256];
	strcpy(PATH_LUT_MS_B3,AuxDataPathName);
	strcat(PATH_LUT_MS_B3,"/Rayleigh/Mid latitude summer/LUT_MS_B3.txt");
	char PATH_LUT_MS_B4[256];
	strcpy(PATH_LUT_MS_B4,AuxDataPathName);
	strcat(PATH_LUT_MS_B4,"/Rayleigh/Mid latitude summer/LUT_MS_B4.txt");

	//Rayleigh --- (2).Mid latitude winter
	char PATH_LUT_MW_B1[256];
	strcpy(PATH_LUT_MW_B1,AuxDataPathName);
	strcat(PATH_LUT_MW_B1,"/Rayleigh/Mid latitude winter/LUT_MW_B1.txt");
	char PATH_LUT_MW_B2[256];
	strcpy(PATH_LUT_MW_B2,AuxDataPathName);
	strcat(PATH_LUT_MW_B2,"/Rayleigh/Mid latitude winter/LUT_MW_B2.txt");
	char PATH_LUT_MW_B3[256];
	strcpy(PATH_LUT_MW_B3,AuxDataPathName);
	strcat(PATH_LUT_MW_B3,"/Rayleigh/Mid latitude winter/LUT_MW_B3.txt");
	char PATH_LUT_MW_B4[256];
	strcpy(PATH_LUT_MW_B4,AuxDataPathName);
	strcat(PATH_LUT_MW_B4,"/Rayleigh/Mid latitude winter/LUT_MW_B4.txt");

	//Rayleigh --- (3).Tropic
	char PATH_LUT_TR_B1[256];
	strcpy(PATH_LUT_TR_B1,AuxDataPathName);
	strcat(PATH_LUT_TR_B1,"/Rayleigh/Tropic/LUT_TR_B1.txt");
	char PATH_LUT_TR_B2[256];
	strcpy(PATH_LUT_TR_B2,AuxDataPathName);
	strcat(PATH_LUT_TR_B2,"/Rayleigh/Tropic/LUT_TR_B2.txt");
	char PATH_LUT_TR_B3[256];
	strcpy(PATH_LUT_TR_B3,AuxDataPathName);
	strcat(PATH_LUT_TR_B3,"/Rayleigh/Tropic/LUT_TR_B3.txt");
	char PATH_LUT_TR_B4[256];
	strcpy(PATH_LUT_TR_B4,AuxDataPathName);
	strcat(PATH_LUT_TR_B4,"/Rayleigh/Tropic/LUT_TR_B4.txt");

	//Rayleigh --- (4).US1962
	char PATH_LUT_US_B1[256];
	strcpy(PATH_LUT_US_B1,AuxDataPathName);
	strcat(PATH_LUT_US_B1,"/Rayleigh/US1962/LUT_US_B1.txt");
	char PATH_LUT_US_B2[256];
	strcpy(PATH_LUT_US_B2,AuxDataPathName);
	strcat(PATH_LUT_US_B2,"/Rayleigh/US1962/LUT_US_B2.txt");
	char PATH_LUT_US_B3[256];
	strcpy(PATH_LUT_US_B3,AuxDataPathName);
	strcat(PATH_LUT_US_B3,"/Rayleigh/US1962/LUT_US_B3.txt");
	char PATH_LUT_US_B4[256];
	strcpy(PATH_LUT_US_B4,AuxDataPathName);
	strcat(PATH_LUT_US_B4,"/Rayleigh/US1962/LUT_US_B4.txt");

	//Gas absorption --- (1).ozone
	char PATH_coef_O3_B1[256];
	strcpy(PATH_coef_O3_B1,AuxDataPathName);
	strcat(PATH_coef_O3_B1,"/Gas absorption/ozone/chouyang1.txt");
	char PATH_coef_O3_B2[256];
	strcpy(PATH_coef_O3_B2,AuxDataPathName);
	strcat(PATH_coef_O3_B2,"/Gas absorption/ozone/chouyang2.txt");
	char PATH_coef_O3_B3[256];
	strcpy(PATH_coef_O3_B3,AuxDataPathName);
	strcat(PATH_coef_O3_B3,"/Gas absorption/ozone/chouyang3.txt");
	char PATH_coef_O3_B4[256];
	strcpy(PATH_coef_O3_B4,AuxDataPathName);
	strcat(PATH_coef_O3_B4,"/Gas absorption/ozone/chouyang4.txt");

	//Gas absorption --- (2).steam
	char PATH_coef_water_B2[256];
	strcpy(PATH_coef_water_B2,AuxDataPathName);
	strcat(PATH_coef_water_B2,"/Gas absorption/steam/shuiqi2.txt");
	char PATH_coef_water_B3[256];
	strcpy(PATH_coef_water_B3,AuxDataPathName);
	strcat(PATH_coef_water_B3,"/Gas absorption/steam/shuiqi3.txt");
	char PATH_coef_water_B4[256];
	strcpy(PATH_coef_water_B4,AuxDataPathName);
	strcat(PATH_coef_water_B4,"/Gas absorption/steam/shuiqi4.txt");

	//Gas absorption --- (3).oxygen
	char PATH_coef_O2_B3[256];
	strcpy(PATH_coef_O2_B3,AuxDataPathName);
	strcat(PATH_coef_O2_B3,"/Gas absorption/oxygen/yangqi3.txt");
	char PATH_coef_O2_B4[256];
	strcpy(PATH_coef_O2_B4,AuxDataPathName);
	strcat(PATH_coef_O2_B4,"/Gas absorption/oxygen/yangqi4.txt");

	char PATH_HDF5_1[256];
	strcpy(PATH_HDF5_1,AuxDataPathName);
	strcat(PATH_HDF5_1,"/Auxiliary data/N201809115_MET_NCEP_1440x0721_f015.hdf");
	char PATH[256];
	strcpy(PATH,AuxDataPathName);
	strcat(PATH,"/Aerosol and scattering");

	/*
	 *
	 * Read data --- 气溶胶和散射(number: 11)
	 *
	 */
	const char* folder_1 = "city";
	float*** city_aeo = read_aeo_txt(PATH, folder_1, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** city_delta = read_delta_txt(PATH, folder_1, 1, 3,_poExHand);//delta文件，二维数组
	float*** city_tra = read_tra_txt(PATH, folder_1, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<city_aeo[0][0][3]<<endl;
//	cout<<city_delta[0][1]<<endl;
//	cout<<city_tra[0][0][3]<<endl;

	const char* folder_2 = "pureSea";
	float*** pureSea_aeo = read_aeo_txt(PATH, folder_2, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** pureSea_delta = read_delta_txt(PATH, folder_2, 1, 3,_poExHand);//delta文件，二维数组
	float*** pureSea_tra = read_tra_txt(PATH, folder_2, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<pureSea_aeo[0][0][3]<<endl;
//	cout<<pureSea_delta[0][1]<<endl;
//	cout<<pureSea_tra[0][0][3]<<endl;

	const char* folder_3 = "sea";
	float*** sea_aeo = read_aeo_txt(PATH, folder_3, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_delta = read_delta_txt(PATH, folder_3, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_tra = read_tra_txt(PATH, folder_3, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_aeo[0][0][3]<<endl;
//	cout<<sea_delta[0][1]<<endl;
//	cout<<sea_tra[0][0][3]<<endl;

	const char* folder_4 = "land";
	float*** land_aeo = read_aeo_txt(PATH, folder_4, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** land_delta = read_delta_txt(PATH, folder_4, 1, 3,_poExHand);//delta文件，二维数组
	float*** land_tra = read_tra_txt(PATH, folder_4, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<land_aeo[0][0][3]<<endl;
//	cout<<land_delta[0][1]<<endl;
//	cout<<land_tra[0][0][3]<<endl;

	const char* folder_5 = "desert";
	float*** desert_aeo = read_aeo_txt(PATH, folder_5, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** desert_delta = read_delta_txt(PATH, folder_5, 1, 3,_poExHand);//delta文件，二维数组
	float*** desert_tra = read_tra_txt(PATH, folder_5, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<desert_aeo[0][0][3]<<endl;
//	cout<<desert_delta[0][1]<<endl;
//	cout<<desert_tra[0][0][3]<<endl;

	const char* folder_6 = "isothermal";
	float*** isothermal_aeo = read_aeo_txt(PATH, folder_6, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** isothermal_delta = read_delta_txt(PATH, folder_6, 1, 3,_poExHand);//delta文件，二维数组
	float*** isothermal_tra = read_tra_txt(PATH, folder_6, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<isothermal_aeo[0][0][3]<<endl;
//	cout<<isothermal_delta[0][1]<<endl;
//	cout<<isothermal_tra[0][0][3]<<endl;

	const char* folder_7 = "sea_1";
	float*** sea_1_aeo = read_aeo_txt(PATH, folder_7, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_1_delta = read_delta_txt(PATH, folder_7, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_1_tra = read_tra_txt(PATH, folder_7, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_1_aeo[0][0][3]<<endl;
//	cout<<sea_1_delta[0][1]<<endl;
//	cout<<sea_1_tra[0][0][3]<<endl;

	const char* folder_8 = "sea_2";
	float*** sea_2_aeo = read_aeo_txt(PATH, folder_8, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_2_delta = read_delta_txt(PATH, folder_8, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_2_tra = read_tra_txt(PATH, folder_8, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_2_aeo[0][0][3]<<endl;
//	cout<<sea_2_delta[0][1]<<endl;
//	cout<<sea_2_tra[0][0][3]<<endl;

	const char* folder_9 = "sea_3";
	float*** sea_3_aeo = read_aeo_txt(PATH, folder_9, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_3_delta = read_delta_txt(PATH, folder_9, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_3_tra = read_tra_txt(PATH, folder_9, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_3_aeo[0][0][3]<<endl;
//	cout<<sea_3_delta[0][1]<<endl;
//	cout<<sea_3_tra[0][0][3]<<endl;

	const char* folder_10 = "sea_4";
	float*** sea_4_aeo = read_aeo_txt(PATH, folder_10, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_4_delta = read_delta_txt(PATH, folder_10, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_4_tra = read_tra_txt(PATH, folder_10, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_4_aeo[0][0][3]<<endl;
//	cout<<sea_4_delta[0][1]<<endl;
//	cout<<sea_4_tra[0][0][3]<<endl;

	const char* folder_11 = "sea_5";
	float*** sea_5_aeo = read_aeo_txt(PATH, folder_11, 9614, 8,_poExHand);//aeo_文件，三维数组
	float** sea_5_delta = read_delta_txt(PATH, folder_11, 1, 3,_poExHand);//delta文件，二维数组
	float*** sea_5_tra = read_tra_txt(PATH, folder_11, 23, 5,_poExHand);//tra_文件，三维数组
//	cout<<sea_5_aeo[0][0][3]<<endl;
//	cout<<sea_5_delta[0][1]<<endl;
//	cout<<sea_5_tra[0][0][3]<<endl;

	//臭氧
	float **coef_O3_B1 = read_txt(PATH_coef_O3_B1, 1, 4,_poExHand);
	float **coef_O3_B2 = read_txt(PATH_coef_O3_B2, 1, 4,_poExHand);
	float **coef_O3_B3 = read_txt(PATH_coef_O3_B3, 1, 4,_poExHand);
	float **coef_O3_B4 = read_txt(PATH_coef_O3_B4, 1, 4,_poExHand);
//	cout<<coef_O3_B1[0][1]<<endl;
//	cout<<coef_O3_B2[0][1]<<endl;
//	cout<<coef_O3_B3[0][1]<<endl;
//	cout<<coef_O3_B4[0][1]<<endl;

	//水汽
	float **coef_water_B2 = read_txt(PATH_coef_water_B2, 1, 4,_poExHand);
	float **coef_water_B3 = read_txt(PATH_coef_water_B3, 1, 4,_poExHand);
	float **coef_water_B4 = read_txt(PATH_coef_water_B4, 1, 4,_poExHand);

	//氧气
	float **coef_O2_B3 = read_txt(PATH_coef_O2_B3, 1, 6,_poExHand);
	float **coef_O2_B4 = read_txt(PATH_coef_O2_B4, 1, 6,_poExHand);

//	if(coef_O2_B4 != NULL)
//	{
//		_poExHand->sendInfoMessage("All txt file read successfully.", CLOG_NO_PRODUCT);
//	}
//	else
//	{
//		_poExHand->sendInfoMessage("Txt file read failed!", CLOG_NO_PRODUCT);
//		exit(0);
//	}

	/*
	 * Read data --- 1.Satellite Azimuth Angle (卫星方位角 0-360)
	 *               2.Satellite Zenith Angle (卫星天顶角 0-90)
	 *               3.Sun Azimuth Angle (太阳方位角 0-360)
	 *               4.Sun Zenith Angle (太阳天顶角 0-90)
	 *               5.latitude
	 *               6.longitude
	 */
	struct HDF_DATA SatelliteAzimuthData = ReadH5Data1(PATH_HDF5,"/Navigation Data","Satellite Azimuth Angle");
	struct HDF_DATA SatelliteZenithData = ReadH5Data1(PATH_HDF5,"/Navigation Data","Satellite Zenith Angle");
	struct HDF_DATA SunAzimuthData = ReadH5Data1(PATH_HDF5,"/Navigation Data","Sun Azimuth Angle");
	struct HDF_DATA SunZenithData = ReadH5Data1(PATH_HDF5,"/Navigation Data","Sun Zenith Angle");

	// ---判断数组大小是否相同---
	if (SatelliteAzimuthData.ROWS != height)
		 //若行不相等,输出错误信息
	{
		Rrc_Rrs_data.AngleFlag = -2;
    	return Rrc_Rrs_data;
	}
	else if ((SatelliteAzimuthData.ROWS == height) && (SatelliteAzimuthData.COLUMNS == width))
		//行列都相等,不进行插值
	{

	}
	else if ((SatelliteAzimuthData.ROWS == height) && (SatelliteAzimuthData.COLUMNS != width))
		//行相等,列不等,进行插值
	{
		// 读取Navigation属性值
		struct HDF_Attribute_Navigation AttributeData = ReadH5Attribute_Navigation(PATH_HDF5);

		// 方位角,天顶角插值
		SatelliteAzimuthData = Angle_Linear_Interpolation(SatelliteAzimuthData, height, width, AttributeData, 360, 1);
		SatelliteZenithData  = Angle_Linear_Interpolation(SatelliteZenithData, height, width, AttributeData, 90, 0);
		SunAzimuthData       = Angle_Linear_Interpolation(SunAzimuthData, height, width, AttributeData, 360, 0);
		SunZenithData        = Angle_Linear_Interpolation(SunZenithData, height, width, AttributeData, 90, 0);

	}

	struct HDF_DATA Lat = ReadH5DataInt_scale_factor(PATH_HDF5,"/Navigation Data","Latitude");//weidu
	struct HDF_DATA Lon = ReadH5DataInt_scale_factor(PATH_HDF5,"/Navigation Data","Longitude");//jingdu

	//judge
	if(Lat.ROWS != height || Lat.COLUMNS != width || Lon.ROWS != height || Lon.COLUMNS != width)
	{
    	_poExHand->sendInfoMessage("Dimension of Latitude and GeoData can't match!", CLOG_NO_PRODUCT);
    	Rrc_Rrs_data.LatLonFlag = -2;
    	return Rrc_Rrs_data;
//    	exit(0);
	}

	float maxLat = Lat.DATASET[0] * 4 + 360;
	float minLat = Lat.DATASET[0] * 4 + 360;
	float maxLon = Lon.DATASET[0] * 4 + 720;
    float minLon = Lon.DATASET[0] * 4 + 720;
    float* lat = (float *)malloc(sizeof(float) * Lat.ROWS * Lat.COLUMNS);
    if(lat == NULL){
    	_poExHand->sendInfoMessage("lat's memory application failed!", CLOG_NO_PRODUCT);
    	exit(0);
    }

    float* lon = (float *)malloc(sizeof(float) * Lon.ROWS * Lon.COLUMNS);
    if(lon == NULL){
    	_poExHand->sendInfoMessage("lon's memory application failed!", CLOG_NO_PRODUCT);
    	exit(0);
    }

    float* lat_gezi = (float *)malloc(sizeof(float) * Lat.ROWS * Lat.COLUMNS);
    if(lat_gezi == NULL){
    	_poExHand->sendInfoMessage("lat_gezi's memory application failed!", CLOG_NO_PRODUCT);
    	exit(0);
    }

    float* lon_gezi = (float *)malloc(sizeof(float) * Lon.ROWS * Lon.COLUMNS);
    if(lon_gezi == NULL){
    	_poExHand->sendInfoMessage("lon_gezi's memory application failed!", CLOG_NO_PRODUCT);
    	exit(0);
    }

//    int flag = 0;
	for(int i = 0; i < Lat.ROWS * Lat.COLUMNS; i++){

		//judge
		if(Lat.DATASET[i] < -90 || Lat.DATASET[i] > 90 || Lon.DATASET[i] <= -180 || Lon.DATASET[i] > 180)
		{
			Rrc_Rrs_data.LatLonFlag = -1;
		}

		lat[i] = Lat.DATASET[i] * 4 + 360;
		lon[i] = Lon.DATASET[i] * 4 + 720;

		lat_gezi[i] = floor(721 - lat[i]);
		lon_gezi[i] = floor(lon[i]);

		if(maxLat <= lat[i]){
			maxLat = lat[i];
		}
		if(minLat >= lat[i]){
			minLat = lat[i];
		}
		if(maxLon <= lon[i]){
			maxLon = lon[i];
		}
		if(minLon >=lon[i]){
			minLon = lon[i];
		}
	}


//	cout<<"maxLat: "<<maxLat<<endl;
//	cout<<"minLat: "<<minLat<<endl;
//	cout<<"maxLon: "<<maxLon<<endl;
//	cout<<"minLon: "<<minLon<<endl;
//
//	cout<<"lat[0]: "<<lat[0]<<endl;
//	cout<<"lon[0]: "<<lon[0]<<endl;
//	cout<<"lat_gezi[0]: "<<lat_gezi[0]<<endl;
//	cout<<"lon_gezi[0]: "<<lon_gezi[0]<<endl;

	//瑞利表
	float **LUT_R_B1;
	float **LUT_R_B2;
	float **LUT_R_B3;
	float **LUT_R_B4;
	float lat_angle = abs(maxLat);

	if(lat_angle <= 30)
	{
		LUT_R_B1 = read_txt(PATH_LUT_TR_B1, 1935, 7,_poExHand);//热带
		LUT_R_B2 = read_txt(PATH_LUT_TR_B2, 1935, 7,_poExHand);
		LUT_R_B3 = read_txt(PATH_LUT_TR_B3, 1935, 7,_poExHand);
		LUT_R_B4 = read_txt(PATH_LUT_TR_B4, 1935, 7,_poExHand);
	}
	else if((lat_angle > 30 && lat_angle <= 60) && (month >= 3 && month <= 8)){
		LUT_R_B1 = read_txt(PATH_LUT_MS_B1, 1935, 7,_poExHand);//中纬度夏季
		LUT_R_B2 = read_txt(PATH_LUT_MS_B2, 1935, 7,_poExHand);
		LUT_R_B3 = read_txt(PATH_LUT_MS_B3, 1935, 7,_poExHand);
		LUT_R_B4 = read_txt(PATH_LUT_MS_B4, 1935, 7,_poExHand);
	}
	else if((lat_angle > 30 && lat_angle <= 60) && (month >=9 || month <=2)){
		LUT_R_B1 = read_txt(PATH_LUT_MW_B1, 1935, 7,_poExHand);//中纬度冬季
		LUT_R_B2 = read_txt(PATH_LUT_MW_B2, 1935, 7,_poExHand);
		LUT_R_B3 = read_txt(PATH_LUT_MW_B3, 1935, 7,_poExHand);
		LUT_R_B4 = read_txt(PATH_LUT_MW_B4, 1935, 7,_poExHand);
	}
	else if(lat_angle > 60){
		LUT_R_B1 = read_txt(PATH_LUT_US_B1, 1935, 7,_poExHand);//US1962
		LUT_R_B2 = read_txt(PATH_LUT_US_B2, 1935, 7,_poExHand);
		LUT_R_B3 = read_txt(PATH_LUT_US_B3, 1935, 7,_poExHand);
		LUT_R_B4 = read_txt(PATH_LUT_US_B4, 1935, 7,_poExHand);
	}
	else{
		_poExHand->sendInfoMessage("There is no conditional Rayleigh table.", CLOG_NO_PRODUCT);
	}

//	cout<<"LUT_R_B1[10][1]: "<<LUT_R_B1[10][1]<<endl;
//	cout<<"LUT_R_B2[10][1]: "<<LUT_R_B1[10][1]<<endl;
//	cout<<"LUT_R_B3[10][1]: "<<LUT_R_B1[10][1]<<endl;
//	cout<<"LUT_R_B4[10][1]: "<<LUT_R_B1[10][1]<<endl;

	if(LUT_R_B1 == NULL || LUT_R_B2 == NULL || LUT_R_B3 == NULL || LUT_R_B4 == NULL)
	{
		_poExHand->sendInfoMessage("Rayleigh table read failed!", CLOG_NO_PRODUCT);
		exit(0);
	}


//******************************************************分别计算四个波段的反射率******************************************************

	/*
	 * initialize variable
	 */

	float pi = 3.1415926;

	float Tg_O2_B3, Tg_O2_B4;//氧气的气体吸收透过率
	float O2_B3_r0 = coef_O2_B3[0][0];//B3氧气查找表的前四个值
	float O2_B3_r1 = coef_O2_B3[0][1];
	float O2_B3_r2 = coef_O2_B3[0][2];
	float O2_B3_r3 = coef_O2_B3[0][3];
	float O2_B4_r0 = coef_O2_B4[0][0];//B4氧气查找表的前四个值
	float O2_B4_r1 = coef_O2_B4[0][1];
	float O2_B4_r2 = coef_O2_B4[0][2];
	float O2_B4_r3 = coef_O2_B4[0][3];

	float Tg_water_B2, Tg_water_B3, Tg_water_B4;//水汽的气体吸收透过率
	float water_B2_r0 = coef_water_B2[0][0];//B2水汽查找表的前两个值
	float water_B2_r1 = coef_water_B2[0][1];
	float water_B3_r0 = coef_water_B3[0][0];//B3水汽查找表的前两个值
	float water_B3_r1 = coef_water_B3[0][1];
	float water_B4_r0 = coef_water_B4[0][0];//B4水汽查找表的前两个值
	float water_B4_r1 = coef_water_B4[0][1];

	float Tg_O3_B1, Tg_O3_B2, Tg_O3_B3, Tg_O3_B4;//臭氧的气体吸收透过率
	float O3_B1_r0 = coef_O3_B1[0][0];//B1臭氧查找表的前四个值
	float O3_B1_r1 = coef_O3_B1[0][1];
	float O3_B2_r0 = coef_O3_B2[0][0];//B2臭氧查找表的前四个值
	float O3_B2_r1 = coef_O3_B2[0][1];
	float O3_B3_r0 = coef_O3_B3[0][0];//B3臭氧查找表的前四个值
	float O3_B3_r1 = coef_O3_B3[0][1];
	float O3_B4_r0 = coef_O3_B4[0][0];//B4臭氧查找表的前四个值
	float O3_B4_r1 = coef_O3_B4[0][1];

	float B1 = 0.490;//中心波长,4个
	float B2 = 0.585;
	float B3 = 0.685;
	float B4 = 0.805;

	float p0 = 101.325;//标准大气压

	float *aerosol_gamma_B3 = new float[11];  //用于存放11个气溶胶文件的gamma_B3
	judgeMemoryApplication(aerosol_gamma_B3,_poExHand,"aerosol_gamma_B3");

	_poExHand->sendInfoMessage("Start AtmosphericCorrection by pixel", CLOG_NO_PRODUCT);

//try
//#pragma omp parallel for num_threads(4)
	//利用for()循环，逐像素进行操作
	for(int j = 0; j <  height * width; j++)
//    for(int j = 0; j < 1 * width; j++) //TODO for
	{
//		if(j%height == 0){
//			cout<<"for: "<<j<<endl;
//		}

		if(CloudDetectData.DATASET[j] + LandMaskData.DATASET[j] == 0 )
		{
			if((Lat.DATASET[j] >= -90 && Lat.DATASET[j] <= 90 && Lon.DATASET[j] > -180 && Lon.DATASET[j] <= 180) &&
					(DataSet.BLUE[j]>0) && (DataSet.GREEN[j]>0) && (DataSet.NIR[j]>0) && (DataSet.RED[j]>0))
			{
				int a = lat_gezi[j];
				int b = lon_gezi[j];

				float z_wind_czi_pipei = h4_NCEP[0][a - 1][b - 1];
				float m_wind_czi_pipei = h4_NCEP[1][a - 1][b - 1];
				float press_czi_pipei = h4_NCEP[2][a - 1][b - 1] / 10;
				float Uwater_czi_pipei = h4_NCEP[3][a - 1][b - 1] / 10;
				float UO3_czi_pipei = h4_NCEP[4][a - 1][b - 1] / 1000;

	//			cout<<"z_wind_czi_pipei: "<<z_wind_czi_pipei<<endl;
	//			cout<<"m_wind_czi_pipei: "<<m_wind_czi_pipei<<endl;
	//			cout<<"press_czi_pipei: "<<press_czi_pipei<<endl;
	//			cout<<"Uwater_czi_pipei: "<<Uwater_czi_pipei<<endl;
	//			cout<<"UO3_czi_pipei: "<<UO3_czi_pipei<<endl;

				float theta0 = SunZenithData.DATASET[j];		 //太阳天顶角
				float thetaV = SatelliteZenithData.DATASET[j];	 //卫星天顶角
				float varphi_s = SunAzimuthData.DATASET[j];		 //太阳方位角
				float SatAData = SatelliteAzimuthData.DATASET[j];//卫星方位角
//				cout<<"theta0: "<<theta0<<endl;
//				cout<<"thetaV: "<<thetaV<<endl;
				if (SunZenithData.DATASET[j]<0 	      || SunZenithData.DATASET[j]>86 || isnan(SunZenithData.DATASET[j]) == 1 ||
						SatelliteZenithData.DATASET[j]<0  || SatelliteZenithData.DATASET[j]>86 || isnan(SatelliteZenithData.DATASET[j]) == 1 ||
						SunAzimuthData.DATASET[j]<0       || SunAzimuthData.DATASET[j]>360|| isnan(SunAzimuthData.DATASET[j]) == 1 ||
						SatelliteAzimuthData.DATASET[j]<0 || SatelliteAzimuthData.DATASET[j]>360 || isnan(SatelliteAzimuthData.DATASET[j]) == 1)
				{
//					_poExHand->sendErrorMessage("Angle's value is error!", "Atmospheric_Correction.cpp, 440 line", "L1B Navigation Angle error!", iRet);
					Rrc_Rrs_data.AngleFlag = -1;

				}

				float deltaphi = abs(varphi_s - SatAData);	//deltaphi = 太阳方位角 - 卫星方位角
				if (deltaphi > 180)
				{
					deltaphi = 360 - deltaphi;
				}

//				float rho_blue =(pi * DataSet.BLUE[j] / (194.0544 * cos(theta0 * pi / 180))) / 10;//波段：蓝的反射率 TODO /10 -> *10
//				float rho_green =(pi * DataSet.GREEN[j] / (181.3399 * cos(theta0 * pi / 180))) / 10;//波段：绿的反射率
//				float rho_red =(pi * DataSet.RED[j] / (156.4845 * cos(theta0 * pi / 180))) / 10;//波段：红的反射率
//				float rho_nir =(pi * DataSet.NIR[j] / (106.5948 * cos(theta0 * pi / 180))) / 10;//波段：近红外的反射率
				float rho_blue =(pi * DataSet.BLUE[j] / (194.0544 * cos(theta0 * pi / 180)));//波段：蓝的反射率 TODO /10 -> *10
				float rho_green =(pi * DataSet.GREEN[j] / (181.3399 * cos(theta0 * pi / 180)));//波段：绿的反射率
				float rho_red =(pi * DataSet.RED[j] / (156.4845 * cos(theta0 * pi / 180)));//波段：红的反射率
				float rho_nir =(pi * DataSet.NIR[j] / (106.5948 * cos(theta0 * pi / 180)));//波段：近红外的反射率

	//------------------------------------------------------Rayleigh Searching------------------------------------------------------

				float rho_r1 = RayleighSearch_2d(LUT_R_B1, theta0, thetaV, deltaphi, pi, _poExHand);
				float rho_r2 = RayleighSearch_2d(LUT_R_B2, theta0, thetaV, deltaphi, pi, _poExHand);
				float rho_r3 = RayleighSearch_2d(LUT_R_B3, theta0, thetaV, deltaphi, pi, _poExHand);
				float rho_r4 = RayleighSearch_2d(LUT_R_B4, theta0, thetaV, deltaphi, pi, _poExHand);

				// Rayleigh表查找
	//			float **pIndex_B1_a = RayleighSearch_2d(LUT_R_B1, theta0, thetaV);
	//			float **pIndex_B2_a = RayleighSearch_2d(LUT_R_B2, theta0, thetaV);
	//			float **pIndex_B3_a = RayleighSearch_2d(LUT_R_B3, theta0, thetaV);
	//			float **pIndex_B4_a = RayleighSearch_2d(LUT_R_B4, theta0, thetaV);
	//
	//
	////------------------------------------------------------Bilinear Interpolation----------------------------------------------------
	//
	//			// 二维线性插值（计算4个波段分别对应的3个系数）
	//			float* B1_rho = Linear_Interpolation_2D(pIndex_B1_a, theta0, thetaV);
	//			float* B2_rho = Linear_Interpolation_2D(pIndex_B2_a, theta0, thetaV);
	//			float* B3_rho = Linear_Interpolation_2D(pIndex_B3_a, theta0, thetaV);
	//			float* B4_rho = Linear_Interpolation_2D(pIndex_B4_a, theta0, thetaV);
	//
	//
	//			// 每个像素计算4个波段瑞利散射反射率
	//			float rho_r1 = B1_rho[0] + 2 * B1_rho[1] * cos(deltaphi * pi / 180) + 2 * B1_rho[2] * cos(2 * deltaphi * pi / 180);
	//			float rho_r2 = B2_rho[0] + 2 * B2_rho[1] * cos(deltaphi * pi / 180) + 2 * B2_rho[2] * cos(2 * deltaphi * pi / 180);
	//			float rho_r3 = B3_rho[0] + 2 * B3_rho[1] * cos(deltaphi * pi / 180) + 2 * B3_rho[2] * cos(2 * deltaphi * pi / 180);
	//			float rho_r4 = B4_rho[0] + 2 * B4_rho[1] * cos(deltaphi * pi / 180) + 2 * B4_rho[2] * cos(2 * deltaphi * pi / 180);

	//--------------------------------------------------------气体吸收透过率*----------------------------------------------------------

				float airmass = 1 / cos(theta0 * pi / 180) + 1 / cos(thetaV * pi / 180);

				Tg_O2_B3 = O2_B3_r0 + O2_B3_r1 * airmass + O2_B3_r2 * pow(airmass, 2) + O2_B3_r3 * pow(airmass, 3);//氧气的气体吸收透过率
				Tg_O2_B4 = O2_B4_r0 + O2_B4_r1 * airmass + O2_B4_r2 * pow(airmass, 2) + O2_B4_r3 * pow(airmass, 3);

	//			float UWater = 0.4655;  /* need to read from NCEP data */
	//			float UO3 = 0.3747;  /* need to read from NCEP data */

				Tg_water_B2 = exp(-exp(water_B2_r0 + water_B2_r1 * log(airmass * Uwater_czi_pipei)));//水汽的气体吸收透过率
				Tg_water_B3 = exp(-exp(water_B3_r0 + water_B3_r1 * log(airmass * Uwater_czi_pipei)));
				Tg_water_B4 = exp(-exp(water_B4_r0 + water_B4_r1 * log(airmass * Uwater_czi_pipei)));

				Tg_O3_B1 = exp(-exp(O3_B1_r0 + O3_B1_r1 * log(airmass * UO3_czi_pipei)));//臭氧的气体吸收透过率
				Tg_O3_B2 = exp(-exp(O3_B2_r0 + O3_B2_r1 * log(airmass * UO3_czi_pipei)));
				Tg_O3_B3 = exp(-exp(O3_B3_r0 + O3_B3_r1 * log(airmass * UO3_czi_pipei)));
				Tg_O3_B4 = exp(-exp(O3_B4_r0 + O3_B4_r1 * log(airmass * UO3_czi_pipei)));

				//校正的瑞利反射率
				float correctRB1 = rho_r1 * Tg_O3_B1;//臭氧的第一个波段
				float correctRB2 = rho_r2 * Tg_O3_B2;//臭氧的第二个波段
				float correctRB3 = rho_r3 * Tg_O3_B3 * Tg_O2_B3;//臭氧的第三个波段 和 氧气的第三个波段
				float correctRB4 = rho_r4 * Tg_O3_B4 * Tg_O2_B4;//臭氧的第四个波段 和 氧气的第四个波段

	//-----------------------------------------------------------白帽计算---------------------------------------------------------------------

				//白帽计算
	//			float z_wind = 0.8362;  /* need to read from NCEP data */
	//			float m_wind = -1.9716;  /* need to read from NCEP data */
				float W =sqrt(pow(z_wind_czi_pipei, 2) + pow(m_wind_czi_pipei, 2));//每个像素的风速

				float rho_wc = 6.49 * pow(10, -7) * pow(W, 3.52);

	//---------------------------------------------------------气溶胶反射率计算---------------------------------------------------------------------

				//气溶胶反射率计算
				float taorB1 = 0.008569 * pow(B1, -4) * (1 + 0.0113 * pow(B1, -2) + 0.00013 * pow(B1, -4));//每个波长的瑞利光学厚度
				float taorB2 = 0.008569 * pow(B2, -4) * (1 + 0.0113 * pow(B2, -2) + 0.00013 * pow(B2, -4));
				float taorB3 = 0.008569 * pow(B3, -4) * (1 + 0.0113 * pow(B3, -2) + 0.00013 * pow(B3, -4));
				float taorB4 = 0.008569 * pow(B4, -4) * (1 + 0.0113 * pow(B4, -2) + 0.00013 * pow(B4, -4));

				//第1波段的大气压校正的混合反射率
				float xB1 = (-(0.6543 - 1.608 * taorB1) + (0.8192 - 1.2541 * taorB1) * log(airmass)) * taorB1 * airmass;
				float facB1 = (1.0 - exp(-xB1 * press_czi_pipei / p0)) / (1.0 - exp(-xB1));

				//第2波段的大气压校正的混合反射率
				float xB2 = (-(0.6543 - 1.608 * taorB2) + (0.8192 - 1.2541 * taorB2) * log(airmass)) * taorB2 * airmass;
				float facB2 = (1.0 - exp(-xB2 * press_czi_pipei / p0)) / (1.0 - exp(-xB2));

				//第三波段的大气压校正的混合反射率
	//			float press = 102.1907;/* 辅助数据大气压信息 need to read from NCEP data */
				float xB3 = (-(0.6543 - 1.608 * taorB3) + (0.8192 - 1.2541 * taorB3) * log(airmass)) * taorB3 * airmass;
				float facB3 = (1.0 - exp(-xB3 * press_czi_pipei / p0)) / (1.0 - exp(-xB3));
				float rou_mixB3 = (rho_red - correctRB3 + correctRB3 * Tg_water_B3) / (Tg_O3_B3 * Tg_O2_B3 * Tg_water_B3);
				rou_mixB3 = rou_mixB3 * facB3;

				//第四波段的大气压校正的混合反射率
				float xB4 = (-(0.6543 - 1.608 * taorB4) + (0.8192 - 1.2541 * taorB4) * log(airmass)) * taorB4 * airmass;
				float facB4 = (1.0 - exp(-xB4 * press_czi_pipei / p0)) / (1.0 - exp(-xB4));
				float rou_mixB4 = (rho_nir - correctRB4 + correctRB4 * Tg_water_B4) / (Tg_O3_B4 * Tg_O2_B4 * Tg_water_B4);
				rou_mixB4 = rou_mixB4 * facB4;

				//求实际的Gamma
				float gamma_B3 = rou_mixB3 / rho_r3;
				float gamma_B4 = rou_mixB4 / rho_r4;

				// raylei查找，3维线性插值

				float** Rn_city_aeo = RayleighSearch_3d_bands(city_aeo, theta0, thetaV, deltaphi,_poExHand);// raylei查找，3维线性插值，返回4个波段的rho_0,rho_1,rho_2,存放在2维数组中
				float** Rn_pureSea_aeo = RayleighSearch_3d_bands(pureSea_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_sea_aeo = RayleighSearch_3d_bands(sea_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_land_aeo = RayleighSearch_3d_bands(land_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_desert_aeo = RayleighSearch_3d_bands(desert_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_isothermal_aeo = RayleighSearch_3d_bands(isothermal_aeo, theta0, thetaV, deltaphi,_poExHand);

				float** Rn_sea_1_aeo = RayleighSearch_3d_bands(sea_1_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_sea_2_aeo = RayleighSearch_3d_bands(sea_2_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_sea_3_aeo = RayleighSearch_3d_bands(sea_3_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_sea_4_aeo = RayleighSearch_3d_bands(sea_4_aeo, theta0, thetaV, deltaphi,_poExHand);
				float** Rn_sea_5_aeo = RayleighSearch_3d_bands(sea_5_aeo, theta0, thetaV, deltaphi,_poExHand);

				//定义一个二维数组，第一行存放真实与计算的gamma距离，第二行存放索引 +1， -1, 0
				float **aerosol_dist = new float*[2];

				for (int i = 0; i < 2; i++)
				{
					aerosol_dist[i] = new float[11];
					if(aerosol_dist[i] == NULL)
					{
						_poExHand->sendInfoMessage("aerosol_dist's point memory application failed!", CLOG_NO_PRODUCT);
						exit(0);
					}
				}
				int gamma_sign = 0;//标记,用于标记是否无解


	//*************************************************************************************************************************

				//城镇文件夹(一共11个这样的文件夹)
				float   city_r0_B1 = Rn_city_aeo[0][0], city_r1_B1 = Rn_city_aeo[0][1], city_r2_B1 = Rn_city_aeo[0][2],
						city_r0_B2 = Rn_city_aeo[1][0], city_r1_B2 = Rn_city_aeo[1][1], city_r2_B2 = Rn_city_aeo[1][2],
						city_r0_B3 = Rn_city_aeo[2][0], city_r1_B3 = Rn_city_aeo[2][1], city_r2_B3 = Rn_city_aeo[2][2],
						city_r0_B4 = Rn_city_aeo[3][0], city_r1_B4 = Rn_city_aeo[3][1], city_r2_B4 = Rn_city_aeo[3][2],
						city_taua_B1, city_taua_B2, city_taua_B3, city_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float city_delta_tmp = pow(city_r1_B4, 2) - 4 * city_r2_B4 * (city_r0_B4 - gamma_B4);
				if(city_delta_tmp >= 0)
				{
					city_taua_B4 = ((-city_r1_B4) + sqrt(city_delta_tmp)) / (2 * city_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					city_taua_B1 = city_taua_B4 * city_delta[0][0];
					city_taua_B2 = city_taua_B4 * city_delta[0][1];
					city_taua_B3 = city_taua_B4 * city_delta[0][2];
	//				city_taua_B1 = city_taua_B4 * city_delta[0][2];
	//				city_taua_B2 = city_taua_B4 * city_delta[0][1];
	//				city_taua_B3 = city_taua_B4 * city_delta[0][0];

					aerosol_gamma_B3[0] = city_r0_B3 + city_r1_B3 * city_taua_B3 + city_r2_B3 * pow(city_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][0] = aerosol_gamma_B3[0] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][0] > 0)
					{
						aerosol_dist[1][0] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][0] < 0)
					{
						aerosol_dist[1][0] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][0] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][0] = 0;//索引设为0
					gamma_sign += 1;
				}

		//*************************************************************************************************************************
				//纯海文件夹
				float   pureSea_r0_B1 = Rn_pureSea_aeo[0][0], pureSea_r1_B1 = Rn_pureSea_aeo[0][1], pureSea_r2_B1 = Rn_pureSea_aeo[0][2],
						pureSea_r0_B2 = Rn_pureSea_aeo[1][0], pureSea_r1_B2 = Rn_pureSea_aeo[1][1], pureSea_r2_B2 = Rn_pureSea_aeo[1][2],
						pureSea_r0_B3 = Rn_pureSea_aeo[2][0], pureSea_r1_B3 = Rn_pureSea_aeo[2][1], pureSea_r2_B3 = Rn_pureSea_aeo[2][2],
						pureSea_r0_B4 = Rn_pureSea_aeo[3][0], pureSea_r1_B4 = Rn_pureSea_aeo[3][1], pureSea_r2_B4 = Rn_pureSea_aeo[3][2],
						pureSea_taua_B1, pureSea_taua_B2, pureSea_taua_B3, pureSea_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float pureSea_delta_tmp = pow(pureSea_r1_B4, 2) - 4 * pureSea_r2_B4 * (pureSea_r0_B4 - gamma_B4);

				if(pureSea_delta_tmp >= 0)
				{
					pureSea_taua_B4 = ((-pureSea_r1_B4) + sqrt(pureSea_delta_tmp)) / (2 * pureSea_r2_B4);//一元二次方程的求根公式

					//纯海文件夹下的delta文件
					pureSea_taua_B1 = pureSea_taua_B4 * pureSea_delta[0][0];
					pureSea_taua_B2 = pureSea_taua_B4 * pureSea_delta[0][1];
					pureSea_taua_B3 = pureSea_taua_B4 * pureSea_delta[0][2];
	//				pureSea_taua_B1 = pureSea_taua_B4 * pureSea_delta[0][2];
	//				pureSea_taua_B2 = pureSea_taua_B4 * pureSea_delta[0][1];
	//				pureSea_taua_B3 = pureSea_taua_B4 * pureSea_delta[0][0];

					aerosol_gamma_B3[1] = pureSea_r0_B3 + pureSea_r1_B3 * pureSea_taua_B3 + pureSea_r2_B3 * pow(pureSea_taua_B3, 2);//计算纯海文件的gamma_B3
					aerosol_dist[0][1] = aerosol_gamma_B3[1] - gamma_B3;

					if(aerosol_dist[0][1] > 0)
					{
						aerosol_dist[1][1] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][1] < 0)
					{
						aerosol_dist[1][1] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正，纯海)"<<endl;
					aerosol_dist[0][1] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][1] = 0;//索引设为0
					gamma_sign += 1;
				}

		//*************************************************************************************************************************
				//海洋文件夹
				float   sea_r0_B1 = Rn_sea_aeo[0][0], sea_r1_B1 = Rn_sea_aeo[0][1], sea_r2_B1 = Rn_sea_aeo[0][2],
						sea_r0_B2 = Rn_sea_aeo[1][0], sea_r1_B2 = Rn_sea_aeo[1][1], sea_r2_B2 = Rn_sea_aeo[1][2],
						sea_r0_B3 = Rn_sea_aeo[2][0], sea_r1_B3 = Rn_sea_aeo[2][1], sea_r2_B3 = Rn_sea_aeo[2][2],
						sea_r0_B4 = Rn_sea_aeo[3][0], sea_r1_B4 = Rn_sea_aeo[3][1], sea_r2_B4 = Rn_sea_aeo[3][2],
						sea_taua_B1, sea_taua_B2, sea_taua_B3, sea_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_delta_tmp = pow(sea_r1_B4, 2) - 4 * sea_r2_B4 * (sea_r0_B4 - gamma_B4);
				if(sea_delta_tmp >= 0)
				{
					sea_taua_B4 = ((-sea_r1_B4) + sqrt(sea_delta_tmp)) / (2 * sea_r2_B4);//一元二次方程的求根公式

					//海洋文件夹下的delta文件
					sea_taua_B1 = sea_taua_B4 * sea_delta[0][0];
					sea_taua_B2 = sea_taua_B4 * sea_delta[0][1];
					sea_taua_B3 = sea_taua_B4 * sea_delta[0][2];
	//				sea_taua_B1 = sea_taua_B4 * sea_delta[0][2];
	//				sea_taua_B2 = sea_taua_B4 * sea_delta[0][1];
	//				sea_taua_B3 = sea_taua_B4 * sea_delta[0][0];

					aerosol_gamma_B3[2] = sea_r0_B3 + sea_r1_B3 * sea_taua_B3 + sea_r2_B3 * pow(sea_taua_B3, 2);//计算海洋文件的gamma_B3
					aerosol_dist[0][2] = aerosol_gamma_B3[2] - gamma_B3;
					if(aerosol_dist[0][2] > 0)
					{
						aerosol_dist[1][2] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][2] < 0)
					{
						aerosol_dist[1][2] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正，海洋)"<<endl;
					aerosol_dist[0][2] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][2] = 0;//索引设为0
					gamma_sign += 1;
				}

		//*************************************************************************************************************************
				//陆地文件夹
				float   land_r0_B1 = Rn_land_aeo[0][0], land_r1_B1 = Rn_land_aeo[0][1], land_r2_B1 = Rn_land_aeo[0][2],
						land_r0_B2 = Rn_land_aeo[1][0], land_r1_B2 = Rn_land_aeo[1][1], land_r2_B2 = Rn_land_aeo[1][2],
						land_r0_B3 = Rn_land_aeo[2][0], land_r1_B3 = Rn_land_aeo[2][1], land_r2_B3 = Rn_land_aeo[2][2],
						land_r0_B4 = Rn_land_aeo[3][0], land_r1_B4 = Rn_land_aeo[3][1], land_r2_B4 = Rn_land_aeo[3][2],
						land_taua_B1, land_taua_B2, land_taua_B3, land_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float land_delta_tmp = pow(land_r1_B4, 2) - 4 * land_r2_B4 * (land_r0_B4 - gamma_B4);
				if(land_delta_tmp >= 0)
				{
					land_taua_B4 = ((-land_r1_B4) + sqrt(land_delta_tmp)) / (2 * land_r2_B4);//一元二次方程的求根公式

					//陆地文件夹下的delta文件
					land_taua_B1 = land_taua_B4 * land_delta[0][0];
					land_taua_B2 = land_taua_B4 * land_delta[0][1];
					land_taua_B3 = land_taua_B4 * land_delta[0][2];
	//				land_taua_B1 = land_taua_B4 * land_delta[0][2];
	//				land_taua_B2 = land_taua_B4 * land_delta[0][1];
	//				land_taua_B3 = land_taua_B4 * land_delta[0][0];

					aerosol_gamma_B3[3] = land_r0_B3 + land_r1_B3 * land_taua_B3 + land_r2_B3 * pow(land_taua_B3, 2);//计算陆地文件的gamma_B3
					aerosol_dist[0][3] = aerosol_gamma_B3[3] - gamma_B3;
					if(aerosol_dist[0][3] > 0)
					{
						aerosol_dist[1][3] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][3] < 0)
					{
						aerosol_dist[1][3] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正，陆地)"<<endl;
					aerosol_dist[0][3] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][3] = 0;//索引设为0
					gamma_sign += 1;
				}

		//*************************************************************************************************************************
				//沙漠文件夹
				float   desert_r0_B1 = Rn_desert_aeo[0][0], desert_r1_B1 = Rn_desert_aeo[0][1], desert_r2_B1 = Rn_desert_aeo[0][2],
						desert_r0_B2 = Rn_desert_aeo[1][0], desert_r1_B2 = Rn_desert_aeo[1][1], desert_r2_B2 = Rn_desert_aeo[1][2],
						desert_r0_B3 = Rn_desert_aeo[2][0], desert_r1_B3 = Rn_desert_aeo[2][1], desert_r2_B3 = Rn_desert_aeo[2][2],
						desert_r0_B4 = Rn_desert_aeo[3][0], desert_r1_B4 = Rn_desert_aeo[3][1], desert_r2_B4 = Rn_desert_aeo[3][2],
						desert_taua_B1, desert_taua_B2, desert_taua_B3, desert_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float desert_delta_tmp = pow(desert_r1_B4, 2) - 4 * desert_r2_B4 * (desert_r0_B4 - gamma_B4);
				if(desert_delta_tmp >= 0)
				{
					desert_taua_B4 = ((-desert_r1_B4) + sqrt(desert_delta_tmp)) / (2 * desert_r2_B4);//一元二次方程的求根公式

					//沙漠文件夹下的delta文件
					desert_taua_B1 = desert_taua_B4 * desert_delta[0][0];
					desert_taua_B2 = desert_taua_B4 * desert_delta[0][1];
					desert_taua_B3 = desert_taua_B4 * desert_delta[0][2];
	//				desert_taua_B1 = desert_taua_B4 * desert_delta[0][2];
	//				desert_taua_B2 = desert_taua_B4 * desert_delta[0][1];
	//				desert_taua_B3 = desert_taua_B4 * desert_delta[0][0];

					aerosol_gamma_B3[4] = desert_r0_B3 + desert_r1_B3 * desert_taua_B3 + desert_r2_B3 * pow(desert_taua_B3, 2);//计算沙漠文件的gamma_B3
					aerosol_dist[0][4] = aerosol_gamma_B3[4] - gamma_B3;
					if(aerosol_dist[0][4] > 0)
					{
						aerosol_dist[1][4] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][4] < 0)
					{
						aerosol_dist[1][4] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正，沙漠)"<<endl;
					aerosol_dist[0][4] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][4] = 0;//索引设为0
					gamma_sign += 1;
				}

		//*************************************************************************************************************************
				//同温文件夹
				float   isothermal_r0_B1 = Rn_isothermal_aeo[0][0], isothermal_r1_B1 = Rn_isothermal_aeo[0][1], isothermal_r2_B1 = Rn_isothermal_aeo[0][2],
						isothermal_r0_B2 = Rn_isothermal_aeo[1][0], isothermal_r1_B2 = Rn_isothermal_aeo[1][1], isothermal_r2_B2 = Rn_isothermal_aeo[1][2],
						isothermal_r0_B3 = Rn_isothermal_aeo[2][0], isothermal_r1_B3 = Rn_isothermal_aeo[2][1], isothermal_r2_B3 = Rn_isothermal_aeo[2][2],
						isothermal_r0_B4 = Rn_isothermal_aeo[3][0], isothermal_r1_B4 = Rn_isothermal_aeo[3][1], isothermal_r2_B4 = Rn_isothermal_aeo[3][2],
						isothermal_taua_B1, isothermal_taua_B2, isothermal_taua_B3, isothermal_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float isothermal_delta_tmp = pow(isothermal_r1_B4, 2) - 4 * isothermal_r2_B4 * (isothermal_r0_B4 - gamma_B4);
				if(isothermal_delta_tmp >= 0)
				{
					isothermal_taua_B4 = ((-isothermal_r1_B4) + sqrt(isothermal_delta_tmp)) / (2 * isothermal_r2_B4);//一元二次方程的求根公式

					//同温文件夹下的delta文件
					isothermal_taua_B1 = isothermal_taua_B4 * isothermal_delta[0][0];
					isothermal_taua_B2 = isothermal_taua_B4 * isothermal_delta[0][1];
					isothermal_taua_B3 = isothermal_taua_B4 * isothermal_delta[0][2];
	//				isothermal_taua_B1 = isothermal_taua_B4 * isothermal_delta[0][2];
	//				isothermal_taua_B2 = isothermal_taua_B4 * isothermal_delta[0][1];
	//				isothermal_taua_B3 = isothermal_taua_B4 * isothermal_delta[0][0];

					aerosol_gamma_B3[5] = isothermal_r0_B3 + isothermal_r1_B3 * isothermal_taua_B3 + isothermal_r2_B3 * pow(isothermal_taua_B3, 2);//计算同温文件的gamma_B3
					aerosol_dist[0][5] = aerosol_gamma_B3[5] - gamma_B3;
					if(aerosol_dist[0][5] > 0)
					{
						aerosol_dist[1][5] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][5] < 0)
					{
						aerosol_dist[1][5] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正，同温)"<<endl;
					aerosol_dist[0][5] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][5] = 0;//索引设为0
					gamma_sign += 1;
				}

	//*************************************************************************************************************************

				//sea_1文件夹(一共11个这样的文件夹)
				float   sea_1_r0_B1 = Rn_sea_1_aeo[0][0], sea_1_r1_B1 = Rn_sea_1_aeo[0][1], sea_1_r2_B1 = Rn_sea_1_aeo[0][2],
						sea_1_r0_B2 = Rn_sea_1_aeo[1][0], sea_1_r1_B2 = Rn_sea_1_aeo[1][1], sea_1_r2_B2 = Rn_sea_1_aeo[1][2],
						sea_1_r0_B3 = Rn_sea_1_aeo[2][0], sea_1_r1_B3 = Rn_sea_1_aeo[2][1], sea_1_r2_B3 = Rn_sea_1_aeo[2][2],
						sea_1_r0_B4 = Rn_sea_1_aeo[3][0], sea_1_r1_B4 = Rn_sea_1_aeo[3][1], sea_1_r2_B4 = Rn_sea_1_aeo[3][2],
						sea_1_taua_B1, sea_1_taua_B2, sea_1_taua_B3, sea_1_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_1_delta_tmp = pow(sea_1_r1_B4, 2) - 4 * sea_1_r2_B4 * (sea_1_r0_B4 - gamma_B4);
				if(sea_1_delta_tmp >= 0)
				{
					sea_1_taua_B4 = ((-sea_1_r1_B4) + sqrt(sea_1_delta_tmp)) / (2 * sea_1_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					sea_1_taua_B1 = sea_1_taua_B4 * sea_1_delta[0][0];
					sea_1_taua_B2 = sea_1_taua_B4 * sea_1_delta[0][1];
					sea_1_taua_B3 = sea_1_taua_B4 * sea_1_delta[0][2];
	//				sea_1_taua_B1 = sea_1_taua_B4 * sea_1_delta[0][2];
	//				sea_1_taua_B2 = sea_1_taua_B4 * sea_1_delta[0][1];
	//				sea_1_taua_B3 = sea_1_taua_B4 * sea_1_delta[0][0];

					aerosol_gamma_B3[6] = sea_1_r0_B3 + sea_1_r1_B3 * sea_1_taua_B3 + sea_1_r2_B3 * pow(sea_1_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][6] = aerosol_gamma_B3[6] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][6] > 0)
					{
						aerosol_dist[1][6] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][6] < 0)
					{
						aerosol_dist[1][6] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][6] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][6] = 0;//索引设为0
					gamma_sign += 1;
				}

	//*************************************************************************************************************************

				//sea_2文件夹(一共11个这样的文件夹)
				float   sea_2_r0_B1 = Rn_sea_2_aeo[0][0], sea_2_r1_B1 = Rn_sea_2_aeo[0][1], sea_2_r2_B1 = Rn_sea_2_aeo[0][2],
						sea_2_r0_B2 = Rn_sea_2_aeo[1][0], sea_2_r1_B2 = Rn_sea_2_aeo[1][1], sea_2_r2_B2 = Rn_sea_2_aeo[1][2],
						sea_2_r0_B3 = Rn_sea_2_aeo[2][0], sea_2_r1_B3 = Rn_sea_2_aeo[2][1], sea_2_r2_B3 = Rn_sea_2_aeo[2][2],
						sea_2_r0_B4 = Rn_sea_2_aeo[3][0], sea_2_r1_B4 = Rn_sea_2_aeo[3][1], sea_2_r2_B4 = Rn_sea_2_aeo[3][2],
						sea_2_taua_B1, sea_2_taua_B2, sea_2_taua_B3, sea_2_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_2_delta_tmp = pow(sea_2_r1_B4, 2) - 4 * sea_2_r2_B4 * (sea_2_r0_B4 - gamma_B4);
				if(sea_2_delta_tmp >= 0)
				{
					sea_2_taua_B4 = ((-sea_2_r1_B4) + sqrt(sea_2_delta_tmp)) / (2 * sea_2_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					sea_2_taua_B1 = sea_2_taua_B4 * sea_2_delta[0][0];
					sea_2_taua_B2 = sea_2_taua_B4 * sea_2_delta[0][1];
					sea_2_taua_B3 = sea_2_taua_B4 * sea_2_delta[0][2];
	//				sea_2_taua_B1 = sea_2_taua_B4 * sea_2_delta[0][2];
	//				sea_2_taua_B2 = sea_2_taua_B4 * sea_2_delta[0][1];
	//				sea_2_taua_B3 = sea_2_taua_B4 * sea_2_delta[0][0];

					aerosol_gamma_B3[7] = sea_2_r0_B3 + sea_2_r1_B3 * sea_2_taua_B3 + sea_2_r2_B3 * pow(sea_2_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][7] = aerosol_gamma_B3[7] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][7] > 0)
					{
						aerosol_dist[1][7] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][7] < 0)
					{
						aerosol_dist[1][7] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][7] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][7] = 0;//索引设为0
					gamma_sign += 1;
				}

	//*************************************************************************************************************************

				//sea_3文件夹(一共11个这样的文件夹)
				float   sea_3_r0_B1 = Rn_sea_3_aeo[0][0], sea_3_r1_B1 = Rn_sea_3_aeo[0][1], sea_3_r2_B1 = Rn_sea_3_aeo[0][2],
						sea_3_r0_B2 = Rn_sea_3_aeo[1][0], sea_3_r1_B2 = Rn_sea_3_aeo[1][1], sea_3_r2_B2 = Rn_sea_3_aeo[1][2],
						sea_3_r0_B3 = Rn_sea_3_aeo[2][0], sea_3_r1_B3 = Rn_sea_3_aeo[2][1], sea_3_r2_B3 = Rn_sea_3_aeo[2][2],
						sea_3_r0_B4 = Rn_sea_3_aeo[3][0], sea_3_r1_B4 = Rn_sea_3_aeo[3][1], sea_3_r2_B4 = Rn_sea_3_aeo[3][2],
						sea_3_taua_B1, sea_3_taua_B2, sea_3_taua_B3, sea_3_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_3_delta_tmp = pow(sea_3_r1_B4, 2) - 4 * sea_3_r2_B4 * (sea_3_r0_B4 - gamma_B4);
				if(sea_3_delta_tmp >= 0)
				{
					sea_3_taua_B4 = ((-sea_3_r1_B4) + sqrt(sea_3_delta_tmp)) / (2 * sea_3_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					sea_3_taua_B1 = sea_3_taua_B4 * sea_3_delta[0][0];
					sea_3_taua_B2 = sea_3_taua_B4 * sea_3_delta[0][1];
					sea_3_taua_B3 = sea_3_taua_B4 * sea_3_delta[0][2];
	//				sea_3_taua_B1 = sea_3_taua_B4 * sea_3_delta[0][2];
	//				sea_3_taua_B2 = sea_3_taua_B4 * sea_3_delta[0][1];
	//				sea_3_taua_B3 = sea_3_taua_B4 * sea_3_delta[0][0];

					aerosol_gamma_B3[8] = sea_3_r0_B3 + sea_3_r1_B3 * sea_3_taua_B3 + sea_3_r2_B3 * pow(sea_3_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][8] = aerosol_gamma_B3[8] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][8] > 0)
					{
						aerosol_dist[1][8] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][8] < 0)
					{
						aerosol_dist[1][8] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][8] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][8] = 0;//索引设为0
					gamma_sign += 1;
				}

	//*************************************************************************************************************************

				//sea_4文件夹(一共11个这样的文件夹)
				float   sea_4_r0_B1 = Rn_sea_4_aeo[0][0], sea_4_r1_B1 = Rn_sea_4_aeo[0][1], sea_4_r2_B1 = Rn_sea_4_aeo[0][2],
						sea_4_r0_B2 = Rn_sea_4_aeo[1][0], sea_4_r1_B2 = Rn_sea_4_aeo[1][1], sea_4_r2_B2 = Rn_sea_4_aeo[1][2],
						sea_4_r0_B3 = Rn_sea_4_aeo[2][0], sea_4_r1_B3 = Rn_sea_4_aeo[2][1], sea_4_r2_B3 = Rn_sea_4_aeo[2][2],
						sea_4_r0_B4 = Rn_sea_4_aeo[3][0], sea_4_r1_B4 = Rn_sea_4_aeo[3][1], sea_4_r2_B4 = Rn_sea_4_aeo[3][2],
						sea_4_taua_B1, sea_4_taua_B2, sea_4_taua_B3, sea_4_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_4_delta_tmp = pow(sea_4_r1_B4, 2) - 4 * sea_4_r2_B4 * (sea_4_r0_B4 - gamma_B4);
				if(sea_4_delta_tmp >= 0)
				{
					sea_4_taua_B4 = ((-sea_4_r1_B4) + sqrt(sea_4_delta_tmp)) / (2 * sea_4_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					sea_4_taua_B1 = sea_4_taua_B4 * sea_4_delta[0][0];
					sea_4_taua_B2 = sea_4_taua_B4 * sea_4_delta[0][1];
					sea_4_taua_B3 = sea_4_taua_B4 * sea_4_delta[0][2];
	//				sea_4_taua_B1 = sea_4_taua_B4 * sea_4_delta[0][2];
	//				sea_4_taua_B2 = sea_4_taua_B4 * sea_4_delta[0][1];
	//				sea_4_taua_B3 = sea_4_taua_B4 * sea_4_delta[0][0];

					aerosol_gamma_B3[9] = sea_4_r0_B3 + sea_4_r1_B3 * sea_4_taua_B3 + sea_4_r2_B3 * pow(sea_4_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][9] = aerosol_gamma_B3[9] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][9] > 0)
					{
						aerosol_dist[1][9] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][9] < 0)
					{
						aerosol_dist[1][9] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][9] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][9] = 0;//索引设为0
					gamma_sign += 1;
				}

	//*************************************************************************************************************************

				//sea_5文件夹(一共11个这样的文件夹)
				float   sea_5_r0_B1 = Rn_sea_5_aeo[0][0], sea_5_r1_B1 = Rn_sea_5_aeo[0][1], sea_5_r2_B1 = Rn_sea_5_aeo[0][2],
						sea_5_r0_B2 = Rn_sea_5_aeo[1][0], sea_5_r1_B2 = Rn_sea_5_aeo[1][1], sea_5_r2_B2 = Rn_sea_5_aeo[1][2],
						sea_5_r0_B3 = Rn_sea_5_aeo[2][0], sea_5_r1_B3 = Rn_sea_5_aeo[2][1], sea_5_r2_B3 = Rn_sea_5_aeo[2][2],
						sea_5_r0_B4 = Rn_sea_5_aeo[3][0], sea_5_r1_B4 = Rn_sea_5_aeo[3][1], sea_5_r2_B4 = Rn_sea_5_aeo[3][2],
						sea_5_taua_B1, sea_5_taua_B2, sea_5_taua_B3, sea_5_taua_B4;

				//判断方程是否有解,方程为gamma = r0 + r1 * tau + r2 * pow(tau, 2)
				float sea_5_delta_tmp = pow(sea_5_r1_B4, 2) - 4 * sea_5_r2_B4 * (sea_5_r0_B4 - gamma_B4);
				if(sea_5_delta_tmp >= 0)
				{
					sea_5_taua_B4 = ((-sea_5_r1_B4) + sqrt(sea_5_delta_tmp)) / (2 * sea_5_r2_B4);//一元二次方程的求根公式

					//城镇文件夹下的delta文件,3个数
					sea_5_taua_B1 = sea_5_taua_B4 * sea_5_delta[0][0];
					sea_5_taua_B2 = sea_5_taua_B4 * sea_5_delta[0][1];
					sea_5_taua_B3 = sea_5_taua_B4 * sea_5_delta[0][2];
	//				sea_5_taua_B1 = sea_5_taua_B4 * sea_5_delta[0][2];
	//				sea_5_taua_B2 = sea_5_taua_B4 * sea_5_delta[0][1];
	//				sea_5_taua_B3 = sea_5_taua_B4 * sea_5_delta[0][0];;

					aerosol_gamma_B3[10] = sea_5_r0_B3 + sea_5_r1_B3 * sea_5_taua_B3 + sea_5_r2_B3 * pow(sea_5_taua_B3, 2);//计算城镇文件的gamma_B3
					aerosol_dist[0][10] = aerosol_gamma_B3[10] - gamma_B3;//城镇的gamma_B3与真实gamma_B3的距离
					if(aerosol_dist[0][10] > 0)
					{
						aerosol_dist[1][10] = +1;//判断距离是正的索引
					}
					else if(aerosol_dist[0][10] < 0)
					{
						aerosol_dist[1][10] = -1;//判断距离是负的索引
					}
				}
				else
				{
	//				cout<<"Unsolved equation of one element and two times!(大气校正, 城镇)"<<endl;
					aerosol_dist[0][10] = 1000;//将距离设的很大，相当于无解
					aerosol_dist[1][10] = 0;//索引设为0
					gamma_sign += 1;
				}

				//如果11个文件求gamma_B3时都无解，则将这个像元置为0
				if(gamma_sign == 11)
				{
	//				DataSet.BLUE[j] = 0;
	//				DataSet.GREEN[j] = 0;

					/* TODO:2018-10-19 */
					Rrc_Rrs_data.Rrs_BLUE[j] = NAN_f;  //Rrs's 2 band is 0
					Rrc_Rrs_data.Rrs_GREEN[j] = NAN_f;

					Rrc_Rrs_data.Rrc_BLUE[j] = NAN_f;  //Rrc's 4 band is 0;
					Rrc_Rrs_data.Rrc_GREEN[j] = NAN_f;
					Rrc_Rrs_data.Rrc_RED[j] = NAN_f;
					Rrc_Rrs_data.Rrc_NIR[j] = NAN_f;

					continue;
				}

		//*************************************************************************************************************************
				/*
				 * 选出跟实际gamma_B3值最接近的两个gamma_B3所在的气溶胶文件
				 */

				//一维插值，通过太阳天顶角(down)及卫星天顶角（up）为索引，取出r_0  ，r_1
				int* traSearch = traSearch_1d(theta0, thetaV,_poExHand);	//索引

				float** city_tra_rho = Linear_Interpolation_1D(traSearch, city_tra, theta0, thetaV,_poExHand);	//rho
				float** pureSea_tra_rho = Linear_Interpolation_1D(traSearch, pureSea_tra, theta0, thetaV,_poExHand);
				float** sea_tra_rho = Linear_Interpolation_1D(traSearch, sea_tra, theta0, thetaV,_poExHand);
				float** land_tra_rho = Linear_Interpolation_1D(traSearch, land_tra, theta0, thetaV,_poExHand);
				float** desert_tra_rho = Linear_Interpolation_1D(traSearch, desert_tra, theta0, thetaV,_poExHand);
				float** isothermal_tra_rho = Linear_Interpolation_1D(traSearch, isothermal_tra, theta0, thetaV,_poExHand);

				float** sea_1_tra_rho = Linear_Interpolation_1D(traSearch, sea_1_tra, theta0, thetaV,_poExHand);
				float** sea_2_tra_rho = Linear_Interpolation_1D(traSearch, sea_2_tra, theta0, thetaV,_poExHand);
				float** sea_3_tra_rho = Linear_Interpolation_1D(traSearch, sea_3_tra, theta0, thetaV,_poExHand);
				float** sea_4_tra_rho = Linear_Interpolation_1D(traSearch, sea_4_tra, theta0, thetaV,_poExHand);
				float** sea_5_tra_rho = Linear_Interpolation_1D(traSearch, sea_5_tra, theta0, thetaV,_poExHand);

				/* 散射透过率 */
				float r_0_upB1_max, r_1_upB1_max, r_0_downB1_max, r_1_downB1_max,
					  r_0_upB2_max, r_1_upB2_max, r_0_downB2_max, r_1_downB2_max,
					  r_0_upB1_min, r_1_upB1_min, r_0_downB1_min, r_1_downB1_min,
					  r_0_upB2_min, r_1_upB2_min, r_0_downB2_min, r_1_downB2_min;//一维插值，通过太阳天顶角(down)及卫星天顶角（up）为索引，取出r_0  ，r_1，

				float r_0_upB3_max, r_1_upB3_max, r_0_downB3_max, r_1_downB3_max,
					  r_0_upB4_max, r_1_upB4_max, r_0_downB4_max, r_1_downB4_max,
					  r_0_upB3_min, r_1_upB3_min, r_0_downB3_min, r_1_downB3_min,
					  r_0_upB4_min, r_1_upB4_min, r_0_downB4_min, r_1_downB4_min; //TODO: Rrc -->2018-10-26

				int index_max = 0, index_min = 0;//代表哪一个气溶胶,取值为0-10,分别代表城镇，纯海，海洋，陆地，沙漠，同温……共11个

				/* Judgment index */
				int positive_index = 0;  /* 统计索引为1或者0的个数 */
				int negative_index = 0;  /* 统计索引为-1或者0的个数 */
				for(int i = 0; i < 11; i++)
				{
					if(aerosol_dist[1][i] == 1 || aerosol_dist[1][i] == 0)
					{
						positive_index += 1;
					}
					else
					{
						break;
					}
				}
				for(int i = 0; i < 11; i++)
				{
					if(aerosol_dist[1][i] == -1 || aerosol_dist[1][i] == 0)
					{
						negative_index -= 1;
					}
					else
					{
						break;
					}
				}

				/* 索引全为1或者0的情况,即计算的gamma_B3都比实际的gamma_B3大 */
				if(positive_index == 11)
				{
					float min = aerosol_dist[0][0];
					int index_right_1 = 0, index_right_2 = 0;
					//判断正索引对应的距离值中最小的那两个
					for(int i = 0; i < 11; i++)
					{
						if(aerosol_dist[1][i] == 0)
						{
							continue;
						}
						else if(aerosol_dist[1][i] == 1 && aerosol_dist[0][i] <= min)
						{
							min = aerosol_dist[0][i];
							index_right_1 = i;
						}
					}
					aerosol_dist[0][index_right_1] = 10000;
					min = aerosol_dist[0][0];
					for(int i = 0; i < 11; i++)
					{
						if(aerosol_dist[1][i] == 0)
						{
							continue;
						}
						else if(aerosol_dist[1][i] == 1 && aerosol_dist[0][i] <= min)
						{
							min = aerosol_dist[0][i];
							index_right_2 = i;
						}
					}
					index_min = index_right_1;
					index_max = index_right_2;
				}
				else if(negative_index == -11)  /* 索引全为-1或者0的情况,即计算的gamma_B3都比实际的gamma_B3小 */
				{
					float min = abs(aerosol_dist[0][0]);
					int index_left_1 = 0, index_left_2 = 0;
					//判断负索引对应的距离值中最小的那两个
					for(int i = 0; i < 11; i++)
					{
						if(aerosol_dist[1][i] == 0)
						{
							continue;
						}
						else if(aerosol_dist[1][i] == -1 && abs(aerosol_dist[0][i]) <= min)
						{
							min = abs(aerosol_dist[0][i]);
							index_left_1 = i;
						}
					}
					aerosol_dist[0][index_left_1] = 10000;
					min = abs(aerosol_dist[0][0]);
					for(int i = 0; i < 11; i++)
					{
						if(aerosol_dist[1][i] == 0)
						{
							continue;
						}
						else if(aerosol_dist[1][i] == 1 && abs(aerosol_dist[0][i]) <= min)
						{
							min = abs(aerosol_dist[0][i]);
							index_left_2 = i;
						}
					}
					index_min = index_left_1;
					index_max = index_left_2;
				}
				else
				{
					float gamma_B3_max = abs(aerosol_dist[0][0]), gamma_B3_min = abs(aerosol_dist[0][0]);//距离真实gamma_B3最近的两个气溶胶
					//判断索引对应的距离值中哪一个最小,正索引一个，负索引一个
					for(int i = 0; i < 11; i++)
					{
						if(aerosol_dist[1][i] == +1 && aerosol_dist[0][i] <= gamma_B3_max)
						{
							gamma_B3_max = aerosol_dist[0][i];
							index_max = i;
						}
						else if(aerosol_dist[1][i] == -1 && abs(aerosol_dist[0][i]) <= gamma_B3_min)
						{
							gamma_B3_min = abs(aerosol_dist[0][i]);
							index_min = i;
						}
					}
				}

				//通过index查找气溶胶文件
	//			const char* name_max;
	//			const char* name_min;
				float gamma_B1_max, gamma_B2_max, taua_B1_max, taua_B2_max,//gamma_B3右侧,得到gamma_B1_max，gamma_B2_max
					  gamma_B1_min, gamma_B2_min, taua_B1_min, taua_B2_min;//gamma_B3左侧，得到gamma_B1_min，gamma_B2_min

				float taua_B3_max, taua_B4_max,
				      taua_B3_min, taua_B4_min;  //TODO: Rrc -->2018-10-26

				switch (index_max){
					case 0:
	//			    name_max = "城镇";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = city_r0_B1 + city_r1_B1 * city_taua_B1 + city_r2_B1 * pow(city_taua_B1, 2);
					gamma_B2_max = city_r0_B2 + city_r1_B2 * city_taua_B2 + city_r2_B2 * pow(city_taua_B2, 2);
					taua_B1_max = city_taua_B1;
					taua_B2_max = city_taua_B2;
					r_0_upB1_max = city_tra_rho[0][0]; r_1_upB1_max = city_tra_rho[0][1];
					r_0_downB1_max = city_tra_rho[0][2]; r_1_downB1_max = city_tra_rho[0][3];
					r_0_upB2_max = city_tra_rho[1][0]; r_1_upB2_max = city_tra_rho[1][1];
					r_0_downB2_max = city_tra_rho[1][2]; r_1_downB2_max = city_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = city_taua_B3;
					taua_B4_max = city_taua_B4;
					r_0_upB3_max = city_tra_rho[2][0]; r_1_upB3_max = city_tra_rho[2][1];
					r_0_downB3_max = city_tra_rho[2][2]; r_1_downB3_max = city_tra_rho[2][3];
					r_0_upB4_max = city_tra_rho[3][0]; r_1_upB4_max = city_tra_rho[3][1];
					r_0_downB4_max = city_tra_rho[3][2]; r_1_downB4_max = city_tra_rho[3][3];
					break;

					case 1:
	//				name_max = "纯海";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = pureSea_r0_B1 + pureSea_r1_B1 * pureSea_taua_B1 + pureSea_r2_B1 * pow(pureSea_taua_B1, 2);
					gamma_B2_max = pureSea_r0_B2 + pureSea_r1_B2 * pureSea_taua_B2 + pureSea_r2_B2 * pow(pureSea_taua_B2, 2);
					taua_B1_max = pureSea_taua_B1;
					taua_B2_max = pureSea_taua_B2;
					r_0_upB1_max = pureSea_tra_rho[0][0]; r_1_upB1_max = pureSea_tra_rho[0][1];
					r_0_downB1_max = pureSea_tra_rho[0][2]; r_1_downB1_max = pureSea_tra_rho[0][3];
					r_0_upB2_max = pureSea_tra_rho[1][0]; r_1_upB2_max = pureSea_tra_rho[1][1];
					r_0_downB2_max = pureSea_tra_rho[1][2]; r_1_downB2_max = pureSea_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = pureSea_taua_B3;
					taua_B4_max = pureSea_taua_B4;
					r_0_upB3_max = pureSea_tra_rho[2][0]; r_1_upB3_max = pureSea_tra_rho[2][1];
					r_0_downB3_max = pureSea_tra_rho[2][2]; r_1_downB3_max = pureSea_tra_rho[2][3];
					r_0_upB4_max = pureSea_tra_rho[3][0]; r_1_upB4_max = pureSea_tra_rho[3][1];
					r_0_downB4_max = pureSea_tra_rho[3][2]; r_1_downB4_max = pureSea_tra_rho[3][3];
					break;

					case 2:
	//				name_max = "海洋";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_r0_B1 + sea_r1_B1 * sea_taua_B1 + sea_r2_B1 * pow(sea_taua_B1, 2);
					gamma_B2_max = sea_r0_B2 + sea_r1_B2 * sea_taua_B2 + sea_r2_B2 * pow(sea_taua_B2, 2);
					taua_B1_max = sea_taua_B1;
					taua_B2_max = sea_taua_B2;
					r_0_upB1_max = sea_tra_rho[0][0]; r_1_upB1_max = sea_tra_rho[0][1];
					r_0_downB1_max = sea_tra_rho[0][2]; r_1_downB1_max = sea_tra_rho[0][3];
					r_0_upB2_max = sea_tra_rho[1][0]; r_1_upB2_max = sea_tra_rho[1][1];
					r_0_downB2_max = sea_tra_rho[1][2]; r_1_downB2_max = sea_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_taua_B3;
					taua_B4_max = sea_taua_B4;
					r_0_upB3_max = sea_tra_rho[2][0]; r_1_upB3_max = sea_tra_rho[2][1];
					r_0_downB3_max = sea_tra_rho[2][2]; r_1_downB3_max = sea_tra_rho[2][3];
					r_0_upB4_max = sea_tra_rho[3][0]; r_1_upB4_max = sea_tra_rho[3][1];
					r_0_downB4_max = sea_tra_rho[3][2]; r_1_downB4_max = sea_tra_rho[3][3];
					break;

					case 3:
	//				name_max = "陆地";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = land_r0_B1 + land_r1_B1 * land_taua_B1 + land_r2_B1 * pow(land_taua_B1, 2);
					gamma_B2_max = land_r0_B2 + land_r1_B2 * land_taua_B2 + land_r2_B2 * pow(land_taua_B2, 2);
					taua_B1_max = land_taua_B1;
					taua_B2_max = land_taua_B2;
					r_0_upB1_max = land_tra_rho[0][0]; r_1_upB1_max = land_tra_rho[0][1];
					r_0_downB1_max = land_tra_rho[0][2]; r_1_downB1_max = land_tra_rho[0][3];
					r_0_upB2_max = land_tra_rho[1][0]; r_1_upB2_max = land_tra_rho[1][1];
					r_0_downB2_max = land_tra_rho[1][2]; r_1_downB2_max = land_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = land_taua_B3;
					taua_B3_max = land_taua_B4;
					r_0_upB3_max = land_tra_rho[2][0]; r_1_upB3_max = land_tra_rho[2][1];
					r_0_downB3_max = land_tra_rho[2][2]; r_1_downB3_max = land_tra_rho[2][3];
					r_0_upB4_max = land_tra_rho[3][0]; r_1_upB4_max = land_tra_rho[3][1];
					r_0_downB4_max = land_tra_rho[3][2]; r_1_downB4_max = land_tra_rho[3][3];
					break;

					case 4:
	//				name_max = "沙漠";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = desert_r0_B1 + desert_r1_B1 * desert_taua_B1 + desert_r2_B1 * pow(desert_taua_B1, 2);
					gamma_B2_max = desert_r0_B2 + desert_r1_B2 * desert_taua_B2 + desert_r2_B2 * pow(desert_taua_B2, 2);
					taua_B1_max = desert_taua_B1;
					taua_B2_max = desert_taua_B2;
					r_0_upB1_max = desert_tra_rho[0][0]; r_1_upB1_max = desert_tra_rho[0][1];
					r_0_downB1_max = desert_tra_rho[0][2]; r_1_downB1_max = desert_tra_rho[0][3];
					r_0_upB2_max = desert_tra_rho[1][0]; r_1_upB2_max = desert_tra_rho[1][1];
					r_0_downB2_max = desert_tra_rho[1][2]; r_1_downB2_max = desert_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = desert_taua_B3;
					taua_B4_max = desert_taua_B4;
					r_0_upB3_max = desert_tra_rho[2][0]; r_1_upB3_max = desert_tra_rho[2][1];
					r_0_downB3_max = desert_tra_rho[2][2]; r_1_downB3_max = desert_tra_rho[2][3];
					r_0_upB4_max = desert_tra_rho[3][0]; r_1_upB4_max = desert_tra_rho[3][1];
					r_0_downB4_max = desert_tra_rho[3][2]; r_1_downB4_max = desert_tra_rho[3][3];
					break;

					case 5:
	//				name_max = "同温";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = isothermal_r0_B1 + isothermal_r1_B1 * isothermal_taua_B1 + isothermal_r2_B1 * pow(isothermal_taua_B1, 2);
					gamma_B2_max = isothermal_r0_B2 + isothermal_r1_B2 * isothermal_taua_B2 + isothermal_r2_B2 * pow(isothermal_taua_B2, 2);
					taua_B1_max = isothermal_taua_B1;
					taua_B2_max = isothermal_taua_B2;
					r_0_upB1_max = isothermal_tra_rho[0][0]; r_1_upB1_max = isothermal_tra_rho[0][1];
					r_0_downB1_max = isothermal_tra_rho[0][2]; r_1_downB1_max = isothermal_tra_rho[0][3];
					r_0_upB2_max = isothermal_tra_rho[1][0]; r_1_upB2_max = isothermal_tra_rho[1][1];
					r_0_downB2_max = isothermal_tra_rho[1][2]; r_1_downB2_max = isothermal_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = isothermal_taua_B3;
					taua_B4_max = isothermal_taua_B4;
					r_0_upB3_max = isothermal_tra_rho[2][0]; r_1_upB3_max = isothermal_tra_rho[2][1];
					r_0_downB3_max = isothermal_tra_rho[2][2]; r_1_downB3_max = isothermal_tra_rho[2][3];
					r_0_upB4_max = isothermal_tra_rho[3][0]; r_1_upB4_max = isothermal_tra_rho[3][1];
					r_0_downB4_max = isothermal_tra_rho[3][2]; r_1_downB4_max = isothermal_tra_rho[3][3];
					break;

					case 6:
	//				name_max = "sea_1";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_1_r0_B1 + sea_1_r1_B1 * sea_1_taua_B1 + sea_1_r2_B1 * pow(sea_1_taua_B1, 2);
					gamma_B2_max = sea_1_r0_B2 + sea_1_r1_B2 * sea_1_taua_B2 + sea_1_r2_B2 * pow(sea_1_taua_B2, 2);
					taua_B1_max = sea_1_taua_B1;
					taua_B2_max = sea_1_taua_B2;
					r_0_upB1_max = sea_1_tra_rho[0][0]; r_1_upB1_max = sea_1_tra_rho[0][1];
					r_0_downB1_max = sea_1_tra_rho[0][2]; r_1_downB1_max = sea_1_tra_rho[0][3];
					r_0_upB2_max = sea_1_tra_rho[1][0]; r_1_upB2_max = sea_1_tra_rho[1][1];
					r_0_downB2_max = sea_1_tra_rho[1][2]; r_1_downB2_max = sea_1_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_1_taua_B3;
					taua_B4_max = sea_1_taua_B4;
					r_0_upB3_max = sea_1_tra_rho[2][0]; r_1_upB3_max = sea_1_tra_rho[2][1];
					r_0_downB3_max = sea_1_tra_rho[2][2]; r_1_downB3_max = sea_1_tra_rho[2][3];
					r_0_upB4_max = sea_1_tra_rho[3][0]; r_1_upB4_max = sea_1_tra_rho[3][1];
					r_0_downB4_max = sea_1_tra_rho[3][2]; r_1_downB4_max = sea_1_tra_rho[3][3];
					break;

					case 7:
	//				name_max = "sea_2";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_2_r0_B1 + sea_2_r1_B1 * sea_2_taua_B1 + sea_2_r2_B1 * pow(sea_2_taua_B1, 2);
					gamma_B2_max = sea_2_r0_B2 + sea_2_r1_B2 * sea_2_taua_B2 + sea_2_r2_B2 * pow(sea_2_taua_B2, 2);
					taua_B1_max = sea_2_taua_B1;
					taua_B2_max = sea_2_taua_B2;
					r_0_upB1_max = sea_2_tra_rho[0][0]; r_1_upB1_max = sea_2_tra_rho[0][1];
					r_0_downB1_max = sea_2_tra_rho[0][2]; r_1_downB1_max = sea_2_tra_rho[0][3];
					r_0_upB2_max = sea_2_tra_rho[1][0]; r_1_upB2_max = sea_2_tra_rho[1][1];
					r_0_downB2_max = sea_2_tra_rho[1][2]; r_1_downB2_max = sea_2_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_2_taua_B3;
					taua_B4_max = sea_2_taua_B4;
					r_0_upB3_max = sea_2_tra_rho[2][0]; r_1_upB3_max = sea_2_tra_rho[2][1];
					r_0_downB3_max = sea_2_tra_rho[2][2]; r_1_downB3_max = sea_2_tra_rho[2][3];
					r_0_upB4_max = sea_2_tra_rho[3][0]; r_1_upB4_max = sea_2_tra_rho[3][1];
					r_0_downB4_max = sea_2_tra_rho[3][2]; r_1_downB4_max = sea_2_tra_rho[3][3];
					break;

					case 8:
	//				name_max = "sea_3";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_3_r0_B1 + sea_3_r1_B1 * sea_3_taua_B1 + sea_3_r2_B1 * pow(sea_3_taua_B1, 2);
					gamma_B2_max = sea_3_r0_B2 + sea_3_r1_B2 * sea_3_taua_B2 + sea_3_r2_B2 * pow(sea_3_taua_B2, 2);
					taua_B1_max = sea_3_taua_B1;
					taua_B2_max = sea_3_taua_B2;
					r_0_upB1_max = sea_3_tra_rho[0][0]; r_1_upB1_max = sea_3_tra_rho[0][1];
					r_0_downB1_max = sea_3_tra_rho[0][2]; r_1_downB1_max = sea_3_tra_rho[0][3];
					r_0_upB2_max = sea_3_tra_rho[1][0]; r_1_upB2_max = sea_3_tra_rho[1][1];
					r_0_downB2_max = sea_3_tra_rho[1][2]; r_1_downB2_max = sea_3_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_3_taua_B3;
					taua_B4_max = sea_3_taua_B4;
					r_0_upB3_max = sea_3_tra_rho[2][0]; r_1_upB3_max = sea_3_tra_rho[2][1];
					r_0_downB3_max = sea_3_tra_rho[2][2]; r_1_downB3_max = sea_3_tra_rho[2][3];
					r_0_upB4_max = sea_3_tra_rho[3][0]; r_1_upB4_max = sea_3_tra_rho[3][1];
					r_0_downB4_max = sea_3_tra_rho[3][2]; r_1_downB4_max = sea_3_tra_rho[3][3];
					break;

					case 9:
	//				name_max = "sea_4";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_4_r0_B1 + sea_4_r1_B1 * sea_4_taua_B1 + sea_4_r2_B1 * pow(sea_4_taua_B1, 2);
					gamma_B2_max = sea_4_r0_B2 + sea_4_r1_B2 * sea_4_taua_B2 + sea_4_r2_B2 * pow(sea_4_taua_B2, 2);
					taua_B1_max = sea_4_taua_B1;
					taua_B2_max = sea_4_taua_B2;
					r_0_upB1_max = sea_4_tra_rho[0][0]; r_1_upB1_max = sea_4_tra_rho[0][1];
					r_0_downB1_max = sea_4_tra_rho[0][2]; r_1_downB1_max = sea_4_tra_rho[0][3];
					r_0_upB2_max = sea_4_tra_rho[1][0]; r_1_upB2_max = sea_4_tra_rho[1][1];
					r_0_downB2_max = sea_4_tra_rho[1][2]; r_1_downB2_max = sea_4_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_4_taua_B3;
					taua_B4_max = sea_4_taua_B4;
					r_0_upB3_max = sea_4_tra_rho[2][0]; r_1_upB3_max = sea_4_tra_rho[2][1];
					r_0_downB3_max = sea_4_tra_rho[2][2]; r_1_downB3_max = sea_4_tra_rho[2][3];
					r_0_upB4_max = sea_4_tra_rho[3][0]; r_1_upB4_max = sea_4_tra_rho[3][1];
					r_0_downB4_max = sea_4_tra_rho[3][2]; r_1_downB4_max = sea_4_tra_rho[3][3];
					break;

					case 10:
	//				name_max = "sea_5";
	//				cout<<"距离实际gamma_B3最近之一的gamma_B3所在的气溶胶文件是： "<<name_max<<endl;
					gamma_B1_max = sea_5_r0_B1 + sea_5_r1_B1 * sea_5_taua_B1 + sea_5_r2_B1 * pow(sea_5_taua_B1, 2);
					gamma_B2_max = sea_5_r0_B2 + sea_5_r1_B2 * sea_5_taua_B2 + sea_5_r2_B2 * pow(sea_5_taua_B2, 2);
					taua_B1_max = sea_5_taua_B1;
					taua_B2_max = sea_5_taua_B2;
					r_0_upB1_max = sea_5_tra_rho[0][0]; r_1_upB1_max = sea_5_tra_rho[0][1];
					r_0_downB1_max = sea_5_tra_rho[0][2]; r_1_downB1_max = sea_5_tra_rho[0][3];
					r_0_upB2_max = sea_5_tra_rho[1][0]; r_1_upB2_max = sea_5_tra_rho[1][1];
					r_0_downB2_max = sea_5_tra_rho[1][2]; r_1_downB2_max = sea_5_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_max = sea_5_taua_B3;
					taua_B4_max = sea_5_taua_B4;
					r_0_upB3_max = sea_5_tra_rho[2][0]; r_1_upB3_max = sea_5_tra_rho[2][1];
					r_0_downB3_max = sea_5_tra_rho[2][2]; r_1_downB3_max = sea_5_tra_rho[2][3];
					r_0_upB4_max = sea_5_tra_rho[3][0]; r_1_upB4_max = sea_5_tra_rho[3][1];
					r_0_downB4_max = sea_5_tra_rho[3][2]; r_1_downB4_max = sea_5_tra_rho[3][3];
					break;
				}

				switch (index_min){
					case 0:
	//				char* name_min = "城镇";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = city_r0_B1 + city_r1_B1 * city_taua_B1 + city_r2_B1 * pow(city_taua_B1, 2);
					gamma_B2_min = city_r0_B2 + city_r1_B2 * city_taua_B2 + city_r2_B2 * pow(city_taua_B2, 2);
					taua_B1_min = city_taua_B1;
					taua_B2_min = city_taua_B2;
					r_0_upB1_min = city_tra_rho[0][0]; r_1_upB1_min = city_tra_rho[0][1];
					r_0_downB1_min = city_tra_rho[0][2]; r_1_downB1_min = city_tra_rho[0][3];
					r_0_upB2_min = city_tra_rho[1][0]; r_1_upB2_min = city_tra_rho[1][1];
					r_0_downB2_min = city_tra_rho[1][2]; r_1_downB2_min = city_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = city_taua_B3;
					taua_B4_min = city_taua_B4;
					r_0_upB3_min = city_tra_rho[2][0]; r_1_upB3_min = city_tra_rho[2][1];
					r_0_downB3_min = city_tra_rho[2][2]; r_1_downB3_min = city_tra_rho[2][3];
					r_0_upB4_min = city_tra_rho[3][0]; r_1_upB4_min = city_tra_rho[3][1];
					r_0_downB4_min = city_tra_rho[3][2]; r_1_downB4_min = city_tra_rho[3][3];
					break;

					case 1:
	//				name_min = "纯海";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = pureSea_r0_B1 + pureSea_r1_B1 * pureSea_taua_B1 + pureSea_r2_B1 * pow(pureSea_taua_B1, 2);
					gamma_B2_min = pureSea_r0_B2 + pureSea_r1_B2 * pureSea_taua_B2 + pureSea_r2_B2 * pow(pureSea_taua_B2, 2);
					taua_B1_min = pureSea_taua_B1;
					taua_B2_min = pureSea_taua_B2;
					r_0_upB1_min = pureSea_tra_rho[0][0]; r_1_upB1_min = pureSea_tra_rho[0][1];
					r_0_downB1_min = pureSea_tra_rho[0][2]; r_1_downB1_min = pureSea_tra_rho[0][3];
					r_0_upB2_min = pureSea_tra_rho[1][0]; r_1_upB2_min = pureSea_tra_rho[1][1];
					r_0_downB2_min = pureSea_tra_rho[1][2]; r_1_downB2_min = pureSea_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = pureSea_taua_B3;
					taua_B4_min = pureSea_taua_B4;
					r_0_upB3_min = pureSea_tra_rho[2][0]; r_1_upB3_min = pureSea_tra_rho[2][1];
					r_0_downB3_min = pureSea_tra_rho[2][2]; r_1_downB3_min = pureSea_tra_rho[2][3];
					r_0_upB4_min = pureSea_tra_rho[3][0]; r_1_upB4_min = pureSea_tra_rho[3][1];
					r_0_downB4_min = pureSea_tra_rho[3][2]; r_1_downB4_min = pureSea_tra_rho[3][3];
					break;

					case 2:
	//				name_min = "海洋";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_r0_B1 + sea_r1_B1 * sea_taua_B1 + sea_r2_B1 * pow(sea_taua_B1, 2);
					gamma_B2_min = sea_r0_B2 + sea_r1_B2 * sea_taua_B2 + sea_r2_B2 * pow(sea_taua_B2, 2);
					taua_B1_min = sea_taua_B1;
					taua_B2_min = sea_taua_B2;
					r_0_upB1_min = sea_tra_rho[0][0]; r_1_upB1_min = sea_tra_rho[0][1];
					r_0_downB1_min = sea_tra_rho[0][2]; r_1_downB1_min = sea_tra_rho[0][3];
					r_0_upB2_min = sea_tra_rho[1][0]; r_1_upB2_min = sea_tra_rho[1][1];
					r_0_downB2_min = sea_tra_rho[1][2]; r_1_downB2_min = sea_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_taua_B3;
					taua_B4_min = sea_taua_B4;
					r_0_upB3_min = sea_tra_rho[2][0]; r_1_upB3_min = sea_tra_rho[2][1];
					r_0_downB3_min = sea_tra_rho[2][2]; r_1_downB3_min = sea_tra_rho[2][3];
					r_0_upB4_min = sea_tra_rho[3][0]; r_1_upB4_min = sea_tra_rho[3][1];
					r_0_downB4_min = sea_tra_rho[3][2]; r_1_downB4_min = sea_tra_rho[3][3];
					break;

					case 3:
	//				name_min = "陆地";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = land_r0_B1 + land_r1_B1 * land_taua_B1 + land_r2_B1 * pow(land_taua_B1, 2);
					gamma_B2_min = land_r0_B2 + land_r1_B2 * land_taua_B2 + land_r2_B2 * pow(land_taua_B2, 2);
					taua_B1_min = land_taua_B1;
					taua_B2_min = land_taua_B2;
					r_0_upB1_min = land_tra_rho[0][0]; r_1_upB1_min = land_tra_rho[0][1];
					r_0_downB1_min = land_tra_rho[0][2]; r_1_downB1_min = land_tra_rho[0][3];
					r_0_upB2_min = land_tra_rho[1][0]; r_1_upB2_min = land_tra_rho[1][1];
					r_0_downB2_min = land_tra_rho[1][2]; r_1_downB2_min = land_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = land_taua_B3;
					taua_B4_min = land_taua_B4;
					r_0_upB3_min = land_tra_rho[2][0]; r_1_upB3_min = land_tra_rho[2][1];
					r_0_downB3_min = land_tra_rho[2][2]; r_1_downB3_min = land_tra_rho[2][3];
					r_0_upB4_min = land_tra_rho[3][0]; r_1_upB4_min = land_tra_rho[3][1];
					r_0_downB4_min = land_tra_rho[3][2]; r_1_downB4_min = land_tra_rho[3][3];
					break;

					case 4:
	//				name_min = "沙漠";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = desert_r0_B1 + desert_r1_B1 * desert_taua_B1 + desert_r2_B1 * pow(desert_taua_B1, 2);
					gamma_B2_min = desert_r0_B2 + desert_r1_B2 * desert_taua_B2 + desert_r2_B2 * pow(desert_taua_B2, 2);
					taua_B1_min = desert_taua_B1;
					taua_B2_min = desert_taua_B2;
					r_0_upB1_min = desert_tra_rho[0][0]; r_1_upB1_min = desert_tra_rho[0][1];
					r_0_downB1_min = desert_tra_rho[0][2]; r_1_downB1_min = desert_tra_rho[0][3];
					r_0_upB2_min = desert_tra_rho[1][0]; r_1_upB2_min = desert_tra_rho[1][1];
					r_0_downB2_min = desert_tra_rho[1][2]; r_1_downB2_min = desert_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = desert_taua_B3;
					taua_B4_min = desert_taua_B4;
					r_0_upB3_min = desert_tra_rho[2][0]; r_1_upB3_min = desert_tra_rho[2][1];
					r_0_downB3_min = desert_tra_rho[2][2]; r_1_downB3_min = desert_tra_rho[2][3];
					r_0_upB4_min = desert_tra_rho[3][0]; r_1_upB4_min = desert_tra_rho[3][1];
					r_0_downB4_min = desert_tra_rho[3][2]; r_1_downB4_min = desert_tra_rho[3][3];
					break;

					case 5:
	//				name_min = "同温";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = isothermal_r0_B1 + isothermal_r1_B1 * isothermal_taua_B1 + isothermal_r2_B1 * pow(isothermal_taua_B1, 2);
					gamma_B2_min = isothermal_r0_B2 + isothermal_r1_B2 * isothermal_taua_B2 + isothermal_r2_B2 * pow(isothermal_taua_B2, 2);
					taua_B1_min = isothermal_taua_B1;
					taua_B2_min = isothermal_taua_B2;
					r_0_upB1_min = isothermal_tra_rho[0][0]; r_1_upB1_min = isothermal_tra_rho[0][1];
					r_0_downB1_min = isothermal_tra_rho[0][2]; r_1_downB1_min = isothermal_tra_rho[0][3];
					r_0_upB2_min = isothermal_tra_rho[1][0]; r_1_upB2_min = isothermal_tra_rho[1][1];
					r_0_downB2_min = isothermal_tra_rho[1][2]; r_1_downB2_min = isothermal_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = isothermal_taua_B3;
					taua_B4_min = isothermal_taua_B4;
					r_0_upB3_min = isothermal_tra_rho[2][0]; r_1_upB3_min = isothermal_tra_rho[2][1];
					r_0_downB3_min = isothermal_tra_rho[2][2]; r_1_downB3_min = isothermal_tra_rho[2][3];
					r_0_upB4_min = isothermal_tra_rho[3][0]; r_1_upB4_min = isothermal_tra_rho[3][1];
					r_0_downB4_min = isothermal_tra_rho[3][2]; r_1_downB4_min = isothermal_tra_rho[3][3];
					break;

					case 6:
	//				name_min = "sea_1";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_1_r0_B1 + sea_1_r1_B1 * sea_1_taua_B1 + sea_1_r2_B1 * pow(sea_1_taua_B1, 2);
					gamma_B2_min = sea_1_r0_B2 + sea_1_r1_B2 * sea_1_taua_B2 + sea_1_r2_B2 * pow(sea_1_taua_B2, 2);
					taua_B1_min = sea_1_taua_B1;
					taua_B2_min = sea_1_taua_B2;
					r_0_upB1_min = sea_1_tra_rho[0][0]; r_1_upB1_min = sea_1_tra_rho[0][1];
					r_0_downB1_min = sea_1_tra_rho[0][2]; r_1_downB1_min = sea_1_tra_rho[0][3];
					r_0_upB2_min = sea_1_tra_rho[1][0]; r_1_upB2_min = sea_1_tra_rho[1][1];
					r_0_downB2_min = sea_1_tra_rho[1][2]; r_1_downB2_min = sea_1_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_1_taua_B3;
					taua_B4_min = sea_1_taua_B4;
					r_0_upB3_min = sea_1_tra_rho[2][0]; r_1_upB3_min = sea_1_tra_rho[2][1];
					r_0_downB3_min = sea_1_tra_rho[2][2]; r_1_downB3_min = sea_1_tra_rho[2][3];
					r_0_upB4_min = sea_1_tra_rho[3][0]; r_1_upB4_min = sea_1_tra_rho[3][1];
					r_0_downB4_min = sea_1_tra_rho[3][2]; r_1_downB4_min = sea_1_tra_rho[3][3];
					break;

					case 7:
	//				name_min = "sea_2";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_2_r0_B1 + sea_2_r1_B1 * sea_2_taua_B1 + sea_2_r2_B1 * pow(sea_2_taua_B1, 2);
					gamma_B2_min = sea_2_r0_B2 + sea_2_r1_B2 * sea_2_taua_B2 + sea_2_r2_B2 * pow(sea_2_taua_B2, 2);
					taua_B1_min = sea_2_taua_B1;
					taua_B2_min = sea_2_taua_B2;
					r_0_upB1_min = sea_2_tra_rho[0][0]; r_1_upB1_min = sea_2_tra_rho[0][1];
					r_0_downB1_min = sea_2_tra_rho[0][2]; r_1_downB1_min = sea_2_tra_rho[0][3];
					r_0_upB2_min = sea_2_tra_rho[1][0]; r_1_upB2_min = sea_2_tra_rho[1][1];
					r_0_downB2_min = sea_2_tra_rho[1][2]; r_1_downB2_min = sea_2_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_2_taua_B3;
					taua_B4_min = sea_2_taua_B4;
					r_0_upB3_min = sea_2_tra_rho[2][0]; r_1_upB3_min = sea_2_tra_rho[2][1];
					r_0_downB3_min = sea_2_tra_rho[2][2]; r_1_downB3_min = sea_2_tra_rho[2][3];
					r_0_upB4_min = sea_2_tra_rho[3][0]; r_1_upB4_min = sea_2_tra_rho[3][1];
					r_0_downB4_min = sea_2_tra_rho[3][2]; r_1_downB4_min = sea_2_tra_rho[3][3];
					break;

					case 8:
	//				name_min = "sea_3";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_3_r0_B1 + sea_3_r1_B1 * sea_3_taua_B1 + sea_3_r2_B1 * pow(sea_3_taua_B1, 2);
					gamma_B2_min = sea_3_r0_B2 + sea_3_r1_B2 * sea_3_taua_B2 + sea_3_r2_B2 * pow(sea_3_taua_B2, 2);
					taua_B1_min = sea_3_taua_B1;
					taua_B2_min = sea_3_taua_B2;
					r_0_upB1_min = sea_3_tra_rho[0][0]; r_1_upB1_min = sea_3_tra_rho[0][1];
					r_0_downB1_min = sea_3_tra_rho[0][2]; r_1_downB1_min = sea_3_tra_rho[0][3];
					r_0_upB2_min = sea_3_tra_rho[1][0]; r_1_upB2_min = sea_3_tra_rho[1][1];
					r_0_downB2_min = sea_3_tra_rho[1][2]; r_1_downB2_min = sea_3_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_3_taua_B3;
					taua_B4_min = sea_3_taua_B4;
					r_0_upB3_min = sea_3_tra_rho[2][0]; r_1_upB3_min = sea_3_tra_rho[2][1];
					r_0_downB3_min = sea_3_tra_rho[2][2]; r_1_downB3_min = sea_3_tra_rho[2][3];
					r_0_upB4_min = sea_3_tra_rho[3][0]; r_1_upB4_min = sea_3_tra_rho[3][1];
					r_0_downB4_min = sea_3_tra_rho[3][2]; r_1_downB4_min = sea_3_tra_rho[3][3];
					break;

					case 9:
	//				name_min = "sea_4";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_4_r0_B1 + sea_4_r1_B1 * sea_4_taua_B1 + sea_4_r2_B1 * pow(sea_4_taua_B1, 2);
					gamma_B2_min = sea_4_r0_B2 + sea_4_r1_B2 * sea_4_taua_B2 + sea_4_r2_B2 * pow(sea_4_taua_B2, 2);
					taua_B1_min = sea_4_taua_B1;
					taua_B2_min = sea_4_taua_B2;
					r_0_upB1_min = sea_4_tra_rho[0][0]; r_1_upB1_min = sea_4_tra_rho[0][1];
					r_0_downB1_min = sea_4_tra_rho[0][2]; r_1_downB1_min = sea_4_tra_rho[0][3];
					r_0_upB2_min = sea_4_tra_rho[1][0]; r_1_upB2_min = sea_4_tra_rho[1][1];
					r_0_downB2_min = sea_4_tra_rho[1][2]; r_1_downB2_min = sea_4_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_4_taua_B3;
					taua_B4_min = sea_4_taua_B4;
					r_0_upB3_min = sea_4_tra_rho[2][0]; r_1_upB3_min = sea_4_tra_rho[2][1];
					r_0_downB3_min = sea_4_tra_rho[2][2]; r_1_downB3_min = sea_4_tra_rho[2][3];
					r_0_upB4_min = sea_4_tra_rho[3][0]; r_1_upB4_min = sea_4_tra_rho[3][1];
					r_0_downB4_min = sea_4_tra_rho[3][2]; r_1_downB4_min = sea_4_tra_rho[3][3];
					break;

					case 10:
	//				name_min = "sea_5";
	//				cout<<"距离实际gamma_B3最近之二的gamma_B3所在的气溶胶文件是： "<<name_min<<endl;
					gamma_B1_min = sea_5_r0_B1 + sea_5_r1_B1 * sea_5_taua_B1 + sea_5_r2_B1 * pow(sea_5_taua_B1, 2);
					gamma_B2_min = sea_5_r0_B2 + sea_5_r1_B2 * sea_5_taua_B2 + sea_5_r2_B2 * pow(sea_5_taua_B2, 2);
					taua_B1_min = sea_5_taua_B1;
					taua_B2_min = sea_5_taua_B2;
					r_0_upB1_min = sea_5_tra_rho[0][0]; r_1_upB1_min = sea_5_tra_rho[0][1];
					r_0_downB1_min = sea_5_tra_rho[0][2]; r_1_downB1_min = sea_5_tra_rho[0][3];
					r_0_upB2_min = sea_5_tra_rho[1][0]; r_1_upB2_min = sea_5_tra_rho[1][1];
					r_0_downB2_min = sea_5_tra_rho[1][2]; r_1_downB2_min = sea_5_tra_rho[1][3];

					//TODO: Rrc -->2018-10-26-------------------------------
					taua_B3_min = sea_5_taua_B3;
					taua_B4_min = sea_5_taua_B4;
					r_0_upB3_min = sea_5_tra_rho[2][0]; r_1_upB3_min = sea_5_tra_rho[2][1];
					r_0_downB3_min = sea_5_tra_rho[2][2]; r_1_downB3_min = sea_5_tra_rho[2][3];
					r_0_upB4_min = sea_5_tra_rho[3][0]; r_1_upB4_min = sea_5_tra_rho[3][1];
					r_0_downB4_min = sea_5_tra_rho[3][2]; r_1_downB4_min = sea_5_tra_rho[3][3];
					break;
				}

		//-----------------------------------------------------------------------------------------------------------------------------------

				//计算混合比例X,固定值，每个波段都一样
				float X;
				X = (gamma_B3 - aerosol_gamma_B3[index_min]) / (aerosol_gamma_B3[index_max] - aerosol_gamma_B3[index_min]);
				// 判断“混合比例”的值是否在0到1之间，若大于1，则置为1，若小于0，则置为0，否则不变
				if (X > 1)
				{
					X = 1;
				}
				if (X < 0)
				{
					X = 0;
				}

				float gamma_B1, gamma_B2;
				gamma_B1 = (X * gamma_B1_max) + (1 - X) * (gamma_B1_min);
				gamma_B2 = (X * gamma_B2_max) + (1 - X) * (gamma_B2_min);

				float rhoa_B1, rhoa_B2;//气溶胶反射率
	//			rhoa_B1 = gamma_B1 * rho_r1 - rho_r1;
	//			rhoa_B2 = gamma_B2 * rho_r2 - rho_r2;

				rhoa_B1 = (gamma_B1 * rho_r1) * facB1 - rho_r1; //(gamma_B1 * rho_r1) * facB1
				rhoa_B2 = (gamma_B2 * rho_r2) * facB2 - rho_r2;//(gamma_B2 * rho_r2) * facB2

		//-----------------------------------------------------------------------------------------------------------------------------------

				//散射透过率
				float T_B_1_maxup, T_B_1_maxdown, T_B_1_minup, T_B_1_mindown,
				      T_B_2_maxup, T_B_2_maxdown, T_B_2_minup, T_B_2_mindown;
				float T_B1_up, T_B1_down, T_all_B1,
				      T_B2_up, T_B2_down, T_all_B2;

				float T_B_3_maxup, T_B_3_maxdown, T_B_3_minup, T_B_3_mindown,
			      	  T_B_4_maxup, T_B_4_maxdown, T_B_4_minup, T_B_4_mindown;
				float T_B3_up, T_B3_down, T_all_B3,
				      T_B4_up, T_B4_down, T_all_B4;  //TODO: Rrc -->2018-10-26

				T_B_1_maxup = r_0_upB1_max * exp(-r_1_upB1_max * taua_B1_max);//气溶胶大的波段1上行
				T_B_1_maxdown = r_0_downB1_max * exp(-r_1_downB1_max * taua_B1_max);//气溶胶大的波段1下行
				T_B_1_minup = r_0_upB1_min * exp(-r_1_upB1_min * taua_B1_min);//气溶胶小的波段1上行
				T_B_1_mindown = r_0_downB1_min * exp(-r_1_downB1_min * taua_B1_min);//气溶胶小的波段1下行
				T_B1_up = (X * T_B_1_maxup) + (1 - X) * T_B_1_minup;
				T_B1_down = (X * T_B_1_maxdown) + (1 - X) * T_B_1_mindown;
				T_all_B1 = T_B1_up * T_B1_down;//波段1的散射透过率

				T_B_2_maxup = r_0_upB2_max * exp(-r_1_upB2_max * taua_B2_max);//气溶胶大的波段1上行
				T_B_2_maxdown = r_0_downB2_max * exp(-r_1_downB2_max * taua_B2_max);//气溶胶大的波段1下行
				T_B_2_minup = r_0_upB2_min * exp(-r_1_upB2_min * taua_B2_min);//气溶胶大的波段1上行
				T_B_2_mindown = r_0_downB2_min * exp(-r_1_downB2_min * taua_B2_min);//气溶胶大的波段1下行
				T_B2_up = (X * T_B_2_maxup) + (1 - X) * T_B_2_minup;
				T_B2_down = (X * T_B_2_maxdown) + (1 - X) * T_B_2_mindown;
				T_all_B2 = T_B2_up * T_B2_down;//波段2的散射透过率

				//TODO: Rrc -->2018-10-26
				T_B_3_maxup = r_0_upB3_max * exp(-r_1_upB3_max * taua_B3_max);//气溶胶大的波段1上行
				T_B_3_maxdown = r_0_downB3_max * exp(-r_1_downB3_max * taua_B3_max);//气溶胶大的波段1下行
				T_B_3_minup = r_0_upB3_min * exp(-r_1_upB3_min * taua_B3_min);//气溶胶小的波段1上行
				T_B_3_mindown = r_0_downB3_min * exp(-r_1_downB3_min * taua_B3_min);//气溶胶小的波段1下行
				T_B3_up = (X * T_B_3_maxup) + (1 - X) * T_B_3_minup;
				T_B3_down = (X * T_B_3_maxdown) + (1 - X) * T_B_3_mindown;
				T_all_B3 = T_B3_up * T_B3_down;//波段3的散射透过率

				T_B_4_maxup = r_0_upB4_max * exp(-r_1_upB4_max * taua_B4_max);//气溶胶大的波段1上行
				T_B_4_maxdown = r_0_downB4_max * exp(-r_1_downB4_max * taua_B4_max);//气溶胶大的波段1下行
				T_B_4_minup = r_0_upB4_min * exp(-r_1_upB4_min * taua_B4_min);//气溶胶小的波段1上行
				T_B_4_mindown = r_0_downB4_min * exp(-r_1_downB4_min * taua_B4_min);//气溶胶小的波段1下行
				T_B4_up = (X * T_B_4_maxup) + (1 - X) * T_B_4_minup;
				T_B4_down = (X * T_B_4_maxdown) + (1 - X) * T_B_4_mindown;
				T_all_B4 = T_B4_up * T_B4_down;//波段4的散射透过率

				//耀斑计算
				float varphi_w = atan(z_wind_czi_pipei / m_wind_czi_pipei);//风向方位角
				float chi, xi, eta, P_1, P_2, P, rho, T_B1, T_B2, rhog_B1, rhog_B2;//旋转角度

				float Z_x, Z_y, Z_x_prime, Z_y_prime;
				float alpha, beta;//斜坡面法线方向的方位角和天顶角α和β

				Z_x = ((-sin(thetaV * pi / 180)) * sin(deltaphi * pi / 180)) / (cos(theta0 * pi / 180) + cos(thetaV * pi / 180));
				Z_y = (sin(theta0 * pi / 180) + sin(thetaV * pi / 180) * cos(deltaphi * pi / 180)) / (cos(theta0 * pi / 180) + cos(thetaV * pi / 180));

				alpha = atan(Z_x / Z_y);//斜坡面法线方向的方位角和天顶角α和β
				beta = atan(Z_x / sin(alpha));
				if(Z_x == 0)
				{
					beta = 0;
				}
				chi = varphi_s * pi / 180 - varphi_w;
				Z_x_prime = cos(chi) * Z_x + sin(chi ) * Z_y;
				Z_y_prime = (-sin(chi )) * Z_x + cos(chi ) * Z_y;

				//斜坡面的概率密度P(Z_x_prime, Z_y_prime)
				float sigma_x_prime = sqrt(0.003 + 0.00192 * W);
				float sigma_y_prime = sqrt(0.00316 * W);
				xi = Z_x_prime / sigma_x_prime;
				eta = Z_y_prime / sigma_y_prime;

				float C_21 = 0.01 - 0.0086 * W;
				float C_22 = 0.12;
				float C_03 = 0.04 - 0.033 * W;
				float C_40 = 0.4;
				float C_04 = 0.23;
				P_1 = exp(-(pow(xi, 2) + pow(eta, 2)) / 2) / (2 * pi * sigma_x_prime * sigma_y_prime);
				P_2 = 1 - (pow(xi, 2)-1)*1/2*C_21 - (pow(eta, 3)-3*eta)*1/6*C_03 + (pow(xi, 4)-6*pow(xi, 2)+3)*1/24*C_40
						+ (pow(xi, 2)-1)*(pow(eta, 2)-1)*1/4*C_22 + (pow(eta, 4)-6*pow(eta, 2)+3)*1/24*C_04;
				P = P_1 * P_2;//斜坡面的概率密度P

				float R = 0.0226;//菲涅尔反射率
				rho = (pi * P * R) / (4 * cos(theta0 * pi / 180) * cos(thetaV * pi / 180) * pow(cos(beta * pi / 180), 4));//耀斑反射率
				if(rho <= 0.0001)
				{
					rho = 0;
				}

				T_B1 = exp((-taorB1) * airmass);
				T_B2 = exp((-taorB2) * airmass);
				rhog_B1 = rho * T_B1;
				rhog_B2 = rho * T_B2;

	//			DataSet.BLUE[j] = (rho_blue - correctRB1 - rhoa_B1 * Tg_O3_B1 - rho_wc * T_B1_up - rhog_B1) / (T_all_B1 * Tg_O3_B1) / pi;
	//			DataSet.GREEN[j] = (rho_green - correctRB2 - rhoa_B2 * Tg_O3_B2 * Tg_water_B2 - rho_wc * T_B2_up - rhog_B2) / (T_all_B2 * Tg_O3_B2 * Tg_water_B2) / pi;

				/* TODO: 2018-10-19 */
				//Rrs's product
				Rrc_Rrs_data.Rrs_BLUE[j] = (rho_blue - correctRB1 - rhoa_B1 * Tg_O3_B1 - rho_wc * T_B1_up - rhog_B1) / (T_all_B1 * Tg_O3_B1) / pi;
				Rrc_Rrs_data.Rrs_GREEN[j] = (rho_green - correctRB2 - rhoa_B2 * Tg_O3_B2 * Tg_water_B2 - rho_wc * T_B2_up - rhog_B2) / (T_all_B2 * Tg_O3_B2 * Tg_water_B2) / pi;

				//Rrc's product
//				Rrc_Rrs_data.Rrc_BLUE[j] = (rho_blue - correctRB1) / T_all_B1 / pi;
//				Rrc_Rrs_data.Rrc_GREEN[j] = (rho_blue - correctRB2) / T_all_B2 / pi;
//				Rrc_Rrs_data.Rrc_RED[j] = (rho_red - correctRB3) / T_all_B3 / pi;
//				Rrc_Rrs_data.Rrc_NIR[j] = (rho_nir - correctRB4) / T_all_B4 / pi;

				Rrc_Rrs_data.Rrc_BLUE[j] = (rho_blue - correctRB1)  / pi;
				Rrc_Rrs_data.Rrc_GREEN[j] = (rho_green - correctRB2)  / pi;
				Rrc_Rrs_data.Rrc_RED[j] = (rho_red - correctRB3)  / pi;
				Rrc_Rrs_data.Rrc_NIR[j] = (rho_nir - correctRB4)  / pi;

				/* TODO: 2018-10-19 */

				/*
				 *
				 * free the memory
				 * *********************************************************************************
				 *
				 */
				for (int iaerosol = 0; iaerosol < 2; iaerosol++){
					RELEASE(aerosol_dist[iaerosol]);
				}
				RELEASE(aerosol_dist);

				for(int aeoRnCount = 0; aeoRnCount < 4;aeoRnCount++){
					RELEASE(Rn_city_aeo[aeoRnCount]); /* free the memory of Three dimensional linear interpolation*/
					RELEASE(Rn_pureSea_aeo[aeoRnCount]);
					RELEASE(Rn_sea_aeo[aeoRnCount]);
					RELEASE(Rn_land_aeo[aeoRnCount]);
					RELEASE(Rn_desert_aeo[aeoRnCount]);
					RELEASE(Rn_isothermal_aeo[aeoRnCount]);

					RELEASE(Rn_sea_1_aeo[aeoRnCount]);
					RELEASE(Rn_sea_2_aeo[aeoRnCount]);
					RELEASE(Rn_sea_3_aeo[aeoRnCount]);
					RELEASE(Rn_sea_4_aeo[aeoRnCount]);
					RELEASE(Rn_sea_5_aeo[aeoRnCount]);
				}
				RELEASE(Rn_city_aeo);  /* free the memory of Three dimensional linear interpolation*/
				RELEASE(Rn_pureSea_aeo);
				RELEASE(Rn_sea_aeo);
				RELEASE(Rn_land_aeo);
				RELEASE(Rn_desert_aeo);
				RELEASE(Rn_isothermal_aeo);

				RELEASE(Rn_sea_1_aeo);
				RELEASE(Rn_sea_2_aeo);
				RELEASE(Rn_sea_3_aeo);
				RELEASE(Rn_sea_4_aeo);
				RELEASE(Rn_sea_5_aeo);

				RELEASE(traSearch);

				for (int itra_rhocount = 0; itra_rhocount < 4 ; itra_rhocount++){
					RELEASE(city_tra_rho[itra_rhocount]);
					RELEASE(pureSea_tra_rho[itra_rhocount]);
					RELEASE(sea_tra_rho[itra_rhocount]);
					RELEASE(land_tra_rho[itra_rhocount]);
					RELEASE(desert_tra_rho[itra_rhocount]);
					RELEASE(isothermal_tra_rho[itra_rhocount]);

					RELEASE(sea_1_tra_rho[itra_rhocount]);
					RELEASE(sea_2_tra_rho[itra_rhocount]);
					RELEASE(sea_3_tra_rho[itra_rhocount]);
					RELEASE(sea_4_tra_rho[itra_rhocount]);
					RELEASE(sea_5_tra_rho[itra_rhocount]);
				}
				RELEASE(city_tra_rho);
				RELEASE(pureSea_tra_rho);
				RELEASE(sea_tra_rho);
				RELEASE(land_tra_rho);
				RELEASE(desert_tra_rho);
				RELEASE(isothermal_tra_rho);

				RELEASE(sea_1_tra_rho);
				RELEASE(sea_2_tra_rho);
				RELEASE(sea_3_tra_rho);
				RELEASE(sea_4_tra_rho);
				RELEASE(sea_5_tra_rho);

			}/* if(Lat)*/
			else
			{
				Rrc_Rrs_data.Rrs_BLUE[j] = NAN_f;  //Rrs's 2 band is 0
				Rrc_Rrs_data.Rrs_GREEN[j] = NAN_f;

				Rrc_Rrs_data.Rrc_BLUE[j] = NAN_f;  //Rrc's 4 band is 0;
				Rrc_Rrs_data.Rrc_GREEN[j] = NAN_f;
				Rrc_Rrs_data.Rrc_RED[j] = NAN_f;
				Rrc_Rrs_data.Rrc_NIR[j] = NAN_f;
			}
		} /* if (CloudDetectData.DATASET[j] + LandMaskData.DATASET[j] == 0 ) */
		else
		{
//			DataSet.BLUE[j] = 0;
//			DataSet.GREEN[j] = 0;

			/* TODO:2018-10-19 */
			Rrc_Rrs_data.Rrs_BLUE[j] = NAN_f;  //Rrs's 2 band is 0
			Rrc_Rrs_data.Rrs_GREEN[j] = NAN_f;

			Rrc_Rrs_data.Rrc_BLUE[j] = NAN_f;  //Rrc's 4 band is 0;
			Rrc_Rrs_data.Rrc_GREEN[j] = NAN_f;
			Rrc_Rrs_data.Rrc_RED[j] = NAN_f;
			Rrc_Rrs_data.Rrc_NIR[j] = NAN_f;

		}  /* else */

	}  /* for (unsigned int j = 0; j <  height * width; j++) */

	for(int i = 0; i < height * width; i++){
//		DataSet.RED[i] = 0;
//		DataSet.NIR[i] = 0;

		/* TODO:2018-10-19 */
		Rrc_Rrs_data.Rrs_RED[i] = 0;  //Rrc's 2 band is 0
		Rrc_Rrs_data.Rrs_NIR[i] = 0;

	}

	_poExHand->sendInfoMessage("End AtmosphericCorrection's by pixel", CLOG_NO_PRODUCT);


//}catch(exception &e){
////	exit(129);
//	DataSet.Flag = -1;
//	DataSet.ErrorInfo = "Atmospheric correction For xunhuan Error";
//
//}

	/*
	 *
	 * free the memory
	 * *********************************************************************************
	 *
	 */
	RELEASE(aerosol_gamma_B3);  /*free the memory*/
//	free(process);

	for(int iLUT = 0; iLUT < 1935; iLUT++){
		RELEASE(LUT_R_B1[iLUT]);
		RELEASE(LUT_R_B2[iLUT]);
		RELEASE(LUT_R_B3[iLUT]);
		RELEASE(LUT_R_B4[iLUT]);
	}
	RELEASE(LUT_R_B1);
	RELEASE(LUT_R_B2);
	RELEASE(LUT_R_B3);
	RELEASE(LUT_R_B4);

	for(int icoefo3 = 0; icoefo3 < 1; icoefo3++){
		RELEASE(coef_O3_B1[icoefo3]);
		RELEASE(coef_O3_B2[icoefo3]);
		RELEASE(coef_O3_B3[icoefo3]);
		RELEASE(coef_O3_B4[icoefo3]);
	}
	RELEASE(coef_O3_B1);
	RELEASE(coef_O3_B2);
	RELEASE(coef_O3_B3);
	RELEASE(coef_O3_B4);

	for(int icoefwater = 0; icoefwater < 1; icoefwater++){
		RELEASE(coef_water_B2[icoefwater]);
		RELEASE(coef_water_B3[icoefwater]);
		RELEASE(coef_water_B4[icoefwater]);
	}
	RELEASE(coef_water_B2);
	RELEASE(coef_water_B3);
	RELEASE(coef_water_B4);

	for(int icoefO2 = 0; icoefO2 < 1; icoefO2++){
		RELEASE(coef_O2_B3[icoefO2]);
		RELEASE(coef_O2_B4[icoefO2]);
	}
	RELEASE(coef_O2_B3);
	RELEASE(coef_O2_B4);

	for(int aeoCount = 0; aeoCount < 4; aeoCount++ ){
		for(int iaeo = 0; iaeo < 9614; iaeo++){
			RELEASE(city_aeo[aeoCount][iaeo]);
			RELEASE(pureSea_aeo[aeoCount][iaeo]);
			RELEASE(sea_aeo[aeoCount][iaeo]);
			RELEASE(land_aeo[aeoCount][iaeo]);
			RELEASE(desert_aeo[aeoCount][iaeo]);
			RELEASE(isothermal_aeo[aeoCount][iaeo]);

			RELEASE(sea_1_aeo[aeoCount][iaeo]);
			RELEASE(sea_2_aeo[aeoCount][iaeo]);
			RELEASE(sea_3_aeo[aeoCount][iaeo]);
			RELEASE(sea_4_aeo[aeoCount][iaeo]);
			RELEASE(sea_5_aeo[aeoCount][iaeo]);

		}
		RELEASE(city_aeo[aeoCount]);
		RELEASE(pureSea_aeo[aeoCount]);
		RELEASE(sea_aeo[aeoCount]);
		RELEASE(land_aeo[aeoCount]);
		RELEASE(desert_aeo[aeoCount]);
		RELEASE(isothermal_aeo[aeoCount]);

		RELEASE(sea_1_aeo[aeoCount]);
		RELEASE(sea_2_aeo[aeoCount]);
		RELEASE(sea_3_aeo[aeoCount]);
		RELEASE(sea_4_aeo[aeoCount]);
		RELEASE(sea_5_aeo[aeoCount]);
	}
	RELEASE(city_aeo);
	RELEASE(pureSea_aeo);
	RELEASE(sea_aeo);
	RELEASE(land_aeo);
	RELEASE(desert_aeo);
	RELEASE(isothermal_aeo);

	RELEASE(sea_1_aeo);
	RELEASE(sea_2_aeo);
	RELEASE(sea_3_aeo);
	RELEASE(sea_4_aeo);
	RELEASE(sea_5_aeo);

	for(int idelta = 0; idelta < 1; idelta++){
		RELEASE(city_delta[idelta]);
		RELEASE(pureSea_delta[idelta]);
		RELEASE(sea_delta[idelta]);
		RELEASE(land_delta[idelta]);
		RELEASE(desert_delta[idelta]);
		RELEASE(isothermal_delta[idelta]);

		RELEASE(sea_1_delta[idelta]);
		RELEASE(sea_2_delta[idelta]);
		RELEASE(sea_3_delta[idelta]);
		RELEASE(sea_4_delta[idelta]);
		RELEASE(sea_5_delta[idelta]);
	}
	RELEASE(city_delta);
	RELEASE(pureSea_delta);
	RELEASE(sea_delta);
	RELEASE(land_delta);
	RELEASE(desert_delta);
	RELEASE(isothermal_delta);

	RELEASE(sea_1_delta);
	RELEASE(sea_2_delta);
	RELEASE(sea_3_delta);
	RELEASE(sea_4_delta);
	RELEASE(sea_5_delta);

	for(int traCount = 0; traCount < 8 ;traCount++){
		for(int itra = 0; itra < 23; itra++){
			RELEASE(city_tra[traCount][itra]);//
			RELEASE(pureSea_tra[traCount][itra]);
			RELEASE(sea_tra[traCount][itra]);
			RELEASE(land_tra[traCount][itra]);
			RELEASE(desert_tra[traCount][itra]);
			RELEASE(isothermal_tra[traCount][itra]);

			RELEASE(sea_1_tra[traCount][itra]);
			RELEASE(sea_2_tra[traCount][itra]);
			RELEASE(sea_3_tra[traCount][itra]);
			RELEASE(sea_4_tra[traCount][itra]);
			RELEASE(sea_5_tra[traCount][itra]);
		}
		RELEASE(city_tra[traCount]);
		RELEASE(pureSea_tra[traCount]);
		RELEASE(sea_tra[traCount]);
		RELEASE(land_tra[traCount]);
		RELEASE(desert_tra[traCount]);
		RELEASE(isothermal_tra[traCount]);

		RELEASE(sea_1_tra[traCount]);
		RELEASE(sea_2_tra[traCount]);
		RELEASE(sea_3_tra[traCount]);
		RELEASE(sea_4_tra[traCount]);
		RELEASE(sea_5_tra[traCount]);
	}
	RELEASE(city_tra);
	RELEASE(pureSea_tra);
	RELEASE(sea_tra);
	RELEASE(land_tra);
	RELEASE(desert_tra);
	RELEASE(isothermal_tra);

	RELEASE(sea_1_tra);
	RELEASE(sea_2_tra);
	RELEASE(sea_3_tra);
	RELEASE(sea_4_tra);
	RELEASE(sea_5_tra);

	xfree(SunZenithData.DATASET);
	xfree(SatelliteZenithData.DATASET);
	xfree(SunAzimuthData.DATASET);
	xfree(SatelliteAzimuthData.DATASET);

	xfree(Lat.DATASET);
	xfree(Lon.DATASET);

    //release
	for(int hdf4Count = 0; hdf4Count < 5 ;hdf4Count++){
		for(int hdf4itra = 0; hdf4itra < 721; hdf4itra++){
			xfree(h4_NCEP[hdf4Count][hdf4itra]);//
		}
		xfree(h4_NCEP[hdf4Count]);
	}
	xfree(h4_NCEP);

	xfree(lat);
	xfree(lon);
	xfree(lon);
	xfree(lat_gezi);
	xfree(lon_gezi);

	_poExHand->sendInfoMessage("Atmospheric correction operation completed.", CLOG_NO_PRODUCT);
	return Rrc_Rrs_data;
}
