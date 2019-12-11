/*
 * Liner_Interpolation.h
 *
 *  Created on: 2018年5月16日
 *      Author: zhongyong
 */

#ifndef ATMOSPHERICCORRECTION_LINEAR_INTERPOLATION_H_
#define ATMOSPHERICCORRECTION_LINEAR_INTERPOLATION_H_


float** Linear_Interpolation_1D(int* pIndex_a, float*** IndexTable, float soz, float sez,CGEN_ExecutionHandler *_poExHand);
float* Linear_Interpolation_2D(float** pIndex_a, float theta0, float thetaV,CGEN_ExecutionHandler *_poExHand);
//float* Linear_Interpolation_3D(float* rho, float** pIndexTable, int* pIndex_n, float theta0, float thetaV, float deltaphi);
float* Linear_Interpolation_3D(float** pIndexTable, int* pIndex_n, float theta0, float thetaV, float deltaphi,CGEN_ExecutionHandler *_poExHand);
struct HDF_DATA Angle_Linear_Interpolation(HDF_DATA InData, int OutHeight, int OutWidth, struct HDF_Attribute_Navigation AttributeData, int angle_scale, int SateAzimflag);

#endif /* ATMOSPHERICCORRECTION_LINEAR_INTERPOLATION_H_ */
