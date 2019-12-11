/*
 * Write_Data_Attr_Xml.cpp
 *
 *  Created on: Jul 7, 2018
 *      Author: zhongyong
 */

#include<iostream>
#include<string.h>
#include"Write_Data_Attr_Xml.hpp"
#include"hdf5.h"
#include"../HDF_STRUCT.h"
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
using namespace std;

#include<cmath>
#include <limits>
//typedef std::numeric_limits<int> Info_;
//int const NAN_i = Info_::quiet_NaN();

typedef std::numeric_limits<float> Info;
float const NAN_f = Info::quiet_NaN();

char* getL2FileOutput(char* path, char level,char* L1BName){

	char tmp_ProductName[128];
	strcpy(tmp_ProductName,L1BName);

//	string data;
	char* result = strtok( tmp_ProductName, "." );
	result[14] = '2';
	result[15] = level;
//	result = strtok( result, "_" );
//	while( result != NULL ) {
//		result = strtok( NULL, "_" );
//		if(result != NULL)
//			ProductName = result;
//	}

//	PathName = Path +
//	data = result;
	return result;

}


void WriteH5DataInt(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	int* data_mem = new int[row * col];
	int** data = new int * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (int)WriteData.DATASET[r * col + c];
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
	WriteAttrToData(dataset, Unit, Slope, Offset, Description, Type);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5DataInt_Navigation(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	int* data_mem = new int[row * col];
	int** data = new int * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (int)WriteData.DATASET[r * col + c];
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
//	WriteAttrToData_SensorBand(dataset, Unit, Slope, Offset, Description, Type, LongName);
	WriteAttrToData(dataset,Unit, "", "" ,"","");
//	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, "", 32);
//	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, "", 32);

	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
//	WriteAttribute(dataset, "Slope", H5T_FLOAT, 0.0, Slope, 0, "", 32);
//	WriteAttribute(dataset, "Offset", H5T_FLOAT, 0.0, Offset, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5DataInt_Navigation_L(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	int* data_mem = new int[row * col];
	int** data = new int * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (int)WriteData.DATASET[r * col + c];
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
//	WriteAttrToData_SensorBand(dataset, Unit, Slope, Offset, Description, Type, LongName);
	WriteAttrToData(dataset,Unit, "", "" ,"","");
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0,"", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0,"", 32);
	WriteAttribute(dataset, "Slope", H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset", H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5DataInt_SensorBand(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	int* data_mem = new int[row * col];
	int** data = new int * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (int)WriteData.DATASET[r * col + c];
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
	WriteAttrToData_SensorBand(dataset, Unit, Slope, Offset, Description, Type, LongName);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5DataInt_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	int* data_mem = new int[row * col];
	int** data = new int * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (int)WriteData.DATASET[r * col + c];
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
//	WriteAttrToData(dataset, Unit, Slope, Offset, Description, Type);
	WriteAttrToData(dataset, Unit, "","","","");
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.DATASET[r * col + c];
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

	WriteAttrToData(dataset,Unit, Slope, Offset, Description, Type);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1_SensorBand(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.DATASET[r * col + c];
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

	WriteAttrToData_SensorBand(dataset,Unit, Slope, Offset, Description, Type, LongName);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5DataFloat_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.DATASET[r * col + c]/10000.0;
//			cout<<data[r][c]<<endl;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate2(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret = H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

// char* Unit, char* Slope, char* Offset, char* Description, char* Type)
//	WriteAttrToData(dataset, Unit, Slope, Offset, Description, Type);
	WriteAttrToData(dataset, Unit, "","","","");
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);

	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}
void WriteH5Data1_DOUBLE(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_DOUBLE WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	double* data_mem = new double[row * col];
	double** data = new double * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (double)WriteData.DATASET[r * col + c];
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_DOUBLE, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_DOUBLE,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

	WriteAttrToData(dataset,Unit, Slope, Offset, Description, Type);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1_DOUBLE_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_DOUBLE WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	double* data_mem = new double[row * col];
	double** data = new double * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
//			data[r][c] = (double)WriteData.DATASET[r * col + c];
			data[r][c] = (double)WriteData.DATASET[r * col + c]/10000.0;
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_DOUBLE, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_DOUBLE,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

	WriteAttrToData(dataset,Unit, "", "", "", "");
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1_Navigation(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.DATASET[r * col + c];
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
//	hid_t dataset_int; // int类型数据集本身的id
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

//	dataset_int = H5Dcreate(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

//	herr_t ret_int=H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

	WriteAttrToData(dataset,Unit, "", "", Description, Type);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	WriteAttribute(dataset, "Slope", H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset", H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);

	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;s
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1_NDVI(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float FillValue, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
//	int* data_mem = new int[row * col];
//	int** data = new int * [row];
	short* data_mem = new short[row * col];
	short** data = new short * [row];
//	float* data_mem = new float[row * col];
//	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			if ( isnan(WriteData.DATASET[r * col + c]) == 1){
				data[r][c] = 32767;
			}
			else{
				// 需要转化为int,乘1000
				float temp =WriteData.DATASET[r * col + c];
				data[r][c] = (short)(temp * 1000);
//				data[r][c] = WriteData.DATASET[r * col + c];
//				data[r][c] = temp ;
			}

		}
	}


	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_SHORT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
//	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
//	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_INT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);


//	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_SHORT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

//	WriteAttrToData(dataset,Unit, Slope, Offset, Description, Type);
	WriteAttrToData(dataset,Unit, "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, LongName, 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteH5Data1_SSC(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float FillValue, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;

	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.DATASET[r * col + c];
		}
	}

	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);

	 // 创建数据集中的数据本身
	hid_t dataset;    // 数据集本身的id
	// dataset_id = H5Dcreate(loc_id, 位置id
	//              const char *name, 数据集名
	//                hid_t dtype_id, 数据类型
	//                hid_t space_id, dataspace的id
	//             连接(link)创建性质,
	//                 数据集创建性质,
	//                 数据集访问性质)
//创建int类型的数据集
//	hid_t dataset_int;
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

//	dataset_int = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	herr_t ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);

//	WriteAttrToData(dataset,Unit, Slope, Offset, Description, Type);
	WriteAttrToData(dataset,Unit, "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, LongName, 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	// 释放动态分配的内存
//	delete[] data;
//	delete[] data_mem;
	RELEASE(data);
	RELEASE(data_mem);


	if(ret < 0){
		return ;
	}

	H5Sclose(dataSpace);
	H5Dclose(dataset);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

void WriteGeoData2(const char* WritePath,struct HDF_GEODATA WriteData){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;


	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	char* GroupName = "Geophysical Data";
	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);
	hid_t dataset;
	herr_t ret;

	char* DataName = "Rrs460";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.BLUE[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 460nm radiance","Float32");
	H5Dclose(dataset);

	DataName = "Rrs560";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.GREEN[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 560nm radiance","Float32");
	H5Dclose(dataset);

	DataName = "Rrs650";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.RED[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 650nm radiance","Float32");
	H5Dclose(dataset);

	DataName = "Rrs825";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.NIR[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 825nm radiance","Float32");
	H5Dclose(dataset);

	H5Sclose(dataSpace);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

// write Rrs data to L2C's h5 file TODO: 2018-10-19
void WriteRrsData(const char* WritePath,struct HDF_Rrc_Rrs WriteData, float Slope, float Offset, float FillValue, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;


	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	char* GroupName = "Geophysical Data";
	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);
	hid_t dataset;
	herr_t ret;

	char* DataName = "Rrs460";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrs_BLUE[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 460nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity at 460 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrs560";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrs_GREEN[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 560nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity at 560 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrs650";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrs_RED[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 650nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity at 650 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrs825";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrs_NIR[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 825nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity at 825 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	//free
	RELEASE(data_mem);
	RELEASE(data);

	H5Dclose(dataset);

	H5Sclose(dataSpace);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

// write Rrc data to L2A's h5 file TODO: 2018-10-19
void WriteRrcData(const char* WritePath,struct HDF_Rrc_Rrs WriteData, float Slope, float Offset, float FillValue, float ValidMin, float ValidMax){

	int row = WriteData.ROWS, col = WriteData.COLUMNS, rank = WriteData.RANK;

	// define the matrix data,为data矩阵填入数据，元素个数的整数部分为行号，分数部分为列号
	float* data_mem = new float[row * col];
	float** data = new float * [row];
	for (int i = 0; i < row; i++)
		data[i] = data_mem + i * col;


	hid_t FileId = H5Fopen(WritePath, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(WritePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return ;
		}
	}

	char* GroupName = "Geophysical Data";
	hid_t GroupId = H5Gopen2(FileId, GroupName, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, GroupName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return ;
		}
	}

	/* While Write the Data, its dataName don't have, so we just create dataset */
	hsize_t dimsf[2];
	dimsf[0]=row;
	dimsf[1]=col;
	hid_t dataSpace = H5Screate_simple(rank, dimsf, NULL);
	hid_t dataset;
	herr_t ret;

	char* DataName = "Rrc460";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrc_BLUE[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 460nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity Corrected by Rayleigh Scattering at 460 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrc560";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrc_GREEN[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 560nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity Corrected by Rayleigh Scattering at 560 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrc650";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrc_RED[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 650nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity Corrected by Rayleigh Scattering at 650 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);
	H5Dclose(dataset);

	DataName = "Rrc825";
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			data[r][c] = (float)WriteData.Rrc_NIR[r * col + c];
		}
	}
	dataset = H5Dcreate(GroupId, DataName, H5T_NATIVE_FLOAT, dataSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ret=H5Dwrite(dataset,H5T_NATIVE_FLOAT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data[0]);
//	WriteAttrToData(dataset,"mW/cm-2 um-1 sr-1", "1", "0" ,"Top of atmosphere 825nm radiance","Float32");
	WriteAttrToData(dataset,"1/sr", "", "" ,"","");
	WriteAttribute(dataset, "LongName", H5T_STRING, 0.0, 0.0, 0, 0, "Remote Sensing Reflectivity Corrected by Rayleigh Scattering at 825 nm Band", 0);
	WriteAttribute(dataset, "Slope",    H5T_FLOAT, 0.0, Slope, 0, 0, "", 32);
	WriteAttribute(dataset, "Offset",   H5T_FLOAT, 0.0, Offset, 0, 0, "", 32);
	WriteAttribute(dataset, "FillValue",H5T_FLOAT, 0.0, FillValue, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMax", H5T_FLOAT, 0.0, ValidMax, 0, 0, "", 32);
	WriteAttribute(dataset, "ValidMin", H5T_FLOAT, 0.0, ValidMin, 0, 0, "", 32);

	//free
	RELEASE(data_mem);
	RELEASE(data);

	H5Dclose(dataset);

	H5Sclose(dataSpace);
	H5Gclose(GroupId);
	H5Fclose(FileId);
}

#include<string.h>
hid_t WriteAttrToData_SensorBand(hid_t DataSetId, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName){

	hid_t dataspace_id = H5Screate(H5S_SCALAR);
	hid_t atype = H5Tcopy(H5T_C_S1);
	hid_t attr_id ;
	herr_t ret;

	int length = strlen(Unit);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Unit", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Unit);
		H5Aclose(attr_id);
	}

//	length = strlen(Slope);
//	if(length != 0){
//		H5Tset_size(atype, length);
//		attr_id = H5Acreate(DataSetId, "Slope", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
//		ret = H5Awrite(attr_id, atype, Slope);
//		H5Aclose(attr_id);
//	}
//
//	length = strlen(Offset);
//	if(length != 0){
//		H5Tset_size(atype, length);
//		attr_id = H5Acreate(DataSetId, "Offset", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
//		ret = H5Awrite(attr_id, atype, Offset);
//		H5Aclose(attr_id);
//	}
//
//	length = strlen(Description);
//	if(length != 0){
//		H5Tset_size(atype, length);
//		attr_id = H5Acreate(DataSetId, "Description", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
//		ret = H5Awrite(attr_id, atype, Description);
//		H5Aclose(attr_id);
//	}
//
//	length = strlen(Type);
//	if(length != 0){
//		H5Tset_size(atype, length);
//		attr_id = H5Acreate(DataSetId, "Type", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
//		ret = H5Awrite(attr_id, atype, Type);
//		H5Aclose(attr_id);
//	}

	length = strlen(LongName);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "LongName", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, LongName);
		H5Aclose(attr_id);
	}

	H5Tclose(atype);
	H5Sclose(dataspace_id);
	return 0;
}

hid_t WriteAttrToData(hid_t DataSetId, char* Unit, char* Slope,char* Offset, char* Description, char* Type){

	hid_t dataspace_id = H5Screate(H5S_SCALAR);
	hid_t atype = H5Tcopy(H5T_C_S1);
	hid_t attr_id ;
	herr_t ret;

	int length = strlen(Unit);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Unit", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Unit);
		H5Aclose(attr_id);
	}

	length = strlen(Slope);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Slope", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Slope);
		H5Aclose(attr_id);
	}

	length = strlen(Offset);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Offset", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Offset);
		H5Aclose(attr_id);
	}

	length = strlen(Description);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Description", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Description);
		H5Aclose(attr_id);
	}

	length = strlen(Type);
	if(length != 0){
		H5Tset_size(atype, length);
		attr_id = H5Acreate(DataSetId, "Type", atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, Type);
		H5Aclose(attr_id);
	}

	H5Tclose(atype);
	H5Sclose(dataspace_id);
	return 0;
}

