/*
 * HDF_STRUCT.h
 *
 *  Created on: 2017年10月20日
 *      Author: zhongyong
 */

#ifndef HDF_STRUCT_H_
#define HDF_STRUCT_H_

// HY-1C/D卫星CZI传感器接口文档-----3.1.1.3 退出码接口
// 成功：0

// 通用不完全成功（1-64）
#define	E_OptInFile_Miss   1		//Optional input file missing
#define	E_OptInFile_RDERR   2 		//Optional input file read failed
#define E_OptInFile_ParExCep    3	//Optional input file parsing exception
#define	E_OptAuxFile_Miss   4 		//Optional auxiliary data file missing
#define	E_OptAuxReadFile_ERR   5	//Optional auxiliary data read failed
#define	E_OptAuxData_ParExCep   6	//Optional auxiliary data parsing exception
#define	E_JobOrderParam_ERR  7		//Job order parameter error
#define	E_ComputerRes_Limit   8		//Limited computer resource application

// 个性不完全成功（65-127）
#define	E_LatLon_ERR   65			//Latitude or Longitude's information is error!
#define	E_Angle_ERR   66			//Angle's value is error!

// 通用失败（128-191）
#define	E_GenJor_Ticket_ERR   128	//General job ticket error
#define	E_JobList_Empty   129		//Job list is empty
#define	E_Job_Param_Excep   130		//Job order parameter exception
#define	E_NeceInFile_Miss  131		//Necessary input file missing
#define	E_ReqInFileRead_ERR   132	//Required input file read failed
#define	E_ReqInFile_ParExcep   133  //Required input file parsing exception
#define	E_NeceAuxFile_Miss   134	//Necessary auxiliary data file missing

#define	E_NeceAuxFile_ReadERR   135	//Necessary auxiliary data read failed
#define	E_NeceAuxFile_ParExCep   136// parsing exception
#define	E_ComputerRes_ERR   137  	//Computer resource application failed
#define	E_Algorithm_ERR   138 		//An error occurred in one part of the algorithm
#define	E_DataFileNotMatch   139 	//Data file does not match

#define	E_FileSYS_ACC_Excep   140 	//file system access exception
#define	E_File_Gener_ERR  141 		//File generation failed
#define	E_Config_Param_ERR   142 	//Configuration file (parameter) error

// 个性失败（192-255）
#define	E_LatLonDimension_ERR  192	//Angle's dimension is error!
#define	E_AngleDimension_ERR   193	//Latitude or Longitude's dimension is error!

/* *
 * Define the struct of the global attrbute
 * Include: count 64
 * ProductName,Satellite,SensorName,SensorMode,SoftwareName,SoftwareVerson,CalibrationVersion,ProcessingTime,ProcessingCenter(9)
 * InputFile,OutputFile,L2PROD,CalFile,NumberOfScanLine,PixelsPerScanLine,SPIXEL,EPIXEL,DPIXEL,SLINE,ELINE,DLINE,PROC_OECEAN(13)
 * PROC_LAND,ATM_COR,GLINT_OPT,METOZ_OPT,METFILE1,METFILE2,METFILE3,ALBEDO,ABSAERl,RHOA_MIN,NLW_MIN,WS_MAX,TAUA_MAX,EPS_MIN(14)
 * EPS_MAX,GLINT_THRESH,Flag_Name,NumOfBands,RangeBeginData,RangeBeginData,RangeBeginTime,RangeEndData,RangeEndTime,UpLeftLat(10)
 * UpLetfLong,UpRightLat,UpRightLong,LowLeftLat,LowLeftLong,LowRightLat,LowRightLat,NorthLat,SourthLat,WestLong,EastLong(11)
 * CenterLat,CenterLong,SceneCenterSolarZenithAngle,OrbitNodeLong,RayleighLUT,RealtimeDelayFlag,OrbitNumber(7)
 * */
struct HDF_Attribute_Data{

