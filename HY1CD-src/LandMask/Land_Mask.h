/*
 * Land_Mask.h
 *
 *  Created on: 2018年3月15日
 *      Author: zhongyong
 */

#ifndef LAND_MASK_H_
#define LAND_MASK_H_

struct HDF_DATA_INT LandMask(struct HDF_GEODATA DataSet, int GuassFilterSize, float NDWIThreshold);

/**
 * 海陆识别模块的功能是识别海洋和陆地范围，并将陆地部分进行掩膜，只保留海洋
 * 部分，以便后续开展大气校正。
 *
 * 具体步骤：
 * 1.NDWI计算
 * 2.图像滤波
 * 3.图像二值化
 * 4.填充陆地和海洋中的空洞
 * 5.腐蚀膨胀
 */

#endif /* LAND_MASK_H_ */
