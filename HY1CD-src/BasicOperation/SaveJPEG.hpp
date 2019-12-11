/*
 * SaveJPEG.hpp
 *
 *  Created on: Jul 12, 2018
 *      Author: zhongyong
 */

#ifndef BASICOPERATION_SAVEJPEG_HPP_
#define BASICOPERATION_SAVEJPEG_HPP_


void DataSaveToJPEG(float* Data, char* OutputJPEGPath, int Row, int Column, int flag);
void NDVISaveToJPEG(float* Data, char* OutputJPEGPath, int Row, int Column, int flag);

void HDFDataSaveToJPEG(struct HDF_DATA Data, char* OutputJPEGPath, int flag);


#endif /* BASICOPERATION_SAVEJPEG_HPP_ */
