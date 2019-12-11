/*
 *  Created on: 2018年5月16日
 *    modified: 2018-08-13
 *  	Author: feidong
 */

#include <iostream>
#include <cmath>
#include "../hy1_czi_l2_pro.hpp"
////extern "C"{
//	#include"../BasicOperation/Read_Data_Attr_Xml.h"
////}
using namespace std;
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array
#define xfree(x) if(x != NULL) {free(x); x = NULL;}  //release array

float** Linear_Interpolation_1D(int* pIndex_a, float*** IndexTable, float soz, float sez,CGEN_ExecutionHandler *_poExHand)
//	1维线性插值*********************************************************************************
{
	int tra1 = pIndex_a[0];
	int tra2 = pIndex_a[1];
	// 波段1-----------------------------------------------------------------------------------

	float a0D_1 = IndexTable[0][tra1-1][1];
	float a1D_1 = IndexTable[0][tra1][1];
	float b0D_1 = IndexTable[0][tra1-1][2];
	float b1D_1 = IndexTable[0][tra1][2];

	float a0U_1 = IndexTable[4][tra2-1][1];
	float a1U_1 = IndexTable[4][tra2][1];
	float b0U_1 = IndexTable[4][tra2-1][2];
	float b1U_1 = IndexTable[4][tra2][2];

	float kD_B1 = (soz - IndexTable[0][tra1-1][0]) / (IndexTable[0][tra1][0] - IndexTable[0][tra1-1][0]);
	float kU_B1 = (soz - IndexTable[4][tra2-1][0]) / (IndexTable[4][tra2][0] - IndexTable[4][tra2-1][0]);

	float tra_B1D_a = a0D_1 * (1-kD_B1) + a1D_1 * kD_B1;
	float tra_B1D_b = b0D_1 * (1-kD_B1) + b1D_1 * kD_B1;
	float tra_B1U_a = a0U_1 * (1-kU_B1) + a1U_1 * kU_B1;
	float tra_B1U_b = b0U_1 * (1-kU_B1) + b1U_1 * kU_B1;

	// 波段2-----------------------------------------------------------------------------------
	float a0D_2 = IndexTable[1][tra1-1][1];
	float a1D_2 = IndexTable[1][tra1][1];
	float b0D_2 = IndexTable[1][tra1-1][2];
	float b1D_2 = IndexTable[1][tra1][2];

	float a0U_2 = IndexTable[5][tra2-1][1];
	float a1U_2 = IndexTable[5][tra2][1];
	float b0U_2 = IndexTable[5][tra2-1][2];
	float b1U_2 = IndexTable[5][tra2][2];

	float kD_B2 = (soz - IndexTable[1][tra1-1][0]) / (IndexTable[1][tra1][0] - IndexTable[1][tra1-1][0]);
	float kU_B2 = (soz - IndexTable[5][tra2-1][0]) / (IndexTable[5][tra2][0] - IndexTable[5][tra2-1][0]);

	float tra_B2D_a = a0D_2 * (1-kD_B2) + a1D_2 * kD_B2;
	float tra_B2D_b = b0D_2 * (1-kD_B2) + b1D_2 * kD_B2;
	float tra_B2U_a = a0U_2 * (1-kU_B2) + a1U_2 * kU_B2;
	float tra_B2U_b = b0U_2 * (1-kU_B2) + b1U_2 * kU_B2;

	// 波段3-----------------------------------------------------------------------------------
	float a0D_3 = IndexTable[2][tra1-1][1];
	float a1D_3 = IndexTable[2][tra1][1];
	float b0D_3 = IndexTable[2][tra1-1][2];
	float b1D_3 = IndexTable[2][tra1][2];

	float a0U_3 = IndexTable[6][tra2-1][1];
	float a1U_3 = IndexTable[6][tra2][1];
	float b0U_3 = IndexTable[6][tra2-1][2];
	float b1U_3 = IndexTable[6][tra2][2];

	float kD_B3 = (soz - IndexTable[2][tra1-1][0]) / (IndexTable[2][tra1][0] - IndexTable[2][tra1-1][0]);
	float kU_B3 = (soz - IndexTable[6][tra2-1][0]) / (IndexTable[6][tra2][0] - IndexTable[6][tra2-1][0]);

	float tra_B3D_a = a0D_3 * (1-kD_B3) + a1D_3 * kD_B3;
	float tra_B3D_b = b0D_3 * (1-kD_B3) + b1D_3 * kD_B3;
	float tra_B3U_a = a0U_3 * (1-kU_B3) + a1U_3 * kU_B3;
	float tra_B3U_b = b0U_3 * (1-kU_B3) + b1U_3 * kU_B3;


	// 波段4-----------------------------------------------------------------------------------
	float a0D_4 = IndexTable[3][tra1-1][1];
	float a1D_4 = IndexTable[3][tra1][1];
	float b0D_4 = IndexTable[3][tra1-1][2];
	float b1D_4 = IndexTable[3][tra1][2];

	float a0U_4 = IndexTable[7][tra2-1][1];
	float a1U_4 = IndexTable[7][tra2][1];
	float b0U_4 = IndexTable[7][tra2-1][2];
	float b1U_4 = IndexTable[7][tra2][2];

	float kD_B4 = (soz - IndexTable[3][tra1-1][0]) / (IndexTable[3][tra1][0] - IndexTable[3][tra1-1][0]);
	float kU_B4 = (soz - IndexTable[7][tra2-1][0]) / (IndexTable[7][tra2][0] - IndexTable[7][tra2-1][0]);

	float tra_B4D_a = a0D_4 * (1-kD_B4) + a1D_4 * kD_B4;
	float tra_B4D_b = b0D_4 * (1-kD_B4) + b1D_4 * kD_B4;
	float tra_B4U_a = a0U_4 * (1-kU_B4) + a1U_4 * kU_B4;
	float tra_B4U_b = b0U_4 * (1-kU_B4) + b1U_4 * kU_B4;

	// 返回值-----------------------------------------------------------------------------------
	float** tra = new float*[4];
	for (int i = 0; i < 4; i++)
	{
		tra[i] = new float[4];
		if(tra == NULL)
		{
			{
				_poExHand->sendInfoMessage("Linear_Interpolation_1D's point memory application failed!", CLOG_NO_PRODUCT);
				exit(0);
			}
		}
	}
	//返回第一个波段和第二个波段的rho
	tra[0][0] = tra_B1D_a;	//r_0_downB1
	tra[0][1] = tra_B1D_b;	//r_1_downB1
	tra[0][2] = tra_B1U_a;	//r_0_upB1
	tra[0][3] = tra_B1U_b;	//r_1_upB1
	tra[1][0] = tra_B2D_a;	//r_0_downB2
	tra[1][1] = tra_B2D_b;	//r_1_downB2
	tra[1][2] = tra_B2U_a;	//r_0_upB2
	tra[1][3] = tra_B2U_b;	//r_1_upB2

	tra[2][0] = tra_B3D_a;	//
	tra[2][1] = tra_B3D_b;	//
	tra[2][2] = tra_B3U_a;	//
	tra[2][3] = tra_B3U_b;	//

	tra[3][0] = tra_B4D_a;	//
	tra[3][1] = tra_B4D_b;	//
	tra[3][2] = tra_B4U_a;	//
	tra[3][3] = tra_B4U_b;	//

	return tra;
}

