/*
 * hy1_czi_l2_pro.cpp
 *
 *  Created on: 2018
 *     Author: zhangyonghui and feidong
 *     version: V1.0
 */

#include "hy1_czi_l2_pro.hpp"
//#include"CGEN_EnumsHandler.hpp"
#include<fstream>
#include<iomanip>
#include<string>
#include"string.h"
#include<cstring>
#include<cstdlib>
#include<time.h>
#include<stdlib.h>
#include"hdf5.h"
#include<sys/stat.h>
#include<sys/types.h>
//#include<direct.h>
extern "C"{
	#include"BasicOperation/Basic_Operation.h"
	#include"BasicOperation/Read_Data_Attr_Xml.h"
}
#include"BasicOperation/ReadWriteH5Char.hpp"
#include"BasicOperation/Write_Data_Attr_Xml.hpp"
#include"BasicOperation/SaveJPEG.hpp"
#include"BasicOperation/rendering.hpp"
#include"hy1_czi_l2b.hpp"
#include"hy1_czi_l2c.hpp"
#include"HDF_STRUCT.h"

#include"CloudDetect/Cloud_detect.h"
#include"LandMask/Land_Mask.h"
#include"AtmosphericCorrection/Atmospheric_Correction.h"
#include <exception>
#include<cmath>
#include<limits>
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}       //release array

using namespace std;
/* define NaN */
typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();
HY1_CZI_L2A_PRO::HY1_CZI_L2A_PRO() : CGEN_IProcessing()
{
}

HY1_CZI_L2A_PRO::~HY1_CZI_L2A_PRO()
{
}

/*
 * 1. 数据处理的前置步骤（可选）。
 */
PIK_Int_t HY1_CZI_L2A_PRO::startPreProcessing(CGEN_ExecutionHandler * ai_poHandlerExec)
{
    CGEN_IProcessing::startPreProcessing( ai_poHandlerExec );
    //_poExHand->sendDebugMessage("==================== custom debug message1 ====================", 0);
    _poExHand->sendInfoMessage("==================== send info message ====================", CLOG_NO_PRODUCT);
    return PIK_SUCCESS;
}

void do_sig(int)
{
	printf("Accept signal 15, it's deleting temporary file\n");

	// 删除四个输出文件
//	if(FileIsExist(OutputH5FileName) == 0)
//	{
//		cout<<"H5 File Is Exist,it's deleting..."<<endl;
//		if(remove(OutputH5FileName)==0){
//			cout<<" delete success."<<endl;
//		}
//	}
//	if(FileIsExist(OutputXmlFileName) == 0)
//	{
//		cout<<"Xml File Is Exist,it's deleting..."<<endl;
//		if(remove(OutputXmlFileName)==0){
//			cout<<" delete success."<<endl;
//		}
//	}
//	if(FileIsExist(OutputJpgFileName) == 0)
//	{
//		cout<<"Jpg File Is Exist,it's deleting..."<<endl;
//		if(remove(OutputJpgFileName)==0){
//			cout<<" delete success."<<endl;
//		}
//	}
//	if(FileIsExist(OutputListFileName) == 0)
//	{
//		cout<<"File List Is Exist,it's deleting..."<<endl;
//		if(remove(OutputListFileName)==0){
//			cout<<" delete success."<<endl;
//		}
//	}
	// 删除GeoDataSet的四个数据集
//	if(GeoDataSet.BLUE != NULL) {
//		free(GeoDataSet.BLUE);
//	}
//	if(GeoDataSet.GREEN != NULL) {
//		free(GeoDataSet.GREEN);
//	}
//	if(GeoDataSet.RED != NULL) {
//		free(GeoDataSet.RED);
//	}
//	if(GeoDataSet.NIR != NULL) {
//		free(GeoDataSet.NIR);
//	}

//	printf("向云控发消息\n");

	exit(-1);
}

/*
 * 2. 数据处理（必选）。
 */

