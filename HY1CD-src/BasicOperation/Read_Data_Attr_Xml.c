/*
 * Read_Data_Attr_Xml.c
 *
 *  Created on: Jul 5, 2018
 *      Author: zhongyong
 */


#include<stdio.h>
#include<stdlib.h>
#include"Basic_Operation.h"
#include"Read_Data_Attr_Xml.h"
#include"../HDF_STRUCT.h"
//#include"Write_Data_Attr_Xml.hpp"
#include"hdf5.h"

#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array
/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param:FilePath,GroupName,DataSetName
 * output param:one struct include the RANK, ROWS, COLUMNS, Flag, DATASET, ErrorInfo of the dataset
 * */
struct HDF_DATA_INT ReadH5DataInt(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA_INT Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	int* DataSet = (int* )malloc(sizeof(int)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_INT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";

		xfree(DataSet);
		return Data;
	}


	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);
	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}
struct HDF_DATA_INT ReadH5DataInt_scale_factor_new(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA_INT Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	int* DataSet = (int* )malloc(sizeof(int)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_INT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";

		xfree(DataSet);
		return Data;
	}

	/******************************  scale_factor  *********************************/

	struct HDF_Attribute_Data H5Attribute;
	char* AttributeName = "scale_factor";
	struct HDF_ATTRIBUTES AttrH5;
	AttrH5 = Read_Attribute(OpenDataSetID,AttributeName,16);
	if(AttrH5.Flag == -1){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get scale_factor";
		//return Data;
	}

	Data.scale_factor = AttrH5.ATTRIBUTES_FLOAT;

	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	return Data;

}
struct HDF_DATA ReadH5DataInt_scale_factor(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	int* DataSet = (int* )malloc(sizeof(int)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_INT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";

		xfree(DataSet);
		return Data;
	}

	/******************************  scale_factor  *********************************/

	struct HDF_Attribute_Data H5Attribute;
	char* AttributeName = "scale_factor";
	struct HDF_ATTRIBUTES AttrH5;
	AttrH5 = Read_Attribute(OpenDataSetID,AttributeName,16);
	if(AttrH5.Flag == -1){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get scale_factor";
		//return Data;
	}

	Data.scale_factor = AttrH5.ATTRIBUTES_FLOAT;

	float* DataSet_float = (float* )malloc(sizeof(float)*width*height);
	int i;
	for (i=0; i++; i<(width*height))
	{
		DataSet_float[i] = (float) DataSet[i] *  Data.scale_factor;

	}
	printf("data: %f", DataSet_float[0]);
	xfree(DataSet);
	/******************************    *********************************/

	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet_float;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);
	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}

/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param:FilePath,GroupName,DataSetName
 * output param:one struct include the RANK, ROWS, COLUMNS, Flag, DATASET, ErrorInfo of the dataset
 * */
struct HDF_DATA ReadH5Data1(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	float* DataSet = (float* )malloc(sizeof(float)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";
//		printf("%d",status);
		return Data;
	}


	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);
	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}
struct HDF_DATA_DOUBLE ReadH5Data1_DOUBLE(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA_DOUBLE Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	double* DataSet = (double* )malloc(sizeof(double)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";
//		printf("%d",status);
		return Data;
	}

	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);
	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}

/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param:GroupId,DataSetName
 * output param:one struct include the RANK, ROWS, COLUMNS, Flag, DATASET, ErrorInfo of the dataset
 * */
struct HDF_DATA ReadH5Data2(hid_t GroupId,const char* DataSetName){

	struct HDF_DATA Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenDataSetID = openDataSet(GroupId,DataSetName);
	if(GroupId < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width =1 ;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	float* DataSet = (float* )malloc(sizeof(float)*width*height);
	status = H5Dread(OpenDataSetID, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";
		return Data;
	}

	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);

	return Data;
}


/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param: DataSetId
 * output param: one struct include the RANK, ROWS, COLUMNS, Flag, DATASET, ErrorInfo of the dataset
 * */
struct HDF_DATA ReadH5Data3(hid_t DataSetId){

