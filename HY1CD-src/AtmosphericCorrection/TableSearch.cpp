/*
 *  Created on: 2018年5月16日
 *    modified: 2018-08-13
 *  	Author: feidong
 */

#include <iostream>
#include <cmath>
#include<malloc.h>
#include "../hy1_czi_l2_pro.hpp"
#include"Linear_Interpolation.h"
using namespace std;
#define RELEASE(x) if(x != NULL) {delete []x; x = NULL;}  //release array

int *traSearch_1d(float soz, float sez, CGEN_ExecutionHandler *_poExHand)
{
	int Index_i, Index_j;

	//计算出索引
	Index_i = floor(soz / 4) + 1;
	Index_j = floor(sez / 4) + 1;

	if (Index_i == 23)
		Index_i -= 1;
	if (Index_j == 23)
		Index_j -= 1;

	int* pIndex_a = new int[2];
	if(pIndex_a == NULL)
	{
		{
			_poExHand->sendInfoMessage("traSearch_1d's  memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}
	pIndex_a[0] = Index_i;
	pIndex_a[1] = Index_j;

	return pIndex_a;
}

float RayleighSearch_2d(float** IndexTable, float soz, float sez ,float deltaphi ,float pi, CGEN_ExecutionHandler *_poExHand)
{
	int Index_i, Index_j;
	//计算出索引
	Index_i = floor(soz / 2) * 43;
	Index_j = Index_i + floor(sez / 2);

//	if (Index_i == 0)
//		Index_i += 1;
//	if (Index_j == 0)
//		Index_j += 1;

	float theta0_js1, theta0_js2;
	theta0_js1 = floor(soz / 2) * 2;  //	theta0_js1 = IndexTable[Index_i-1][0];
	theta0_js2 = floor(soz / 2) * 2 + 2;    //	theta0_js2 = IndexTable[Index_i][0];
	float thetav_jv1, thetav_jv2;
	thetav_jv1 = floor(sez / 2) * 2;    //thetav_jv1 = IndexTable[Index_j-1][1];
	thetav_jv2 = floor(sez / 2) * 2 + 2;      //thetav_jv2 = IndexTable[Index_j][1];

	// 根据上下界找到表中的索引，根据4个索引（I，J，M，N）返回表中a00--a11对应的数据
//	int Index_I = Index_j-44, Index_J = Index_j-43, Index_M = Index_j-1, Index_N = Index_j;
//	int Index_I = Index_j, Index_J = Index_j + 43, Index_M = Index_j + 1, Index_N = Index_j + 44;
	int Index_I = Index_j, Index_J = Index_j + 1, Index_M = Index_j + 43, Index_N = Index_j + 44;

	// 返回一个包含5×4个元素的二维数组
	float** pIndex_a = new float*[5];
	for (int i = 0; i < 5; i++)
	{
		pIndex_a[i] = new float[4];
		if(pIndex_a[i] == NULL)
		{
			{
				_poExHand->sendInfoMessage("RayleighSearch_2d's pIndex_a memory application failed!", CLOG_NO_PRODUCT);
				exit(0);
			}
		}
	}

	//将索引查找出的值（每行4个对应于a00,a01,a10,a11的rho1,rho2,rho3）存放到二维数组
	pIndex_a[0][0] = IndexTable[Index_I][2];	pIndex_a[0][1] = IndexTable[Index_I][3];	pIndex_a[0][2] = IndexTable[Index_I][4];	pIndex_a[0][3] = Index_I;
	pIndex_a[1][0] = IndexTable[Index_J][2];	pIndex_a[1][1] = IndexTable[Index_J][3];	pIndex_a[1][2] = IndexTable[Index_J][4];	pIndex_a[1][3] = Index_J;
	pIndex_a[2][0] = IndexTable[Index_M][2];	pIndex_a[2][1] = IndexTable[Index_M][3];	pIndex_a[2][2] = IndexTable[Index_M][4];	pIndex_a[2][3] = Index_M;
	pIndex_a[3][0] = IndexTable[Index_N][2];	pIndex_a[3][1] = IndexTable[Index_N][3];	pIndex_a[3][2] = IndexTable[Index_N][4];	pIndex_a[3][3] = Index_N;
	pIndex_a[4][0] = theta0_js1;				pIndex_a[4][1] = theta0_js2;				pIndex_a[4][2] = thetav_jv1;				pIndex_a[4][3] = thetav_jv2;

	// compute p, r, rho.
	float p, r;
	float* rho = new float[3];
	if(rho == NULL)
	{
		{
			_poExHand->sendInfoMessage("RayleighSearch_2d's rho memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}

	p = (sez - thetav_jv1) / (thetav_jv2 - thetav_jv1);
	r = (soz - theta0_js1) / (theta0_js2 - theta0_js1);

	rho[0] = (1-p) * (1-r) * pIndex_a[0][0] + p * r * pIndex_a[3][0] + p * (1-r) * pIndex_a[1][0] + (1-p) * r * pIndex_a[2][0];
	rho[1] = (1-p) * (1-r) * pIndex_a[0][1] + p * r * pIndex_a[3][1] + p * (1-r) * pIndex_a[1][1] + (1-p) * r * pIndex_a[2][1];
	rho[2] = (1-p) * (1-r) * pIndex_a[0][2] + p * r * pIndex_a[3][2] + p * (1-r) * pIndex_a[1][2] + (1-p) * r * pIndex_a[2][2];

	float rho_r1 = rho[0] + 2 * rho[1] * cos(deltaphi * pi / 180) + 2 * rho[2] * cos(2 * deltaphi * pi / 180);

	RELEASE(rho);
	for(int iIndex = 0; iIndex < 5; iIndex++ ){
		RELEASE(pIndex_a[iIndex]);
	}
	RELEASE(pIndex_a);
	return rho_r1;
}
int *RayleighSearch_3d(float** pIndexTable, float soz, float sez, float deltaphi, CGEN_ExecutionHandler *_poExHand)
{

	int Index_i, Index_j, Index_k;

	Index_i = (floor(soz / 4) + 1) * 418;
	Index_j = Index_i + (floor(sez / 4) + 1) * 19;
	Index_k = Index_j + floor(deltaphi / 10);

	if (Index_i == 0)
		Index_i += 1;
	if (Index_j == 0)
		Index_j += 1;
	if (Index_k == 0)
		Index_k += 1;

	// 返回一个包含1×3个元素的1维数组
	int* pIndex = new int[3];
	if(pIndex == NULL)
	{
		{
			_poExHand->sendInfoMessage("RayleighSearch_3d's point memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}

	//将索引查找除的值（i, j, k）存放到1维数组
	pIndex[0] = Index_i;
	pIndex[1] = Index_j;
	pIndex[2] = Index_k;
//	cout<<"-------------"<< Index_i <<endl;
//	cout<<"-------------"<< Index_j <<endl;
//	cout<<"-------------"<< Index_k <<endl;
	return pIndex;
}

float **RayleighSearch_3d_bands(float*** pIndexTable, float soz, float sez, float deltaphi, CGEN_ExecutionHandler *_poExHand)
// 返回4个波段的查找值,并作线性插值,返回4×3
{

	int* pIndex_1 = RayleighSearch_3d(pIndexTable[0], soz, sez, deltaphi,_poExHand);// 返回查找到的索引
	// 3维线性插值，返回一个波段的rho_0,rho_1,rho_2,存放在一维数组中
	float* B1_shiji = Linear_Interpolation_3D( pIndexTable[0], pIndex_1, soz, sez, deltaphi,_poExHand);


	int* pIndex_2 = RayleighSearch_3d(pIndexTable[1], soz, sez, deltaphi,_poExHand);// 返回查找到的索引
	float* B2_shiji = Linear_Interpolation_3D(pIndexTable[1], pIndex_2, soz, sez, deltaphi,_poExHand);

	int* pIndex_3 = RayleighSearch_3d(pIndexTable[2], soz, sez, deltaphi,_poExHand);// 返回查找到的索引
	float* B3_shiji = Linear_Interpolation_3D(pIndexTable[2], pIndex_3, soz, sez, deltaphi,_poExHand);

	int* pIndex_4 = RayleighSearch_3d(pIndexTable[3], soz, sez, deltaphi,_poExHand);// 返回查找到的索引
	float* B4_shiji = Linear_Interpolation_3D(pIndexTable[3], pIndex_4, soz, sez, deltaphi,_poExHand);

	// 返回一个包含4×3个元素的二维数组
	float** pIndex_a = new float*[4];
//	for (int i = 0; i < 4; i++)
//	{
//		pIndex_a[i] = new float[3];
//	}
	if(pIndex_a == NULL)
	{
		{
			_poExHand->sendInfoMessage("RayleighSearch_3d_bands's point memory application failed!", CLOG_NO_PRODUCT);
			exit(0);
		}
	}

	pIndex_a[0] = B1_shiji;
	pIndex_a[1] = B2_shiji;
	pIndex_a[2] = B3_shiji;
	pIndex_a[3] = B4_shiji;


	RELEASE(pIndex_1);
	RELEASE(pIndex_2);
	RELEASE(pIndex_3);
	RELEASE(pIndex_4);

	return pIndex_a;

}