PIK_Int_t HY1_CZI_L2A_PRO::startProcessing()
{
    /*
     * 从Joborder获取输出文件名。
     * 目前的PIK未对File_Name_Type做处理，本示例程序假定使用的为Stem形式。
     * 文件全名组成: A/B.C, 其中A表示目录, B 表示文件名，C表示扩展名（例如h5、nc、xml、jpg、tif）
                   - B     Logical
                   - A/B   Stem
                   - A/B.C Physical
    */

	/*
	 * start time of program
	 */
	time_t startT = time(0);
	char startTime[32] ;
	strftime(startTime, sizeof(startTime), "%Y-%m-%dT%X",localtime(&startT));

	PIK_Int_t iRet = E_OK; // Exit Code

	char* SoftVerison = "1.0";

/***************************************************************************  Getting the Input and Output file list  *****************************************************************************/

	/*
	 * Getting the Input and Output file list
	 */
	 _poExHand->sendInfoMessage("Start getting the Input file list and Output File list", CLOG_NO_PRODUCT);

	// get the Input File Path and Name
	vector<CFGEN_JobOrder_File> oL1BInFileNameList = _poExHand->getJobOrder().getInputFilesList();

	int InPutListSize = oL1BInFileNameList.size(); // Get Size of Input List

//	string pathTmpL1b = oL1BInFileNameList.at(0).getPath();

	/* H1C  OPER  CZI  L1B  20190101T010000 20190101T010500  00103  10.h5 to stringL1BName */
	char tmpL1BName[256], tmpL1BName_h5[256];
	strcpy(tmpL1BName, oL1BInFileNameList.at(0).getName().c_str()); // Get All Name of L1B
	strcpy(tmpL1BName_h5, tmpL1BName);

	/* 切割字符串 */
	string stringL1BName[16];
	char* resultName1 = strtok( tmpL1BName, "_" );
	int i = 0;
	stringL1BName[i] = resultName1;
	while(resultName1 != NULL){
		resultName1 = strtok( NULL, "_" );
		if(resultName1 != NULL){
			i++;
			stringL1BName[i] = resultName1;
		}
	}

	/* get input file */
	CFGEN_JobOrder_File InputFile;
	string InputName[InPutListSize];
	char InName[InPutListSize][256];
	for(int i=0;i<InPutListSize;i++){
		InputFile = oL1BInFileNameList.at(i);
		InputName[i] = InputFile.getPath() + "/" + InputFile.getName();
		strcpy(InName[i], InputName[i].c_str());
		//cout<<InName[i]<<endl;
	}

	char* cInputH5Path = InName[0];
	char* MetaData = ""; char* NCEPPathName = ""; char* COCTSPathName = "";

	/* Judge */
	if(strcmp(InName[1]," ") != 0 || InName[1] != NULL){//Mate file
		if(FileIsExist(InName[1]) != 0){
			iRet = E_NeceInFile_Miss;
			_poExHand->sendErrorMessage("The Mate Data does not exist!", "hy1_czi_l2_pro.cpp, 151 line", "GetInputList(char*)", iRet);
			return iRet;
		}
		else{
			MetaData = InName[1];
		}
	}else{
		iRet = E_NeceInFile_Miss;
		_poExHand->sendErrorMessage("The Mate Data Path does not exist in the JobOrder!", "hy1_czi_l2_pro.cpp, 151 line", "GetInputList(char*)", iRet);
		return iRet;
	}

	/* get L2 output path */
	PIK_String_t oL2OutputPath = _poExHand->getJobOrder().getOutputFileName("L2");

	/* H1C  OPER  CZI */
	PIK_String_t L1BPreThree = stringL1BName[0] + "_" + stringL1BName[1] + "_" +stringL1BName[2];

	/* 10.h5中的10 */
	char* VersionData = "10"; // TODO zhen dui data Version
	/* get L2A output path */
	char oL2APath[512];
	PIK_String_t L2AToltalName = L1BPreThree + "_L2A_" + stringL1BName[4] + "_" + stringL1BName[5] + "_" +stringL1BName[6];
	strcpy(oL2APath, oL2OutputPath.c_str());
	strcat(oL2APath,"/");
	strcat(oL2APath,L2AToltalName.c_str());
	strcat(oL2APath,"_");
	strcat(oL2APath,VersionData);

	//L2A_filename
	char L2A_productname[256];  //用于后面的写入属性
	strcpy(L2A_productname, L2AToltalName.c_str());
	strcat(L2A_productname, "_");
	strcat(L2A_productname, VersionData);

	/* get L2B output path */
	char oL2BPath[512];
	PIK_String_t L2BToltalName = L1BPreThree + "_L2B_" + stringL1BName[4] + "_" + stringL1BName[5] + "_" +stringL1BName[6];
	strcpy(oL2BPath, oL2OutputPath.c_str());
	strcat(oL2BPath,"/");
	strcat(oL2BPath,L2BToltalName.c_str());
	strcat(oL2BPath,"_");
	strcat(oL2BPath,VersionData);

	//L2B_filename
	char L2B_filename[256];  //用于后面的写入属性
	strcpy(L2B_filename, L2BToltalName.c_str());
	strcat(L2B_filename, "_");
	strcat(L2B_filename, VersionData);

	/* get L2C output path */
	char oL2CPath[512];
	PIK_String_t L2CToltalName = L1BPreThree + "_L2C_" + stringL1BName[4] + "_" + stringL1BName[5] + "_" +stringL1BName[6];
	strcpy(oL2CPath, oL2OutputPath.c_str());
	strcat(oL2CPath,"/");
	strcat(oL2CPath,L2CToltalName.c_str());
	strcat(oL2CPath,"_");
	strcat(oL2CPath,VersionData);

	//L2C_filename
	char L2C_filename[256];  //用于后面的写入属性
	strcpy(L2C_filename, L2CToltalName.c_str());
	strcat(L2C_filename, "_");
	strcat(L2C_filename, VersionData);

	/* 创建L2输出的文件夹 */
	mkdir(oL2APath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
	mkdir(oL2BPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
	mkdir(oL2CPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);

	char oL2APath_URL[512];	strcat(oL2APath_URL, oL2APath);
	char oL2BPath_URL[512];	strcat(oL2BPath_URL, oL2BPath);
	char oL2CPath_URL[512];	strcat(oL2CPath_URL, oL2CPath);

	/* 创建L2A L2B L2C输出的文件路径及名称（部分） */
	strcat(oL2APath,"/");
	strcat(oL2APath,L2AToltalName.c_str());
	strcat(oL2BPath,"/");
	strcat(oL2BPath,L2BToltalName.c_str());
	strcat(oL2CPath,"/");
	strcat(oL2CPath,L2CToltalName.c_str());

	PIK_String_t oL2AStemFileName = oL2APath;
	PIK_String_t oL2BStemFileName = oL2BPath;
	PIK_String_t oL2CStemFileName = oL2CPath;

	/*
	 * 生成列表文件的全名，与主产品文件放在同一目录。
	 *
	 * TODO: OrderId > 2^31 (2.147yi)hui ERROR
	 */
	PIK_String_t oL2AListFileName = "",
				 oL2ARepoFileName = "";
	stringstream _oStreamL2A;
	_oStreamL2A << "/" \
			 	 <<setfill('0') << setw(10) << _poExHand->getJobOrder().getOrderId(); //TODO:need modify 10 to 9
	_oStreamL2A >> oL2AListFileName;
	oL2ARepoFileName = oL2AListFileName;
	oL2AListFileName = oL2OutputPath + oL2AListFileName + ".product.xml";
	oL2ARepoFileName = oL2OutputPath + oL2ARepoFileName + ".report.xml";

	char L2ListFilePath[256];	strcpy(L2ListFilePath,oL2AListFileName.c_str());
	char L2RepoFilePath[256];	strcpy(L2RepoFilePath,oL2ARepoFileName.c_str());

	/*
	 * 分别定义L2A数据、元数据、缩略图，并初始化。
	 *
	 * TODO: 2018-10-19
	 */
	PIK_String_t  oL2ADataFileName = oL2AStemFileName + "_" + VersionData + ".h5",
				  oL2AMetaFileName = oL2AStemFileName + "_" + VersionData + ".meta.xml",
				  oL2AThumBlueName = oL2AStemFileName + "_Rc1_" +VersionData + ".jpg", // FIXME _Rrs460_
				  oL2AThumGreenName = oL2AStemFileName + "_Rc2_" +VersionData + ".jpg",
				  oL2AThumRedName = oL2AStemFileName + "_Rc3_" +VersionData + ".jpg",
				  oL2AThumNirName = oL2AStemFileName + "_Rc4_" +VersionData + ".jpg";

	char L2AH5Path[256]; 		strcpy(L2AH5Path,oL2ADataFileName.c_str());
	char L2AMetaPath[256];		strcpy(L2AMetaPath,oL2AMetaFileName.c_str());
	char L2AJpgBluePath[256];	strcpy(L2AJpgBluePath,oL2AThumBlueName.c_str());
	char L2AJpgGreenPath[256];	strcpy(L2AJpgGreenPath,oL2AThumGreenName.c_str());
	char L2AJpgRedPath[256];	strcpy(L2AJpgRedPath,oL2AThumRedName.c_str());
	char L2AJpgNirPath[256];	strcpy(L2AJpgNirPath,oL2AThumNirName.c_str());


	/*
	 * 分别定义L2B数据、元数据、缩略图，并初始化。
	 */
	PIK_String_t  oL2BDataFileName = oL2BStemFileName + "_" + VersionData + ".h5",
				  oL2BMetaFileName = oL2BStemFileName + "_" + VersionData + ".meta.xml",
				  oL2BThumNDVIName = oL2BStemFileName  + "_NVI_" + VersionData + ".jpg",
				  oL2BThumSSCName = oL2BStemFileName + "_SSC_" + VersionData + ".jpg";


	char L2BH5Path[256]; 		strcpy(L2BH5Path,oL2BDataFileName.c_str());
	char L2BMetaPath[256];		strcpy(L2BMetaPath,oL2BMetaFileName.c_str());
	char L2BJpgNDVIPath[256];	strcpy(L2BJpgNDVIPath,oL2BThumNDVIName.c_str());
	char l2BJpgSSCPath[256];	strcpy(l2BJpgSSCPath,oL2BThumSSCName.c_str());

	/*
	 * 分别定义L2C数据、元数据、缩略图，并初始化。
     */
	PIK_String_t  oL2CDataFileName = oL2CStemFileName + "_" + VersionData + ".h5",
	              oL2CMetaFileName = oL2CStemFileName + "_" + VersionData + ".meta.xml",
	              oL2CThumChlName = oL2CStemFileName + "_CHL_" + VersionData + ".jpg",
				  oL2CThumSDDName = oL2CStemFileName + "_SDD_" + VersionData + ".jpg",

				  oL2CThumBlueName = oL2CStemFileName + "_Rs1_" +VersionData + ".jpg", // TODO: 2018-10-19
				  oL2CThumGreenName = oL2CStemFileName + "_Rs2_" +VersionData + ".jpg",
				  oL2CThumRedName = oL2CStemFileName + "_Rs3_" +VersionData + ".jpg",
				  oL2CThumNirName = oL2CStemFileName + "_Rs4_" +VersionData + ".jpg";

	char L2CH5Path[256]; 		strcpy(L2CH5Path,oL2CDataFileName.c_str());
	char L2CMetaPath[256];		strcpy(L2CMetaPath,oL2CMetaFileName.c_str());
	char L2CJpgChlPath[256];	strcpy(L2CJpgChlPath,oL2CThumChlName.c_str());
	char l2CJpgSDDPath[256];	strcpy(l2CJpgSDDPath,oL2CThumSDDName.c_str());

	//Rrs's jpg to L2C
	char L2CJpgBluePath[256];	strcpy(L2CJpgBluePath,oL2CThumBlueName.c_str());  //TODO: 2018-10-19
	char L2CJpgGreenPath[256];	strcpy(L2CJpgGreenPath,oL2CThumGreenName.c_str());
	char L2CJpgRedPath[256];	strcpy(L2CJpgRedPath,oL2CThumRedName.c_str());
	char L2CJpgNirPath[256];	strcpy(L2CJpgNirPath,oL2CThumNirName.c_str());

	_poExHand->sendInfoMessage("Getting the Input file list and Output File list successful", CLOG_NO_PRODUCT);

/***************************************************************************  Read Data and Attribute from H5 file  *****************************************************************************/

	/* Read the H5 input dataSet form the Input H5 path */
	_poExHand->sendInfoMessage("Start Reading the H5 file from the Input File Paths", CLOG_NO_PRODUCT);

	/* Read the some H5 attributes */
	struct HDF_Attribute_Data AttributeData = ReadH5Attribute(cInputH5Path);

	/* Judge */
	if( AttributeData.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage(AttributeData.ErrorInfo, "hy1_czi_l2_pro.cpp,288 line","ReadH5Attribute(char*)",iRet);
		return iRet;
	}
	if(AttributeData.NumOfBands != 4){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("The H5 file don't have four bands!", "hy1_czi_l2_pro.cpp,284 line","ReadH5Attribute(char*  )",iRet);
		return iRet;
	}

	/* Read the Geopyhsical data */
	struct HDF_GEODATA GeoDataSet;
	GeoDataSet = ReadGeoData(cInputH5Path);

	/* judge ReadGeoData */
	if(GeoDataSet.Flag == -1 ){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage(GeoDataSet.ErrorInfo, "hy1_czi_l2_pro.cpp,288 line","ReadGEOData(char*)",iRet);
		return iRet;
	}
	_poExHand->sendInfoMessage("Reading the H5 file Successful", CLOG_NO_PRODUCT);

    _poExHand->sendProgressMessage(20, 0);

	/* 获得数据集大小的尺寸 */
	int height = GeoDataSet.ROWS;
	int width = GeoDataSet.COLUMNS;

	// FIXME
	/* judge */
	if(AttributeData.PixelsPerScanLine != width || AttributeData.NumberOfScanLine != height){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("The H5 File Dataset size and Attributes don't match!", "hy1_czi_l2_pro.cpp,288 line","ReadGEOData(char*)",iRet);
		return iRet;
	}

	/*
	 * **********************************************************************************************
	 * 处理数据操作
	 * 1.云检测模块Cloud Detect
	 * 2.NDVI模块
	 * 3.海陆掩膜模块Sea and land mask
	 * 4.大气矫正模块Atmospheric correction
	 * 5.将生成的结果四个波段的数据写入jpg文件中。
	 * **********************************************************************************************
	 */

	/*
	 * ------------------------------------------读取 config-OWN-xml----------------------------------------
	 * 处理模块所需参数均从Config文件中读取
	 * CNF_OWN__hy1_czi_l2_pro.xml
	 */

	/* Read Config */
	char* myConfigName = "../config/CNF_OWN__hy1_czi_l2_pro.xml";
	struct MyConfig myConfigXml = ReadMyConfig(myConfigName);

	/* Judge */
	if( myConfigXml.Flag == -1){
		iRet = E_OptInFile_RDERR;
		_poExHand->sendErrorMessage(myConfigXml.ErrorInfo, "hy1_czi_l2_pro.cpp,421 line","ReadMyConfig(const char*)",iRet);
		return iRet;
	}
/***************************************************************************  Cloud detect  *****************************************************************************/

	 /* **********************************************************************************************
	 *
	 * Cloud detect
	 *
	 * Start the Cloud Detect Modules,and
	 * Read some thresholds which can use the some modules from the joborder
	 * **********************************************************************************************
	 */

	_poExHand->sendInfoMessage("Start processing the Cloud Detect Module", CLOG_NO_PRODUCT);
	_poExHand->sendInfoMessage("Start Reading the thresholds of Cloud Detect Module", CLOG_NO_PRODUCT);

	/* 获取参数 */
//	PIK_String_t CloudThreshold = _poExHand->getJobOrder().getDynamicProcessingParameterValue("CloudThreshold");  //read data from Joborder
	PIK_String_t CloudThreshold = myConfigXml.CloudThreshold;  //read data from CNF
	float CloudT1 = atof(CloudThreshold.c_str());
//	PIK_String_t ShadowThreshold = _poExHand->getJobOrder().getDynamicProcessingParameterValue("ShadowThreshold");
	PIK_String_t ShadowThreshold = myConfigXml.ShadowThreshold;
	float CloudT2 = atof(ShadowThreshold.c_str());
//	PIK_String_t MedianFilterSize = _poExHand->getJobOrder().getDynamicProcessingParameterValue("MedianFilterSize");
	PIK_String_t MedianFilterSize = myConfigXml.MedianFilterSize;
	int CloudT3 = atoi(MedianFilterSize.c_str());

	AttributeData.ALBEDO = CloudT1; 		//Cloud detect Threshold

	/* Cloud Detect */
	struct HDF_DATA_INT CloudDetectData = CloudDetect(GeoDataSet, CloudT1, CloudT2, CloudT3);

	/* Judge */
	if(CloudDetectData.Flag == -1){
		//向云控发消息
		iRet = E_Algorithm_ERR;
		_poExHand->sendErrorMessage("Data processing error on the Cloud detect module!", "hy1_czi_l2_pro.cpp,266 line","CloudDetect(char* , float , float, int )", iRet);
		return iRet;
	}
	_poExHand->sendInfoMessage("Processing the Cloud detect Module successful", CLOG_NO_PRODUCT);
	_poExHand->sendProgressMessage(30, 100);

/*******************************************************************************  NDVI  ****************************************************************************/

	 /* ************************************************************************************************
	 *
	 * NDVI module (NDVI模块的计算与写入在后面)
	 *
	 * ************************************************************************************************
	 */
	_poExHand->sendInfoMessage("Start processing the NDVI Module", CLOG_NO_PRODUCT);
	_poExHand->sendInfoMessage("Start Reading the thresholds of NDVI Module", CLOG_NO_PRODUCT);

	/* 获取参数 */
//	PIK_String_t NDVIThreshold = _poExHand->getJobOrder().getDynamicProcessingParameterValue("NDVIThreshold");
	PIK_String_t NDVIThreshold = myConfigXml.NDVIThreshold;
	float NDVIThresh = atof(NDVIThreshold.c_str());
	PIK_String_t NDWIThreshold = myConfigXml.NDWIThreshold;
	float NDWIThresh = atof(NDWIThreshold.c_str());

/********************************************************************************* Sea and land mask **************************************************************************/

	/* ***********************************************************************************************
	 *
	 * Sea and land mask
	 *
	 * ***********************************************************************************************
	 */
	_poExHand->sendInfoMessage("Start processing the Sea and Land Mask Module", CLOG_NO_PRODUCT);
	_poExHand->sendInfoMessage("Start Reading the thresholds of Sea and Land Mask Module", CLOG_NO_PRODUCT);

	/* 获取参数 */
//	PIK_String_t NDWIThreshold = _poExHand->getJobOrder().getDynamicProcessingParameterValue("NDWIThreshold");
//	PIK_String_t GuassFilteSize = _poExHand->getJobOrder().getDynamicProcessingParameterValue("GuassFilteSize");
	PIK_String_t GuassFilteSize = myConfigXml.GuassFilteSize;
	int GuassSize = atoi(GuassFilteSize.c_str());

	/* Land Mask */
	struct HDF_DATA_INT LandMaskData = LandMask(GeoDataSet, GuassSize, NDWIThresh);

	/* Judge */
	if(LandMaskData.Flag == -1){
		//向云控发消息
		iRet = E_Algorithm_ERR;
		_poExHand->sendErrorMessage("Data processing error on the Sea and Land Mask module!", "hy1_czi_l2_pro.cpp,354 line","LandMask(struct * ,int ,float )", iRet);
		return iRet;
	}

	_poExHand->sendInfoMessage("Processing the Sea and Land Mask Module successful", CLOG_NO_PRODUCT);
	_poExHand->sendProgressMessage(45, 0);

/******************************************************************************  Atmospheric correction  *************************************************************************/

	/* *************************************************************************************
	 *
	 *Atmospheric correction
	 *
	 * *************************************************************************************
	 */
	_poExHand->sendInfoMessage("Start processing the Atmospheric Correction Module", CLOG_NO_PRODUCT);
//	_poExHand->sendInfoMessage("Start Reading the thresholds of Atmospheric Correction Module", CLOG_NO_PRODUCT);

	/* 获取参数 */
	/* 1, NCEP real-time data; 2, NCEP climate data; 3, COCTS */

//	PIK_String_t AtmosphCorrecteMethod = _poExHand->getJobOrder().getDynamicProcessingParameterValue("AtmosphCorrecteMethod");
	PIK_String_t AtmosphCorrecteMethod = myConfigXml.AtmosphCorrecteMethod;
	int ASCMethod = atoi(AtmosphCorrecteMethod.c_str());

//	PIK_String_t AuxDataPathName = _poExHand->getJobOrder().getDynamicProcessingParameterValue("AuxDataPathName");
	char NCEPDataName[256];

	PIK_String_t NCEPName, NCEPClimeName, COCTSName ;
//	NCEPClimeName = "./processing/data/N201809115_MET_NCEP_1440x0721_f015.hdf";

	switch(ASCMethod){
		case 1:
//			NCEPName = _poExHand->getJobOrder().getDynamicProcessingParameterValue("NCEPPathName");
			NCEPPathName = myConfigXml.NCEPPathName;
			NCEPName = NCEPPathName;//改为从JobOrder中读取
			strcpy(NCEPDataName, NCEPName.c_str());
			if(openhdf4(NCEPDataName) == -1){
				//向云控发消息
				iRet = E_NeceInFile_Miss;
				_poExHand->sendErrorMessage("The NCEP dataSet File don't exist, please check the config file", "hy1_czi_l2_pro.cpp,354 line","openhdf4(char* )", iRet);
				return iRet;
			}

			break;
		case 2:
//			NCEPClimeName = _poExHand->getJobOrder().getDynamicProcessingParameterValue("NCEPClimePathName");

			NCEPPathName = myConfigXml.NCEPClimePathName;
			NCEPClimeName = NCEPPathName;
			strcpy(NCEPDataName, NCEPClimeName.c_str());
			break;
		case 3:
//			COCTSName = _poExHand->getJobOrder().getDynamicProcessingParameterValue("COCTSPathName");
			COCTSPathName = myConfigXml.COCTSPathName;
			COCTSName = COCTSPathName;//改为从JobOrder中读取
			strcpy(NCEPDataName, COCTSName.c_str());
			break;
	}

	// 截取路径，保存至H5 product
	string NCEPDataName_str = NCEPDataName;
	string NCEPDataName_cut= NCEPDataName_str.substr(NCEPDataName_str.find_last_of("/")+1);
	char* NCEPDataName_h5 = (char*)NCEPDataName_cut.data();

	strcpy(AttributeData.METFILE1 , NCEPDataName_h5);   //not use the 气候数据, so the three file is null
	strcpy(AttributeData.METFILE2 , COCTSPathName);	// TODO:METFILE1
	strcpy(AttributeData.METFILE3 , "");
//	strcpy(AttributeData.OZONEFILE1 , "0"); //16
	strcpy(AttributeData.OZONEFILE1 , "N201825303_MET_NCEP_1440x0721_f015.hdf"); //16

//	char* AuxDataPath = myConfigXml.AuxDataPath;
	char* AuxDataPath = "./data/AtmosphericCorrection";

	_poExHand->sendInfoMessage("Load the path and  thresholds of Atmospheric Correction Module successful!", CLOG_NO_PRODUCT);

	struct HDF_Rrc_Rrs AtmosphericCorrectionData  = Atmospheric_Correction(GeoDataSet, CloudDetectData, LandMaskData, AuxDataPath, NCEPDataName, ASCMethod, cInputH5Path, _poExHand);

	if(AtmosphericCorrectionData.LatLonFlag == -1)
	{
		iRet = E_LatLon_ERR;
		_poExHand->sendErrorMessage("Latitude or Longitude's value is error!", "Atmospheric_Correction.cpp, 457 line", "L1B Navigation Lat and Lon error!", iRet);
	}
	if(AtmosphericCorrectionData.LatLonFlag == -2)
	{
		iRet = E_LatLonDimension_ERR;
		_poExHand->sendErrorMessage("The H5 file: Latitude or Longitude's array dimension is error!", "Atmospheric_Correction.cpp, 457 line", "L1B Navigation Lat and Lon error!", iRet);
	    _iCodeRet = iRet;
		return iRet;
	}
    if(AtmosphericCorrectionData.AngleFlag == -1)
	{
		iRet = E_Angle_ERR;
		_poExHand->sendErrorMessage("Navigation Angle's value is error!", "Atmospheric_Correction.cpp, 440 line", "L1B Navigation Angle error!", iRet);
	}
    if(AtmosphericCorrectionData.AngleFlag == -2)
	{
		iRet = E_AngleDimension_ERR;
		_poExHand->sendErrorMessage("The H5 file: Navigation Angle's array dimension is error!", "Atmospheric_Correction.cpp, 440 line", "L1B Navigation Angle error!", iRet);
	    _iCodeRet = iRet;
		return iRet;
	}

	if(AtmosphericCorrectionData.Flag == -1){
		//向云控发消息
		iRet = E_Algorithm_ERR;
		_poExHand->sendErrorMessage(AtmosphericCorrectionData.ErrorInfo, "hy1_czi_l2a_pro.cpp,354 line","Atmospheric_Correction(struct* ,char*  ,char *)", iRet);
		return iRet;
	}

	if(AtmosphericCorrectionData.ClimateFlag == -3){
		//向云控发消息
		iRet = E_DataFileNotMatch;
		_poExHand->sendErrorMessage(AtmosphericCorrectionData.ErrorInfo, "hy1_czi_l2a_pro.cpp,354 line","Atmospheric_Correction(struct* ,char*  ,char *)", iRet);
		return iRet;
	}

//	cout<<AtmosphericCorrectionData.Rrc_BLUE[0]<<endl;
	_poExHand->sendInfoMessage("Processing the Atmospheric Correction module successful ", CLOG_NO_PRODUCT);

	_poExHand->sendProgressMessage(70, 0);

/***************************************************************************  write Geophysical Data *******************************************************************************/

	/*
	 * Marker position
	 * Include L2A's Rrc and L2C's Rrs
	 * Modify time: 2018-10-24
	 */

	char* DataName = "l2_flags";
	unsigned int* data_mem_int = new unsigned int[height * width];
	unsigned int** data_int = new unsigned int * [height];
	for (int i = 0; i < height; i++)
		data_int[i] = data_mem_int + i * width;
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			data_int[r][c] = 0 ;
			if(CloudDetectData.DATASET[r*width+c] == 1){
				data_int[r][c] = data_int[r][c] + 512;
			}
			if(LandMaskData.DATASET[r*width+c] == 1){
				data_int[r][c] = data_int[r][c] + 2;

			}
		}
	}

	hsize_t dimsf[2]; dimsf[0]= height; dimsf[1]= width;

	/* write to L2A's H5 file */
	hid_t L2A_openFileId = H5Fcreate(oL2ADataFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	hid_t L2A_openGroupId = H5Gcreate2(L2A_openFileId, "Geophysical Data", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hid_t L2A_dataSpace = H5Screate_simple(2, dimsf, NULL);
	hid_t L2A_dataset = H5Dcreate(L2A_openGroupId, DataName, H5T_NATIVE_UINT32, L2A_dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	herr_t L2A_ret = H5Dwrite(L2A_dataset, H5T_NATIVE_UINT32, H5S_ALL,H5S_ALL, H5P_DEFAULT, data_int[0]);
//	WriteAttrToData(L2A_dataset,"sr-1^2", "1", "0" ,"Mask and Identification","Int32");//TODO:单位改成sr -1次方
//	WriteAttribute(L2A_dataset, "Flag Names", H5T_STRING, 0.0, 0.0, 0, "Flag Names", 0);
	WriteFlags(L2A_dataset);

	H5Dclose(L2A_dataset);
	H5Sclose(L2A_dataSpace);
	H5Gclose(L2A_openGroupId);
	H5Fclose(L2A_openFileId);

	/* write to L2B's H5 file */
	hid_t L2B_openFileId = H5Fcreate(oL2BDataFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	hid_t L2B_openGroupId = H5Gcreate2(L2B_openFileId, "Geophysical Data", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hid_t L2B_dataSpace = H5Screate_simple(2, dimsf, NULL);
	hid_t L2B_dataset = H5Dcreate(L2B_openGroupId, DataName, H5T_NATIVE_UINT32, L2B_dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	herr_t L2B_ret = H5Dwrite(L2B_dataset, H5T_NATIVE_UINT32, H5S_ALL,H5S_ALL, H5P_DEFAULT, data_int[0]);
//	WriteAttrToData(L2B_dataset,"sr-1^2", "1", "0" ,"Mask and Identification","Int32");//TODO:单位改成sr -1次方
//	WriteAttribute(L2B_dataset, "Flag Names", H5T_STRING, 0.0, 0.0, 0, "Flag Names", 0);
	WriteFlags(L2A_dataset);

	H5Dclose(L2B_dataset);
	H5Sclose(L2B_dataSpace);
	H5Gclose(L2B_openGroupId);
	H5Fclose(L2B_openFileId);

	/* write to L2C's H5 file */
	hid_t L2C_openFileId = H5Fcreate(oL2CDataFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	hid_t L2C_openGroupId = H5Gcreate2(L2C_openFileId, "Geophysical Data", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	hid_t L2C_dataSpace = H5Screate_simple(2, dimsf, NULL);

	hid_t L2C_dataset = H5Dcreate(L2C_openGroupId, DataName, H5T_NATIVE_UINT32, L2C_dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	herr_t L2C_ret = H5Dwrite(L2C_dataset, H5T_NATIVE_UINT32, H5S_ALL,H5S_ALL, H5P_DEFAULT, data_int[0]);
//	WriteAttrToData(L2C_dataset,"sr-1^2", "1", "0" ,"Mask and Identification","Int32");//TODO:单位改成sr -1次方
//	WriteAttribute(L2C_dataset, "Flag Names", H5T_STRING, 0.0, 0.0, 0, "Flag Names", 0);
	WriteFlags(L2A_dataset);

	H5Dclose(L2C_dataset);
	H5Sclose(L2C_dataSpace);
	H5Gclose(L2C_openGroupId);
	H5Fclose(L2C_openFileId);

	RELEASE(data_int);
	RELEASE(data_mem_int);
/************************************************************ write NDVI to l2b_h5 *********************************************************************************************/

	struct HDF_DATA NDVIData = NDVI(GeoDataSet, CloudDetectData, NDVIThresh, L2BH5Path, L2BJpgNDVIPath, NDWIThresh);

	/* Judge */
	if(NDVIData.Flag == -1){
		//向云控发消息
		iRet = E_Algorithm_ERR;
		_poExHand->sendErrorMessage(NDVIData.ErrorInfo, "hy1_czi_l2_pro.cpp,151 line"," NDVI(struct *, float , char* ,char*)",iRet);
		return iRet;
	}

	//NDVI渲染
	Rendering_NDVI(NDVIData.DATASET, L2BJpgNDVIPath, CloudDetectData, GeoDataSet); //Color of Blue -> Green

	/* 后面不用，在此释放 */
	xfree(NDVIData.DATASET);

	_poExHand->sendInfoMessage("Processing the NDVI Module successful", CLOG_NO_PRODUCT);
	_poExHand->sendProgressMessage(75, 0);
/*************************************************************************************************************************************************************/


	//start write the Rrc and Rrs Data to L2A and L2C H5 File
	try{//float Slope, float Offset, float FillValue, float ValidMax, float ValidMin
		WriteRrcData(L2AH5Path, AtmosphericCorrectionData, 1.0, 0.0, NAN_f, -0.015, 0.115);  //TODO: 2018-10-19
		WriteRrsData(L2CH5Path, AtmosphericCorrectionData, 1, 0.0, NAN_f, -0.05,  0.4);
	}catch(exception &e){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage("Write the L2A H5 File Failed. ", "hy1_czi_l2a_pro.cpp,354 line","WriteRrcData(char* ,struct * )", iRet);
		return iRet;
	}

	_poExHand->sendInfoMessage("Write the Geophysical Data of the L2A product successful ", CLOG_NO_PRODUCT);

/*************************************************************************************************************************************************************/

	/* TODO: 2018-10-19 */
    //生产Rrc's缩略图JPG文件 to L2A's file。
	try{
		Rendering_Rrs(AtmosphericCorrectionData.Rrc_BLUE, L2AJpgBluePath, CloudDetectData, LandMaskData, "Rrc", _poExHand);
		Rendering_Rrs(AtmosphericCorrectionData.Rrc_GREEN, L2AJpgGreenPath, CloudDetectData, LandMaskData,"Rrc",_poExHand);
		Rendering_Rrs(AtmosphericCorrectionData.Rrc_RED, L2AJpgRedPath, CloudDetectData, LandMaskData, "Rrc",_poExHand);
		Rendering_Rrs(AtmosphericCorrectionData.Rrc_NIR, L2AJpgNirPath, CloudDetectData, LandMaskData, "Rrc",_poExHand);
	}catch(exception &e){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage("Write L2 Jpg File Failed. ", "hy1_czi_l2_pro.cpp,388 line","DataSaveToJPEG(float * ,char * ,int ,int  ,int )", iRet);
		return iRet;
	}
	_poExHand->sendInfoMessage("Write the JPG File the L2A product successful ", CLOG_NO_PRODUCT);

    //生产Rrs's缩略图JPG文件 to L2C's file。
	try{
		Rendering_Rrs(AtmosphericCorrectionData.Rrs_BLUE, L2CJpgBluePath, CloudDetectData, LandMaskData, "Rrs",_poExHand);
		Rendering_Rrs(AtmosphericCorrectionData.Rrs_GREEN, L2CJpgGreenPath, CloudDetectData, LandMaskData, "Rrs",_poExHand);
//		DataSaveToJPEG(AtmosphericCorrectionData.Rrs_RED, L2CJpgRedPath, height, width, 1);
//		DataSaveToJPEG(AtmosphericCorrectionData.Rrs_NIR, L2CJpgNirPath, height, width ,1);
	}catch(exception &e){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage("Write L2 Jpg File Failed. ", "hy1_czi_l2_pro.cpp,388 line","DataSaveToJPEG(float * ,char * ,int ,int  ,int )", iRet);
		return iRet;
	}
	_poExHand->sendInfoMessage("Write the JPG File the L2C product successful ", CLOG_NO_PRODUCT);
	/* TODO: 2018-10-19 */

/******************************************************************  SSC and Rendering  *****************************************************************************/

	_poExHand->sendInfoMessage("Start Write the SSC Data of the L2B product.", CLOG_NO_PRODUCT);
	struct HDF_DATA SSCData = SSC(AtmosphericCorrectionData, L2BH5Path, l2BJpgSSCPath);  //TODO: 2018-10-19
	if(SSCData.Flag == -1){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage(SSCData.ErrorInfo, "hy1_czi_l2_pro.cpp,394 line","SSC(struct* ,char* ,char* )", 138);
		return iRet;
	}
	Rendering(SSCData, l2BJpgSSCPath, "SSC", CloudDetectData, LandMaskData, _poExHand);
	_poExHand->sendInfoMessage("Write the SSC Data of the L2B product successful ", CLOG_NO_PRODUCT);
	free(SSCData.DATASET);

/******************************************************************  CHL and Rendering  *****************************************************************************/

	_poExHand->sendInfoMessage("Start Write the CHL Data of the L2C product.", CLOG_NO_PRODUCT);
	struct HDF_DATA CHLData = CHL(AtmosphericCorrectionData, L2CH5Path, L2CJpgChlPath);  //TODO: 2018-10-19
	if(CHLData.Flag == -1){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage(CHLData.ErrorInfo, "hy1_czi_l2_pro.cpp,403 line","CHL(struct* ,char* ,char* )", 138);
		return iRet;
	}
//	struct HDF_DATA datatest = ReadH5Data1(L2CH5Path,"Geophysical Data","chl_a");
//	cout<<datatest.DATASET[0]<<endl;
	Rendering(CHLData, L2CJpgChlPath, "CHL", CloudDetectData, LandMaskData, _poExHand);

	_poExHand->sendInfoMessage("Write the CHL Data of the L2C product successful ", CLOG_NO_PRODUCT);
	xfree(CHLData.DATASET);

/******************************************************************  SDD and Rendering  *****************************************************************************/

	_poExHand->sendInfoMessage("Start Write the SDD Data of the L2C product.", CLOG_NO_PRODUCT);
	struct HDF_DATA SDDData = SDD(AtmosphericCorrectionData, L2CH5Path, l2CJpgSDDPath);  //TODO: 2018-10-19
	if(SDDData.Flag == -1){
		//向云控发消息
		iRet = E_File_Gener_ERR;
		_poExHand->sendErrorMessage(SDDData.ErrorInfo, "hy1_czi_l2_pro.cpp,412 line","SDD(struct* ,char* ,char* )", 138);
		return iRet;
	}
	Rendering(SDDData, l2CJpgSDDPath, "SDD", CloudDetectData, LandMaskData, _poExHand);
	_poExHand->sendInfoMessage("Write the SDD Data of the L2C product successful ", CLOG_NO_PRODUCT);
	xfree(SDDData.DATASET);

	//释放数据
	xfree(GeoDataSet.BLUE);
	xfree(GeoDataSet.GREEN );
	xfree(GeoDataSet.RED );
	xfree(GeoDataSet.NIR );

	xfree(CloudDetectData.DATASET );
	xfree(LandMaskData.DATASET );

	//These can not release
//	xfree(AtmosphericCorrectionData.BLUE);
//	xfree(AtmosphericCorrectionData.GREEN );
//	xfree(AtmosphericCorrectionData.RED );
//	xfree(AtmosphericCorrectionData.NIR );

	xfree(AtmosphericCorrectionData.Rrc_BLUE );
	xfree(AtmosphericCorrectionData.Rrc_GREEN );
	xfree(AtmosphericCorrectionData.Rrc_NIR );
	xfree(AtmosphericCorrectionData.Rrc_RED );

	xfree(AtmosphericCorrectionData.Rrs_BLUE );
	xfree(AtmosphericCorrectionData.Rrs_GREEN );
	xfree(AtmosphericCorrectionData.Rrs_NIR );
	xfree(AtmosphericCorrectionData.Rrs_RED );


	_poExHand->sendProgressMessage(80, 0);
//	AuxDataPathName

/************************************************************************************************************************/

/*****************************************************   读取L1B元数据文件  ************************************************************/
	struct L1B_XML_DATA L1B_Xml = L1B_XmlRead(MetaData);
	/* Judge */
	if( L1B_Xml.Flag == -1){
		iRet = E_OptInFile_RDERR;
		_poExHand->sendErrorMessage(L1B_Xml.ErrorInfo, "hy1_czi_l2_pro.cpp,1297 line","L1B_XmlRead(const char*)",iRet);
		return iRet;
	}
/*****************************************************   global attributes  ***********************************************/

	_poExHand->sendInfoMessage("Setting the global attributes", CLOG_NO_PRODUCT);

	//大气矫正模块's attributes 16+12 ==28
	AttributeData.SPIXEL = 1;
	AttributeData.EPIXEL = height ;
	AttributeData.DPIXEL = 1;
	AttributeData.SLINE = 1;
	AttributeData.ELINE = width;
	AttributeData.DLINE = 1;

	AttributeData.PROC_OECEAN = 1; //海洋信息处理选项 (1处理；0未处理)
	AttributeData.PROC_LAND = 0;   //陆地信息处理选项 (1处理；0未处理) L2B为1 (L2A与L2C没有，为0）

	AttributeData.ATM_COR = 0;     //大气修正选项 (1借用COCTS修正；0采用独立开发算法)
	AttributeData.GLINT_OPT = 1;   //耀斑修正选项 (1修正；0不修正)
	AttributeData.METOZ_OPT = 0;   //辅助数据选项 (1 NCEP和EPTOMS数据；0 气候数据)
	AttributeData.RayleighLUT = 1; //瑞利散射计算选项: 0单次散射计算，1自带查找表；2基于SeaWiFS查找表转换计算;

	AttributeData.ALBEDOFlag = 1 ;

	AttributeData.ABSAER = 0.0;			//????????????????????? TODO xu que ren
	AttributeData.RHOA_MIN = 0.0;		//?????????????????????xu que ren
	AttributeData.NLW_MIN = -0.925;		//?????????????????????xu que ren
	AttributeData.WS_MAX = 8.5; 		//?????????????????????xu que ren
	AttributeData.TAUA_MAX = 3.0;		//?????????????????????xu que ren
	AttributeData.EPS_MIN = 0.9;		//?????????????????????xu que ren
	AttributeData.EPS_MAX = 1.5;		//?????????????????????xu que ren
	AttributeData.GLINT_THRESH = 0.0001;	//?????????????????????xu que ren

	strcpy(AttributeData.OrbitFlag, L1B_Xml.OrbitFlag);

	strcpy(AttributeData.SensorFile , "NULL");
//	strcpy(AttributeData.Flag_Name , "00000000000000100000001011000000");//TODO:final Version bu queding
	strcpy(AttributeData.Flag_Name , "ATMFAIL,LAND,BADANC,HISUNGLINT,HILT,HISENZ,BATH,NEGLW,STLIGHT,CLOUD,COCO,TURBID,HISOLZ,"
			"HITAUA,LOWLW,CHLFAIL,NAVWARN,ABSAER,TRICHO,MAXAERITER,MODSUNGLINT,CHLRANGE,ATMWARN,DARKPIXEL,SEAICE,NAVFAIL,FILTER,"
			"BADSST,BADSSTALG,HIPOL,SPARE, SPARE");
	AttributeData.NumOfBands = 4;
//	AttributeData.OrbitNodeLon = 123.12; //?????????????????????12

//    _poExHand->sendProgressMessage(70, 0);

/**********************************************************************************************************************/

//	 * ***********************************************************************************************
//	 * 定义数据输出
//	 * 1. 输出jpg文件，为四个文件，这步在大气矫正后就可以生成，因为该数据集在生成写入Geophysical Data中就释放
//	 * 2. 输出的h5文件
//	 *   写入全局属性表，有64个，已固定写入，其中的大气矫正的属性要传入
//	 *   写入Geophysical Data 数据集，四个波段nlw460,nlw560,nlw650和nlw825，并释放数据数据
//	 *   写入Scan-Line Attribute数据集和属性表。
//	 *   写入Sensor Band Parameters数据集和属性表
//	 *   写入Navigation Data数据集和属性表L2A_productname
//	 * 3. 输出元数据
//	 * 4. 输出文件列表
//	 * ***********************************************************************************************

	char PATH_L2A[256];
	strcpy(PATH_L2A, oL2ADataFileName.c_str());

	char tmp_ProductName[128];
	strcpy(tmp_ProductName,oL2ADataFileName.c_str());
	char* ProductName ; //This is the Production Name
	char* result = strtok( tmp_ProductName, "//" );
	while( result != NULL ) {
		result = strtok( NULL, "//" );
		if(result != NULL)
			ProductName = result;
	}

    //Write the h5 File
	//Write the global attributes
	// H1C(H1D) - OPER - CZI - L2A - 20190101T010000 - 20190101T010500 - 00103 - 10 - 00123(10),>10 ok yet
	char ContentOfProduct[11][32];
	result = strtok( ProductName, "_" );
	int ContentNum=0;
	while( result != NULL ) {
		strcpy(ContentOfProduct[ContentNum], result);
		result = strtok( NULL, "_" );
		ContentNum++;
	}
	string FileContent = ContentOfProduct[4];
	int Year = atoi(FileContent.substr(0,4).c_str());
	int Month = atoi(FileContent.substr(4,2).c_str());
	int Day = atoi(FileContent.substr(6,2).c_str());
	int dayCount = getDay(Year,Month,Day);

/************************************************************ L2A ***********************************************************/

	//This is 12 / 64
	if(strcmp(ContentOfProduct[0],"H1C") == 0){
//		strcpy(AttributeData.ProductName,"HY-1C Level2A Data");
		strcpy(AttributeData.ProductName,L2A_productname);
		strcpy(AttributeData.SatelliteName ,"HY-1C");
	}else{
//		strcpy(AttributeData.ProductName,"HY-1D Level2A Data");
		strcpy(AttributeData.ProductName,L2A_productname);
		strcpy(AttributeData.SatelliteName ,"HY-1D");
	}

	strcpy(AttributeData.SensorName , ContentOfProduct[2]);

//	strcpy(AttributeData.SensorMode, AttrSensorMode.ATTRIBUTES_CHAR); //N表示正常工作模式，T表示俯仰模式，S表示侧摆模式
//	strcpy(AttributeData.SoftwareName, "hy1c_czi_l2a_pro");
	strcpy(AttributeData.SoftwareName, "hy1c_czi_l2_pro");//统一改为hy1c_czi_l2_pro
	strcpy(AttributeData.SoftwareVerson, SoftVerison);
//	strcpy(AttributeData.CalibrationVersion, "V1.0"); //??????????
//	cout<<AttributeData.SensorMode<<endl;

	time_t t = time(0);
	char proceTime[32] ;
	strftime( proceTime, sizeof(proceTime), "%Y-%m-%dT%X",localtime(&t));
//	strcat(proceTime,".000000");
	strcpy(AttributeData.ProcessingTime, proceTime);
	strcpy(AttributeData.ProcessingCenter, "NSOAS");//
//	strcpy(AttributeData.InputFile, cInputH5Path);
	strcpy(AttributeData.InputFile, tmpL1BName_h5);
//	strcpy(AttributeData.OutputFile, oL2AStemFileName.c_str());//???????????
	char L2A_outPutFile[256];  strcpy(L2A_outPutFile, L2A_productname);  strcat(L2A_outPutFile, ".h5");
	strcpy(AttributeData.OutputFile, L2A_outPutFile);
//	strcpy(AttributeData.L2PROD, "Rrs460,Rrs560,Rrs650,Rrs825");//???????????
	strcpy(AttributeData.L2PROD, "Rrc460,Rrc560,Rrc650,Rrc825");
	char* SensorName = "../Sensor_data.dat";
	strcpy(AttributeData.CalFile,SensorName);//???????????

	 _poExHand->sendInfoMessage("Start Writing global attribute of the L2A File!", CLOG_NO_PRODUCT);

	//write
	int createL2aAttr = CreateL2AAttribute(PATH_L2A, "/", AttributeData);
	if(createL2aAttr == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Write the L2a Attributes failed", "hy1_czi_l2_pro.cpp,412 line","CreateL2AAttribute(char* ,char* ,char* )", iRet);
		return iRet;
	}

/***********************************************************  L2B  ************************************************************/


	if(strcmp(ContentOfProduct[0],"H1C") == 0){
//		strcpy(AttributeData.ProductName,"HY-1C Level2B Data");
		strcpy(AttributeData.ProductName,L2B_filename);
	}else{
//		strcpy(AttributeData.ProductName,"HY-1D Level2B Data");
		strcpy(AttributeData.ProductName,L2B_filename);
	}
//	strcpy(AttributeData.SoftwareName, "Hy1c_czi_l2b_pro");
	strcpy(AttributeData.SoftwareName, "hy1c_czi_l2_pro");//统一改为hy1c_czi_l2_pro

	t = time(0);
	strftime( proceTime, sizeof(proceTime), "%Y-%m-%dT%X",localtime(&t));
//	strcat(proceTime,".000000");
	strcpy(AttributeData.ProcessingTime, proceTime);
//	strcpy(AttributeData.InputFile, L2AH5Path);
	strcpy(AttributeData.InputFile, L2A_outPutFile);
//	strcpy(AttributeData.OutputFile, L2BH5Path);//???????????
	char L2B_outPutFile[256];  strcpy(L2B_outPutFile, L2B_filename);  strcat(L2B_outPutFile, ".h5");
	strcpy(AttributeData.OutputFile, L2B_outPutFile);

	strcpy(AttributeData.L2PROD, "SSC,NDVI");

	_poExHand->sendInfoMessage("Start Writing global attribute of the L2B File!", CLOG_NO_PRODUCT);

	//write
	int createL2bAttr = CreateL2AAttribute(L2BH5Path, "/", AttributeData); // gen mulu - > SDS Global Attributes
	if(createL2bAttr == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Write the L2b Attributes failed", "hy1_czi_l2_pro.cpp,412 line","CreateL2AAttribute(char* ,char* ,char* )", iRet);
		return iRet;
	}

/************************************************************   L2C  ***********************************************************/


	if(strcmp(ContentOfProduct[0],"H1C") == 0){
//		strcpy(AttributeData.ProductName,"HY-1C Level2C Data");
		strcpy(AttributeData.ProductName,L2C_filename);
	}else{
//		strcpy(AttributeData.ProductName,"HY-1D Level2C Data");
		strcpy(AttributeData.ProductName,L2C_filename);
	}
//	strcpy(AttributeData.SoftwareName, "hy1c_czi_l2c_pro");
	strcpy(AttributeData.SoftwareName, "hy1c_czi_l2_pro");//统一改为hy1c_czi_l2_pro

	t = time(0);
	strftime( proceTime, sizeof(proceTime), "%Y-%m-%dT%X",localtime(&t));
//	strcat(proceTime,".000000");
	strcpy(AttributeData.ProcessingTime, proceTime);
//	strcpy(AttributeData.InputFile, L2AH5Path);
	strcpy(AttributeData.InputFile, L2A_outPutFile);
//	strcpy(AttributeData.OutputFile, L2CH5Path);//???????????
	char L2C_outPutFile[256];  strcpy(L2C_outPutFile, L2C_filename);  strcat(L2C_outPutFile, ".h5");
	strcpy(AttributeData.OutputFile, L2C_outPutFile);

	strcpy(AttributeData.L2PROD, "Rrs460,Rrs560,Rrs650,Rrs825,SDD,chl_a");

	_poExHand->sendInfoMessage("Start Writing global attribute of the L2C File!", CLOG_NO_PRODUCT);

	//write
	int createL2cAttr = CreateL2AAttribute(L2CH5Path, "/", AttributeData);
	if(createL2cAttr == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Write the L2c Attributes failed", "hy1_czi_l2_pro.cpp,412 line","CreateL2AAttribute(char* ,char* ,char* )", iRet);
		return iRet;
	}
/*********************************************************************************************************************************************/

/*************************************************     Navigation Data    ********************************************************************/
	//Write the Navigation Data 5(include L2A,L2B,L2C)
	 _poExHand->sendInfoMessage("Start Writing Navigation Data, Scan-Line Attributes, Sensor Band Attributes of the L2A File!", CLOG_NO_PRODUCT);
	 _poExHand->sendInfoMessage("Start Writing Navigation Data, Scan-Line Attributes, Sensor Band Attributes of the L2B File!", CLOG_NO_PRODUCT);
	 _poExHand->sendInfoMessage("Start Writing Navigation Data, Scan-Line Attributes, Sensor Band Attributes of the L2C File!", CLOG_NO_PRODUCT);

	char* GroupName = "Navigation Data";
	char* DataSetName = "Latitude";

	//char* Unit, char* Slope, char* Offset, char* Description, char* Type)
	struct HDF_DATA_INT NavigationLat = ReadH5DataInt_scale_factor_new(cInputH5Path, GroupName,DataSetName);
	if(NavigationLat.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Latitude dataset of the Navigation group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataInt_Navigation_L(PATH_L2A, GroupName, DataSetName, NavigationLat,"degree", 0.0001, 0, "", "","", -90.0, 90.0);
	WriteH5DataInt_Navigation_L(L2BH5Path, GroupName, DataSetName, NavigationLat,"degree", 0.0001, 0, "", "","", -90.0, 90.0);
	WriteH5DataInt_Navigation_L(L2CH5Path, GroupName, DataSetName, NavigationLat,"degree", 0.0001, 0, "", "","", -90.0, 90.0);
	xfree(NavigationLat.DATASET);

	DataSetName = "Longitude";
	struct HDF_DATA_INT NavigationLong = ReadH5DataInt_scale_factor_new(cInputH5Path, GroupName,DataSetName);
	if(NavigationLong.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Longitude dataset of the NavigatoL2AStemFileName.c_str()ion group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataInt_Navigation_L(PATH_L2A, GroupName, DataSetName, NavigationLong,"degree", 0.0001, 0, "", "","", -180.0, 180.0);
	WriteH5DataInt_Navigation_L(L2BH5Path, GroupName, DataSetName, NavigationLong,"degree", 0.0001, 0, "", "","", -180.0, 180.0);
	WriteH5DataInt_Navigation_L(L2CH5Path, GroupName, DataSetName, NavigationLong,"degree", 0.0001, 0, "", "","", -180.0, 180.0);
	xfree(NavigationLong.DATASET);

//*****************************************************************************************************************************************
	char *NavigationPointCountsName = "Navigation Point Counts";
	char *FirstNavigationPointName = "First Navigation Point";
	char *Pixel_intervalsNavigationPointName = "Pixel-intervals Of Navigation Point";
//	struct HDF_DATA_INT NavigationPointCounts = ReadH5DataInt(cInputH5Path, GroupName,DataSetName);
	hid_t OpenId = openFile(cInputH5Path);
	hid_t GroupId = openGroup(OpenId, GroupName);
	struct HDF_ATTRIBUTES NPC = Read_Attribute(GroupId, NavigationPointCountsName, 16);
	struct HDF_ATTRIBUTES FNP = Read_Attribute(GroupId, FirstNavigationPointName, 16);
	struct HDF_ATTRIBUTES PNP = Read_Attribute(GroupId, Pixel_intervalsNavigationPointName, 16);
//	NPC = Read_Attribute(GroupId, NavigationPointCountsName, 16);
//	FNP = Read_Attribute(GroupId, FirstNavigationPointName, 16);
//	PNP = Read_Attribute(GroupId, Pixel_intervalsNavigationPointName, 16);
	struct NavigationData NavitationAttrData;
//	if(AttrH5.Flag == -1){
//		NavitationAttrData.Flag = -1;
//		NavitationAttrData.ErrorInfo = StringStitching(NavitationAttrName);
//		return NavitationAttrData;
//	}
	NavitationAttrData.NavigationPoints = NPC.ATTRIBUTES_INT;
	NavitationAttrData.FirstNavigationPoint = FNP.ATTRIBUTES_INT;
	NavitationAttrData.PixelinterOfNavigationPoints = PNP.ATTRIBUTES_INT;

	//write to Navigation Data of l2a l2b l2c
//	CreateNavigationAttribute(PATH_L2A, "Navigation Data", NavitationAttrData);
//	CreateNavigationAttribute(L2BH5Path, "Navigation Data", NavitationAttrData);
//	CreateNavigationAttribute(L2CH5Path, "Navigation Data", NavitationAttrData);
//
	H5Gclose(GroupId);
	H5Fclose(OpenId);


	//Write the Navigation Data of l2a l2b l2c
	DataSetName = "Navigation Points";
	char* LongName = "Number of positioning pixels per row";
	struct HDF_DATA_INT NavigationPoints ;
	int* data_NavigationPoints = (int* )malloc(sizeof(int)*1);
	data_NavigationPoints[0] = NavitationAttrData.NavigationPoints;
	NavigationPoints.DATASET = data_NavigationPoints;
	NavigationPoints.RANK = 2;
	NavigationPoints.ROWS = 1;
	NavigationPoints.COLUMNS = 1;
	NavigationPoints.Flag = 0;
//	WriteH5DataInt_Navigation(PATH_L2A, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 0, 0);
//	WriteH5DataInt_Navigation(L2BH5Path, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 0, 0);
//	WriteH5DataInt_Navigation(L2CH5Path, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 0, 0);
	WriteH5DataInt_Navigation(PATH_L2A, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 21800, 21800);
	WriteH5DataInt_Navigation(L2BH5Path, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 21800, 21800);
	WriteH5DataInt_Navigation(L2CH5Path, GroupName, DataSetName, NavigationPoints,"none",1,0,"","Int32", LongName, 21800, 21800);
	xfree(NavigationPoints.DATASET);

	DataSetName = "First Navigation Point";
	LongName = "Pixel Number for Starting Location of Each Line";
	struct HDF_DATA_INT FirstNavigationPoint ;
	int* data_FirstNavigationPoint = (int* )malloc(sizeof(int)*1);
	data_FirstNavigationPoint[0] = NavitationAttrData.FirstNavigationPoint;
	FirstNavigationPoint.DATASET = data_FirstNavigationPoint;
	FirstNavigationPoint.RANK = 2;
	FirstNavigationPoint.ROWS = 1;
	FirstNavigationPoint.COLUMNS = 1;
	FirstNavigationPoint.Flag = 0;
	WriteH5DataInt_Navigation(PATH_L2A, GroupName, DataSetName, FirstNavigationPoint,"none",1,0,"","Int32", LongName, 1, 1);
	WriteH5DataInt_Navigation(L2BH5Path, GroupName, DataSetName, FirstNavigationPoint,"none",1,0,"","Int32", LongName, 1, 1);
	WriteH5DataInt_Navigation(L2CH5Path, GroupName, DataSetName, FirstNavigationPoint,"none",1,0,"","Int32", LongName, 1, 1);
	xfree(FirstNavigationPoint.DATASET);

	DataSetName = "Pixel-intervals of Navigation Points";
	LongName = "Number of pixels per row positioning interval";
	struct HDF_DATA_INT PixelinterOfNavigationPoints ;
	int* data_PixelinterOfNavigationPoints = (int* )malloc(sizeof(int)*1);
	data_PixelinterOfNavigationPoints[0] = NavitationAttrData.PixelinterOfNavigationPoints;
	PixelinterOfNavigationPoints.DATASET = data_PixelinterOfNavigationPoints;
	PixelinterOfNavigationPoints.RANK = 2;
	PixelinterOfNavigationPoints.ROWS = 1;
	PixelinterOfNavigationPoints.COLUMNS = 1;
	PixelinterOfNavigationPoints.Flag = 0;
	WriteH5DataInt_Navigation(PATH_L2A, GroupName, DataSetName, PixelinterOfNavigationPoints,"none",1,0,"","Int32", LongName, 1, 1);
	WriteH5DataInt_Navigation(L2BH5Path, GroupName, DataSetName, PixelinterOfNavigationPoints,"none",1,0,"","Int32", LongName, 1, 1);
	WriteH5DataInt_Navigation(L2CH5Path, GroupName, DataSetName, PixelinterOfNavigationPoints,"none",1,0,"","Int32", LongName, 1, 1);
	xfree(PixelinterOfNavigationPoints.DATASET);
//*************************************************************************************************************************************

	// FIXME Point Counts dataset of the Navigation group failed
//	if(NavigationPointCounts.Flag == -1){
//		iRet = E_ReqInFileRead_ERR;
//		_poExHand->sendErrorMessage("Read the Point Counts dataset of the Navigation group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
//		return iRet;
//	}
//	WriteH5DataInt(PATH_L2A, GroupName, "Navigation Points", NavigationPointCounts,"","","","","");
//	WriteH5DataInt(L2BH5Path, GroupName, "Navigation Points", NavigationPointCounts,"","","","","");
//	WriteH5DataInt(L2CH5Path, GroupName, "Navigation Points", NavigationPointCounts,"","","","","");
//	free(NavigationPointCounts.DATASET);

//	DataSetName = "First Navigation Point";
//	struct HDF_DATA_INT FirstNavigationPoint = ReadH5DataInt(cInputH5Path, GroupName,DataSetName);
	// FIXME Point Counts dataset of the Navigation group failed
//	if(FirstNavigationPoint.Flag == -1){
//		iRet = E_ReqInFileRead_ERR;
//		_poExHand->sendErrorMessage("Read the First Navigation Point dataset of the Navigation group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
//		return iRet;
//	}
//	WriteH5DataInt(PATH_L2A, GroupName, DataSetName, FirstNavigationPoint,"","","","","");
//	WriteH5DataInt(L2BH5Path, GroupName, DataSetName, FirstNavigationPoint,"","","","","");
//	WriteH5DataInt(L2CH5Path, GroupName, DataSetName, FirstNavigationPoint,"","","","","");
//	free(FirstNavigationPoint.DATASET);

//	DataSetName = "Pixel-intervals of Navigation Points";
//	struct HDF_DATA_INT PixelInterNaviPoint = ReadH5DataInt(cInputH5Path, GroupName,DataSetName);
	// FIXME Point Counts dataset of the Navigation group failed
//	if(PixelInterNaviPoint.Flag == -1){
//		iRet = E_ReqInFileRead_ERR;
//		_poExHand->sendErrorMessage("Read the Pixel-intervals of Navigation Points dataset of the Navigation group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
//		return iRet;
//	}
//	WriteH5DataInt(PATH_L2A, GroupName, DataSetName, PixelInterNaviPoint,"","","","","");
//	WriteH5DataInt(L2BH5Path, GroupName, DataSetName, PixelInterNaviPoint,"","","","","");
//	WriteH5DataInt(L2CH5Path, GroupName, DataSetName, PixelInterNaviPoint,"","","","","");

//	free(NavigationLat.DATASET);

//*************************************************************************************************************************************

//******************************************************    Scan Line Attributes   *******************************************************************************

	//Write the Scan Line Attributes
//	GroupName = "Scan Line Attributes"; FIXME Scan-Line
	GroupName = "Scan Line Attributes";
	DataSetName = "Year";
	int* dataDayScanLine = (int* )malloc(sizeof(int) * height);
	struct HDF_DATA_INT ScanLineAttiYear;
	ScanLineAttiYear.Flag = 0;
	for(int iYear = 0; iYear < height; iYear++){
		dataDayScanLine[iYear] = Year;
	}
//	dataScanLine[0] = 2019;
	ScanLineAttiYear.DATASET = dataDayScanLine;
	ScanLineAttiYear.RANK = 2;
	ScanLineAttiYear.ROWS = height;
	ScanLineAttiYear.COLUMNS = 1;

//	ValidMax = 2006, ValidMin = 2100;
//	WriteH5DataInt_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineAttiYear,"none","1","0","year","Int32", 2006, 2100);
//	WriteH5DataInt_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineAttiYear,"none","1","0","year","Int32", 2006, 2100);
//	WriteH5DataInt_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineAttiYear,"none","1","0","year","Int32", 2006, 2100);
	WriteH5DataInt_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineAttiYear,"years","1","0","year","Int32", 2006, 2100);
	WriteH5DataInt_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineAttiYear,"years","1","0","year","Int32", 2006, 2100);
	WriteH5DataInt_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineAttiYear,"years","1","0","year","Int32", 2006, 2100);
	xfree(ScanLineAttiYear.DATASET);

	DataSetName = "Day";
	struct HDF_DATA_INT ScanLineAttiDay;
	ScanLineAttiDay.Flag = 0;
	int* dataDayScan = (int* )malloc(sizeof(int) * height);
	for(int iDay = 0; iDay < height; iDay++){
		dataDayScan[iDay] = dayCount;
	}
//	dataScanLine[0] = 1;
	ScanLineAttiDay.DATASET = dataDayScan;
	ScanLineAttiDay.RANK = 2;
	ScanLineAttiDay.ROWS = height;
	ScanLineAttiDay.COLUMNS = 1;
//	ValidMax = 1, ValidMin = 366;
//	WriteH5DataInt_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineAttiDay,"none","1","0","Day","Int32", 1, 366);
//	WriteH5DataInt_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineAttiDay,"none","1","0","Day","Int32", 1, 366);
//	WriteH5DataInt_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineAttiDay,"none","1","0","Day","Int32", 1, 366);
	WriteH5DataInt_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineAttiDay,"days","1","0","Day","Int32", 1, 366);
	WriteH5DataInt_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineAttiDay,"days","1","0","Day","Int32", 1, 366);
	WriteH5DataInt_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineAttiDay,"days","1","0","Day","Int32", 1, 366);
	xfree(ScanLineAttiDay.DATASET);

	DataSetName = "Millisecond"; // TODO: Read L1B Millisecond
//	struct HDF5_CHAR MillisecondDATA =  ReadH5Char(cInputH5Path, "Scan Line Attributes", "Millisecond");
//	string totalStr = MillisecondDATA.Datachar;
//	int* dataMilliSecondScan = (int* )malloc(sizeof(int) * height);
//
//	for(int imilliSecond = 0; imilliSecond < height; imilliSecond++){
//		dataMilliSecondScan[imilliSecond] = atoi(totalStr.substr(imilliSecond * MillisecondDATA.nsize, MillisecondDATA.nsize).c_str());
//	}
//
//	struct HDF_DATA_INT ScanLineAttiMilliSecond;
//
//	ScanLineAttiMilliSecond.Flag = 0;
//
////	dataScanLine[0] = 5000; //[0，86399999]
//	ScanLineAttiMilliSecond.DATASET = dataMilliSecondScan;
//	ScanLineAttiMilliSecond.RANK = 2;
//	ScanLineAttiMilliSecond.ROWS = height;
//	ScanLineAttiMilliSecond.COLUMNS = 1;
//	//ValidMax = 0, ValidMin = 86399999;
//	WriteH5DataInt_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineAttiMilliSecond,"ms","1","0","Millisecond","Int32", 0, 86399999);
//	WriteH5DataInt_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineAttiMilliSecond,"ms","1","0","Millisecond","Int32", 0, 86399999);
//	WriteH5DataInt_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineAttiMilliSecond,"ms","1","0","Millisecond","Int32", 0, 86399999);
//	xfree(ScanLineAttiMilliSecond.DATASET);

	DataSetName = "Start Latitude";
//	struct HDF_DATA_DOUBLE ScanLineStartLat = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineStartLat = ReadH5Data1(cInputH5Path, GroupName,DataSetName);

	//FIXME
	if(ScanLineStartLat.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Start Latitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
//	ValidMax = -90.0, ValidMin = 90.0;
//	WriteH5Data1_DOUBLE_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
//	WriteH5Data1_DOUBLE_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
//	WriteH5Data1_DOUBLE_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineStartLat,"degree", "1", "0", "the start latitude of the pixels", "Float32", -90.0, 90.0);
	xfree(ScanLineStartLat.DATASET);

	DataSetName = "Start Longitude";
//	struct HDF_DATA_DOUBLE ScanLineStartLong = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineStartLong = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	//FIXME
	if(ScanLineStartLong.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Start Longitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineStartLong,"degree", "1", "0", "the start longitude of the pixels", "Float32", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineStartLong,"degree", "1", "0", "the start longitude of the pixels", "Float32", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineStartLong,"degree", "1", "0", "the start longitude of the pixels", "Float32", -180.0, 180.0);
	xfree(ScanLineStartLong.DATASET);

	DataSetName = "Center Latitude";
//	struct HDF_DATA_DOUBLE ScanLineCenterLat = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineCenterLat = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	//FIXME
	if(ScanLineCenterLat.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Center Latitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
//	WriteH5Data1_DOUBLE_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
//	WriteH5Data1_DOUBLE_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
//	WriteH5Data1_DOUBLE_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineCenterLat,"degree", "1", "0", "the center latitude of the pixels", "Float32", -90.0, 90.0);
	xfree(ScanLineCenterLat.DATASET);

	DataSetName = "Center Longitude";
//	struct HDF_DATA_DOUBLE ScanLineCenterLong = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineCenterLong = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	//FIXME
	if(ScanLineCenterLong.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Center Longitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineCenterLong,"degree", "1", "0", "the center longitude of the pixels", "Float64", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineCenterLong,"degree", "1", "0", "the center longitude of the pixels", "Float64", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineCenterLong,"degree", "1", "0", "the center longitude of the pixels", "Float64", -180.0, 180.0);
	xfree(ScanLineCenterLong.DATASET);

	DataSetName = "End Latitude";
//	struct HDF_DATA_DOUBLE ScanLineEndLat = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineEndLat = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	if(ScanLineEndLat.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the End Latitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineEndLat,"degree", "1", "0", "the end latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineEndLat,"degree", "1", "0", "the end latitude of the pixels", "Float32", -90.0, 90.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineEndLat,"degree", "1", "0", "the end latitude of the pixels", "Float32", -90.0, 90.0);
	xfree(ScanLineEndLat.DATASET);

	DataSetName = "End Longitude";
//	struct HDF_DATA_DOUBLE ScanLineEndLong = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineEndLong = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	if(ScanLineEndLong.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the End Longitude dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineEndLong, "degree", "1", "0", "the end longitude of the pixels", "Float32", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineEndLong, "degree", "1", "0", "the end longitude of the pixels", "Float32", -180.0, 180.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineEndLong, "degree", "1", "0", "the end longitude of the pixels", "Float32", -180.0, 180.0);
	xfree(ScanLineEndLong.DATASET);

	DataSetName = "Center Solar Zenith Angle";
//	struct HDF_DATA_DOUBLE ScanLineCSZA = ReadH5Data1_DOUBLE(cInputH5Path, GroupName,DataSetName);
	struct HDF_DATA ScanLineCSZA = ReadH5Data1(cInputH5Path, GroupName,DataSetName);
	if(ScanLineCSZA.Flag == -1){
		iRet = E_ReqInFileRead_ERR;
		_poExHand->sendErrorMessage("Read the Center Solar Zenith Angle dataset of the Scan-Line Attributes group failed", "hy1_czi_l2_pro.cpp,412 line","ReadH5Data1(char* ,char* ,char* )", iRet);
		return iRet;
	}
	WriteH5DataFloat_ScanLine(PATH_L2A, GroupName, DataSetName, ScanLineCSZA, "degree", "1", "0", "Center Solar Zenith Angle", "Float32", 0.0, 90.0);
	WriteH5DataFloat_ScanLine(L2BH5Path, GroupName, DataSetName, ScanLineCSZA, "degree", "1", "0", "Center Solar Zenith Angle", "Float32", 0.0, 90.0);
	WriteH5DataFloat_ScanLine(L2CH5Path, GroupName, DataSetName, ScanLineCSZA, "degree", "1", "0", "Center Solar Zenith Angle", "Float32", 0.0, 90.0);
	xfree(ScanLineCSZA.DATASET);
/******************************************************************************************************************************************/

/************************************************************   Sensor Band Attributes   ******************************************************************************/

	//Write the Sensor Band Attributes
	GroupName = "Sensor Band parameters";

	DataSetName = "Wavelength";
	LongName = "none";
	int* data2Wave = (int* )malloc(sizeof(int)*4);
//	memset(data2, sizeof(data2), 0);
	data2Wave[0] = 460;
	data2Wave[1] = 560;
	data2Wave[2] = 650;
	data2Wave[3] = 825;

	struct HDF_DATA_INT SensorBandAttrWavel ;
//	memset(&SensorBandAttrWavel, sizeof(SensorBandAttrWavel), 0);
	SensorBandAttrWavel.DATASET = data2Wave;
	SensorBandAttrWavel.RANK = 2;
	SensorBandAttrWavel.ROWS = 4;
	SensorBandAttrWavel.COLUMNS = 1;
	SensorBandAttrWavel.Flag = 0;
	LongName = "Wave length";
	WriteH5DataInt_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttrWavel,"nm","1","0","Wavelength","Int32", LongName);
	WriteH5DataInt_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttrWavel,"nm","1","0","Wavelength","Int32", LongName);
	WriteH5DataInt_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttrWavel,"nm","1","0","Wavelength","Int32", LongName);
	xfree(SensorBandAttrWavel.DATASET);

	struct HDF_DATA SensorBandAttr ;
	memset(&SensorBandAttr, sizeof(SensorBandAttr), 0);
	float* dataSensorBand = (float* )malloc(sizeof(float)*4);

	dataSensorBand[0] = 1.0;
	dataSensorBand[1] = 1.0;
	dataSensorBand[2] = 1.0;
	dataSensorBand[3] = 1.0;
	SensorBandAttr.DATASET = dataSensorBand;
	SensorBandAttr.RANK = 2;
	SensorBandAttr.ROWS = 4;
	SensorBandAttr.COLUMNS = 1;
	SensorBandAttr.Flag = 0;

	DataSetName = "Syscal Gain";

	SensorBandAttr.DATASET = dataSensorBand;
	LongName = "System Calibration Linear Coefficient";
	WriteH5Data1_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttr,"none", "1", "0", "Syscal Gain", "Float32", LongName);
	WriteH5Data1_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttr,"none", "1", "0", "Syscal Gain", "Float32", LongName);
	WriteH5Data1_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttr,"none", "1", "0", "Syscal Gain", "Float32", LongName);
//	xfree(SensorBandAttr.DATASET);

	DataSetName = "Syscal Offset";
	dataSensorBand[0] = 0.0;
	dataSensorBand[1] = 0.0;
	dataSensorBand[2] = 0.0;
	dataSensorBand[3] = 0.0;
	SensorBandAttr.DATASET = dataSensorBand;
	LongName = "System Calibration Coefficient Migration";
	WriteH5Data1_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttr,"mW/cm2.um. sr", "1", "0", "Syscal Offset", "Float32", LongName);
	WriteH5Data1_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttr,"mW/cm2.um. sr", "1", "0", "Syscal Offset", "Float32", LongName);
	WriteH5Data1_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttr,"mW/cm2.um. sr", "1", "0", "Syscal Offset", "Float32", LongName);

	DataSetName = "F0";
	dataSensorBand[0] = 194.0544;
	dataSensorBand[1] = 181.3399;
	dataSensorBand[2] = 156.4845;
	dataSensorBand[3] = 106.5948;
	SensorBandAttr.DATASET = dataSensorBand;