/* *
 * Read the global attribute
 * include read Satellite Name,Sensor Name,Number of Scan Lines,Pixels Per Scan Line,NumOfBands,RangeBeginDate,RangeBeginTime
 * 	Range Ending Date,RangeEndTime,UpLeftLat( + seven point),NorthLat(+ three point),SceneCenterLat,SceneCenterLong
 * 	SceneCenterSolarZenithAngle,RealtimeDelayFlag(has problem),OrbitNumber
 * input param: FilePath
 * output param: one struct  Attributes of the H5 global Attributes
 * */
int CreateL2AAttribute(const char* PATH_WRITE, char* Group_name, struct HDF_Attribute_Data AttributeData){

	hid_t FileId = H5Fopen(PATH_WRITE, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(PATH_WRITE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return -1;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, Group_name, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, Group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return -1;
		}
	}

//	13+17+9+6+16+3==64
	WriteAttribute(GroupId, "Product Name",        H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.ProductName, 0);
	WriteAttribute(GroupId, "Satellite Name", 	   H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SatelliteName, 0);
	WriteAttribute(GroupId, "Sensor Name",         H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SensorName, 0);
	WriteAttribute(GroupId, "Sensor Mode", 		   H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SensorMode, 0);
	WriteAttribute(GroupId, "Software Name", 	   H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SoftwareName, 0);
	WriteAttribute(GroupId, "Software Version",    H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SoftwareVerson, 0);
	WriteAttribute(GroupId, "Calibration Version", H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.CalibrationVersion, 0);
	WriteAttribute(GroupId, "Processing Time",     H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.ProcessingTime, 0);
	WriteAttribute(GroupId, "Processing Center ",  H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.ProcessingCenter, 0);
	WriteAttribute(GroupId, "Input File",          H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.InputFile, 0);
	WriteAttribute(GroupId, "Output File",         H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.OutputFile, 0);
	WriteAttribute(GroupId, "L2PROD", 			   H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.L2PROD, 0);
	WriteAttribute(GroupId, "Sensor File",         H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.SensorFile, 0);//传感器特性数据文件
//	WriteAttribute(GroupId, "Censor Name", 			   H5T_STRING, 0.0, 0.0, 0, AttributeData.CalFile, 0);

	WriteAttribute(GroupId, "Number of Scan Lines", H5T_INTEGER, 0.0, 0.0, 0, AttributeData.NumberOfScanLine, "", 16);
	WriteAttribute(GroupId, "Pixels Per Scan Line", H5T_INTEGER, 0.0, 0.0, 0, AttributeData.PixelsPerScanLine, "", 16);
	WriteAttribute(GroupId, "SPIXEL",               H5T_INTEGER, 0.0, 0.0, 0, AttributeData.SPIXEL, "", 16);
	WriteAttribute(GroupId, "EPIXEL",               H5T_INTEGER, 0.0, 0.0, 0, AttributeData.EPIXEL, "", 16);
	WriteAttribute(GroupId, "DPIXEL",               H5T_INTEGER, 0.0, 0.0, 0, AttributeData.DPIXEL, "", 16);
	WriteAttribute(GroupId, "SLINE",                H5T_INTEGER, 0.0, 0.0, 0, AttributeData.SLINE, "", 16);
	WriteAttribute(GroupId, "ELINE",                H5T_INTEGER, 0.0, 0.0, 0, AttributeData.ELINE, "", 16);
	WriteAttribute(GroupId, "DLINE",                H5T_INTEGER, 0.0, 0.0, 0, AttributeData.DLINE, "", 16);
	WriteAttribute(GroupId, "PROC_OCEAN", 			H5T_INTEGER, 0.0, 0.0, 0, AttributeData.PROC_OECEAN, "", 16);
	WriteAttribute(GroupId, "PROC_LAND", 			H5T_INTEGER, 0.0, 0.0, 0, AttributeData.PROC_LAND, "", 16);
	WriteAttribute(GroupId, "ATM_COR", 				H5T_INTEGER, 0.0, 0.0, 0, AttributeData.ATM_COR, "", 16);//以后根据大气校正选项填入相应的值
	WriteAttribute(GroupId, "Rayleigh LUT", 		H5T_INTEGER, 0.0, 0.0, 0, AttributeData.RayleighLUT, "", 16);
	WriteAttribute(GroupId, "GLINT_OPT",			H5T_INTEGER, 0.0, 0.0, 0, AttributeData.GLINT_OPT, "", 16);
	WriteAttribute(GroupId, "METOZ_OPT", 			H5T_INTEGER, 0.0, 0.0, 0, AttributeData.METOZ_OPT, "", 16);//动态，将NCEP数据作为JobOrder的输入项
	WriteAttribute(GroupId, "METFILE",             H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.METFILE1, 0);
//	WriteAttribute(GroupId, "METFILE2", 		 	H5T_STRING, 0.0, 0.0, 0, AttributeData.METFILE2, 0);
//	WriteAttribute(GroupId, "METFILE3", 			H5T_STRING, 0.0, 0.0, 0, AttributeData.METFILE3, 0);
	WriteAttribute(GroupId, "OZONEFILE", 			H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.OZONEFILE1, 0);

	WriteAttribute(GroupId, "ALBEDO Flag",          H5T_INTEGER, 0.0, 0.0, 0, AttributeData.ALBEDOFlag, "", 16);
	WriteAttribute(GroupId, "ALBEDO",               H5T_FLOAT, 0.0, AttributeData.ALBEDO, 0, 0, "", 32);
	WriteAttribute(GroupId, "ABS_AER",              H5T_FLOAT, 0.0, AttributeData.ABSAER, 0, 0, "", 32);//使用COCTS辅助大气校正方法，则从COCTS数据中读取；若没使用，则填入0.
	WriteAttribute(GroupId, "RHOA_MIN",             H5T_FLOAT, 0.0, AttributeData.RHOA_MIN, 0, 0, "", 32);
	WriteAttribute(GroupId, "NLW_MIN",              H5T_FLOAT, 0.0, AttributeData.NLW_MIN, 0, 0, "", 32);//NLW_MIN 改为RRS_MIN，值为0
	WriteAttribute(GroupId, "WS_MAX",               H5T_FLOAT, 0.0, AttributeData.WS_MAX, 0, 0, "", 32);
//	WriteAttribute(GroupId, "TAUA_MAX",             H5T_FLOAT, 0.0, AttributeData.TAUA_MAX, 0, "", 32);
//	WriteAttribute(GroupId, "EPS_MIN",              H5T_FLOAT, 0.0, AttributeData.EPS_MIN, 0, "", 32);
//	WriteAttribute(GroupId, "EPS_MAX",              H5T_FLOAT, 0.0, AttributeData.EPS_MAX, 0, "", 32);
	WriteAttribute(GroupId, "GLINT_THRESH",         H5T_FLOAT, 0.0, AttributeData.GLINT_THRESH, 0, 0, "", 32);

//	WriteAttribute(GroupId, "Flag Names", 			H5T_STRING, 0.0, 0.0, 0, AttributeData.Flag_Name, 0);
	WriteAttribute(GroupId, "Number of Bands", 		H5T_INTEGER, 0.0, 0.0, 0, AttributeData.NumOfBands, "", 16);
	WriteAttribute(GroupId, "Range Beginning Date", H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.RangeBeginDate, 0);
	WriteAttribute(GroupId, "Range Beginning Time", H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.RangeBeginTime, 0);
	WriteAttribute(GroupId, "Range Ending Date", 	H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.RangeEndDate, 0);
	WriteAttribute(GroupId, "Range Ending Time", 	H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.RangeEndTime, 0);

//	WriteAttribute(GroupId, "Upper Left Latitude",  H5T_FLOAT, AttributeData.UpLeftLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Upper Left Longitude", H5T_FLOAT, AttributeData.UpLeftLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Upper Right Latitude", H5T_FLOAT, AttributeData.UpRightLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Upper Right Longitude",H5T_FLOAT, AttributeData.UpRightLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Lower Left Latitude",  H5T_FLOAT, AttributeData.LowLeftLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Lower Left Longitude", H5T_FLOAT, AttributeData.LowLeftLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Lower Right Latitude", H5T_FLOAT, AttributeData.LowRightLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Lower Right Longitude",H5T_FLOAT, AttributeData.LowRightLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Northernmost Latitude",H5T_FLOAT, AttributeData.NorthLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Southernmost Latitude",H5T_FLOAT, AttributeData.SourthLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Westernmost Longitude",H5T_FLOAT, AttributeData.WestLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Easternmost Longitude",H5T_FLOAT, AttributeData.EastLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Scene Center Latitude",H5T_FLOAT, AttributeData.SceneCenterLat, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Scene Center Longitude",H5T_FLOAT, AttributeData.SceneCenterLong, 0.0, 0, 0, "", 64);
//	WriteAttribute(GroupId, "Scene Center Solar Zenith Angle", H5T_FLOAT, AttributeData.SceneCenterSolarZenithAngle, 0.0, 0, 0, "", 64);
	WriteAttribute(GroupId, "Upper Left Latitude",  H5T_FLOAT, 0.0, AttributeData.UpLeftLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Upper Left Longitude", H5T_FLOAT, 0.0, AttributeData.UpLeftLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Upper Right Latitude", H5T_FLOAT, 0.0, AttributeData.UpRightLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Upper Right Longitude",H5T_FLOAT, 0.0, AttributeData.UpRightLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Lower Left Latitude",  H5T_FLOAT, 0.0, AttributeData.LowLeftLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Lower Left Longitude", H5T_FLOAT, 0.0, AttributeData.LowLeftLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Lower Right Latitude", H5T_FLOAT, 0.0, AttributeData.LowRightLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Lower Right Longitude",H5T_FLOAT, 0.0, AttributeData.LowRightLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Northernmost Latitude",H5T_FLOAT, 0.0, AttributeData.NorthLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Southernmost Latitude",H5T_FLOAT, 0.0, AttributeData.SourthLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Westernmost Longitude",H5T_FLOAT, 0.0, AttributeData.WestLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Easternmost Longitude",H5T_FLOAT, 0.0, AttributeData.EastLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Scene Center Latitude",H5T_FLOAT, 0.0, AttributeData.SceneCenterLat, 0, 0, "", 32);
	WriteAttribute(GroupId, "Scene Center Longitude",H5T_FLOAT, 0.0, AttributeData.SceneCenterLong, 0, 0, "", 32);
	WriteAttribute(GroupId, "Scene Center Solar Zenith Angle", H5T_FLOAT, 0.0, AttributeData.SceneCenterSolarZenithAngle, 0, 0, "", 32);

	WriteAttribute(GroupId, "OrbitFlag",            H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.OrbitFlag, 0);
	WriteAttribute(GroupId, "Orbit Node Longitude", H5T_FLOAT, 0.0, AttributeData.OrbitNodeLong, 0, 0, "", 32);

	WriteAttribute(GroupId, "Realtime or Delay Flag", H5T_STRING, 0.0, 0.0, 0, 0, AttributeData.RealtimeDelayFlag, 0);
	WriteAttribute(GroupId, "Orbit Number", 		H5T_INTEGER, 0.0, 0.0, 0, AttributeData.OrbitNumber, "", 32);


	H5Gclose(GroupId);
	H5Fclose(FileId);
	return 0;
}

