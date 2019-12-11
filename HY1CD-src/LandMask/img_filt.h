/*
 * image_filtering.h
 *
 * 1.图像高斯滤波
 * 2.对NDWI图像进行图像滚动递归引导滤波操作，消除图像中的噪声点
 *
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#ifndef LANDMASK_IMG_FILT_H_
#define LANDMASK_IMG_FILT_H_

IplImage *img_filt(int nYSize, int nXSize, IplImage *pNDWI, IplImage *IMAGE, int GuassFilterSize);

#endif /* LANDMASK_IMG_FILT_H_ */
