/*
 * Atmospheric_Correction.h
 *
 *  Created on: 2018年5月16日
 *  	Author: zhangyonghui AND feidong
 */

#ifndef ATMOSPHERICCORRECTION_ATMOSPHERIC_CORRECTION_H_
#define ATMOSPHERICCORRECTION_ATMOSPHERIC_CORRECTION_H_


struct HDF_Rrc_Rrs Atmospheric_Correction(struct HDF_GEODATA DataSet, struct HDF_DATA_INT CloudDetectData, struct HDF_DATA_INT LandMaskData,  char* AuxDataPathName, char* NCEPDataName, int ASCMethod, char* PATH_HDF5, CGEN_ExecutionHandler *_poExHand);


#endif /* ATMOSPHERICCORRECTION_ATMOSPHERIC_CORRECTION_H_ */
