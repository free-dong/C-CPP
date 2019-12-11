/*
 * HDF_write_to_IPLImage.h
 *
 *  Created on: 2018年4月2日
 *      Author: zhangyonghui
 */

#ifndef CLOUD_DETECT_HDF_WRITE_TO_IPLIMAGE_H_
#define CLOUD_DETECT_HDF_WRITE_TO_IPLIMAGE_H_

IplImage *HDF_write_to_IPLImage(IplImage* img_rgb, int height, int width, struct HDF_GEODATA dataset);

struct HDF_DATA_INT IPLImage_write_to_HDF(IplImage* ipl_data, int height, int width);

#endif /* CLOUD_DETECT_HDF_WRITE_TO_IPLIMAGE_H_ */