	char ProductName[256];
	char SatelliteName[256];
	char SensorName[128];
	char SensorFile[256];//传感器特性数据文件
	char SensorMode[2]; //N表示正常工作模式，T表示俯仰模式，S表示侧摆模式
	char SoftwareName[128];
	char SoftwareVerson[64];
	char CalibrationVersion[64];  //The Version of the calibration (unknown)

	char ProcessingTime[256];
	char ProcessingCenter[64];
	char InputFile[256];
	char OutputFile[256];

	char L2PROD[256];  //The product list of the L2 (unknown)
	char CalFile[256]; //The file of Calibration (unknown)

//	int NumberOfScanLine; //The number of Scan Line(height)
//	int PixelsPerScanLine; //The number of the pixel about the per Line(width)
	short int NumberOfScanLine; //The number of Scan Line(height)
	short int PixelsPerScanLine; //The number of the pixel about the per Line(width)


//	int SPIXEL ; //The start of the pixel
	short int SPIXEL ;
//	int EPIXEL ; //The end of the pixel
	short int EPIXEL ;
//	int DPIXEL ; //The space(jian,ge) of the pixel
	short int DPIXEL ; //The space(jian,ge) of the pixel
//	int SLINE ; //The Line of the start
	short int SLINE ; //The Line of the start
//	int ELINE ; //The line of the end
	short int ELINE ; //The line of the end
//	int DLINE ; //The space(jiange) of the Line
	short int DLINE ; //The space(jiange) of the Line

	//(unknown)
//	int PROC_OECEAN ; //The processing options of the Ocean info(0 or 1),1 is processing ,0 is unprocessing
//	int PROC_LAND ; //The processing options of the Land info(0 or 1),1 is processing ,0 is unprocessing
//	int ATM_COR ; // The option of the atmospheric correction(0 or 1),1 is the COCTS, 0 is others
//	int GLINT_OPT ; //The option of the flare(yaoban) correction,1 is the correction, 0 is uncorrection
//	int METOZ_OPT ; //The option of the supplementary(fuzhu) data,1 is NCEP data, 0 is qihou Data
	short int PROC_OECEAN ; //The processing options of the Ocean info(0 or 1),1 is processing ,0 is unprocessing
	short int PROC_LAND ; //The processing options of the Land info(0 or 1),1 is processing ,0 is unprocessing
	short int ATM_COR ; // The option of the atmospheric correction(0 or 1),1 is the COCTS, 0 is others
	short int GLINT_OPT ; //The option of the flare(yaoban) correction,1 is the correction, 0 is uncorrection
	short int METOZ_OPT ; //The option of the supplementary(fuzhu) data,1 is NCEP data, 0 is qihou Data

	char METFILE1[256] ; //The file of the qihou supplementary1
	char METFILE2[256] ; //The file of the qihou supplementary2
	char METFILE3[256] ; //The file of the qihou supplementary3
	char OZONEFILE1[256] ; //The file of the ozone supplementary1  30

//	int ALBEDOFlag;
	short int ALBEDOFlag;
	float ALBEDO ; //The threshold of the cloud reflect
	float ABSAER ; //The threshold of the absorbed Aerosol(qi,rong,jiao)

	float RHOA_MIN ; //The min  of the reflect threshold on the one to four bands
//	float RRS_MIN ; //The min of the normalized water radiance(guiyihua,lishui,fuliangdu) threshold
	float NLW_MIN;  //1-4波段最小归一化离水辐亮度阈值
	float WS_MAX ; // The max of the wind speed
	float TAUA_MAX ; //Maximum threshold of aerosol optical thickness(qirongjiao,guangxue,houdu) in the 865 wavelength
	float EPS_MIN ; //Minimum threshold of epsilon index for 750 wavelength and 865 wavelength
	float EPS_MAX ; //Maxmum threshold of epsilon index for 750 wavelength and 865 wavelength
	float GLINT_THRESH ; //The threshold of the flare reflect

