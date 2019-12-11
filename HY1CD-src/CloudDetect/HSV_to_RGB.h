/*
 * HSV_to_RGB.h
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */

#ifndef CLOUD_DETECT_HSV_TO_RGB_H_
#define CLOUD_DETECT_HSV_TO_RGB_H_

IplImage *HSV_to_RGB(IplImage* new_RGB, IplImage *img_hsv, IplImage *img_rgb, int height, int width);

#endif /* CLOUD_DETECT_HSV_TO_RGB_H_ */