//	LongName = "Average Outer Atmospheric Solar Irradiation";
	LongName = "extraterrestrial solar irradiance";
	WriteH5Data1_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttr,  "mW/cm2.um", "1", "0", "F0", "Float32", LongName);
	WriteH5Data1_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttr, "mW/cm2.um", "1", "0", "F0", "Float32", LongName);
	WriteH5Data1_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttr, "mW/cm2.um", "1", "0", "F0", "Float32", LongName);

	DataSetName = "k_oz";
	dataSensorBand[0] = 0.002;
	dataSensorBand[1] = 0.087;
	dataSensorBand[2] = 0.077;
	dataSensorBand[3] = 0.001;
	SensorBandAttr.DATASET = dataSensorBand;
	LongName = "Band Equivalent Ozone Absorption Coefficient";
	WriteH5Data1_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttr,  "1/cm", "1", "0", "k_oz", "Float32", LongName);
	WriteH5Data1_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttr,  "1/cm", "1", "0", "k_oz", "Float32", LongName);
	WriteH5Data1_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttr,  "1/cm", "1", "0", "k_oz", "Float32", LongName);

	DataSetName = "tau_r";
	//0.2669、0.0951、0.0525、0.0221
	dataSensorBand[0] = 0.2669;
	dataSensorBand[1] = 0.0951;
	dataSensorBand[2] = 0.0525;
	dataSensorBand[3] = 0.0221;
	SensorBandAttr.DATASET = dataSensorBand;
	LongName = "Band Equivalent Rayleigh Optical Thickness";
	WriteH5Data1_SensorBand(PATH_L2A, GroupName, DataSetName, SensorBandAttr,  "none", "1", "0", "tau_r", "Float32", LongName);
	WriteH5Data1_SensorBand(L2BH5Path, GroupName, DataSetName, SensorBandAttr,  "none", "1", "0", "tau_r", "Float32", LongName);
	WriteH5Data1_SensorBand(L2CH5Path, GroupName, DataSetName, SensorBandAttr,  "none", "1", "0", "tau_r", "Float32", LongName);
	xfree(SensorBandAttr.DATASET);
	_poExHand->sendInfoMessage("Write the H5 File Successful!", CLOG_NO_PRODUCT);
    _poExHand->sendProgressMessage(95, 0);

