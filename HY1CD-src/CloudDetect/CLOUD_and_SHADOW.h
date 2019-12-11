/*
 * CLOUD_and_SHADOW.h
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */

#ifndef CLOUD_DETECT_CLOUD_AND_SHADOW_H_
#define CLOUD_DETECT_CLOUD_AND_SHADOW_H_

IplImage *CLOUD(IplImage *new_RGB, IplImage *B_cloud, int height, int width, float cloud_threshold, int median_filter_window);

IplImage *SHADOW(IplImage *new_RGB, IplImage *R_shadow, int height, int width, float shadow_threshold);

#endif /* CLOUD_DETECT_CLOUD_AND_SHADOW_H_ */