int CreateNavigationAttribute(const char* PATH_WRITE, char* Group_name, struct NavigationData NavitationAttrData){

	hid_t FileId = H5Fopen(PATH_WRITE, H5F_ACC_RDWR, H5P_DEFAULT);
	if(FileId < 0){
		FileId = H5Fcreate(PATH_WRITE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if(FileId < 0){
			return -1;
		}
	}

	hid_t GroupId = H5Gopen2(FileId, Group_name, H5P_DEFAULT);
	if(GroupId < 0){
		GroupId = H5Gcreate2(FileId, Group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if(GroupId < 0){
			return -1;
		}
	}

	WriteAttribute(GroupId, "First Navigation Point", 		H5T_INTEGER, 0.0, 0.0, NavitationAttrData.FirstNavigationPoint, 0, "", 32);
	WriteAttribute(GroupId, "Navigation Point Counts", H5T_INTEGER, 0.0, 0.0, NavitationAttrData.NavigationPoints,0, "", 32);
	WriteAttribute(GroupId, "Pixel-intervals Of Navigation Point", 		H5T_INTEGER, 0.0, 0.0, NavitationAttrData.PixelinterOfNavigationPoints, 0, "", 32);


	H5Gclose(GroupId);
	H5Fclose(FileId);
	return 0;
}

/**
 * 该方法实现了hdf5属性表文件的写入。
 * 其中传入参数有：PATH_WRITE：要写入属性表的文件路径名称
 * 				data_Name：要写入属性表的文件名称
 * 				DATA：要写入的文件属性表的结构体
 * */
void WriteAttribute(hid_t Id, char* AttrName, int dataType, double Doubledata, float FloatData, int IntData, short ShortData, char* CharData, int bit){

	hid_t atype = H5Tcopy(H5T_C_S1);
	int length = 0;
	hid_t dataspace_id = H5Screate(H5S_SCALAR);
	hid_t attr_id;
	herr_t ret;
	switch(dataType)
	{
	case H5T_FLOAT:
		if(bit == 32){
			attr_id  = H5Acreate(Id, AttrName, H5T_NATIVE_FLOAT, dataspace_id,H5P_DEFAULT,H5P_DEFAULT);
			ret = H5Awrite(attr_id, H5T_NATIVE_FLOAT, &FloatData);
		}else if(bit == 64){
			attr_id  = H5Acreate(Id, AttrName, H5T_NATIVE_DOUBLE, dataspace_id,H5P_DEFAULT,H5P_DEFAULT);
			ret = H5Awrite(attr_id, H5T_NATIVE_DOUBLE, &Doubledata);
		}

		break;
//	case H5T_INTEGER:
//		attr_id  = H5Acreate(Id, AttrName, H5T_NATIVE_INT, dataspace_id,H5P_DEFAULT,H5P_DEFAULT);
//		ret = H5Awrite(attr_id, H5T_NATIVE_INT, &IntData);
//		break;
//将int类型变成既可以32位又可以16位的，增强代码的健壮性
	case H5T_INTEGER:
		if(bit == 16){
			attr_id  = H5Acreate(Id, AttrName, H5T_NATIVE_SHORT, dataspace_id,H5P_DEFAULT,H5P_DEFAULT);
			ret = H5Awrite(attr_id, H5T_NATIVE_SHORT, &ShortData);
		}else if(bit == 32){
			attr_id  = H5Acreate(Id, AttrName, H5T_NATIVE_INT, dataspace_id,H5P_DEFAULT,H5P_DEFAULT);
			ret = H5Awrite(attr_id, H5T_NATIVE_INT, &IntData);
		}

		break;

	case H5T_STRING:
		length = strlen(CharData);
			H5Tset_size(atype, length);
		attr_id = H5Acreate(Id, AttrName, atype, dataspace_id, H5P_DEFAULT,H5P_DEFAULT);
		ret = H5Awrite(attr_id, atype, CharData);
		break;
	}
	H5Tclose(atype);
	H5Sclose(dataspace_id);
	H5Aclose(attr_id);
}

void WriteFlags(hid_t L2_dataset){
//
//	char* FlagNames_1 = "ATMFAIL,LAND,BADANC,HISUNGLINT,HILT,HISENZ,SPARE,NEGLW,STLIGHT,CLOUD,";
//	char* FlagNames_2 = "SPARE,TURBID,HISOLZ,HITAUA,LOWLW,CHLFAIL,SPARE,ABSAER,SPARE,MAXAERITER,";
//	char* FlagNames_3 =	"MODSUNGLINT,CHLRANGE,ATMWARN,DARKPIXEL,SEAICE,NAVFAIL,FILTER,BADSST,BADSSTALG,SPARE,SPARE,SPARE";
//	char FlagName[512];  strcpy(FlagName, FlagNames_1);  strcat(FlagName, FlagNames_2);  strcat(FlagName, FlagNames_3);
//
//	char FlagValue[512] = "1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648";
//	WriteAttribute(L2_dataset, "atmospheric correction failure from invalid inputs (ATMFAIL)", H5T_STRING, 0.0, 0.0, 0, "1", 0);
//	WriteAttribute(L2_dataset, "Land (LAND)", H5T_STRING, 0.0, 0.0, 0, "1", 0);
//	WriteAttribute(L2_dataset, "satellite zenith angle above limit (HISENZ)", H5T_STRING, 0.0, 0.0, 0, "2", 0);
//	WriteAttribute(L2_dataset, "atmospheric correction failure from invalid inputs (ATMFAIL)", H5T_STRING, 0.0, 0.0, 0, "32", 0);
//	WriteAttribute(L2_dataset, "clouds and/or ice (CLOUD)", H5T_STRING, 0.0, 0.0, 0, "512", 0);
//
//	WriteAttribute(L2_dataset, "solar zenith angle above limit (HISOLZ)", H5T_STRING, 0.0, 0.0, 0, "4096", 0);
//	WriteAttribute(L2_dataset, "chlorophyll out of range (CHLRANGE)v", H5T_STRING, 0.0, 0.0, 0, "2097152", 0);
//	WriteAttribute(L2_dataset, "SDD out of range (CHLRANGE)v", H5T_STRING, 0.0, 0.0, 0, "536870912", 0);
//	WriteAttribute(L2_dataset, "SSD out of range (CHLRANGE)v", H5T_STRING, 0.0, 0.0, 0, "1073741824", 0);
//	WriteAttribute(L2_dataset, "xxx out of range (CHLRANGE)v", H5T_STRING, 0.0, 0.0, 0, "2147483648", 0);

	char FlagName[512];
	strcpy(FlagName, "Flag Names");

	char* FlagValue_1 = "ATMFAIL,LAND,BADANC,HISUNGLINT,HILT,HISENZ,SPARE,NEGLW,STLIGHT,CLOUD,";
	char* FlagValue_2 = "SPARE,TURBID,HISOLZ,HITAUA,LOWLW,CHLFAIL,SPARE,ABSAER,SPARE,MAXAERITER,";
	char* FlagValue_3 =	"MODSUNGLINT,CHLRANGE,ATMWARN,DARKPIXEL,SEAICE,NAVFAIL,FILTER,BADSST,BADSSTALG,SPARE,SPARE,SPARE";
	char FlagValue[512]; strcpy(FlagValue, FlagValue_1);  strcat(FlagValue, FlagValue_2);  strcat(FlagValue, FlagValue_3);

	WriteAttribute(L2_dataset, FlagName, H5T_STRING, 0.0, 0.0, 0, 0, FlagValue, 0);

}



#include "tinyxml.h"
#include "tinystr.h"
#include "tinystr.cpp"
#include "tinyxml.cpp"
#include "tinyxmlerror.cpp"
#include "tinyxmlparser.cpp"

/*
	TiXmlDocument：文档类，它代表了整个xml文件
	TiXmlDeclaration：声明类，它表示文件的声明部分
	TiXmlComment：注释类，它表示文件的注释部分
	TiXmlElement：元素类，它是文件的主要部分，并且支持嵌套结构，一般使用这种结构来分类的存储信息，它可以包含属性类和文本类
	TiXmlAttribute/TiXmlAttributeSet：元素属性，它一般嵌套在元素中，用于记录此元素的一些属性
	TiXmlText：文本对象，它嵌套在某个元素内部
*/

//解析xml文件
//  Read L1B meta.xml
//解析 L1B meta.xml文件----------------------------------------------
struct L1B_XML_DATA L1B_XmlRead(const char* PATH_Xml)
{

struct L1B_XML_DATA Xml_Parameter;	//初始化存放xml参数的结构体
Xml_Parameter.Flag = 0;// No Error
try{
	//-----------------------------------ProductMeta--------------------------------
	TiXmlDocument mydoc(PATH_Xml); //xml文档对象
	bool loadOk=mydoc.LoadFile(); //加载文档
	//判断xml文件是否存在
	if(!loadOk)
	{
	//cout<<"could not load the xml file.Error:"<<mydoc.ErrorDesc()<<endl;
	//exit(-1);
	Xml_Parameter.Flag = -1;
	Xml_Parameter.ErrorInfo = "Error: could not load the H1C_OPER_CZI_L1B meta xml file!";
	}

	TiXmlElement *RootElement=mydoc.RootElement();	//根元素************************
	if (!(strcmp("ProductMetaData", RootElement->Value()) == 0))
	{
	//cout<< "L1B root element is not true!" <<endl;
	Xml_Parameter.Flag = -1;
	Xml_Parameter.ErrorInfo = "Error: H1C_OPER_CZI_L1B meta xml root element is not true!";
	//exit(-1);
	}

//	TiXmlElement *SatelliteID = RootElement->FirstChildElement("SatelliteID");
//	const char* SatelliteID_char = SatelliteID->GetText();
//	strcpy(Xml_Parameter.SatelliteID, SatelliteID_char);

//	TiXmlElement *SensorID = RootElement->FirstChildElement("SensorID");
//	const char* SensorID_char = SensorID->GetText();
//	strcpy(Xml_Parameter.SensorID, SensorID_char);
//
//	TiXmlElement *OrbitID = RootElement->FirstChildElement("OrbitID");
//	const char* OrbitID_char = OrbitID->GetText();
//	strcpy(Xml_Parameter.OrbitID, OrbitID_char);
//
	TiXmlElement *ReceiveStation = RootElement->FirstChildElement("ReceiveStation");
	const char* ReceiveStation_char = ReceiveStation->GetText();
	strcpy(Xml_Parameter.ReceiveStation, ReceiveStation_char);
//
//	TiXmlElement *SceneID = RootElement->FirstChildElement("SceneID");
//	const char* SceneID_char = SceneID->GetText();
//	strcpy(Xml_Parameter.SceneID, SceneID_char);
//
//	TiXmlElement *ProductName = RootElement->FirstChildElement("ProductName");
//	const char* ProductName_char = ProductName->GetText();
//	strcpy(Xml_Parameter.ProductName, ProductName_char);
//
//	TiXmlElement *TimeType = RootElement->FirstChildElement("TimeType");
//	const char* TimeType_char = TimeType->GetText();
//	strcpy(Xml_Parameter.TimeType, TimeType_char);
//
//	TiXmlElement *ProductUnit = RootElement->FirstChildElement("ProductUnit");
//	const char* ProductUnit_char = ProductUnit->GetText();
//	strcpy(Xml_Parameter.ProductUnit, ProductUnit_char);
//
//	TiXmlElement *ContactInfo = RootElement->FirstChildElement("ContactInfo");
//	const char* ContactInfo_char = ContactInfo->GetText();
//	strcpy(Xml_Parameter.ContactInfo, ContactInfo_char);
//
	//----------------------------SatelliteInfo------------------------
	TiXmlElement *SatelliteInfo = RootElement->FirstChildElement("SatelliteInfo");

	TiXmlElement *OrbitFlag = SatelliteInfo->FirstChildElement("OrbitFlag");
	const char* OrbitFlag_char = OrbitFlag->GetText();
	strcpy(Xml_Parameter.OrbitFlag, OrbitFlag_char);
//
//	TiXmlElement *WorkMode = SatelliteInfo->FirstChildElement("WorkMode");
//	const char* WorkMode_char = WorkMode->GetText();
//	strcpy(Xml_Parameter.WorkMode, WorkMode_char);
//
//	//----------------------------QualityInfo------------------------
//	TiXmlElement *QualityInfo = RootElement->FirstChildElement("QualityInfo");
//
//	TiXmlElement *ProductIntegrity = QualityInfo->FirstChildElement("ProductIntegrity");
//	const char* ProductIntegrity_char = ProductIntegrity->GetText();
//	strcpy(Xml_Parameter.ProductIntegrity, ProductIntegrity_char);
//
//	//----------------------------ProductInfo------------------------
//	TiXmlElement *ProductInfo = RootElement->FirstChildElement("ProductInfo");
//
//	TiXmlElement *ProductLevel = ProductInfo->FirstChildElement("ProductLevel");
//	const char* ProductLevel_char = ProductLevel->GetText();
//	strcpy(Xml_Parameter.ProductLevel, ProductLevel_char);
//
//	TiXmlElement *ProductFormat = ProductInfo->FirstChildElement("ProductFormat");
//	const char* ProductFormat_char = ProductFormat->GetText();
//	strcpy(Xml_Parameter.ProductFormat, ProductFormat_char);
//
//	TiXmlElement *ProductVersion = ProductInfo->FirstChildElement("ProductVersion");
//	const char* ProductVersion_char = ProductVersion->GetText();
//	strcpy(Xml_Parameter.ProductVersion, ProductVersion_char);
//
//	TiXmlElement *SoftwareName = ProductInfo->FirstChildElement("SoftwareName");
//	const char* SoftwareName_char = SoftwareName->GetText();
//	strcpy(Xml_Parameter.SoftwareName, SoftwareName_char);
//
//	TiXmlElement *SoftwareVersion = ProductInfo->FirstChildElement("SoftwareVersion");
//	const char* SoftwareVersion_char = SoftwareVersion->GetText();
//	strcpy(Xml_Parameter.SoftwareVersion, SoftwareVersion_char);
//
//	TiXmlElement *SourseData = ProductInfo->FirstChildElement("SourseData");
//	const char* SourseData_char = SourseData->GetText();
//	strcpy(Xml_Parameter.SourseData, SourseData_char);
//
//	TiXmlElement *ProduceTime = ProductInfo->FirstChildElement("ProduceTime");
//	const char* ProduceTime_char = ProduceTime->GetText();
//	strcpy(Xml_Parameter.ProduceTime, ProduceTime_char);
//
//	TiXmlElement *Resolution = ProductInfo->FirstChildElement("Resolution");
//	const char* Resolution_char = Resolution->GetText();
//	strcpy(Xml_Parameter.Resolution, Resolution_char);
//
//	//-----------------------CorrectionParams---------------------------------
//	TiXmlElement *CorrectionParams = RootElement->FirstChildElement("CorrectionParams");
//
//	TiXmlElement *EarthEllipsoid = CorrectionParams->FirstChildElement("EarthEllipsoid");
//	const char* EarthEllipsoid_char = EarthEllipsoid->GetText();
//	strcpy(Xml_Parameter.EarthEllipsoid, EarthEllipsoid_char);
//
//	TiXmlElement *ProjectType = CorrectionParams->FirstChildElement("ProjectType");
//	const char* ProjectType_char = ProjectType->GetText();
//	strcpy(Xml_Parameter.ProjectType, ProjectType_char);
//
	//----------------------------ImageInfo------------------------------
	TiXmlElement *ImageInfo = RootElement->FirstChildElement("ImageInfo");

	TiXmlElement *StartTime = ImageInfo->FirstChildElement("StartTime");
	const char* StartTime_char = StartTime->GetText();
	strcpy(Xml_Parameter.StartTime, StartTime_char);

	TiXmlElement *EndTime = ImageInfo->FirstChildElement("EndTime");
	const char* EndTime_char = EndTime->GetText();
	strcpy(Xml_Parameter.EndTime, EndTime_char);

	TiXmlElement *CenterTime = ImageInfo->FirstChildElement("CentreTime");
	const char* CenterTime_char = CenterTime->GetText();
	strcpy(Xml_Parameter.CenterTime, CenterTime_char);
//
//	TiXmlElement *DataSize = ImageInfo->FirstChildElement("DataSize");
//	const char* DataSize_char = DataSize->GetText();
//	strcpy(Xml_Parameter.DataSize, DataSize_char);
//
//	TiXmlElement *DataUnit = ImageInfo->FirstChildElement("DataUnit");
//	const char* DataUnit_char = DataUnit->GetText();
//	strcpy(Xml_Parameter.DataUnit, DataUnit_char);
//
//	TiXmlElement *CentreLocation = ImageInfo->FirstChildElement("CentreLocation");
//
//	TiXmlElement *CenterLongitude = CentreLocation->FirstChildElement("CenterLongitude");
//	const char* CenterLongitude_char = CenterLongitude->GetText();
//	strcpy(Xml_Parameter.CenterLocation_Latitude, CenterLongitude_char);
//
//	TiXmlElement *CenterLatitude = CentreLocation->FirstChildElement("CenterLatitude");
//	const char* CenterLatitude_char = CenterLatitude->GetText();
//	strcpy(Xml_Parameter.CenterLocation_Longitude, CenterLatitude_char);
//
//	TiXmlElement *Corners = ImageInfo->FirstChildElement("Corners");
//
//	TiXmlElement *TopLeftLongitude = Corners->FirstChildElement("TopLeft")->FirstChildElement("TopLeft_Longitude");
//	const char* TopLeftLongitude_char = TopLeftLongitude->GetText();
//	strcpy(Xml_Parameter.TopLeft_Longitude, TopLeftLongitude_char);
//
//	TiXmlElement *TopLeftLatitude = Corners->FirstChildElement("TopLeft")->FirstChildElement("TopLeft_Latitude");
//	const char* TopLeftLatitude_char = TopLeftLatitude->GetText();
//	strcpy(Xml_Parameter.TopLeft_Latitude, TopLeftLatitude_char);
//
//	TiXmlElement *TopRightLongitude = Corners->FirstChildElement("TopRight")->FirstChildElement("TopRight_Longitude");
//	const char* TopRightLongitude_char = TopRightLongitude->GetText();
//	strcpy(Xml_Parameter.TopRight_Longitude, TopRightLongitude_char);
//
//	TiXmlElement *TopRightLatitude = Corners->FirstChildElement("TopRight")->FirstChildElement("TopRight_Latitude");
//	const char* TopRightLatitude_char = TopRightLatitude->GetText();
//	strcpy(Xml_Parameter.TopRight_Latitude, TopRightLatitude_char);
//
//	TiXmlElement *BottomRightLongitude = Corners->FirstChildElement("BottomRight")->FirstChildElement("BottomRight_Longitude");
//	const char* BottomRightLongitude_char = BottomRightLongitude->GetText();
//	strcpy(Xml_Parameter.BottomRight_Longitude, BottomRightLongitude_char);
//
//	TiXmlElement *BottomRightLatitude = Corners->FirstChildElement("BottomRight")->FirstChildElement("BottomRight_Latitude");
//	const char* BottomRightLatitude_char = BottomRightLatitude->GetText();
//	strcpy(Xml_Parameter.BottomRight_Latitude, BottomRightLatitude_char);
//
//	TiXmlElement *BottomLeftLongitude = Corners->FirstChildElement("BottomLeft")->FirstChildElement("BottomLeft_Longitude");
//	const char* BottomLeftLongitude_char = BottomLeftLongitude->GetText();
//	strcpy(Xml_Parameter.BottomLeft_Longitude, BottomLeftLongitude_char);
//
//	TiXmlElement *BottomLeftLatitude = Corners->FirstChildElement("BottomLeft")->FirstChildElement("BottomLeft_Latitude");
//	const char* BottomLeftLatitude_char = BottomLeftLatitude->GetText();
//	strcpy(Xml_Parameter.BottomLeft_Latitude, BottomLeftLatitude_char);

}catch(exception& e){
//cout<< e.what()<<endl;
exit(1);
}
return Xml_Parameter;
}

#include <sstream>
const char* inttochar(int value){
	std::ostringstream o;
	if(!(o << value))
		return "";
	const char* data = o.str().c_str();
	return data;
}
const char* floattochar(float value){
	std::ostringstream o;
	if(!(o << value))
		return "";
	const char* data = o.str().c_str();
	return data;
}


#include<exception>
//function：create a xml file
int XmlMetaWrite(const char* PATH_Xml, struct XML_DATA Xml_Parameter){
	try{
		TiXmlDocument *doc = new TiXmlDocument();               //创建xml文档对象

		TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","UTF-8","");
		doc->LinkEndChild(pDeclaration);

		TiXmlElement *ProductMeta = new TiXmlElement("ProductMeta");    //创建一个根结点 ProductMeta
		doc->LinkEndChild(ProductMeta);	//链接到上一节点下，下同

		TiXmlElement *SatelliteName = new TiXmlElement("SatelliteID");    //创建一个二级结点 SatelliteName
		ProductMeta->LinkEndChild(SatelliteName);
		TiXmlText *SatelliteName_Text = new TiXmlText(Xml_Parameter.SatelliteID);	//创建XmlText文本
		SatelliteName->LinkEndChild(SatelliteName_Text);

		TiXmlElement *SensorName = new TiXmlElement("SensorID");
		ProductMeta->LinkEndChild(SensorName);
		TiXmlText *SensorName_Text = new TiXmlText(Xml_Parameter.SensorID);
		SensorName->LinkEndChild(SensorName_Text);

		TiXmlElement *OrbitNumber = new TiXmlElement("OrbitID");
		ProductMeta->LinkEndChild(OrbitNumber);
		TiXmlText *OrbitNumber_Text = new TiXmlText(Xml_Parameter.OrbitID);
		OrbitNumber->LinkEndChild(OrbitNumber_Text);

		TiXmlElement *ReceiveStation = new TiXmlElement("ReceiveStation");
		ProductMeta->LinkEndChild(ReceiveStation);
		TiXmlText *ReceiveStation_Text = new TiXmlText(Xml_Parameter.ReceiveStation);
		ReceiveStation->LinkEndChild(ReceiveStation_Text);

//		TiXmlElement *SceneID = new TiXmlElement("SceneID");
//		ProductMeta->LinkEndChild(SceneID);
//		TiXmlText *SceneID_Text = new TiXmlText(Xml_Parameter.SceneID);
//		ReceiveStation->LinkEndChild(SceneID_Text);

		TiXmlElement *ProductName = new TiXmlElement("ProductName");
		ProductMeta->LinkEndChild(ProductName);
		TiXmlText *ProductName_Text = new TiXmlText(Xml_Parameter.ProductName);
		ProductName->LinkEndChild(ProductName_Text);

		TiXmlElement *FileName = new TiXmlElement("FileName");
		ProductMeta->LinkEndChild(FileName);
		TiXmlText *FileName_Text = new TiXmlText(Xml_Parameter.FileName);
		FileName->LinkEndChild(FileName_Text);

		TiXmlElement *TimeType = new TiXmlElement("TimeType");
		ProductMeta->LinkEndChild(TimeType);
		TiXmlText *TimeType_Text = new TiXmlText(Xml_Parameter.TimeType);
		TimeType->LinkEndChild(TimeType_Text);

		TiXmlElement *ProductUnit = new TiXmlElement("ProductUnit");
		ProductMeta->LinkEndChild(ProductUnit);
		TiXmlText *ProductUnit_Text = new TiXmlText(Xml_Parameter.ProductUnit);
		ProductUnit->LinkEndChild(ProductUnit_Text);

		TiXmlElement *ContactInfo = new TiXmlElement("ContactInfo");
		ProductMeta->LinkEndChild(ContactInfo);
		TiXmlText *ContactInfo_Text = new TiXmlText(Xml_Parameter.ContactInfo);
		ContactInfo->LinkEndChild(ContactInfo_Text);
		//-------------------------------------------------SatelliteInfo-----------------------------------------------------
		TiXmlElement *SatelliteInfo = new TiXmlElement("SatelliteInfo");
		ProductMeta->LinkEndChild(SatelliteInfo);

		TiXmlElement *OrbitFlag = new TiXmlElement("OrbitFlag");
		SatelliteInfo->LinkEndChild(OrbitFlag);
		TiXmlText *OrbitFlag_Text = new TiXmlText(Xml_Parameter.OrbitFlag);
		OrbitFlag->LinkEndChild(OrbitFlag_Text);

		TiXmlElement *WorkMode = new TiXmlElement("WorkMode");
		SatelliteInfo->LinkEndChild(WorkMode);
		TiXmlText *WorkMode_Text = new TiXmlText(Xml_Parameter.WorkMode);
		WorkMode->LinkEndChild(WorkMode_Text);
		//-------------------------------------------------QualityInfo-----------------------------------------------------
		TiXmlElement *QualityInfo = new TiXmlElement("QualityInfo");
		ProductMeta->LinkEndChild(QualityInfo);
		TiXmlElement *ProductIntegrity = new TiXmlElement("ProductIntegrity");
		QualityInfo->LinkEndChild(ProductIntegrity);

		TiXmlElement *AuxData1 = new TiXmlElement("AuxData1");
		ProductIntegrity->LinkEndChild(AuxData1);
		TiXmlText *AuxData1_Text = new TiXmlText(Xml_Parameter.AuxData1);
		AuxData1->LinkEndChild(AuxData1_Text);

		TiXmlElement *AuxData2 = new TiXmlElement("AuxData2");
		ProductIntegrity->LinkEndChild(AuxData2);
		TiXmlText *AuxData2_Text = new TiXmlText(Xml_Parameter.AuxData2);
		AuxData2->LinkEndChild(AuxData2_Text);

		TiXmlElement *AuxData3 = new TiXmlElement("AuxData3");
		ProductIntegrity->LinkEndChild(AuxData3);
		TiXmlText *AuxData3_Text = new TiXmlText(Xml_Parameter.AuxData3);
		AuxData3->LinkEndChild(AuxData3_Text);
		//-------------------------------------------------ProductInfo-----------------------------------------------------
		TiXmlElement *ProductInfo = new TiXmlElement("ProductInfo");
		ProductMeta->LinkEndChild(ProductInfo);

		TiXmlElement *ProductLevel = new TiXmlElement("ProductLevel");
		ProductInfo->LinkEndChild(ProductLevel);
		TiXmlText *ProductLevel_Text = new TiXmlText(Xml_Parameter.ProductLevel);
		ProductLevel->LinkEndChild(ProductLevel_Text);

		TiXmlElement *ProductFormat = new TiXmlElement("ProductFormat");
		ProductInfo->LinkEndChild(ProductFormat);
		TiXmlText *ProductFormat_Text = new TiXmlText(Xml_Parameter.ProductFormat);
		ProductFormat->LinkEndChild(ProductFormat_Text);

		TiXmlElement *ProductVersion = new TiXmlElement("ProductVersion");
		ProductInfo->LinkEndChild(ProductVersion);
		TiXmlText *ProductVersion_Text = new TiXmlText(Xml_Parameter.ProductVersion);
		ProductVersion->LinkEndChild(ProductVersion_Text);

		TiXmlElement *SoftwareName = new TiXmlElement("SoftwareName");
		ProductInfo->LinkEndChild(SoftwareName);
		TiXmlText *SoftwareName_Text = new TiXmlText(Xml_Parameter.SoftwareName);
		SoftwareName->LinkEndChild(SoftwareName_Text);

		TiXmlElement *SoftwareVersion = new TiXmlElement("SoftwareVersion");
		ProductInfo->LinkEndChild(SoftwareVersion);
		TiXmlText *SoftwareVersion_Text = new TiXmlText(Xml_Parameter.SoftwareVersion);
		SoftwareVersion->LinkEndChild(SoftwareVersion_Text);

		TiXmlElement *SourseData = new TiXmlElement("SourseData");
		ProductInfo->LinkEndChild(SourseData);
		TiXmlText *SourseData_Text = new TiXmlText(Xml_Parameter.SourseData);
		SourseData->LinkEndChild(SourseData_Text);

		TiXmlElement *ProduceTime = new TiXmlElement("ProduceTime");
		ProductInfo->LinkEndChild(ProduceTime);
		TiXmlText *ProduceTime_Text = new TiXmlText(Xml_Parameter.ProduceTime);
		ProduceTime->LinkEndChild(ProduceTime_Text);

		TiXmlElement *Resolution = new TiXmlElement("Resolution");
		ProductInfo->LinkEndChild(Resolution);
		TiXmlText *Resolution_Text = new TiXmlText(Xml_Parameter.Resolution);
		Resolution->LinkEndChild(Resolution_Text);

		TiXmlElement *QuickViewURL = new TiXmlElement("QuickViewURL");
		ProductInfo->LinkEndChild(QuickViewURL);
		TiXmlText *QuickViewURL_Text = new TiXmlText(Xml_Parameter.QuickViewURL);	//创建XmlText文本
		QuickViewURL->LinkEndChild(QuickViewURL_Text);
		//-------------------------------------------------CorrectionParams-----------------------------------------------------
		TiXmlElement *CorrectionParams = new TiXmlElement("CorrectionParams");
		ProductMeta->LinkEndChild(CorrectionParams);

		TiXmlElement *EarthEllipsoid = new TiXmlElement("EarthEllipsoid");
		CorrectionParams->LinkEndChild(EarthEllipsoid);
		TiXmlText *EarthEllipsoid_Text = new TiXmlText(Xml_Parameter.EarthEllipsoid);
		EarthEllipsoid->LinkEndChild(EarthEllipsoid_Text);

		TiXmlElement *ProjectType = new TiXmlElement("ProjectType");
		CorrectionParams->LinkEndChild(ProjectType);
		TiXmlText *ProjectType_Text = new TiXmlText(Xml_Parameter.ProjectType);
		ProjectType->LinkEndChild(ProjectType_Text);

		//-------------------------------------------------ImageInfo-----------------------------------------------------
		TiXmlElement *ImageInfo = new TiXmlElement("ImageInfo");
		ProductMeta->LinkEndChild(ImageInfo);

		TiXmlElement *StartTime = new TiXmlElement("StartTime");
		ImageInfo->LinkEndChild(StartTime);
		TiXmlText *StartTime_Text = new TiXmlText(Xml_Parameter.StartTime);
		StartTime->LinkEndChild(StartTime_Text);

		TiXmlElement *EndTime = new TiXmlElement("EndTime");
		ImageInfo->LinkEndChild(EndTime);
		TiXmlText *EndTime_Text = new TiXmlText(Xml_Parameter.EndTime);
		EndTime->LinkEndChild(EndTime_Text);

		TiXmlElement *CenterTime = new TiXmlElement("CenterTime");
		ImageInfo->LinkEndChild(CenterTime);
		TiXmlText *CenterTime_Text = new TiXmlText(Xml_Parameter.CenterTime);
		CenterTime->LinkEndChild(CenterTime_Text);

		TiXmlElement *DataSize = new TiXmlElement("DataSize");
		ImageInfo->LinkEndChild(DataSize);
		TiXmlText *DataSize_Text = new TiXmlText(Xml_Parameter.DataSize);
		DataSize->LinkEndChild(DataSize_Text);

		TiXmlElement *DataUnit = new TiXmlElement("DataUnit");
		ImageInfo->LinkEndChild(DataUnit);
		TiXmlText *DataUnit_Text = new TiXmlText(Xml_Parameter.DataUnit);
		DataUnit->LinkEndChild(DataUnit_Text);
		//-------------------------------------------------ImageInfo-----------------------------------------------------
		TiXmlElement *CentreLocation = new TiXmlElement("CentreLocation");
		ImageInfo->LinkEndChild(CentreLocation);

		TiXmlElement *CenterLongitude = new TiXmlElement("CenterLongitude");
		CentreLocation->LinkEndChild(CenterLongitude);
		TiXmlText *CenterLongitude_Text = new TiXmlText(Xml_Parameter.CenterLocation_Longitude);
		CenterLongitude->LinkEndChild(CenterLongitude_Text);

		TiXmlElement *CenterLatitude = new TiXmlElement("CenterLatitude");
		CentreLocation->LinkEndChild(CenterLatitude);
		TiXmlText *CenterLatitude_Text = new TiXmlText(Xml_Parameter.CenterLocation_Latitude);
		CenterLatitude->LinkEndChild(CenterLatitude_Text);

		TiXmlElement *Corners = new TiXmlElement("Corners");
		ImageInfo->LinkEndChild(Corners);

		TiXmlElement *TopLeft = new TiXmlElement("TopLeft");
		Corners->LinkEndChild(TopLeft);

		TiXmlElement *TopLeftLongitude = new TiXmlElement("TopLeftLongitude");
		TopLeft->LinkEndChild(TopLeftLongitude);
		TiXmlText *TopLeftLongitude_Text = new TiXmlText(Xml_Parameter.TopLeft_Longitude);
		TopLeftLongitude->LinkEndChild(TopLeftLongitude_Text);

		TiXmlElement *TopLeftLatitude = new TiXmlElement("TopLeftLatitude");
		TopLeft->LinkEndChild(TopLeftLatitude);
		TiXmlText *TopLeftLatitude_Text = new TiXmlText(Xml_Parameter.TopLeft_Latitude);
		TopLeftLatitude->LinkEndChild(TopLeftLatitude_Text);

		TiXmlElement *TopRight = new TiXmlElement("TopRight");
		Corners->LinkEndChild(TopRight);

		TiXmlElement *TopRightLongitude = new TiXmlElement("TopRightLongitude");
		TopRight->LinkEndChild(TopRightLongitude);
		TiXmlText *TopRightLongitude_Text = new TiXmlText(Xml_Parameter.TopRight_Longitude);
		TopRightLongitude->LinkEndChild(TopRightLongitude_Text);

		TiXmlElement *TopRightLatitude = new TiXmlElement("TopRightLatitude");
		TopRight->LinkEndChild(TopRightLatitude);
		TiXmlText *TopRightLatitude_Text = new TiXmlText(Xml_Parameter.TopRight_Latitude);
		TopRightLatitude->LinkEndChild(TopRightLatitude_Text);

		TiXmlElement *BottomRight = new TiXmlElement("BottomRight");
		Corners->LinkEndChild(BottomRight);

		TiXmlElement *BottomRightLongitude = new TiXmlElement("BottomRightLongitude");
		BottomRight->LinkEndChild(BottomRightLongitude);
		TiXmlText *BottomRightLongitude_Text = new TiXmlText(Xml_Parameter.BottomRight_Longitude);
		BottomRightLongitude->LinkEndChild(BottomRightLongitude_Text);

		TiXmlElement *BottomRightLatitude = new TiXmlElement("BottomRightLatitude");
		BottomRight->LinkEndChild(BottomRightLatitude);
		TiXmlText *BottomRightLatitude_Text = new TiXmlText(Xml_Parameter.BottomRight_Latitude);
		BottomRightLatitude->LinkEndChild(BottomRightLatitude_Text);

		TiXmlElement *BottomLeft = new TiXmlElement("BottomLeft");
		Corners->LinkEndChild(BottomLeft);

		TiXmlElement *BottomLeftLongitude = new TiXmlElement("BottomLeftLongitude");
		BottomLeft->LinkEndChild(BottomLeftLongitude);
		TiXmlText *BottomLeftLongitude_Text = new TiXmlText(Xml_Parameter.BottomLeft_Longitude);
		BottomLeftLongitude->LinkEndChild(BottomLeftLongitude_Text);

		TiXmlElement *BottomLeftLatitude = new TiXmlElement("BottomLeftLatitude");
		BottomLeft->LinkEndChild(BottomLeftLatitude);
		TiXmlText *BottomLeftLatitude_Text = new TiXmlText(Xml_Parameter.BottomLeft_Latitude);
		BottomLeftLatitude->LinkEndChild(BottomLeftLatitude_Text);

		doc->SaveFile(PATH_Xml);//保存到文件

		delete doc;
	}catch(exception& e){
		return -1;
	}
    return 0;
}

// 创建L2a文件列表
int ListXmlWrite(const char* PATH_Xml, char* L2AH5Path, char* L2AMatePath,  char* JpgBluePath, char* JpgGreenPath,  char* JpgRedPath, char* JpgNirPath, char* L2BH5Path, char* JpgNDVIPath, char* JpgSSCPath, char* L2BH5MatePath, char* L2CH5Path, char* L2CH5MatePath, char* JpgChlPath, char* JpgSDDPath, char* L2CJpgBluePath, char* L2CJpgGreenPath, char* L2CJpgRedPath, char* L2CJpgNirPath){
	try{
		TiXmlDocument *doc = new TiXmlDocument();                         //创建xml文档对象

		TiXmlElement *product_list = new TiXmlElement("product_list");    //创建一个根结点 product_list
		product_list->SetAttribute("count", "3");                         //创建product_list 属性
		doc->LinkEndChild(product_list);	//链接到上一节点下，下同

		TiXmlElement* productL2A = new TiXmlElement("product");  //创建一个二级结点 product
		product_list->LinkEndChild(productL2A);
		productL2A->SetAttribute("index", "1");             //创建属性1
		productL2A->SetAttribute("File_Type", "CZI_L2A");   //创建属性2

		TiXmlElement* fileL2A = new TiXmlElement("file");  //创建一个二级结点 file
		productL2A->LinkEndChild(fileL2A);
		TiXmlText *fileTextL2A = new TiXmlText(L2AH5Path);
		fileL2A->LinkEndChild(fileTextL2A);

		fileL2A = new TiXmlElement("file");
		productL2A->LinkEndChild(fileL2A);
		fileTextL2A = new TiXmlText(L2AMatePath);
		fileL2A->LinkEndChild(fileTextL2A);

		fileL2A = new TiXmlElement("file");
		productL2A->LinkEndChild(fileL2A);
		fileTextL2A = new TiXmlText(JpgBluePath);
		fileL2A->LinkEndChild(fileTextL2A);

		fileL2A = new TiXmlElement("file");
		productL2A->LinkEndChild(fileL2A);
		fileTextL2A = new TiXmlText(JpgGreenPath);
		fileL2A->LinkEndChild(fileTextL2A);

		fileL2A = new TiXmlElement("file");
		productL2A->LinkEndChild(fileL2A);
		fileTextL2A = new TiXmlText(JpgRedPath);
		fileL2A->LinkEndChild(fileTextL2A);

		fileL2A = new TiXmlElement("file");
		productL2A->LinkEndChild(fileL2A);
		fileTextL2A = new TiXmlText(JpgNirPath);
		fileL2A->LinkEndChild(fileTextL2A);

	//    fileL2A = new TiXmlElement("file");  //创建一个二级结点 file
	//    productL2A->LinkEndChild(fileL2A);
	//    fileTextL2A = new TiXmlText(JpgbluePath);
	//    fileL2A->LinkEndChild(fileTextL2A);
	//    fileL2A = new TiXmlElement("file");  //创建一个二级结点 file
	//    productL2A->LinkEndChild(fileL2A);
	//    fileTextL2A = new TiXmlText(JpggreenPath);
	//    fileL2A->LinkEndChild(fileTextL2A);

		TiXmlElement* productL2B = new TiXmlElement("product");
		product_list->LinkEndChild(productL2B);
		productL2B->SetAttribute("index", "2");             //创建属性1
		productL2B->SetAttribute("File_Type", "CZI_L2B");   //创建属性2

		TiXmlElement* fileL2B = new TiXmlElement("file");  //创建一个二级结点 file
		productL2B->LinkEndChild(fileL2B);
		TiXmlText* fileTextL2B = new TiXmlText(L2BH5Path);
		fileL2B->LinkEndChild(fileTextL2B);

		fileL2B = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2B->LinkEndChild(fileL2B);
		fileTextL2B = new TiXmlText(JpgSSCPath);
		fileL2B->LinkEndChild(fileTextL2B);

		fileL2B = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2B->LinkEndChild(fileL2B);
		fileTextL2B = new TiXmlText(JpgNDVIPath);
		fileL2B->LinkEndChild(fileTextL2B);

		fileL2B = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2B->LinkEndChild(fileL2B);
		fileTextL2B = new TiXmlText(L2BH5MatePath);
		fileL2B->LinkEndChild(fileTextL2B);

		TiXmlElement* productL2C = new TiXmlElement("product");  //创建一个二级结点 product
		product_list->LinkEndChild(productL2C);
		productL2C->SetAttribute("index", "3");             //创建属性1
		productL2C->SetAttribute("File_Type", "CZI_L2C");   //创建属性2

		TiXmlElement* fileL2C = new TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		TiXmlText* fileTextL2C = new TiXmlText(L2CH5Path);	//
		fileL2C->LinkEndChild(fileTextL2C);

		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		fileTextL2C = new TiXmlText(L2CH5MatePath);
		fileL2C->LinkEndChild(fileTextL2C);

		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		fileTextL2C = new TiXmlText(JpgChlPath);
		fileL2C->LinkEndChild(fileTextL2C);

		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		fileTextL2C = new TiXmlText(JpgSDDPath);
		fileL2C->LinkEndChild(fileTextL2C);
//--------------------------------------------------------------------
		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		fileTextL2C = new TiXmlText(L2CJpgBluePath);
		fileL2C->LinkEndChild(fileTextL2C);

		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
		productL2C->LinkEndChild(fileL2C);
		fileTextL2C = new TiXmlText(L2CJpgGreenPath);
		fileL2C->LinkEndChild(fileTextL2C);

//		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
//		productL2C->LinkEndChild(fileL2C);
//		fileTextL2C = new TiXmlText(L2CJpgRedPath);
//		fileL2C->LinkEndChild(fileTextL2C);
//
//		fileL2C = new  TiXmlElement("file");  //创建一个二级结点 file
//		productL2C->LinkEndChild(fileL2C);
//		fileTextL2C = new TiXmlText(L2CJpgNirPath);
//		fileL2C->LinkEndChild(fileTextL2C);

		doc->SaveFile(PATH_Xml);      //保存到文件

		delete doc;
	}catch(exception &e){
		return -1;
	}
    return 0;
}
int ReportXmlWrite(const char* PATH_Report, char* startTime, char* stopTime, char** InputList, char** OutputList, char* ErrorInfo){
	try{
		TiXmlDocument *doc = new TiXmlDocument();                         //创建xml文档对象

		TiXmlElement *product_list = new TiXmlElement("Report");    //创建一个根结点 product_list
		doc->LinkEndChild(product_list);	//链接到上一节点下，下同

		TiXmlElement* StartTime = new TiXmlElement("ProcessingStartTime");  //创建一个二级结点 file
		product_list->LinkEndChild(StartTime);
		TiXmlText *starttime = new TiXmlText(startTime);
		StartTime->LinkEndChild(starttime);

		TiXmlElement* StopTime = new TiXmlElement("ProcessingStopTime");  //创建一个二级结点 file
		product_list->LinkEndChild(StopTime);
		TiXmlText *stoptime = new TiXmlText(stopTime);
		StopTime->LinkEndChild(stoptime);

		TiXmlElement* InPutFileList = new TiXmlElement("InputList");  //创建一个二级结点 file
		InPutFileList->SetAttribute("count", "2");             //创建属性1
		product_list->LinkEndChild(InPutFileList);
		TiXmlElement* L1BFilePath = new TiXmlElement("H5InputFile");  //创建一个二级结点 file
		InPutFileList->LinkEndChild(L1BFilePath);
		TiXmlText *InPutL1BFile = new TiXmlText(InputList[0]);
		L1BFilePath->LinkEndChild(InPutL1BFile);
		TiXmlElement* AuxFilePath = new TiXmlElement("AuxInputPath");  //创建一个二级结点 file
		InPutFileList->LinkEndChild(AuxFilePath);
		TiXmlText *InPutAuxFile = new TiXmlText(InputList[1]);
		AuxFilePath->LinkEndChild(InPutAuxFile);

		TiXmlElement* outPutFileList = new TiXmlElement("OutputList");  //创建一个二级结点 file
		outPutFileList->SetAttribute("count", "3");             //创建属性1
		product_list->LinkEndChild(outPutFileList);
		TiXmlElement* L2AFilePath = new TiXmlElement("L2AH5OutputFile");  //创建一个二级结点 file
		outPutFileList->LinkEndChild(L2AFilePath);
		TiXmlText *InPutL2AFile = new TiXmlText(OutputList[0]);
		L2AFilePath->LinkEndChild(InPutL2AFile);
		TiXmlElement* L2BFilePath = new TiXmlElement("L2BH5OutputFile");  //创建一个二级结点 file
		outPutFileList->LinkEndChild(L2BFilePath);
		TiXmlText *InPutL2BFile = new TiXmlText(OutputList[1]);
		L2BFilePath->LinkEndChild(InPutL2BFile);
		TiXmlElement* L2CFilePath = new TiXmlElement("L2CH5OutputFile");  //创建一个二级结点 file
		outPutFileList->LinkEndChild(L2CFilePath);
		TiXmlText *InPutL2CFile = new TiXmlText(OutputList[2]);
		L2CFilePath->LinkEndChild(InPutL2CFile);

		TiXmlElement* ErrInfo = new TiXmlElement("ErrorInfo");  //创建一个二级结点 file
		product_list->LinkEndChild(ErrInfo);
		TiXmlText *errorinfo = new TiXmlText(ErrorInfo);
		ErrInfo->LinkEndChild(errorinfo);

		doc->SaveFile(PATH_Report);

		delete doc;
	}catch(exception &e){
		return -1;
	}
    //保存到文件
    return 0;
}

struct MyConfig ReadMyConfig(const char* myConfigName){

	struct MyConfig Config_Parameter;	//初始化存放xml参数的结构体
	Config_Parameter.Flag = 0;// No Error

	try{
		//--------------------------------------------ProductMeta-------------------------------------------------------

		TiXmlDocument mydoc(myConfigName); //xml文档对象
		bool loadOk=mydoc.LoadFile(); //加载文档
		if(!loadOk)
		{
		//cout<<"could not load the xml file.Error:"<<mydoc.ErrorDesc()<<endl;
		//exit(-1);
		Config_Parameter.Flag = -1;
		Config_Parameter.ErrorInfo = "Error: could not load the CNF_OWN__hy1_czi_l2_pro.xml!";
		}

		TiXmlElement *RootElement=mydoc.RootElement();	//根元素************************
		if (!(strcmp("ConfigureParameter", RootElement->Value()) == 0))
		{
		Config_Parameter.Flag = -1;
		Config_Parameter.ErrorInfo = "Error: CNF_OWN__hy1_czi_l2_pro.xml root element is not true!";
		}

		TiXmlElement *CloudThreshold = RootElement->FirstChildElement("CloudThreshold");	//SatelliteName*************************
		const char* CloudThreshold_char = CloudThreshold->GetText();
		strcpy(Config_Parameter.CloudThreshold , CloudThreshold_char);

		TiXmlElement *ShadowThreshold = RootElement->FirstChildElement("ShadowThreshold");
		const char* ShadowThreshold_char = ShadowThreshold->GetText();
		strcpy(Config_Parameter.ShadowThreshold , ShadowThreshold_char);

		TiXmlElement *MedianFilterSize = RootElement->FirstChildElement("MedianFilterSize");
		const char* MedianFilterSize_char = MedianFilterSize->GetText();
		strcpy(Config_Parameter.MedianFilterSize , MedianFilterSize_char);

		TiXmlElement *NDVIThreshold = RootElement->FirstChildElement("NDVIThreshold");
		const char* NDVIThreshold_char = NDVIThreshold->GetText();
		strcpy(Config_Parameter.NDVIThreshold , NDVIThreshold_char);

		TiXmlElement *NDWIThreshold = RootElement->FirstChildElement("NDWIThreshold");
		const char* NDWIThreshold_char = NDWIThreshold->GetText();
		strcpy(Config_Parameter.NDWIThreshold , NDWIThreshold_char);

		TiXmlElement *GuassFilteSize = RootElement->FirstChildElement("GuassFilteSize");
		const char* GuassFilteSize_char = GuassFilteSize->GetText();
		strcpy(Config_Parameter.GuassFilteSize ,GuassFilteSize_char);

		TiXmlElement *AtmosphCorrecteMethod = RootElement->FirstChildElement("AtmosphCorrecteMethod");
		const char* AtmosphCorrecteMethod_char = AtmosphCorrecteMethod->GetText();
		strcpy(Config_Parameter.AtmosphCorrecteMethod , AtmosphCorrecteMethod_char);

		TiXmlElement *NCEPPathName = RootElement->FirstChildElement("NCEPPathName");
		const char* NCEPPathName_char = NCEPPathName->GetText();
		strcpy(Config_Parameter.NCEPPathName ,NCEPPathName_char);

		TiXmlElement *NCEPClimePathName = RootElement->FirstChildElement("NCEPClimePathName");
		const char* NCEPClimePathName_char = NCEPClimePathName->GetText();
		strcpy(Config_Parameter.NCEPClimePathName ,NCEPClimePathName_char);

		TiXmlElement *COCTSPathName = RootElement->FirstChildElement("COCTSPathName");
		const char* COCTSPathName_char = COCTSPathName->GetText();
		strcpy(Config_Parameter.COCTSPathName ,COCTSPathName_char);

//		TiXmlElement *AuxDataPath = RootElement->FirstChildElement("AuxDataPath");
//		const char* AuxDataPath_char = AuxDataPath->GetText();
//		strcpy(Config_Parameter.AuxDataPath ,AuxDataPath_char);

		TiXmlElement *SSCCamParam1 = RootElement->FirstChildElement("SSCCamParam1");
		const char* SSCCamParam1_char = SSCCamParam1->GetText();
		strcpy(Config_Parameter.SSCCamParam1 ,SSCCamParam1_char);

		TiXmlElement *SSCCamParam2= RootElement->FirstChildElement("SSCCamParam2");
		const char* SSCCamParam2_char = SSCCamParam2->GetText();
		strcpy(Config_Parameter.SSCCamParam2 ,SSCCamParam2_char);

		TiXmlElement *SSCCamParam3 = RootElement->FirstChildElement("SSCCamParam3");
		const char* SSCCamParam3_char = SSCCamParam3->GetText();
		strcpy(Config_Parameter.SSCCamParam3 ,SSCCamParam3_char);

		TiXmlElement *SSCCamParam4 = RootElement->FirstChildElement("SSCCamParam4");
		const char* SSCCamParam4_char = SSCCamParam4->GetText();
		strcpy(Config_Parameter.SSCCamParam4 ,SSCCamParam4_char);

		TiXmlElement *CHLCamParam1 = RootElement->FirstChildElement("CHLCamParam1");
		const char* CHLCamParam1_char = CHLCamParam1->GetText();
		strcpy(Config_Parameter.CHLCamParam1 ,CHLCamParam1_char);

		TiXmlElement *CHLCamParam2 = RootElement->FirstChildElement("CHLCamParam2");
		const char* CHLCamParam2_char = CHLCamParam2->GetText();
		strcpy(Config_Parameter.CHLCamParam2 ,CHLCamParam2_char);

		TiXmlElement *CHLCamParam3 = RootElement->FirstChildElement("CHLCamParam3");
		const char* CHLCamParam3_char = CHLCamParam3->GetText();
		strcpy(Config_Parameter.CHLCamParam3 ,CHLCamParam3_char);

		TiXmlElement *CHLCamParam4 = RootElement->FirstChildElement("CHLCamParam4");
		const char* CHLCamParam4_char = CHLCamParam4->GetText();
		strcpy(Config_Parameter.CHLCamParam4 ,CHLCamParam4_char);

		TiXmlElement *CHLCamParam5 = RootElement->FirstChildElement("CHLCamParam5");
		const char* CHLCamParam5_char = CHLCamParam5->GetText();
		strcpy(Config_Parameter.CHLCamParam5 ,CHLCamParam5_char);

		TiXmlElement *SDDCamParam1 = RootElement->FirstChildElement("SDDCamParam1");
		const char* SDDCamParam1_char = SDDCamParam1->GetText();
		strcpy(Config_Parameter.SDDCamParam1 ,SDDCamParam1_char);

		TiXmlElement *SDDCamParam2 = RootElement->FirstChildElement("SDDCamParam2");
		const char* SDDCamParam2_char = SDDCamParam2->GetText();
		strcpy(Config_Parameter.SDDCamParam2 ,SDDCamParam2_char);

		TiXmlElement *SDDCamParam3 = RootElement->FirstChildElement("SDDCamParam3");
		const char* SDDCamParam3_char = SDDCamParam3->GetText();
		strcpy(Config_Parameter.SDDCamParam3 ,SDDCamParam3_char);

	}catch(exception &e){
		exit(-1);
	}
//	cout<<Config_Parameter.ProductionLevel<<"     ";
//	cout<<Config_Parameter.StartDate<<endl;
	return Config_Parameter;
}
