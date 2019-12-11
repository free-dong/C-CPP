/*
 * rendering.hpp
 *
 *  Created on: Aug 12, 2018
 *      Author: zhangyonghui
 */

#ifndef RENDERING_HPP_
#define RENDERING_HPP_


void Rendering(struct HDF_DATA gray_image, const char* path, const char* flag, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData, CGEN_ExecutionHandler *_poExHand);

void Rendering_NDVI(float* data, const char* path, struct HDF_DATA_INT CloudDetectData, struct HDF_GEODATA GeoData);

void Rendering_Rrs(float* data, const char* path, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData, const char* flag,CGEN_ExecutionHandler *_poExHand);

#endif /* RENDERING_HPP_ */
