/*
 * Basic_Operation.c
 *
 *  Created on: 2017年10月20日
 *      Author: zhongyong
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"hdf5.h"
#include"Basic_Operation.h"


/* 报错机制，关闭不存在组，数据集，属性时出现的错误**/
void CloseErrorMechanism(hid_t openID,char* NAME);

//打开文件，创建文件，删除文件
hid_t openFile(char* FILE_PATH)
{
//	hid_t open_ID=H5Fopen(FILE_PATH,H5F_ACC_RDWR,H5P_DEFAULT);
	hid_t open_ID=H5Fopen(FILE_PATH,H5F_ACC_RDONLY,H5P_DEFAULT);
	CloseErrorMechanism(open_ID,FILE_PATH);
	return open_ID;
}
hid_t createFile(char* FILE_PATH){
	hid_t create_ID=H5Fcreate(FILE_PATH,H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);
	CloseErrorMechanism(create_ID,FILE_PATH);
	return create_ID;
}
void delFile(char* FILE_PATH){

}

//打开组，创建组(创建组时要一层一层的创建），删除组
hid_t openGroup(hid_t FILE_ID,char* GROUP_PATH){
	hid_t open_ID=H5Gopen2(FILE_ID,GROUP_PATH,H5P_DEFAULT);
	CloseErrorMechanism(open_ID,GROUP_PATH);
	return open_ID;
}
hid_t createGroup(hid_t FILE_ID,char* GROUP_PATH){
	hid_t create_ID=H5Gcreate2(FILE_ID,GROUP_PATH,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
	CloseErrorMechanism(create_ID,GROUP_PATH);
	return create_ID;
}
void delGroup(hid_t FILE_ID,char* GROUP_PATH){

}

//打开数据集，创建数据集，删除数据集
hid_t openDataSet(hid_t FILE_ID,char* DATASET_PATH){
	hid_t open_ID=H5Dopen2(FILE_ID,DATASET_PATH,H5P_DEFAULT);
	CloseErrorMechanism(FILE_ID,DATASET_PATH);
	return open_ID;
}
hid_t createDataSet(hid_t FILE_ID,char* DATASET_PATH,hid_t DATASPACE,hid_t NATIVE_INT_DOUBLE){
	hid_t create_ID=H5Dcreate2(FILE_ID,DATASET_PATH,NATIVE_INT_DOUBLE,DATASPACE,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
	CloseErrorMechanism(FILE_ID,DATASET_PATH);
	return create_ID;
}
void delDataSet(hid_t FILE_ID,char* DATASET_PATH){

}

/* 报错机制，关闭不存在组，数据集，属性时出现的错误**/
void CloseErrorMechanism(hid_t openID,char* NAME){
	herr_t status=H5Eset_auto(H5E_DEFAULT,NULL,NULL);
	status=H5Lget_info(openID,NAME,NULL,H5P_DEFAULT);
}

// judge the file exist ,如果返回0的话就是文件存在
#include <unistd.h>
int FileIsExist(const char *pathname){
	int fileId = access(pathname,0);
	return fileId;
}

long getFileSize(char* FileName){
    FILE * pFile;
    long size;

    pFile = fopen (FileName,"rb");
    if (pFile==NULL)
          perror ("Error opening file");
    else
    {
          fseek (pFile, 0, SEEK_END);   ///将文件指针移动文件结尾
          size=ftell (pFile); ///求出当前文件指针距离文件开始的字节数
          fclose (pFile);
//          printf ("Size of file.cpp: %ld bytes.\n",size);
    }
    return size;
}


#include "mfhdf.h"    /* ...since mfhdf.h already includes hdf.h */
// hdf4 read return.
int openhdf4(char* FILE_NAME){

	int32 sd_id = SDstart (FILE_NAME, DFACC_READ);
    if (sd_id == -1) {
    	return -1;
    }
    return 0;
}

int getDay(int Year, int Month, int Day){ //儒略日
	int addDay = 0;
	if(Year % 4 == 0 && Year % 100 != 0 || Year % 400 == 0){
		addDay = 1;
	}
	int DayCount = 0;
	if(Month > 1)   DayCount = DayCount + 31;
	if(Month > 2)   DayCount = DayCount + 28 + addDay;
	if(Month > 3)   DayCount = DayCount + 31;
	if(Month > 4)   DayCount = DayCount + 30;
	if(Month > 5)   DayCount = DayCount + 31;
	if(Month > 6)   DayCount = DayCount + 30;
	if(Month > 7)   DayCount = DayCount + 31;
	if(Month > 8)   DayCount = DayCount + 31;
	if(Month > 9)   DayCount = DayCount + 30;
	if(Month > 10)   DayCount = DayCount + 31;
	if(Month > 11)   DayCount = DayCount + 30;
//	if(Month > 12)   DayCount = DayCount + 31;

	DayCount = DayCount + Day;


	return DayCount;
}


