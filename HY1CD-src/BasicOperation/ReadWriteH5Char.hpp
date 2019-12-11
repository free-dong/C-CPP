/*
 * ReadWriteH5Char.hpp
 *
 *  Created on: Aug 27, 2018
 *      Author: zhongyong
 */

#ifndef READWRITEH5CHAR_HPP_
#define READWRITEH5CHAR_HPP_


int WriteH5Char(const char* OutputPath,struct HDF5_CHAR Data, char* GroupName, char* DataName);

struct HDF5_CHAR ReadH5Char(const char* InputPath, char* groupName, char* childName);


//void WriteH5DataInt(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type);
//void WriteH5DataInt_SensorBand(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName);

#endif /* READWRITEH5CHAR_HPP_ */