	struct HDF_DATA Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	if(DataSetId < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group or dataset failed.";
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(DataSetId);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int height, width = 1;
	if(rank ==  1)
		height = dimsr[0];
	else{
		height = dimsr[0];
		width = dimsr[1];
	}

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	//define the float dataset , its size is row * col
	float* DataSet = (float* )malloc(sizeof(float)*width*height);
	status = H5Dread(DataSetId, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
	//TODO: H5T_NATIVE_FLOAT -> H5T_NATIVE_INT
	if(status < 0)
	{
		Data.Flag = -1;
		free(DataSet);
		Data.ErrorInfo = "Read the Dataset failed.";
		return Data;
	}

	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

	H5Sclose(memSpace);
	H5Sclose(DataSpace);
	H5Dclose(DataSetId);

	return Data;
}

/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param: FilePath
 * output param: one struct include the RANK, ROWS, COLUMNS, Flag, ErrorInfo, BLUE, GREEN, RED, NIR of the dataset
 * */
struct HDF_GEODATA ReadGeoData(const char* FilePath){

	struct HDF_GEODATA Data;
	Data.Flag = 0;
	//	memset(&Data, sizeof(Data), 0);
	//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,"Geophysical Data");

	if(OpenFileID < 0 || OpenGroupID < 0){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the file,group failed.";
		return Data;
	}

//	char* DataSetName = "L 443";
	char* DataSetName = "L_460";
	hid_t DataSetId = openDataSet(OpenGroupID,DataSetName);
	struct HDF_DATA DataBlue = ReadH5Data3(DataSetId);
	if(DataBlue.Flag == -1){
		Data.ErrorInfo = "Read the Blue band Failed";
		Data.Flag = -1;
		return Data;
	}
	Data.BLUE = DataBlue.DATASET;

//	DataSetName = "L 565";
	DataSetName = "L_560";
	DataSetId = openDataSet(OpenGroupID,DataSetName);
	struct HDF_DATA DataGreen = ReadH5Data3(DataSetId);
	if(DataGreen.Flag == -1){
		Data.ErrorInfo = "Read the GREEN band Failed";
		Data.Flag = -1;
		free(Data.BLUE);
		return Data;
	}
	Data.GREEN = DataGreen.DATASET;

//	DataSetName = "L 665";
	DataSetName = "L_650";
	DataSetId = openDataSet(OpenGroupID,DataSetName);
	struct HDF_DATA DataRed = ReadH5Data3(DataSetId);
	if(DataRed.Flag == -1){
		Data.ErrorInfo = "Read the RED band Failed";
		Data.Flag = -1;
		free(Data.BLUE);
		free(Data.GREEN);
		return Data;
	}
	Data.RED = DataRed.DATASET;

//	DataSetName = "L 685";
	DataSetName = "L_825";
	DataSetId = openDataSet(OpenGroupID,DataSetName);
	struct HDF_DATA DataNir = ReadH5Data3(DataSetId);
	if(DataNir.Flag == -1){
		Data.ErrorInfo = "Read the NIR band Failed";
		Data.Flag = -1;
		free(Data.BLUE);
		free(Data.GREEN);
		free(Data.RED);
		return Data;
	}
	Data.NIR = DataNir.DATASET;

	Data.RANK = DataRed.RANK;
	Data.ROWS = DataGreen.ROWS;
	Data.COLUMNS = DataBlue.COLUMNS;
	Data.ErrorInfo = "none";

	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}


char* StringStitching(char* AttributeName){
	char ErrorInfo[256];
	strcpy(ErrorInfo,"Read the Attribute ");
	strcat(ErrorInfo,AttributeName);
	strcat(ErrorInfo," Failed");
	return ErrorInfo;
}

#include<string.h>

/* *
 * Read the global attribute 25 / 64
 * include read Number of Scan Lines,Pixels Per Scan Line,NumOfBands,RangeBeginDate,RangeBeginTime
 * 	Range Ending Date,RangeEndTime,UpLeftLat( + seven point),NorthLat(+ three point),SceneCenterLat,SceneCenterLong
 * 	RealtimeDelayFlag(has problem)
 * input param: FilePath
 * output param: one struct  Attributes of the H5 global Attributes
 * */

struct HDF_Attribute_Data ReadH5Attribute(const char* FilePath){

	struct HDF_Attribute_Data H5Attribute;
	hid_t OpenId = openFile(FilePath);
	hid_t GroupId = openGroup(OpenId, "/"); //TODO:Group Name "/" to Global Attributes

	/******************************  1  *********************************/
	char* AttributeName = "Number of Scan Lines";
	struct HDF_ATTRIBUTES AttrH5;
	AttrH5.Flag = 0;
	AttrH5 =  Read_Attribute(GroupId,AttributeName,16);
//	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.NumberOfScanLine = AttrH5.ATTRIBUTES_INT;

	/******************************  2  *********************************/
	AttributeName = "Pixels Per Scan Line";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.PixelsPerScanLine = AttrH5.ATTRIBUTES_INT;

	/******************************  3  *********************************/
	AttributeName = "Number of Bands";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1 ){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.NumOfBands = AttrH5.ATTRIBUTES_INT;

	/******************************  4  *********************************/
	AttributeName = "Range Beginning Date";
	AttrH5 = Read_Attribute(OpenId,AttributeName,20); //TODO 12 -> 13
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.RangeBeginDate , AttrH5.ATTRIBUTES_CHAR);

	/******************************  5  *********************************/
	AttributeName = "Range Beginning Time";
	AttrH5 = Read_Attribute(OpenId,AttributeName,20);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.RangeBeginTime , AttrH5.ATTRIBUTES_CHAR);

