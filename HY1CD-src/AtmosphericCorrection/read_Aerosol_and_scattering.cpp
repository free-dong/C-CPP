/*
 *  Created on: 2018年5月21日
 *  	Author: zhangyonghui
 */
#include "../hy1_czi_l2_pro.hpp"
#include<opencv2/opencv.hpp>
#include<iostream>
//#include<math.h>
#include"hdf5.h"
#include"Linear_Interpolation.h"
#include"read_txt.h"
#include"read_Aerosol_and_scattering.h"
#include"read_txt.h"
extern "C"{
	#include"../HDF_STRUCT.h"
	#include"../BasicOperation/Basic_Operation.h"
}
#include<cstring>
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array
using namespace std;
using namespace cv;


char* getName(char* PATH_GROUP,const char* data_Name){
	int length=strlen(data_Name)+2+strlen(PATH_GROUP);
	char *dataName=(char* )malloc(sizeof(char)*length);
	strcpy(dataName,PATH_GROUP);
	strcat(dataName,"/");
	strcat(dataName,data_Name);
	dataName[length-1]='\0';
	return dataName;
}

/*
 * read_aeo_txt()
 * 读取文件名头缀为aeo的txt文件，共四个
 * 返回三维指针
 */
float*** read_aeo_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand)
{
	int TableTxt = FileIsExist(PATH);
		if(TableTxt != 0)
		{
//			cout<<"Search table File Isn't Exist,it's exiting..."<<endl;
		}
	char* Red_Name = getName(PATH, folder);//气绒胶和散射下11个文件夹的路径

	//文件夹下的txt文件路径
	char* p_aeo_B1 = getName(Red_Name, "aeo_B1.txt");
	char* p_aeo_B2 = getName(Red_Name, "aeo_B2.txt");
	char* p_aeo_B3 = getName(Red_Name, "aeo_B3.txt");
	char* p_aeo_B4 = getName(Red_Name, "aeo_B4.txt");

	//read data
	float **aeo_B1 = read_txt(p_aeo_B1, height, width, _poExHand);
	float **aeo_B2 = read_txt(p_aeo_B2, height, width, _poExHand);
	float **aeo_B3 = read_txt(p_aeo_B3, height, width, _poExHand);
	float **aeo_B4 = read_txt(p_aeo_B4, height, width, _poExHand);



	//初始化三维数组point
	float*** point_aeo = new float**[4];
//	for(int i = 0; i < 4; i++)
//	{
//		point_aeo[i] = new float*[height];
//		for(int j = 0; j < height; j++)
//		{
//			point_aeo[i][j] = new float[width];
//		}
//	}

	//用三维指针指向这四个二维指针指向的地址
	point_aeo[0] = aeo_B1;
	point_aeo[1] = aeo_B2;
	point_aeo[2] = aeo_B3;
	point_aeo[3] = aeo_B4;

	// free
	xfree(Red_Name);

	xfree(p_aeo_B1);
	xfree(p_aeo_B2);
	xfree(p_aeo_B3);
	xfree(p_aeo_B4);

	return point_aeo;
}

/*
 * read_delta_txt()
 * 读取文件名为delta的txt文件，共一个
 * 返回二维指针
 */
float** read_delta_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand)
{
	int TableTxt = FileIsExist(PATH);
		if(TableTxt != 0)
		{
//			cout<<"Search table File Isn't Exist,it's exiting..."<<endl;
		}
	char* Red_Name = getName(PATH, folder);//气绒胶和散射下11个文件夹的路径

	char* p_delta = getName(Red_Name, "delta.txt");//文件夹下的txt文件路径

	//read data
	float **delta = read_txt(p_delta, height, width,_poExHand);

	xfree(Red_Name);
	xfree(p_delta);

	return delta;
}

/*
 * read_tra_txt()
 * 读取文件名头缀为tra的txt文件，共八个
 * 返回三维指针
 */
float*** read_tra_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand)
{
	char* Red_Name = getName(PATH, folder);//气绒胶和散射下11个文件夹的路径

	//文件夹下的txt文件路径
	char* p_tra_doB1 = getName(Red_Name, "tra_doB1.txt");
	char* p_tra_doB2 = getName(Red_Name, "tra_doB2.txt");
	char* p_tra_doB3 = getName(Red_Name, "tra_doB3.txt");
	char* p_tra_doB4 = getName(Red_Name, "tra_doB4.txt");
	char* p_tra_upB1 = getName(Red_Name, "tra_upB1.txt");
	char* p_tra_upB2 = getName(Red_Name, "tra_upB2.txt");
	char* p_tra_upB3 = getName(Red_Name, "tra_upB3.txt");
	char* p_tra_upB4 = getName(Red_Name, "tra_upB4.txt");


	//read data
	float **tra_doB1 = read_txt(p_tra_doB1, height, width,_poExHand);
	float **tra_doB2 = read_txt(p_tra_doB2, height, width,_poExHand);
	float **tra_doB3 = read_txt(p_tra_doB3, height, width,_poExHand);
	float **tra_doB4 = read_txt(p_tra_doB4, height, width,_poExHand);
	float **tra_upB1 = read_txt(p_tra_upB1, height, width,_poExHand);
	float **tra_upB2 = read_txt(p_tra_upB2, height, width,_poExHand);
	float **tra_upB3 = read_txt(p_tra_upB3, height, width,_poExHand);
	float **tra_upB4 = read_txt(p_tra_upB4, height, width,_poExHand);

	//初始化三维数组point
	float*** point_aeo = new float**[8];
//	for(int i = 0; i < 8; i++)
//	{
//		point_aeo[i] = new float*[height];
//		for(int j = 0; j < height; j++)
//		{
//			point_aeo[i][j] = new float[width];
//		}
//	}

	//用三维指针指向这四个二维指针指向的地址
	point_aeo[0] = tra_doB1;
	point_aeo[1] = tra_doB2;
	point_aeo[2] = tra_doB3;
	point_aeo[3] = tra_doB4;
	point_aeo[4] = tra_upB1;
	point_aeo[5] = tra_upB2;
	point_aeo[6] = tra_upB3;
	point_aeo[7] = tra_upB4;

	xfree(Red_Name);

	xfree(p_tra_doB1);
	xfree(p_tra_doB2);
	xfree(p_tra_doB3);
	xfree(p_tra_doB4);
	xfree(p_tra_upB1);
	xfree(p_tra_upB2);
	xfree(p_tra_upB3);
	xfree(p_tra_upB4);

	return point_aeo;
}
