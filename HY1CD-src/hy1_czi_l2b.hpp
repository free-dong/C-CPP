/*
 * hy1_czi_l2b.hpp
 *
 *  Created on: Jul 20, 2018
 *      Author: zhongyong
 */

#ifndef HY1_CZI_L2B_HPP_
#define HY1_CZI_L2B_HPP_


struct HDF_DATA NDVI(struct HDF_GEODATA GeoData, struct HDF_DATA_INT CloudDetectData, float NDVIThreshold, char* L2BH5Path, char* L2BH5ThumbPath, float NDWIThreshold);

//void WriteAttribute(struct HDF_Attribute_Data AttrData, char* L2BH5Path);


struct HDF_DATA SSC(struct HDF_Rrc_Rrs GeoData, char* L2BH5Path, char* L2BH5ThumbPath);

#endif /* HY1_CZI_L2B_HPP_ */