	/******************************  6  *********************************/
	AttributeName = "Range Ending Date";
	AttrH5 = Read_Attribute(OpenId,AttributeName,20);  //TODO 12 -> 13
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.RangeEndDate , AttrH5.ATTRIBUTES_CHAR);

	/******************************  7  *********************************/
	AttributeName = "Range Ending Time";
	AttrH5 = Read_Attribute(OpenId,AttributeName,20);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.RangeEndTime , AttrH5.ATTRIBUTES_CHAR);

	/******************************  8  *********************************/
	AttributeName = "Upper Left Latitude";
//	AttributeName = "Earth-Sun Distance";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.UpLeftLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  9  *********************************/
	AttributeName = "Upper Left Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.UpLeftLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  10  *********************************/
	AttributeName = "Upper Right Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.UpRightLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  11  *********************************/
	AttributeName = "Upper Right Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.UpRightLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  12  *********************************/
	AttributeName = "Lower Left Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.LowLeftLat= AttrH5.ATTRIBUTES_FLOAT;

	/******************************  13  *********************************/
	AttributeName = "Lower Left Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.LowLeftLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  14  *********************************/
	AttributeName = "Lower Right Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.LowRightLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  15  *********************************/
	AttributeName = "Lower Right Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.LowRightLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  16  *********************************/
	AttributeName = "Northernmost Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.NorthLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  17  *********************************/
	AttributeName = "Southernmost Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.SourthLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  18  *********************************/
	AttributeName = "Westernmost Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.WestLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  19  *********************************/
	AttributeName = "Easternmost Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.EastLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  20  *********************************/
	AttributeName = "Scene Center Latitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.SceneCenterLat = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  21  *********************************/
	AttributeName = "Scene Center Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.SceneCenterLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  22  *********************************/
	AttributeName = "Scene Center Solar Zenith Angle";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.SceneCenterSolarZenithAngle = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  23  *********************************/
	AttributeName = "Realtime Delay Flag";
	AttrH5 = Read_Attribute(OpenId,AttributeName,32);
	if(AttrH5.Flag == -1){
//		AttrH5.ATTRIBUTES_CHAR = NULL;
		strcpy(AttrH5.ATTRIBUTES_CHAR, "");
	}
	strcpy(H5Attribute.RealtimeDelayFlag , AttrH5.ATTRIBUTES_CHAR);
	strcpy(AttrH5.ATTRIBUTES_CHAR, "");

	/******************************  24  *********************************/
	AttributeName = "Orbit Number";
	AttrH5 = Read_Attribute(OpenId,AttributeName,1);
	if(AttrH5.Flag == -1){
		AttrH5.ATTRIBUTES_INT = 1;
	}
	H5Attribute.OrbitNumber = AttrH5.ATTRIBUTES_INT;

	/******************************  25  *********************************/
	AttributeName = "Sensor Mode";
	AttrH5 = Read_Attribute(OpenId,AttributeName,10);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	char SensorModeTmp[2];
	SensorModeTmp[0] = AttrH5.ATTRIBUTES_CHAR[0];
	SensorModeTmp[1] = '\0';
	strcpy(H5Attribute.SensorMode ,SensorModeTmp);

	/******************************  26  *********************************/
	AttributeName = "Orbit Node Longitude";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	H5Attribute.OrbitNodeLong = AttrH5.ATTRIBUTES_FLOAT;

	/******************************  27  *********************************/
	AttributeName = "Scene Center Time";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.SceneCenterTime , AttrH5.ATTRIBUTES_CHAR);

	/******************************  28  *********************************/
	AttributeName = "Realtime Delay Flag";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.RealtimeDelayFlag , AttrH5.ATTRIBUTES_CHAR);

	/******************************  29  *********************************/
	AttributeName = "Calibration Version";
	AttrH5 = Read_Attribute(OpenId,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute.Flag = -1;
		H5Attribute.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute;
	}
	strcpy(H5Attribute.CalibrationVersion , AttrH5.ATTRIBUTES_CHAR);

	H5Fclose(OpenId);

	/*********************************************************************/
	H5Attribute.ErrorInfo = "none" ;
	return H5Attribute;
}
struct HDF_Attribute_Navigation ReadH5Attribute_Navigation(const char* FilePath){

	struct HDF_Attribute_Navigation H5Attribute_Navigation;
	hid_t OpenId = openFile(FilePath);
	hid_t GroupId = openGroup(OpenId, "/"); //TODO:Group Name "/" to Global Attributes
	hid_t GroupId_Navigation = openGroup(OpenId, "Navigation Data"); //