/*************************************************************************************************************************/

/*****************************************************   生产元数据文件  ************************************************************/

//    _poExHand->sendInfoMessage("Write the JPEG File Successful!", CLOG_NO_PRODUCT);
    //生产元数据文件。
    struct XML_DATA XmlParamter;
    memset(&XmlParamter, sizeof(XmlParamter), NULL);

    _poExHand->sendInfoMessage("Start Write the Mate File!", CLOG_NO_PRODUCT);

	strcpy(XmlParamter.SatelliteID , AttributeData.SatelliteName);
	strcpy(XmlParamter.SensorID , AttributeData.SensorName);
	strcpy(XmlParamter.OrbitID , inttochar(AttributeData.OrbitNumber));
	strcpy(XmlParamter.ReceiveStation , L1B_Xml.ReceiveStation);
//	strcpy(XmlParamter.SceneID , "");//TODO:景号,delate-2018-10-19
	strcpy(XmlParamter.ProductName , L2A_productname); //L2A_productname
	//char L2A_filename[256];//写入
	strcat(L2A_productname, ".h5");
	strcpy(XmlParamter.FileName , L2A_productname); //L2AH5Path.h5
	strcpy(XmlParamter.TimeType , "DAILY");
	strcpy(XmlParamter.ProductUnit , "NSOAS");
	strcpy(XmlParamter.ContactInfo , "dts@mail.nsoas.org.cn");
