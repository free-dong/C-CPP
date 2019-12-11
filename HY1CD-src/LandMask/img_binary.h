/*
 * img_binary.h
 *
 * 对NDWI图像二值化（陆地为0，海洋为1）。
 *
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#ifndef LANDMASK_IMG_BINARY_H_
#define LANDMASK_IMG_BINARY_H_

IplImage *img_binary(IplImage *IMAGE, IplImage *pBW, float NDWIThreshold);

#endif /* LANDMASK_IMG_BINARY_H_ */