	/******************************  1  *********************************/
	char* AttributeName = "Number of Scan Lines";
	struct HDF_ATTRIBUTES AttrH5;
	AttrH5.Flag = 0;

	/******************************  29  *********************************/
	AttributeName = "Navigation Angle Point Counts";
	AttrH5 = Read_Attribute(GroupId_Navigation,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute_Navigation.Flag = -1;
		H5Attribute_Navigation.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute_Navigation;
	}
	H5Attribute_Navigation.AnglePoint = AttrH5.ATTRIBUTES_INT;

	/******************************  30  *********************************/
	AttributeName = "First Navigation Angle Point";
	AttrH5 = Read_Attribute(GroupId_Navigation,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute_Navigation.Flag = -1;
		H5Attribute_Navigation.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute_Navigation;
	}
	H5Attribute_Navigation.AnglePoint_First = AttrH5.ATTRIBUTES_INT;

	/******************************  31  *********************************/
	AttributeName = "Pixel-intervals of Navigation Angle Point";
	AttrH5 = Read_Attribute(GroupId_Navigation,AttributeName,16);
	if(AttrH5.Flag == -1){
		H5Attribute_Navigation.Flag = -1;
		H5Attribute_Navigation.ErrorInfo = StringStitching(AttributeName);
		return H5Attribute_Navigation;
	}
	H5Attribute_Navigation.AnglePoint_intervals = AttrH5.ATTRIBUTES_INT;

	/*********************************************************************/

	H5Fclose(OpenId);

	H5Attribute_Navigation.ErrorInfo = "none" ;
	return H5Attribute_Navigation;
}

#include<string.h>
int HasEscape(char* info,const char* spl){

	int length = strlen(info);
	char dst[length+1];
    int n = 0;

    strcpy(dst,info);
    printf("%s,%d",dst,length);
    int i;
    char a ;
    for(i = 0; i < length ; i++){
    	a = dst[i];
    	printf("%c",a);
    }

    return n;
}

/**
 * 该方法实现了hdf5文件中属性表的读取数据。
 * 其中输入参数有：PATH_OPEN：要读取属性表的文件路径
 * 				PATH_GROUP：要读取数据的组路径
 * 				length：根据长度读取数据表中的字符串类型，而Float,Int类型的可以不用指定长度。
 * 	返回值是定义的一个属性表的结构体：一个int类型的变量存储读取属性表的类别
 * 								三个一维数组分别存储字符串，Float，Int类型的数据表的数据
 **/
struct HDF_ATTRIBUTES Read_Attribute(hid_t OpenId,const char* data_Name,int length)
{
	struct HDF_ATTRIBUTES hdf_arr;
	hdf_arr.Flag = 0;

	//打开hdf5文件的属性表
	hid_t attr = H5Aopen(OpenId,data_Name,H5P_DEFAULT);
	hid_t atype  = H5Aget_type(attr);
	if(attr < 0 || atype < 0){
		hdf_arr.Flag = -1;
		return hdf_arr;
	}
	herr_t ret;

	int int_array = 0;
	if (H5T_INTEGER == H5Tget_class(atype)) {
		ret = H5Aread(attr, atype, &int_array);
		hdf_arr.TYPE = H5T_INTEGER;
	}
	char char_array[length];
	if (H5T_STRING == H5Tget_class(atype)) {
		atype =  H5Tcopy(H5T_C_S1);
		        H5Tset_size(atype, length);
		ret = H5Aread(attr, atype, char_array);
//		HasEscape(char_array,"\\");
		if(strcmp(char_array," ") == 0){
			strcpy(char_array , "null");
		}
		char_array[length -1 ] = '\0';
		hdf_arr.TYPE = H5T_STRING;
	}

	//float类型64位
	double float_array64 = 0.0;
	if (H5T_FLOAT == H5Tget_class(atype)) {
		ret = H5Aread(attr, atype, &float_array64);
		hdf_arr.TYPE = H5T_FLOAT;
	}
	hdf_arr.ATTRIBUTES_FLOAT = float_array64;

	//float类型32位
	float float_array=0.0;
	if(hdf_arr.ATTRIBUTES_FLOAT == 0.0){
		if (H5T_FLOAT == H5Tget_class(atype))
		{
			ret = H5Aread(attr, atype, &float_array);
		}
		hdf_arr.ATTRIBUTES_FLOAT = float_array;
	}

//	float float_array=0.0;
//	if (H5T_FLOAT == H5Tget_class(atype)) {
//		ret = H5Aread(attr, atype, &float_array);
//		hdf_arr.TYPE = H5T_FLOAT;
//	}
//	hdf_arr.ATTRIBUTES_FLOAT = float_array;

