/*
 * img_cor_exp.h
 *
 *利用腐蚀膨胀操作对海陆分割边界进行平滑处理。
 *
 *  Created on: 2018年4月1日
 *      Author: feidong
 */

#ifndef LANDMASK_IMG_COR_EXP_H_
#define LANDMASK_IMG_COR_EXP_H_

struct HDF_DATA_INT img_cor_exp(IplImage *pBW, int height, int width);

#endif /* LANDMASK_IMG_COR_EXP_H_ */
