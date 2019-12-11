#include "../hy1_czi_l2_pro.hpp"
#include<iostream>
#include<fstream>
#include"read_txt.h"
//#include<malloc.h>
using namespace std;
extern "C"{
	#include"../BasicOperation/Basic_Operation.h"
}

float** read_txt(char* PATH_txt, int height, int width, CGEN_ExecutionHandler *_poExHand)
{
	int TableTxt = FileIsExist(PATH_txt);
		if(TableTxt != 0)
		{
			//TODO: log + exit Code
//			cout<<"Search table File Isn't Exist,it's exiting..."<<endl;
			_poExHand->sendInfoMessage("Search table File Isn't Exist,it's exiting... ", CLOG_NO_PRODUCT);

		}
	ifstream in(PATH_txt);//打开文件
	float **ptxt = new float*[height];
	for (int i = 0; i < height; i++)
	{
		ptxt[i] = new float[width];

		//judge memory application
		if(ptxt[i] == NULL)
		{
			_poExHand->sendInfoMessage("read_txt's point memory application failed! ", CLOG_NO_PRODUCT);
			exit(0);
		}

		for(int j = 0; j < width; ++j){
			in >> ptxt[i][j];	//读取数据

			//Judge
//			if(ptxt[i][j] == NULL)
//			{
//				_poExHand->sendInfoMessage("one txt file read failed! ", CLOG_NO_PRODUCT);
////				exit(0);
//			}
		}
	}



	//读取数据
//	for(int i = 0; i < height; ++i){
//		for(int j = 0; j < width; ++j){
//			in >> ptxt[i][j];
//		}
//	}
	in.close();//关闭文件

	return ptxt;
}