	hdf_arr.ATTRIBUTES_CHAR = char_array;
	// TODO: REPLACE strcpy() with strncpy()
//	char_array[length-1] = '\0';
//	strcpy(hdf_arr.ATTRIBUTES_CHAR, char_array);
	hdf_arr.ATTRIBUTES_INT = int_array;

	H5Aclose(atype);
	H5Aclose(attr);

	return hdf_arr;
}


/**
 * 该方法实现了NECP-hdf4文件中读取数据。
 * 其中输入参数有：FILE_NAME：要读取属性表的文件路径
 * 				SDS_NAME：要读取数据的组路径
 * 	返回值：2维数组
 **/
// hdf4 NCEPData read ------------------------------------------------------
#include "mfhdf.h"    /* ...since mfhdf.h already includes hdf.h */
//function： read a hdf4 file
float** get_sub_date(char *FILE_NAME, char *SDS_NAME)
{
    // Variable declaration
    int32   sd_id, sds_id, sds_index;
    int32   n_datasets, n_file_attrs, rank;
    int32   attr_index, data_type, n_values;
    int32   dimensions[2], n_attrs;
    intn    status;
//    char    attr_name[H4_MAX_NC_NAME];
    char    sds_name[H4_MAX_NC_NAME];
    int     i, j;
    /***************************** SD: start ******************************/
    //Open the file and initialize SD interface.
    sd_id = SDstart (FILE_NAME, DFACC_READ);
//    if (sd_id == -1) {
//        printf("-E- %s:  Error opening file %s.\n",
//                __FILE__, FILE_NAME);
//        exit(1);
//    }

    /************************ SD: read data set ***************************/

    // Print out file information
    status = SDfileinfo(sd_id, &n_datasets, &n_file_attrs);
    //printf("num of datasets = %d\n", n_datasets);
    //printf("num of file attrs = %d\n", n_file_attrs);
    //Select the data set from SDS_NAME.
    sds_index = SDnametoindex (sd_id, SDS_NAME);
    sds_id = SDselect (sd_id, sds_index);
    status = SDgetinfo(sds_id, sds_name, &rank, dimensions, &data_type, &n_attrs);
//    printf("Read hdf4- %s ...\n", sds_name);
//    printf("rank = %d\n", rank);
//    printf("dimensions = %d * %d\n", dimensions[0], dimensions[1]);
    //Set elements of array start to 0, elements of array edges
    //to SDS dimensions,and use NULL for the argument stride in SDreaddata
    //to read the entire data.
    float sub_data[dimensions[0]][dimensions[1]];
	float **sub_data_2 = (float **)malloc(dimensions[0] * sizeof(float *));
	for (i = 0; i < dimensions[0]; i++)
	{
		sub_data_2[i] = (float *)malloc(dimensions[1] * sizeof(float));
		if(sub_data_2[i] == NULL)
		{
			exit(0);
		}
	}
    int32 start[2], edges[2];
    start[0] = 0;
    start[1] = 0;
    edges[0] = dimensions[0];
    edges[1] = dimensions[1];
    status = SDreaddata (sds_id, start, NULL, edges, (VOIDP)sub_data);
    // return data =========================================================
//    float sub_data_one = sub_data[2][3];
//	printf("sub_data_one: %f\n", sub_data_one);

    for (i = 0; i<721; i++)
    {
    	for (j = 0; j<1440; j++)
    		sub_data_2[i][j] = sub_data[i][j];
    }
//	printf("sub_data_one: %f\n", sub_data_2[2][3]);

    status = SDendaccess (sds_id);
    status = SDend (sd_id);
    /********************* end of SD: read data set ***********************/

    return sub_data_2;
}

/**
 * 该方法实现了NECP-hdf4文件中读取数据。
 * 其中输入参数有：FILE_NAME：要读取属性表的文件路径
 * 	返回值：3维数组
 **/
