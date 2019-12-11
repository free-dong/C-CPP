/*
 * Write_Data_Attr_Xml.hpp
 *
 *  Created on: Jul 7, 2018
 *      Author: zhongyong
 */

#ifndef BASICOPERATION_WRITE_DATA_ATTR_XML_HPP_
#define BASICOPERATION_WRITE_DATA_ATTR_XML_HPP_
#include"hdf5.h"

char* getL2FileOutput(char* path, char level,char* L1BName);

void WriteH5DataInt(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type);
void WriteH5DataInt_Navigation(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float ValidMax, float ValidMin);
void WriteH5DataInt_Navigation_L(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float ValidMin, float ValidMax);
void WriteH5DataInt_SensorBand(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName);
void WriteH5DataInt_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_INT WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMax, float ValidMin);

void WriteH5Data1(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type);
void WriteH5Data1_SensorBand(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName);
void WriteH5Data1_Navigation(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, float ValidMax, float ValidMin);
void WriteH5DataFloat_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMin, float ValidMax);
void WriteH5Data1_DOUBLE(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_DOUBLE WriteData, char* Unit, char* Slope, char* Offset, char* Description, char* Type);
void WriteH5Data1_DOUBLE_ScanLine(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA_DOUBLE WriteData,
		char* Unit, char* Slope, char* Offset, char* Description, char* Type, float ValidMax, float ValidMin);
void WriteH5Data1_SSC(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float FillValue, float ValidMax, float ValidMin);
void WriteH5Data1_NDVI(const char* WritePath, const char* GroupName, const char* DataName, struct HDF_DATA WriteData,
		char* Unit, float Slope, float Offset, char* Description, char* Type, char* LongName, float FillValue, float ValidMax, float ValidMin);

void WriteGeoData2(const char* WritePath,struct HDF_GEODATA WriteData);

void WriteRrcData(const char* WritePath,struct HDF_Rrc_Rrs WriteData, float Slope, float Offset, float FillValue, float ValidMax, float ValidMin);
void WriteRrsData(const char* WritePath,struct HDF_Rrc_Rrs WriteData, float Slope, float Offset, float FillValue, float ValidMax, float ValidMin);

hid_t WriteAttrToData(hid_t DataSetId, char* Unit, char* Slope, char* Offset, char* Description , char* Type);
hid_t WriteAttrToData_SensorBand(hid_t DataSetId, char* Unit, char* Slope, char* Offset, char* Description, char* Type, char* LongName);

int CreateL2AAttribute(const char* PATH_WRITE, char* Group_name, struct HDF_Attribute_Data AttributeData);
int CreateNavigationAttribute(const char* PATH_WRITE, char* Group_name, struct NavigationData NavitationAttrData);
void WriteAttribute(hid_t Id, char* AttrName, int dataType, double Doubledata, float FloatData, int IntData, short ShortData,char* CharData, int bit);

void WriteFlags(hid_t L2_dataset);

struct L1B_XML_DATA L1B_XmlRead(const char* PATH_Xml);
struct XML_DATA XmlRead(const char* PATH_Xml);
int XmlMetaWrite(const char* PATH_Xml, struct XML_DATA XmlParamter);

int ListXmlWrite(const char* PATH_Xml, char* L2AH5Path, char* L2AMatePath,  char* JpgBluePath, char* JpgGreenPath,  char* JpgRedPath, char* JpgNirPath, char* L2BH5Path, char* JpgNDVIPath, char* JpgSSCPath, char* L2BH5MatePath, char* L2CH5Path, char* L2CH5MatePath, char* JpgChlPath, char* JpgSDDPath, char* L2CJpgBluePath, char* L2CJpgGreenPath, char* L2CJpgRedPath, char* L2CJpgNirPath);


int ReportXmlWrite(const char* PATH_Report, char* startTime, char* stopTime, char** InputList, char** OutputList, char* ErrorInfo);


const char* inttochar(int value);
const char* floattochar(float value);


struct MyConfig ReadMyConfig(const char* myConfigName);

#endif /* BASICOPERATION_WRITE_DATA_ATTR_XML_HPP_ */