//	strcpy(XmlParamter.OrbitFlag , AttributeData.SensorMode);//
	strcpy(XmlParamter.OrbitFlag , L1B_Xml.OrbitFlag);
	strcpy(XmlParamter.WorkMode , AttributeData.SensorMode);
	strcpy(XmlParamter.AuxData1 , NCEPDataName_h5);//动态填入NCEP文件名
	COCTSPathName =  myConfigXml.COCTSPathName;
	strcpy(XmlParamter.AuxData2 , COCTSPathName);//动态填入COCTS文件名
	strcpy(XmlParamter.AuxData3 , "");//空着
	strcpy(XmlParamter.ProductLevel , "L2A");
	strcpy(XmlParamter.ProductFormat , "HDF5");//TODO:ProductFormat
	strcpy(XmlParamter.ProductVersion , "10");//TODO:ProductVersion
	strcpy(XmlParamter.SoftwareName , "hy1_czi_l2a_pro");
	strcpy(XmlParamter.SoftwareVersion , AttributeData.SoftwareVerson);
	strcpy(XmlParamter.SourseData , tmpL1BName_h5);//
	strcpy(XmlParamter.ProduceTime ,proceTime);
	strcpy(XmlParamter.Resolution , floattochar(50));//固定写入
	strcpy(XmlParamter.QuickViewURL , oL2APath_URL);//
	strcpy(XmlParamter.EarthEllipsoid , "WGS_84");
	strcpy(XmlParamter.ProjectType , "UTM");