float*** hdf4Read_Realtime(char *FILE_NAME)
{
	int i, j;
    //char *FILE_NAME = "./N201811015_MET_NCEP_1440x0721_f015.hdf";
    char *SDS_NAME_z_wind = "z_wind";
    //char *SDS_NAME_z_wind_QC = "z_wind_QC";
    char *SDS_NAME_m_wind = "m_wind";
    //char *SDS_NAME_m_wind_QC = "m_wind_QC";
    char *SDS_NAME_press = "press";
    //char *SDS_NAME_press_QC = "press_QC";
    char *SDS_NAME_p_water = "p_water";
    //char *SDS_NAME_p_water_QC = "p_water_QC";
    char *SDS_NAME_rel_hum = "rel_hum";
    //char *SDS_NAME_rel_hum_QC = "rel_hum_QC";
    char *SDS_NAME_ozone = "ozone";
    //char *SDS_NAME_ozone_QC = "ozone_QC";

    // 读取数据
    float**sub_data_z_wind  = get_sub_date(FILE_NAME, SDS_NAME_z_wind);
    float**sub_data_m_wind  = get_sub_date(FILE_NAME, SDS_NAME_m_wind);
    float** sub_data_press   = get_sub_date(FILE_NAME, SDS_NAME_press);
    float** sub_data_p_water = get_sub_date(FILE_NAME, SDS_NAME_p_water);
//    float** sub_data_rel_hum = get_sub_date(FILE_NAME, SDS_NAME_rel_hum);
    float** sub_data_ozone   = get_sub_date(FILE_NAME, SDS_NAME_ozone);

    // 返回值:在3维数组中保存5个2维数组
	float*** sub_data = (float*** )malloc(5 * sizeof(float *));
//	for(i = 0; i < 5; i++)
//	{
//		sub_data[i] = (float **)malloc(721 * sizeof(float *));
//		for(j = 0; j < 721; j++)
//		{
//			sub_data[i][j] = (float *)malloc(1440 * sizeof(float));
//		}
//	}

    sub_data[0] = sub_data_z_wind;
    sub_data[1] = sub_data_m_wind;
    sub_data[2] = sub_data_press;
    sub_data[3] = sub_data_p_water;
//    sub_data[4] = sub_data_rel_hum;
    sub_data[4] = sub_data_ozone;

    return sub_data;
}

struct HDF_get_sub_date_NECP get_sub_date_NECP(char *FILE_NAME, char *SDS_NAME)
{//read sub hdfwenjian

	struct HDF_get_sub_date_NECP Data;
	Data.Flag = 0;

    // Variable declaration
    int32   sd_id, sds_id, sds_index;
    int32   n_datasets, n_file_attrs, rank;
    int32   attr_index, data_type, n_values;
    int32   dimensions[2], n_attrs;
    intn    status;
//    char    attr_name[H4_MAX_NC_NAME];
    char    sds_name[H4_MAX_NC_NAME];
    int     i, j;
    /***************************** SD: start ******************************/
    //Open the file and initialize SD interface.
    sd_id = SDstart (FILE_NAME, DFACC_READ);
//    if (sd_id == -1) {
//        printf("-E- %s:  Error opening file %s.\n",
//                __FILE__, FILE_NAME);
//        exit(1);
//    }

    /************************ SD: read data set ***************************/

    // Print out file information
    status = SDfileinfo(sd_id, &n_datasets, &n_file_attrs);
    //printf("num of datasets = %d\n", n_datasets);
    //printf("num of file attrs = %d\n", n_file_attrs);
    //Select the data set from SDS_NAME.
    sds_index = SDnametoindex (sd_id, SDS_NAME);
	if(sds_index < 0)
	{
//		printf("Read the Dataset failed");
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get Climate DataSet";
		return Data;
	}

    sds_id = SDselect (sd_id, sds_index);
    status = SDgetinfo(sds_id, sds_name, &rank, dimensions, &data_type, &n_attrs);
//    printf("Read hdf4- %s ...\n", sds_name);
//    printf("rank = %d\n", rank);
//    printf("dimensions = %d * %d\n", dimensions[0], dimensions[1]);
    //Set elements of array start to 0, elements of array edges
    //to SDS dimensions,and use NULL for the argument stride in SDreaddata
    //to read the entire data.
    double sub_data[dimensions[0]][dimensions[1]];
    float **sub_data_2 = (float **)malloc(dimensions[0] * sizeof(float *));
	for (i = 0; i < dimensions[0]; i++)
	{
		sub_data_2[i] = (float *)malloc(dimensions[1] * sizeof(float));
		if(sub_data_2[i] == NULL)
		{
			exit(0);
		}
	}

    int32 start[2], edges[2];
    start[0] = 0;
    start[1] = 0;
    edges[0] = dimensions[0];
    edges[1] = dimensions[1];
    status = SDreaddata (sds_id, start, NULL, edges, (VOIDP)sub_data);
    // return data =========================================================
//    float sub_data_one = (float)sub_data[2][3];
//	printf("sub_data_one: %f\n", sub_data_one);

    for (i = 0; i<721; i++)
    {
    	for (j = 0; j<1440; j++)
    		sub_data_2[i][j] = (float)sub_data[i][j];
    }
//	printf("sub_data_one: %f\n", sub_data_2[2][3]);

    status = SDendaccess (sds_id);
    status = SDend (sd_id);
    /********************* end of SD: read data set ***********************/

    Data.get_sub_date_NECP = sub_data_2;

