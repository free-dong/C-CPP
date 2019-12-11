/*
 * DATA_MASK.h
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */

#ifndef CLOUD_DETECT_DATA_MASK_H_
#define CLOUD_DETECT_DATA_MASK_H_

IplImage *CLOUD_SHADOW_MERGE(IplImage *R_shadow, IplImage *B_cloud, int height, int width);

struct HDF_GEODATA DATA_MASK(struct HDF_GEODATA DataSe, struct HDF_DATA_INT TrueValueGraph);

#endif /* CLOUD_DETECT_DATA_MASK_H_ */
