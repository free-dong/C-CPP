/*
 * Read_Data_Attr_Xml.h
 *
 *  Created on: Jul 5, 2018
 *      Author: zhongyong
 */

#ifndef BASICOPERATION_READ_DATA_ATTR_XML_H_
#define BASICOPERATION_READ_DATA_ATTR_XML_H_

struct HDF_DATA_INT ReadH5DataInt(const char* FilePath, const char* GroupName,const char* DataSetName);
struct HDF_DATA_INT ReadH5DataInt_scale_factor_new(const char* FilePath, const char* GroupName,const char* DataSetName);
struct HDF_DATA ReadH5DataInt_scale_factor(const char* FilePath, const char* GroupName,const char* DataSetName);
struct HDF_DATA ReadH5Data1(const char* FilePath, const char* GroupName,const char* DataSetName);
struct HDF_DATA_DOUBLE ReadH5Data1_DOUBLE(const char* FilePath, const char* GroupName,const char* DataSetName);
struct HDF_DATA ReadH5Data2(hid_t GroupId,const char* DataSetName);
struct HDF_DATA ReadH5Data3(hid_t DataSetId);

struct HDF_GEODATA ReadGeoData(const char* FilePath);
char* StringStitching(char* AttributeName);
struct HDF_Attribute_Data ReadH5Attribute(const char* FilePath);
struct HDF_ATTRIBUTES Read_Attribute(hid_t OpenId, const char* data_Name, int length);
struct HDF_Attribute_Navigation ReadH5Attribute_Navigation(const char* FilePath);

struct XML_DATA ReadMetaXml(const char* XmlPath);

float** get_sub_date(char *FILE_NAME, char *SDS_NAME);
float*** hdf4Read_Realtime(char *FILE_NAME);

//float** get_sub_date_NECP(char *FILE_NAME, char *SDS_NAME);
struct HDF_get_sub_date_NECP get_sub_date_NECP(char *FILE_NAME, char *SDS_NAME);
//float*** hdf4Read_Climatedata(char *FILE_NAME, char* month_char);
struct HDF_hdf4Read_Climatedata hdf4Read_Climatedata(char *FILE_NAME, char* month_char);

struct HDF_DATA_NECP get_sub_date_h5(const char* FilePath, const char* GroupName,const char* DataSetName);
float*** h5Read_Climatedata(char *FILE_NAME);

#endif /* BASICOPERATION_READ_DATA_ATTR_XML_H_ */