    return Data;
}

struct HDF_hdf4Read_Climatedata hdf4Read_Climatedata(char *FILE_NAME, char* month_char){

	struct HDF_hdf4Read_Climatedata Data;
	Data.Flag = 0;

    char SDS_NAME_z_wind[10] = "";
    strcpy(SDS_NAME_z_wind ,"uwnd");
    char SDS_NAME_m_wind[10] = "";
    strcpy(SDS_NAME_m_wind ,"vwnd");
    char SDS_NAME_press[10] = "";
    strcpy(SDS_NAME_press ,"press");
    char SDS_NAME_p_water[10] = "";
    strcpy(SDS_NAME_p_water ,"water");
    char SDS_NAME_ozone[10] = "";
    strcpy(SDS_NAME_ozone ,"ozone");

	strcat(SDS_NAME_z_wind  , month_char);
	strcat(SDS_NAME_m_wind  , month_char);
	strcat(SDS_NAME_press   , month_char);
	strcat(SDS_NAME_p_water , month_char);
	strcat(SDS_NAME_ozone   , month_char);

    // 读取数据
	struct HDF_get_sub_date_NECP sub_data_z_wind   = get_sub_date_NECP(FILE_NAME, SDS_NAME_z_wind);

	if(sub_data_z_wind.Flag < 0)
	{
//		printf("Read the Dataset failed");
		Data.Flag = -1;
		Data.ErrorInfo = sub_data_z_wind.ErrorInfo;
		return Data;
	}

	struct HDF_get_sub_date_NECP sub_data_m_wind   = get_sub_date_NECP(FILE_NAME, SDS_NAME_m_wind);
    struct HDF_get_sub_date_NECP sub_data_press   = get_sub_date_NECP(FILE_NAME, SDS_NAME_press);
    struct HDF_get_sub_date_NECP sub_data_p_water = get_sub_date_NECP(FILE_NAME, SDS_NAME_p_water);
    struct HDF_get_sub_date_NECP sub_data_ozone   = get_sub_date_NECP(FILE_NAME, SDS_NAME_ozone);

    // 返回值:在3维数组中保存5个2维数组
    float*** sub_data = (float*** )malloc(5 * sizeof(float *));

    sub_data[0] = sub_data_z_wind.get_sub_date_NECP;
    sub_data[1] = sub_data_m_wind.get_sub_date_NECP;
    sub_data[2] = sub_data_press.get_sub_date_NECP;
    sub_data[3] = sub_data_p_water.get_sub_date_NECP;
    sub_data[4] = sub_data_ozone.get_sub_date_NECP;

    Data.hdf4Read_Climatedata = sub_data;

    return Data;
}
/* *
 * Read the General data set
 * include read the 1D and 2D data
 * input param:FilePath,GroupName,DataSetName
 * output param:one struct include the RANK, ROWS, COLUMNS, Flag, DATASET, ErrorInfo of the dataset
 * */
struct HDF_DATA_NECP get_sub_date_h5(const char* FilePath, const char* GroupName,const char* DataSetName){

	struct HDF_DATA_NECP Data;
	Data.Flag = 0;
//	memset(&Data, sizeof(Data), 0);
//	memset(Data.ErrorInfo, 0 ,256);

	hid_t OpenFileID = openFile(FilePath);
	hid_t OpenGroupID = openGroup(OpenFileID,GroupName);
	hid_t OpenDataSetID = openDataSet(OpenGroupID,DataSetName);
	if(OpenFileID < 0 || OpenGroupID < 0 || OpenDataSetID < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Open the NECP file,group or dataset failed.";
//		printf("%d,%d,%d",OpenFileID,OpenGroupID,OpenDataSetID);
		return Data;
	}

	/* get the rank,height,width infomations of the dateset*/
	hid_t DataSpace = H5Dget_space(OpenDataSetID);
	int rank = H5Sget_simple_extent_ndims(DataSpace);

	hsize_t dimsr[rank];
	herr_t status = H5Sget_simple_extent_dims(DataSpace,dimsr,NULL);
	if(status < 0 ){
		Data.Flag = -1;
		Data.ErrorInfo = "Failed To Get The Length And Width Of The DataSet";
		return Data;
	}

	int month, height, width;
	month = dimsr[0]; // 共12个月
	height = dimsr[1]; // 721
	width = dimsr[2]; // 1440

	hid_t memSpace = H5Screate_simple(rank, dimsr, NULL);