	char Flag_Name[256];//The flag bit
//	int NumOfBands;
	short int NumOfBands;

	char RangeBeginDate[32]; //The start scan data and total day of the first line
	char RangeBeginTime[32]; //The start scan time of the first line
	char RangeEndDate[32]; //The end scan data and total day of the first line
	char RangeEndTime[32]; //The end scan time of the first line

	// The eight point
//	double UpLeftLat;
//	double UpLeftLong;
//	double UpRightLat;
//	double UpRightLong;
//	double LowLeftLat;
//	double LowLeftLong;
//	double LowRightLat;
//	double LowRightLong;
	float UpLeftLat;
	float UpLeftLong;
	float UpRightLat;
	float UpRightLong;
	float LowLeftLat;
	float LowLeftLong;
	float LowRightLat;
	float LowRightLong;

//	double NorthLat; //The latitude of the most Northern
//	double SourthLat;//The latitude of the most Southern
//	double WestLong;//The longitude of the most Western
//	double EastLong;//The longitude of the most Eastern
//	double SceneCenterLat;
//	double SceneCenterLong;
	float NorthLat; //The latitude of the most Northern
	float SourthLat;//The latitude of the most Southern
	float WestLong;//The longitude of the most Western
	float EastLong;//The longitude of the most Eastern
	float SceneCenterLat;
	float SceneCenterLong;
	char SceneCenterTime[256]; //

//	double SceneCenterSolarZenithAngle;//center taiyang tiandingjiao
	float SceneCenterSolarZenithAngle;//center taiyang tiandingjiao
	float OrbitNodeLong; //guidao jiangjiaodian longitude(unknown)
//	int RayleighLUT; //The options of the Rayleigh
	short int RayleighLUT;
	//瑞利散射计算选项: 0单次散射计算，1自带查找表；2基于SeaWiFS查找表转换计算; (unknown)

	char OrbitFlag[256];

	char RealtimeDelayFlag[64];
	int OrbitNumber;

	char* ErrorInfo;
	int Flag;

};

struct HDF_Attribute_Navigation{

	//Navigation Data Linear_Interpolation
	int AnglePoint;			 // Navigation Angle Point Counts
	int AnglePoint_First;	 // First Navigation Angle Point
	int AnglePoint_intervals;// Pixel-intervals of Navigation Angle Point

	char* ErrorInfo;
	int Flag;

};

/** 定义一个结构体：HDF_DATA
 *  包含了数组的长，宽和维数
 *  其中包含了一个一维数组，也就是所要提取的数据集（二维数组）存储在一个一维数组中。
 */
struct HDF_GEODATA{
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	float* RED;
	float* GREEN;
	float* BLUE;
	float* NIR;
	char* ErrorInfo;
};

/** 定义一个结构体：HDF_DATA
 *  包含了数组的长，宽和维数
 *  其中包含了一个一维数组，也就是所要提取的数据集（二维数组）存储在一个一维数组中。
 */
struct HDF_Rrc_Rrs{
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	int AngleFlag;
	int LatLonFlag;
	int ClimateFlag;

	float* Rrc_RED;
	float* Rrc_GREEN;
	float* Rrc_BLUE;
	float* Rrc_NIR;

	float* Rrs_RED;
	float* Rrs_GREEN;
	float* Rrs_BLUE;
	float* Rrs_NIR;

	char* ErrorInfo;
};

/** 定义一个结构体：HDF_DATA
 *  包含了数组的长，宽和维数
 *  其中包含了一个一维数组，也就是所要提取的数据集（二维数组）存储在一个一维数组中。
 */
struct HDF_DATA{
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	float* DATASET;
	float scale_factor;
	char* ErrorInfo;
};
struct HDF_DATA_DOUBLE{
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	double* DATASET;
	char* ErrorInfo;
};

