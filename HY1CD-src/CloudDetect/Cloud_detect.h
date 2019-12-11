/*
 * Cloud_detect.h
 *
 *  Created on: 2018年3月15日
 *      Author: zhangyonghui
 */

#ifndef CLOUD_DETECT_H_
#define CLOUD_DETECT_H_

struct HDF_DATA_INT CloudDetect(struct HDF_GEODATA DataSet, float cloud_threshold, float shadow_threshold, int median_filter_window);

#endif /* CLOUD_DETECT_H_ */
