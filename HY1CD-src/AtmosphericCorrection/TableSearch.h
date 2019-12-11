/*
 * RayleighSearch.h
 *
 *  Created on: 2018年5月17日
 *      Author: zhongyong
 */

#ifndef ATMOSPHERICCORRECTION_TABLESEARCH_H_
#define ATMOSPHERICCORRECTION_TABLESEARCH_H_

int *traSearch_1d(float soz, float sez, CGEN_ExecutionHandler *_poExHand);
float RayleighSearch_2d(float** IndexTable, float soz, float sez, float deltaphi, float pi,CGEN_ExecutionHandler *_poExHand);
int *RayleighSearch_3d(float** IndexTable, float soz, float sez, float deltaphi,CGEN_ExecutionHandler *_poExHand);
float **RayleighSearch_3d_bands(float*** pIndexTable, float soz, float sez, float deltaphi,CGEN_ExecutionHandler *_poExHand);

#endif /* ATMOSPHERICCORRECTION_TABLESEARCH_H_ */