float* Linear_Interpolation_2D(float** pIndex_a, float theta0, float thetaV,CGEN_ExecutionHandler *_poExHand)
//	2维线性插值*********************************************************************************
{
//	cout<< "Linear_Interpolation_2D" <<endl;

	float theta0_js1 = pIndex_a[4][0];
	float theta0_js2 = pIndex_a[4][1];
	float thetav_jv1 = pIndex_a[4][2];
	float thetav_jv2 = pIndex_a[4][3];

	// compute p, r, rho.
	float p, r;
	float* rho = new float[3];
	if(rho == NULL)
	{
		{
			_poExHand->sendInfoMessage("Linear_Interpolation_2D's point memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}

	p = (thetaV - thetav_jv1) / (thetav_jv2 - thetav_jv1);
	r = (theta0 - theta0_js1) / (theta0_js2 - theta0_js1);

	rho[0] = (1-p) * (1-r) * pIndex_a[0][0] + p * r * pIndex_a[3][0] + p * (1-r) * pIndex_a[1][0] + (1-p) * r * pIndex_a[2][0];
	rho[1] = (1-p) * (1-r) * pIndex_a[0][1] + p * r * pIndex_a[3][1] + p * (1-r) * pIndex_a[1][1] + (1-p) * r * pIndex_a[2][1];
	rho[2] = (1-p) * (1-r) * pIndex_a[0][2] + p * r * pIndex_a[3][2] + p * (1-r) * pIndex_a[1][2] + (1-p) * r * pIndex_a[2][2];

	return rho;
}

//float* Linear_Interpolation_3D(float* rho, float** pIndexTable, int* pIndex_n, float soz, float sez, float deltaphi)
float* Linear_Interpolation_3D(float** pIndexTable, int* pIndex_n, float soz, float sez, float deltaphi,CGEN_ExecutionHandler *_poExHand)
//	3维线性插值*********************************************************************************
{
	int soz_a_xuhao = floor(soz/4);
	int sez_a_xuhao = floor(sez/4);
	int deltaphi_a_xuhao = floor(deltaphi/10);

	int hang_a000 = soz_a_xuhao * 418 + sez_a_xuhao * 19 + deltaphi_a_xuhao;
	int hang_a100 = (soz_a_xuhao+1) * 418 + sez_a_xuhao * 19 + deltaphi_a_xuhao;
	int hang_a010 = soz_a_xuhao * 418 + (sez_a_xuhao+1) * 19 + deltaphi_a_xuhao;
	int hang_a001 = soz_a_xuhao * 418 + sez_a_xuhao * 19 + deltaphi_a_xuhao + 1;
	int hang_a110 = (soz_a_xuhao+1) * 418 + (sez_a_xuhao+1) * 19 + deltaphi_a_xuhao;
	int hang_a101 = (soz_a_xuhao+1) * 418 + sez_a_xuhao * 19 + deltaphi_a_xuhao + 1;
	int hang_a011 = soz_a_xuhao * 418 + (sez_a_xuhao+1) * 19 + deltaphi_a_xuhao + 1;
	int hang_a111 = (soz_a_xuhao+1) * 418+(sez_a_xuhao+1) * 19 + deltaphi_a_xuhao + 1;

	float a000_a = pIndexTable[hang_a000][3];
	float a000_b = pIndexTable[hang_a000][4];
	float a000_c = pIndexTable[hang_a000][5];

	float a100_a = pIndexTable[hang_a100][3];
	float a100_b = pIndexTable[hang_a100][4];
	float a100_c = pIndexTable[hang_a100][5];

	float a010_a = pIndexTable[hang_a010][3];
	float a010_b = pIndexTable[hang_a010][4];
	float a010_c = pIndexTable[hang_a010][5];

	float a001_a = pIndexTable[hang_a001][3];
	float a001_b = pIndexTable[hang_a001][4];
	float a001_c = pIndexTable[hang_a001][5];

	float a110_a = pIndexTable[hang_a110][3];
	float a110_b = pIndexTable[hang_a110][4];
	float a110_c = pIndexTable[hang_a110][5];

	float a101_a = pIndexTable[hang_a101][3];
	float a101_b = pIndexTable[hang_a101][4];
	float a101_c = pIndexTable[hang_a101][5];


	float a011_a = pIndexTable[hang_a011][3];
	float a011_b = pIndexTable[hang_a011][4];
	float a011_c = pIndexTable[hang_a011][5];

	float a111_a = pIndexTable[hang_a111][3];
	float a111_b = pIndexTable[hang_a111][4];
	float a111_c = pIndexTable[hang_a111][5];

	// compute三维插值
	int deltaphi_js1 = deltaphi_a_xuhao*10;		int deltaphi_js2 = deltaphi_js1+10;
	int theta0_a_js1 = soz_a_xuhao*4;			int theta0_a_js2 = theta0_a_js1+4;
	int thetav_a_jv1 = sez_a_xuhao*4;			int thetav_a_jv2 = thetav_a_jv1+4;
	float p_z=(deltaphi - deltaphi_js1) / (deltaphi_js2 - deltaphi_js1);

	float ia_00 = a000_a * (1-p_z) + a001_a * p_z;
	float ia_01 = a010_a * (1-p_z) + a011_a * p_z;
	float ia_10 = a100_a * (1-p_z) + a101_a * p_z;
	float ia_11 = a110_a * (1-p_z) + a111_a * p_z;

	float ib_00 = a000_b * (1-p_z) + a001_b * p_z;
	float ib_01 = a010_b * (1-p_z) + a011_b * p_z;
	float ib_10 = a100_b * (1-p_z) + a101_b * p_z;
	float ib_11 = a110_b * (1-p_z) + a111_b * p_z;

	float ic_00 = a000_c * (1-p_z) + a001_c * p_z;
	float ic_01 = a010_c * (1-p_z) + a011_c * p_z;
	float ic_10 = a100_c * (1-p_z) + a101_c * p_z;
	float ic_11 = a110_c * (1-p_z) + a111_c * p_z;

	float p_x = (sez-thetav_a_jv1) / (thetav_a_jv2-thetav_a_jv1);
	float r_y = (soz-theta0_a_js1) / (theta0_a_js2-theta0_a_js1);

	float Band_shiji_a = (1-p_x)*(1-r_y)*ia_00 + p_x*r_y*ia_11 + p_x*(1-r_y)*ia_10 + (1-p_x)*r_y*ia_01;
	float Band_shiji_b = (1-p_x)*(1-r_y)*ib_00 + p_x*r_y*ib_11 + p_x*(1-r_y)*ib_10 + (1-p_x)*r_y*ib_01;
	float Band_shiji_c = (1-p_x)*(1-r_y)*ic_00 + p_x*r_y*ic_11 + p_x*(1-r_y)*ic_10 + (1-p_x)*r_y*ic_01;

	float* rho = new float[3];	//保存至一维数组
	if(rho == NULL)
	{
		{
			_poExHand->sendInfoMessage("Linear_Interpolation_3D's point memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}
	rho[0] = Band_shiji_a;
	rho[1] = Band_shiji_b;
	rho[2] = Band_shiji_c;

	return rho;
}


struct HDF_DATA Angle_Linear_Interpolation(HDF_DATA InData, int OutHeight, int OutWidth, struct HDF_Attribute_Navigation AttributeData, int angle_scale, int SateAzimflag)
{
	struct HDF_DATA OutData;
	OutData.ROWS = OutHeight;
	OutData.COLUMNS = OutWidth;
	OutData.Flag = 0;
	OutData.ErrorInfo = "";

	// print test
//	cout<<"Navigation Angle Point Counts:  "<<AttributeData.AnglePoint<<endl;
//	cout<<"First Navigation Angle Point:  "<<AttributeData.AnglePoint_First<<endl;
//	cout<<"Pixel-intervals of Navigation Angle Point:  "<<AttributeData.AnglePoint_intervals<<endl;
//	cout<<"OutWidth:  "<<OutWidth<<endl;
//	cout<<"InData.COLUMNS:  "<<InData.COLUMNS<<endl;

	int n = AttributeData.AnglePoint;			//抽样后的列数
	int First = AttributeData.AnglePoint_First;	//抽样 第一个点
	int N = AttributeData.AnglePoint_intervals;	//抽样间隔

    //一维整型数组转换为二维整型数组
	//申请内存
	float** Data_2d = new float*[OutHeight];
    for(int i=0; i<OutHeight; i++)
    {
    	Data_2d[i] = new float[OutWidth];
    }


    //每行进行插值
	for (int h = 0; h < InData.ROWS; h++)
	{
		 //进行插值
		for (int i=1; i<=n-1; i++)
		{
			Data_2d[h][(i-1)*N] = InData.DATASET[h*n + i-1];
			Data_2d[h][i*N]     = InData.DATASET[h*n + i];
//			cout<<"InData.DATASET[h*n + i]:  "<<InData.DATASET[h*n + i]<<endl;
			for (int j=1; j<=N-1; j++)
			{
				Data_2d[h][(i-1)*N + j] = (float(j)/(N-1)) * (InData.DATASET[h*n + i] - InData.DATASET[h*n + i-1]) + InData.DATASET[h*n + i-1];
//				cout<<"Data_2d[h][(i-1)*N + j]:  "<<Data_2d[h][(i-1)*N + j]<<endl;
			}
		}

		// 解决中心跳变问题，这块只针对卫星方位角，其他几个角度不需要
		if (SateAzimflag == 1)
		{
			for (int i=1; i<=4; i++)
			{
				Data_2d[h][687 * 16 + i] = Data_2d[h][687 * 16];
			}
			for (int i=1; i<=11; i++)
			{
				Data_2d[h][687 * 16 + i + 4] = Data_2d[h][688 * 16];
			}
		}

		//
		if ((n-1)*N == OutWidth) //
		{
			Data_2d[h][(n-1)*N] = InData.DATASET[h*n + n-1];
			Data_2d[h][(n-2)*N] = InData.DATASET[h*n + n-2];

			for (int j=1; j<=N-1; j++)
			{
				Data_2d[h][(n-1)*N + j] = (float(j+15)/(N-1)) * (InData.DATASET[h*n + n-1] - InData.DATASET[h*n + n-2]) + InData.DATASET[h*n + n-2];
			}
		}

		//
		if ((n-1)*N < OutWidth) //
		{
			int last_n = OutWidth-1 - (n-1)*N; //剩余的数

			Data_2d[h][(n-1)*N] = InData.DATASET[h*n + n-1];
			Data_2d[h][(n-2)*N] = InData.DATASET[h*n + n-2];

			for (int j=1; j<=last_n; j++)
			{
				Data_2d[h][(n-1)*N + j] = (float(j+15)/(N-1)) * (InData.DATASET[h*n + n-1] - InData.DATASET[h*n + n-2]) + InData.DATASET[h*n + n-2];
			}
		}
	}

    //二维整型数组转换为一维整型数组
	//申请内存
	//float* Data = new float[OutHeight * OutWidth]; //output
	float* Data = (float* )malloc(sizeof(float)*OutHeight*OutWidth);
    for(int i=0; i<OutHeight; i++)
    {
        //转换
        for(int j=0; j<OutWidth; j++)
        {
        	Data[i*OutWidth + j] = Data_2d[i][j];
        }
    }


	//判断插值是否正确
//	for (int i = 0; i < OutHeight * OutWidth; i++)
//	{
//		//判断角度是否在范围内
//		if (Data[i]<=0 || Data[i]>=angle_scale)
//		{
//			cout<<"Navigation Angle error:  "<<i<<endl;
//			cout<<"Data[] error:  "<<Data[i]<<endl;
//			Data[i] = Data[i-1];
//			cout<<"Data[] change:  "<<Data[i]<<endl;
//		}
//	}


	//free
	for(int i = 0; i < OutHeight; i++)
	{
		RELEASE(Data_2d[i]);
	}
	RELEASE(Data_2d);
	xfree(InData.DATASET);

	OutData.DATASET = Data;

	return OutData;
}
