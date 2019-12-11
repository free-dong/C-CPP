/*
 * Basic_Operation.h
 *
 *  Created on: 2017年10月20日
 *      Author: zhongyong
 */

#ifndef BASIC_OPERATION_H_
#define BASIC_OPERATION_H_

#include"hdf5.h"

//打开文件，创建文件，删除文件
hid_t openFile(char* FILE_PATH);
hid_t createFile(char* FILE_PATH);
void delFile(char* FILE_PATH);

//打开组，创建组，删除组
hid_t openGroup(hid_t FILE_ID,char* GROUP_PATH);
hid_t createGroup(hid_t FILE_ID,char* GROUP_PATH);
void delGroup(hid_t FILE_ID,char* GROUP_PATH);

//打开数据集，创建数据集，删除数据集
hid_t openDataSet(hid_t FILE_ID,char* DATASET_PATH);
hid_t createDataSet(hid_t FILE_ID,char* DATASET_PATH,hid_t DATASPACE,hid_t NATIVE_INT_DOUBLE);
void delDataSet(hid_t FILE_ID,char* DATASET_PATH);

char* getLogFileName(const char* name,char* log_path);
int FileIsExist(const char *pathname);

long getFileSize(char* FileName);
int openhdf4(char* FILE_NAME);
int getDay(int Year, int Month, int Day);

#endif /* BASIC_OPERATION_H_ */