//	//StartTime: 20180107D01:02:20.1
//	int RangeBeginDate_int = atoi(AttributeData.RangeBeginDate);
//	int RangeBeginTime_int = atoi(AttributeData.RangeBeginTime);
//	strcat(XmlParamter.StartTime, inttochar(RangeBeginDate_int));
//	strcat(XmlParamter.StartTime, "D");
//	strcat(XmlParamter.StartTime, inttochar(RangeBeginTime_int));
//
//	//EndTime
//	int RRangeEndDate_int = atoi(AttributeData.RangeEndDate);
//	int RangeEndDate_int = atoi(AttributeData.RangeEndDate);
//	strcat(XmlParamter.EndTime, inttochar(RRangeEndDate_int));
//	strcat(XmlParamter.EndTime, "D");
//	strcat(XmlParamter.EndTime, inttochar(RangeEndDate_int));

	strcpy(XmlParamter.StartTime , L1B_Xml.StartTime);//
	strcpy(XmlParamter.EndTime , L1B_Xml.EndTime);//
	strcpy(XmlParamter.CenterTime ,L1B_Xml.CenterTime);
//	strcpy(XmlParamter.CenterTime ,AttributeData.SceneCenterTime);
	long L2AFileSize = getFileSize(L2AH5Path);
	strcpy(XmlParamter.DataSize , floattochar(float(L2AFileSize/1024/1024)));
	strcpy(XmlParamter.DataUnit , "MB");

	strcpy(XmlParamter.CenterLocation_Latitude , floattochar(AttributeData.SceneCenterLat));
	strcpy(XmlParamter.CenterLocation_Longitude , floattochar(AttributeData.SceneCenterLong));

	strcpy(XmlParamter.TopLeft_Latitude , floattochar(AttributeData.UpLeftLat));
	strcpy( XmlParamter.TopLeft_Longitude , floattochar(AttributeData.UpLeftLong));
	strcpy(XmlParamter.TopRight_Latitude , floattochar(AttributeData.UpRightLat));
	strcpy( XmlParamter.TopRight_Longitude , floattochar(AttributeData.UpRightLong));

	strcpy(XmlParamter.BottomLeft_Latitude ,floattochar(AttributeData.LowLeftLat));
	strcpy(XmlParamter.BottomLeft_Longitude ,floattochar(AttributeData.LowLeftLong));
	strcpy(XmlParamter.BottomRight_Latitude , floattochar(AttributeData.LowRightLat));
	strcpy(XmlParamter.BottomRight_Longitude , floattochar(AttributeData.LowRightLong));

    int matexml = XmlMetaWrite(L2AMetaPath, XmlParamter);
    if(matexml  == -1){
		iRet = 132;
		_poExHand->sendErrorMessage("Write the L2A Meta Error!", "hy1_czi_l2a_pro.cpp,647 line","XmlMetaWrite(struct *, char* )",iRet);
		return iRet;
    }

    long L2BFileSize = getFileSize(L2BH5Path);
    strcpy(XmlParamter.DataSize , floattochar(float(L2BFileSize/1024/1024)));
    strcpy(XmlParamter.ProduceTime , proceTime);
    strcpy(XmlParamter.ProductLevel , "L2B");
    strcpy(XmlParamter.ProductName , L2B_filename);
	strcat(L2B_filename, ".h5");
	strcpy(XmlParamter.FileName , L2B_filename); //L2AH5Path.h5
    strcpy(XmlParamter.SoftwareName , "hy1_czi_l2b_pro");
	strcpy(XmlParamter.QuickViewURL , oL2BPath_URL);//

    matexml = XmlMetaWrite(L2BMetaPath, XmlParamter);
    if(matexml  == -1){
		iRet = 132;
		_poExHand->sendErrorMessage("Write the L2B Meta Error!", "hy1_czi_l2a_pro.cpp,647 line","XmlMetaWrite(struct *, char* )",iRet);
		return iRet;
    }

    long L2CFileSize = getFileSize(L2CH5Path);
    strcpy(XmlParamter.DataSize , floattochar(float(L2CFileSize/1024/1024)));
    strcpy(XmlParamter.ProduceTime , proceTime);
    strcpy(XmlParamter.ProductLevel ,"L2C");
    strcpy(XmlParamter.ProductName , L2C_filename);
	strcat(L2C_filename, ".h5");
	strcpy(XmlParamter.FileName , L2C_filename); //L2AH5Path.h5
	strcpy(XmlParamter.SoftwareName , "hy1_czi_l2c_pro");
	strcpy(XmlParamter.QuickViewURL , oL2CPath_URL);//

    matexml = XmlMetaWrite(L2CMetaPath, XmlParamter);
    if(matexml  == -1){
		iRet = 132;
		_poExHand->sendErrorMessage("Write the L2C Meta Error!", "hy1_czi_l2a_pro.cpp,647 line","XmlMetaWrite(struct *, char* )",iRet);
		return iRet;
    }
    _poExHand->sendInfoMessage("Write the Meta File Successful!", CLOG_NO_PRODUCT);

    //生产列表文件。
    _poExHand->sendInfoMessage("Start Write the product File list!", CLOG_NO_PRODUCT);


    int xmllist = ListXmlWrite(L2ListFilePath, L2AH5Path, L2AMetaPath, L2AJpgBluePath, L2AJpgGreenPath, L2AJpgRedPath, L2AJpgNirPath, L2BH5Path, L2BJpgNDVIPath, l2BJpgSSCPath, L2BMetaPath, L2CH5Path, L2CMetaPath, L2CJpgChlPath, l2CJpgSDDPath, L2CJpgBluePath, L2CJpgGreenPath, L2CJpgRedPath, L2CJpgNirPath);
	    if(xmllist  == -1){
			iRet = 132;
			_poExHand->sendErrorMessage("Write the L2 List File Error!", "hy1_czi_l2a_pro.cpp,647 line","ListXmlWrite(char *, char *, char* ,char *, char* ,char*)",iRet);
			return iRet;
	    }
