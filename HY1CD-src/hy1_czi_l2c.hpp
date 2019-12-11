/*
 * hy1_czi_l2c.hpp
 *
 *  Created on: Jul 20, 2018
 *      Author: zhongyong
 */

#ifndef HY1_CZI_L2C_HPP_
#define HY1_CZI_L2C_HPP_



struct HDF_DATA CHL(struct HDF_Rrc_Rrs GeoData, char* L2CH5Path, char* L2CCHLPath);

//void WriteAttribute(struct HDF_Attribute_Data AttrData, char* L2BH5Path);


struct HDF_DATA SDD(struct HDF_Rrc_Rrs GeoData, char* L2CH5Path, char* L2BSDDPath);

#endif /* HY1_CZI_L2C_HPP_ */