/** 定义一个结构体：HDF_DATA
 *  包含了数组的长，宽和维数
 *  其中包含了一个一维数组，也就是所要提取的数据集（二维数组）存储在一个一维数组中。
 */
struct HDF_DATA_INT{
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	int* DATASET;
	float scale_factor;
	char* ErrorInfo;
};
/* *
 * Define the struct of Sensor Band Parameters
 * the data : Wavelength,SyscalGain,SyscalOffset,F0,k_oz,tau_r
 * */

struct Sensor_Band_Param{
	int Wavelength[4]; //bochuang ,There is a unit attribute (nm)
	float SyscalGain[4]; //dingbiao xishu,(none)
	float SyscalOffset[4]; //dingbiao pianyi(mW/cm2.µm. sr)
	float F0[4];	// Average atmospheric solar irradiance (mW/cm2.µm)
	float k_oz[4]; //Band equivalent ozone absorption coefficient(1/cm)
	float tau_r[4]; //Band equivalent Rayleigh optical thickness(none)

};

/* *
 * Define the struct of ScanLineAttributes
 * the data : Year,Day,Millisecond,StartLong,StartLat,CenterLong,CenterLat,EndLong,EndLat,CenterSolarZenithAngle
 * */

struct ScanLineAttributes{
	int Year; //scan years(years)
	int Day;  //scan days(years)
	int Millisecond; //scan millisecond(millisecond)
	float* StartLong; //The start scan pixel longitude(degree) n*1
	float* StartLat; //The start scan pixel latitude(degree)n*1
	float* CenterLong; //The Center scan pixel longitude(degree)n*1
	float* CenterLat; //The Center scan pixel latitude(degree)n*1
	float* EndLong; //The end scan pixel longitude(degree)n*1
	float* EndLat; //The end scan pixel latitude(degree)n*1
	float* CenterSolarZenithAngle; //The Center scan pixel Solar Zenith Angle(degree)n*1

};

/* *
 * Define the struct of NavigationData
 * the data : Latitude,Longitude,NavigationPoints,FirstNavigationPoint,PixelinterOfNavigationPoints
 * */

struct NavigationData{
	float* Latitude; //The pixel of Image latitude(degree)  n*m
	float* Longitude; //The pixel of Image longitude(degree) n*m
	int NavigationPoints; //The count pixel of the per line  (none)
	int FirstNavigationPoint;  //The start pixel num per line(none)
	int PixelinterOfNavigationPoints; //The jiange pixel num per line(none)

	char* ErrorInfo;
	int Flag;

};


struct HDF_ATTRIBUTES{
	int TYPE;
	int length;
	int ATTRIBUTES_INT;
	float ATTRIBUTES_FLOAT;
	char* ATTRIBUTES_CHAR;
	int Flag;
};

struct L1B_XML_DATA{
	int Flag;
	char* ErrorInfo;
	// 从L1B中读的数据------------
	// ProductMetaData
	char SatelliteID[256];
	char SensorID[256];
	char OrbitID[256];
	char ReceiveStation[256];
	char SceneID[256];
	char ProductName[256];
	char TimeType[256];
	char ProductUnit[256];
	char ContactInfo[256];
		// SatelliteInfo
	char OrbitFlag[256];
	char WorkMode[256];
		// QualityInfo
	char ProductIntegrity[256];
		// ProductInfo
	char ProductLevel[256];
	char ProductFormat[256];
	char ProductVersion[256];
	char SoftwareName[256];
	char SoftwareVersion[256];
	char ProduceTime[256];
		// CorrectionParams
	char EarthEllipsoid[256];
	char ProjectType[256];
		// ImageInfo
	char StartTime[256];
	char EndTime[256];
	char CenterTime[256];
	char DataSize[256];
	char DataUnit[256];

	char CenterLocation_Latitude[256];
	char CenterLocation_Longitude[256];

	char TopLeft_Latitude[256];
	char TopLeft_Longitude[256];