//    struct XML_DATA XmlRead(const char* PATH_Xml);
//    int xmllist = ListXmlWrite(L2AListFilePath, "CZI_L2A", L2AH5Path, L2AJpgBluePath, l2AJpgGreenPath);
//    if(xmllist  == -1){
//		iRet = 132;
//		_poExHand->sendErrorMessage("Write the L2A List File Error!", "hy1_czi_l2a_pro.cpp,647 line","ListXmlWrite(char *, char* ,char *, char* ,char*)",iRet);
//		return iRet;
//    }
//
//    xmllist = ListXmlWrite(L2BListFilePath, "CZI_L2B", L2BH5Path, L2BJpgNDVIPath, l2BJpgSSCPath);
//    if(xmllist  == -1){
//    	iRet = 132;
//    	_poExHand->sendErrorMessage("Write the L2B List File Error!", "hy1_czi_l2a_pro.cpp,647 line","ListXmlWrite(char *, char* ,char *, char* ,char*)",iRet);
//    	return iRet;
//    }
//
//    xmllist = ListXmlWrite(L2CListFilePath, "CZI_L2C", L2CH5Path, L2CJpgChlPath, l2CJpgSDDPath);
//    if(xmllist  == -1){
//    	iRet = 132;
//    	_poExHand->sendErrorMessage("Write the L2C List File Error!", "hy1_czi_l2a_pro.cpp,647 line","ListXmlWrite(char *, char* ,char *, char* ,char*)",iRet);
//    	return iRet;
//    }
    _poExHand->sendInfoMessage("Write the File List Successful!", CLOG_NO_PRODUCT);

	time_t stopT = time(0);
	char stopTime[32] ;
	strftime( stopTime, sizeof(stopTime), "%Y-%m-%dT%X",localtime(&stopT));

