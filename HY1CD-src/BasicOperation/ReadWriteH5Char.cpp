/*
 * ReadWriteH5Char.cpp
 *
 *  Created on: Aug 27, 2018
 *      Author: zhongyong
 */

#include <iostream>
#include <vector>
#include <string.h>
#include "H5Fpublic.h"
#include "hdf5.h"
#include "hdf5_hl.h"
#include"H5LTpublic.h"
#include"../HDF_STRUCT.h"
extern "C"{
	#include"Basic_Operation.h"
}
using namespace std;
//#include <vector>
//#include <string.h>
//#include "H5Fpublic.h"
//#include "hdf5.h"
//#include "hdf5_hl.h"
/* 报错机制，关闭不存在组，数据集，属性时出现的错误**/
void CloseErrorMechanism(hid_t openID,const char* NAME){
	herr_t status=H5Eset_auto(H5E_DEFAULT,NULL,NULL);
	status=H5Lget_info(openID,NAME,NULL,H5P_DEFAULT);
}
struct HDF5_CHAR ReadH5Char(const char* InputPath, char *groupName, char *childName){
//    const char* readH5File = "testWriteh5String.h5"; // 文件在可执行目录下

    struct HDF5_CHAR Data;
    Data.Flag = 0;
    Data.ERRORINFO = "";
    Data.Datachar = "";
    Data.RANK = 1;
    Data.Height = 1;
    Data.Width = 1;
    Data.nsize = 1;

    hid_t hdfId = H5Fopen(InputPath, H5F_ACC_RDONLY, H5P_DEFAULT); // hdfId文件句柄
    CloseErrorMechanism(hdfId,InputPath);
    if(hdfId<0){
    	Data.Flag = -1;
        return Data;
    }

    int ndims = 0;
    herr_t herr;
    int row, col = 1;
    hid_t groupid = H5Gopen2(hdfId, groupName,H5P_DEFAULT); // 打开文件
    CloseErrorMechanism(groupid,InputPath);
    if(groupid<0)
    {
        H5Fclose(hdfId);
        Data.Flag = -1;
        return Data;
    }
    herr = H5LTget_dataset_ndims(groupid, childName, &ndims); // 获得维度
    hsize_t dims[ndims];
    size_t nsz;
    herr = H5LTget_dataset_info(groupid, childName, dims, NULL, &nsz);
    if(herr<0)
    {
        H5Fclose(hdfId);
        Data.Flag = -1;
        return Data;
    }


    if(ndims == 2)
    {
        row = dims[0];
        col = dims[1];
    }else{
    	row = dims[0];
    }

    int size = nsz; // 单个字符串长度
    char *checkedTime = new char[row*size]; // 分配数据总长度空间
    herr= H5LTread_dataset_string(groupid, childName, checkedTime); // 读取char*数据
    if(herr<0)
    {
        H5Fclose(hdfId);
        Data.Flag = -1;
        return Data;
    }

    Data.Datachar = checkedTime;
    Data.RANK = ndims;
    Data.Height = row;
    Data.Width = 1;
    Data.nsize = nsz;
//    cout << "read success!" << endl;

    return Data;
}


int WriteH5Char(const char* OutputPath,struct HDF5_CHAR Data,char* GroupName, char* DataName){

    // 写数据
    int rank = Data.RANK; // 写入维度
    int width = Data.nsize; //
//    const char *childStr = "/ROOTCHILD1";
    int length = Data.Height; // 写入一个char*的长度
    string tempStr = "";
    tempStr = Data.Datachar;

    hid_t space,atype;
    herr_t  status;
    hsize_t fdim[] = {(hsize_t)length};

     space = H5Screate_simple(rank,fdim,NULL);
     if(space<0)
         return -1;
     atype= H5Tcopy(H5T_C_S1);
     status=H5Tset_size(atype,width);
     if(status<0)
           return -1;
     hid_t hdfId,dataset;
     hdfId = H5Fcreate(OutputPath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

     if(hdfId<0)
         return -1;

     dataset = H5Dcreate1(hdfId,DataName,atype,space,H5P_DEFAULT);
     if(dataset<0)
           return -1;
     status=H5Dwrite(dataset,atype,H5S_ALL,H5S_ALL,H5P_DEFAULT,tempStr.c_str());
     if(status<0)
           return -1;
     H5Sclose(space);
     H5Dclose(dataset);
     H5Tclose(atype);

     // 关闭
     herr_t herr;
     herr=H5Fclose(hdfId);
     if(herr<0)
         return -2;


//    cout << "write success!" << endl;
    return 0;
}

