/*
 * NDWI.h
 *公式：NDWI =(p(Green)-p(NIR))/(p(Green)+p(NIR))
 *基于绿波段与近红外波段的归一化比值指数,用来提取影像中的水体信息.
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#ifndef LANDMASK_NDWI_H_
#define LANDMASK_NDWI_H_

IplImage *NDWI(int nYSize, int nXSize, struct HDF_GEODATA dataset, IplImage *pNDWI, float NDWIThreshold);

#endif /* LANDMASK_NDWI_H_ */