//	cout<<startTime<<" "<<stopTime<<endl;
	char** InputList = (char** )malloc(sizeof(char*)*2);
	char** OutputList = (char** )malloc(sizeof(char*)*3);;

	InputList[0] = cInputH5Path;
	InputList[1] = AuxDataPath;

	OutputList[0] = L2AH5Path;
	OutputList[1] = L2BH5Path;
	OutputList[2] = L2CH5Path;

	char* Errinfo = "NULL";

	int reportFlag = ReportXmlWrite(L2RepoFilePath, startTime, stopTime, InputList,  OutputList,  Errinfo);
	if(reportFlag == -1){
		iRet = 132;
		_poExHand->sendErrorMessage("Write the Report File Error!", "hy1_czi_l2_pro.cpp,647 line","ReportXmlWrite(char *, char *, char* ,char *, char* ,char*)",iRet);
		return iRet;
	}

	free(InputList);
	free(OutputList);

    _poExHand->sendProgressMessage(99, 0);  // 100%保留给PIK使用。

    //设置返回码：0~255。0表示成功，255表示取消。其它根据接口控制文档定义。
    _iCodeRet = iRet;  // $?  != 0
    return PIK_SUCCESS;
}

//  * ***********************************************************************************************
//  * 3. 数据处理（可选）。
//
//  * ***********************************************************************************************

PIK_Int_t HY1_CZI_L2A_PRO::startPostProcessing()
{
//	cout<<"this is the PostProcessing"<<endl;
    return PIK_SUCCESS;
}

//0. Linux Signal 15处理（建议）。
void HY1_CZI_L2A_PRO::cancel()
{
	if (signal(SIGTERM, do_sig) == SIG_ERR)
	{
		// TODO:Linux Signal 15, delete the file (producted)
	     perror("signal");
	     exit(1);
	}
//	cout<<"this is the signal 15 Processing"<<endl;
}

