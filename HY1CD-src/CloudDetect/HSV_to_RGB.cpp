/*
 * HSV_to_RGB.cpp
 *
 *  Created on: 2018年4月3日
 *      Author: zhangyonghui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"hdf5.h"
#include"HSV_to_RGB.h"

using namespace std;
using namespace cv;

/*
 * 定义了一个类：Histogram1D，两个函数：hist_specify()和HSV_transform()，
 * 类：Histogram1D和函数：hist_specify()用于直方图匹配，并被函数：HSV_transform()调用，
 * 函数：HSV_transform()主要完成通道的分离、合并、调用直方图匹配函数，
 * 还有将图像从RGB空间转换到HSV空间的操作，即逆变换。
 *
 * 直方图匹配:
 * 将原RGB模型中的R通道与HSV模型中的V通道进行直方图匹配;
 *
 * 逆变换：
 * 利用匹配后的R通道代替V通道，然后将新的HSV转换为RGB;
 */

//定义一个类，用于直方图匹配;
class Histogram1D
{
private:
    int histSize[1]; // 项的数量
    float hranges[2]; // 统计像素的最大值和最小值
    const float* ranges[1];
    int channels[1]; // 仅计算一个通道

public:
    Histogram1D()
    {
        // 准备1D直方图的参数
        histSize[0] = 256;
        hranges[0] = 0.0f;
        hranges[1] = 255.0f;
        ranges[0] = hranges;
        channels[0] = 0;
    }

    MatND getHistogram(const Mat &image)
    {
        MatND hist;
        // 计算直方图
        calcHist(&image ,// 要计算图像的
            1,                // 只计算一幅图像的直方图
            channels,        // 通道数量
            Mat(),            // 不使用掩码
            hist,            // 存放直方图
            1,                // 1D直方图
            histSize,        // 统计的灰度的个数
            ranges);        // 灰度值的范围
        return hist;
    }

    Mat getHistogramImage(const Mat &image)
    {
        MatND hist = getHistogram(image);

        // 最大值，最小值
        double maxVal = 0.0f;
        double minVal = 0.0f;

        minMaxLoc(hist, &minVal, &maxVal);

        //显示直方图的图像
        Mat histImg(histSize[0], histSize[0], CV_8U, Scalar(255));

        // 设置最高点为nbins的90%
        int hpt = static_cast<int>(0.9 * histSize[0]);
        //每个条目绘制一条垂直线
        for (int h = 0; h < histSize[0]; h++)
        {
            float binVal = hist.at<float>(h);
            int intensity = static_cast<int>(binVal * hpt / maxVal);
            // 两点之间绘制一条直线
            line(histImg, Point(h, histSize[0]), Point(h, histSize[0] - intensity), Scalar::all(0));
        }
        return histImg;
    }
};

//直方图匹配函数;
Mat hist_specify(const Mat &src, const Mat &dst,Mat &result)
{
    Histogram1D hist1D;
    MatND src_hist = hist1D.getHistogram(src);
    MatND dst_hist = hist1D.getHistogram(dst);

    float src_cdf[256] = { 0 };
    float dst_cdf[256] = { 0 };

    // 源图像和目标图像的大小不一样，要将得到的直方图进行归一化处理
    src_hist /= (src.rows * src.cols);
    dst_hist /= (dst.rows * dst.cols);

    // 计算原始直方图和规定直方图的累积概率
    for (int i = 0; i < 256; i++)
    {
        if (i == 0)
        {
            src_cdf[i] = src_hist.at<float>(i);
            dst_cdf[i] = dst_hist.at<float>(i);
        }
        else
        {
            src_cdf[i] = src_cdf[i - 1] + src_hist.at<float>(i);
            dst_cdf[i] = dst_cdf[i - 1] + dst_hist.at<float>(i);
        }
    }

    // 累积概率的差值
    float diff_cdf[256][256];
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 256; j++)
            diff_cdf[i][j] = fabs(src_cdf[i] - dst_cdf[j]);

    // 构建灰度级映射表
    Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; i++)
    {
        // 查找源灰度级为ｉ的映射灰度
        //　和ｉ的累积概率差值最小的规定化灰度
        float min = diff_cdf[i][0];
        int index = 0;
        for (int j = 1; j < 256; j++)
        {
            if (min > diff_cdf[i][j])
            {
                min = diff_cdf[i][j];
                index = j;
            }
        }
        lut.at<uchar>(i) = static_cast<uchar>(index);
    }

    // 应用查找表，做直方图规定化
    LUT(src, lut, result);
    return result;
}

IplImage *HSV_to_RGB(IplImage* new_RGB, IplImage *img_hsv, IplImage *img_rgb, int height, int width)
{
	//创建HSV各通道分量;
	IplImage* H_img_hsv = cvCreateImage(cvGetSize(img_hsv), IPL_DEPTH_8U, 1);
	IplImage* S_img_hsv = cvCreateImage(cvGetSize(img_hsv), IPL_DEPTH_8U, 1);
	IplImage* V_img_hsv = cvCreateImage(cvGetSize(img_hsv), IPL_DEPTH_8U, 1);

    //HSV各通道分离，用于直方图匹配;
    cvSplit(img_hsv, H_img_hsv, S_img_hsv, V_img_hsv, 0);

    //分离出R通道，用于直方图匹配;
    IplImage* R_img_rgb = cvCreateImage(cvGetSize(img_hsv), IPL_DEPTH_8U, 1);
    cvSplit(img_rgb, 0, 0, R_img_rgb, 0);

    /**直方图匹配:
     * 将原RGB模型中的R通道与HSV模型中的V通道进行直方图匹配;
    */

    //将IplImage向Mat转换;
    Mat R_mat = cvarrToMat(R_img_rgb);
    Mat V_mat = cvarrToMat(V_img_hsv);

    //定义一个result,用于存储直方图匹配后的结果;
    Mat result;

    //直方图匹配;
    result = hist_specify(R_mat, V_mat, result);

    //将Mat向IplImage转换;
    IplImage R_hist = result;

    /**逆变换：
     * 利用匹配后的R通道代替V通道，然后将新的HSV转换为RGB;
    */

    //创建一个IplImage* new_HSV和一个IplImage* new_RGB;
    IplImage* new_HSV = cvCreateImage(cvGetSize(img_hsv), IPL_DEPTH_8U, 3); //3通道分别为H，S，R_hist;
//    IplImage* new_RGB = cvCreateImage(cvGetSize(img_rgb), IPL_DEPTH_8U, 3); //3通道分别为B，G，R;

    //利用匹配后的R通道代替V通道，然后将三个通道合并;
    cvMerge(H_img_hsv, S_img_hsv, &R_hist, 0, new_HSV);

    //将HSV转换成RGB;
    cvCvtColor(new_HSV, new_RGB, CV_HSV2BGR);

    //释放内存;
    cvReleaseImage(&img_rgb);
    cvReleaseImage(&img_hsv);
    cvReleaseImage(&H_img_hsv);
    cvReleaseImage(&S_img_hsv);
    cvReleaseImage(&V_img_hsv);
    cvReleaseImage(&new_HSV);
    cvReleaseImage(&R_img_rgb);

	return new_RGB;
}
