/*
 * read_Aerosol_and_scattering.h
 *
 *  Created on: 2018年5月21日
 *      Author: zhangyonghui
 */

#ifndef ATMOSPHERICCORRECTION_READ_AEROSOL_AND_SCATTERING_H_
#define ATMOSPHERICCORRECTION_READ_AEROSOL_AND_SCATTERING_H_

float*** read_aeo_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand);

float** read_delta_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand);

float*** read_tra_txt(char* PATH, const char* folder, int height, int width, CGEN_ExecutionHandler *_poExHand);

#endif /* ATMOSPHERICCORRECTION_READ_AEROSOL_AND_SCATTERING_H_ */