	char TopRight_Latitude[256];
	char TopRight_Longitude[256];

	char BottomRight_Latitude[256];
	char BottomRight_Longitude[256];

	char BottomLeft_Latitude[256];
	char BottomLeft_Longitude[256];

	// L2写入的数据-------------
	char AuxData1[256]; // 辅助数据 1
	char SourseData[256]; // 输入文件,包含辅助数据
	char Resolution[256]; // 产品分辨率
	char QuickViewURL[256]; //拇指图路径 （存档或运控分系统提供）

};
struct XML_DATA{ //

	// ------------
	// ProductMetaData
	char SatelliteID[256];
	char SensorID[256];
	char OrbitID[256];
	char ReceiveStation[256];
	char SceneID[256];
	char ProductName[256];
	char FileName[256];//added-2018-10-15
	char TimeType[256];
	char ProductUnit[256];
	char ContactInfo[256];
		// SatelliteInfo
	char OrbitFlag[256];
	char WorkMode[256];
		// QualityInfo
	//char ProductIntegrity[256];
	char AuxData1[256];
	char AuxData2[256];
	char AuxData3[256];
		// ProductInfo
	char ProductLevel[256];
	char ProductFormat[256];
	char ProductVersion[256];
	char SoftwareName[256];
	char SoftwareVersion[256];
	char ProduceTime[256];
		// CorrectionParams
	char EarthEllipsoid[256];
	char ProjectType[256];
		// ImageInfo
	char StartTime[256];
	char EndTime[256];
	char CenterTime[256];
	char DataSize[256];
	char DataUnit[256];

	char CenterLocation_Latitude[256];
	char CenterLocation_Longitude[256];

	char TopLeft_Latitude[256];
	char TopLeft_Longitude[256];

	char TopRight_Latitude[256];
	char TopRight_Longitude[256];

	char BottomRight_Latitude[256];
	char BottomRight_Longitude[256];

	char BottomLeft_Latitude[256];
	char BottomLeft_Longitude[256];

	// L2写入的数据-------------
	char SourseData[256]; // 输入文件,包含辅助数据
	char Resolution[256]; // 产品分辨率
	char QuickViewURL[256]; //拇指图路径 （存档或运控分系统提供）

};


struct MyConfig{
	int Flag;
	char* ErrorInfo;

	char CloudThreshold[8];
	char ShadowThreshold[8];
	char MedianFilterSize[8];

	char NDVIThreshold[8];

	char NDWIThreshold[8];
	char GuassFilteSize[8];

	char AtmosphCorrecteMethod[8];
	char NCEPPathName[256];
	char NCEPClimePathName[256];
	char COCTSPathName[256];
	char AuxDataPath[256];

	char SSCCamParam1[8];
	char SSCCamParam2[8];
	char SSCCamParam3[8];
	char SSCCamParam4[8];

	char CHLCamParam1[8];
	char CHLCamParam2[8];
	char CHLCamParam3[8];
	char CHLCamParam4[8];
	char CHLCamParam5[8];

	char SDDCamParam1[8];
	char SDDCamParam2[8];
	char SDDCamParam3[8];
};


struct HDF5_CHAR{
	int RANK;
	int Height;
	int Width;
	char* Datachar;
	int nsize;
	int Flag;
	char* ERRORINFO;

};

// h5 NECP data
struct HDF_DATA_NECP{
	int month;
	int RANK;
	int ROWS;
	int COLUMNS;
	int Flag;
	float* DATASET;
	char* ErrorInfo;
};

// h4 sub NECP data
struct HDF_get_sub_date_NECP{
	float** get_sub_date_NECP;
	int Flag;
	char* ErrorInfo;
};

// h4 NECP data
struct HDF_hdf4Read_Climatedata{
	float*** hdf4Read_Climatedata;
	int Flag;
	char* ErrorInfo;
};
#endif /* HDF_STRUCT_H_ */