	float* DataSet = (float* )malloc(sizeof(float)*month*width*height);
	if(DataSet == NULL)
	{
		exit(0);
	}
	status = H5Dread(OpenDataSetID, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,H5P_DEFAULT, DataSet);
//	cout<<"[0][2][3]:"<<endl;
//	printf("[0]:%f\n", DataSet[0]);
//	printf("[3]:%f\n", DataSet[3]);
//	printf("[1439]:%f\n", DataSet[1439]);
//	printf("[1440]:%f\n", DataSet[1440]);
	if(status < 0)
	{
		Data.Flag = -1;
		Data.ErrorInfo = "Read the Dataset failed.";
//		printf("%d",status);
		return Data;
	}

	Data.month = month;
	Data.RANK = rank;
	Data.ROWS = height;
	Data.COLUMNS = width;
	Data.DATASET = DataSet;
	Data.ErrorInfo = "none";

//	H5Sclose(memSpace);
//	H5Sclose(DataSpace);
	H5Dclose(OpenDataSetID);
	H5Gclose(OpenGroupID);
	H5Fclose(OpenFileID);

	return Data;
}
float*** h5Read_Climatedata(char *FILE_NAME)
{
	int month = 1;

	char *GroupName = "/";
    char *SDS_NAME_z_wind  = "z_wind";
    char *SDS_NAME_m_wind  = "m_wind";
    char *SDS_NAME_press   = "press";
    char *SDS_NAME_p_water = "water";
    char *SDS_NAME_ozone   = "ozone";

    // 读取数据
    struct HDF_DATA_NECP sub_data_z_wind_12    = get_sub_date_h5(FILE_NAME, GroupName, SDS_NAME_z_wind);
    struct HDF_DATA_NECP sub_data_m_wind_12    = get_sub_date_h5(FILE_NAME, GroupName, SDS_NAME_m_wind);
    struct HDF_DATA_NECP sub_data_press_12     = get_sub_date_h5(FILE_NAME, GroupName, SDS_NAME_press);
    struct HDF_DATA_NECP sub_data_p_water_12   = get_sub_date_h5(FILE_NAME, GroupName, SDS_NAME_p_water);
    struct HDF_DATA_NECP sub_data_ozone_12     = get_sub_date_h5(FILE_NAME, GroupName, SDS_NAME_ozone);

    // selece month
    // Initialization
	float **sub_data_z_wind  = (float** )malloc(sizeof(float)*721);
	float **sub_data_m_wind  = (float** )malloc(sizeof(float)*721);
	float **sub_data_press   = (float** )malloc(sizeof(float)*721);
	float **sub_data_p_water = (float** )malloc(sizeof(float)*721);
	float **sub_data_ozone   = (float** )malloc(sizeof(float)*721);

	int i, j;
	for (i = 0; i < 721; i++)
	{	//Initialization
		sub_data_z_wind[i]  = (float* )malloc(sizeof(float)*1440); if(sub_data_z_wind[i] == NULL){exit(0);}
		sub_data_m_wind[i]  = (float* )malloc(sizeof(float)*1440); if(sub_data_m_wind[i] == NULL){exit(0);}
		sub_data_press[i]   = (float* )malloc(sizeof(float)*1440); if(sub_data_press[i] == NULL){exit(0);}
		sub_data_p_water[i] = (float* )malloc(sizeof(float)*1440); if(sub_data_p_water[i] == NULL){exit(0);}
		sub_data_ozone[i]   = (float* )malloc(sizeof(float)*1440); if(sub_data_ozone[i] == NULL){exit(0);}

		for(j = 0; j < 1440; ++j)
		{	// 三维数组转化为二维数组
			sub_data_z_wind[i][j]  = sub_data_z_wind_12.DATASET [(month-1)*721*1440 + i*1440 + j];
			sub_data_m_wind[i][j]  = sub_data_m_wind_12.DATASET [(month-1)*721*1440 + i*1440 + j];
			sub_data_press[i][j]   = sub_data_press_12.DATASET  [(month-1)*721*1440 + i*1440 + j];
			sub_data_p_water[i][j] = sub_data_p_water_12.DATASET[(month-1)*721*1440 + i*1440 + j];
			sub_data_ozone[i][j]   = sub_data_ozone_12.DATASET  [(month-1)*721*1440 + i*1440 + j];
		}
	}

    // 返回值:在3维数组中保存5个2维数组
	float*** sub_data = (float*** )malloc(5 * sizeof(float **));

    sub_data[0] = sub_data_z_wind;
    sub_data[1] = sub_data_m_wind;
    sub_data[2] = sub_data_press;
    sub_data[3] = sub_data_p_water;
    sub_data[4] = sub_data_ozone;

    //xfree
    xfree(sub_data_z_wind_12.DATASET);
    xfree(sub_data_m_wind_12.DATASET);
    xfree(sub_data_press_12.DATASET);
    xfree(sub_data_p_water_12.DATASET);
    xfree(sub_data_ozone_12.DATASET);
    return sub_data;
}


