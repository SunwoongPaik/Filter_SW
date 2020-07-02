// Sonar_Object.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<opencv2\opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "BlueView.h"
#include <thread>
#include <Windows.h>

using namespace std;
using namespace cv;
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;
Mat dst; Mat src0;
void BlueView_Thread();
const double pi = 3.14159265358979f;

CBlueView m_pSonar;
int m_nCNT=0;

int _tmain(int argc, _TCHAR* argv[])
{
	//namedWindow("Display window", WINDOW_AUTOSIZE);
	//Mat src = imread("lena.png", 1);
	//imshow("Display window", src);

	//waitKey(0);

	m_pSonar.Initialize();

	std::thread t1(&BlueView_Thread);

	t1.join();

	/*
	namedWindow(window_name, WINDOW_AUTOSIZE);
	src = imread("lena.png", 1);
	if (display_caption("Original Image") != 0) { return 0; }
	dst = src.clone();
	if (display_dst(DELAY_CAPTION) != 0) { return 0; }
	if (display_caption("Homogeneous Blur") != 0) { return 0; }
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		blur(src, dst, Size(i, i), Point(-1, -1));
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
	if (display_caption("Gaussian Blur") != 0) { return 0; }
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		GaussianBlur(src, dst, Size(i, i), 0, 0);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
	if (display_caption("Median Blur") != 0) { return 0; }
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		medianBlur(src, dst, i);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
	if (display_caption("Bilateral Blur") != 0) { return 0; }
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		bilateralFilter(src, dst, i, i * 2, i / 2);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
	display_caption("End: Press a key!");
	waitKey(0);
	*/

	return 0;
}

void BlueView_Thread() {
	

	
	while (1)
	{

		//namedWindow("Display window", WINDOW_AUTOSIZE);
		//Mat src = imread("lena.png", 1);
		//imshow("Display window", src);

		//waitKey(0);


		m_pSonar.SetPing();

		std::vector<float> vRanges, vAngles, vIntensities;
		std::vector<int> vPixels;

		m_pSonar.GetRangeData(vRanges); //call by reference
		m_pSonar.GetAngleData(vAngles);
		m_pSonar.GetIntensityData(vIntensities);

		int nHeight, nWidth;
		m_pSonar.GetImgHeight(nHeight);
		m_pSonar.GetImgWidth(nWidth);
		m_pSonar.GetPixel(vPixels);

		//Mat src0(vPixels);
		//Mat src = src0.reshape(1, int(nHeight/2));

		//Mat matrix = Mat::zeros((int)nHeight, (int)nWidth, CV_32FC1);
		//matrix.col(0).copyTo(vPixels);
		//Mat src = Mat::ones(nHeight, nWidth, CV_32FC3);
		//src.copyTo(vPixels);

		Mat result;
		Mat src0 = Mat(vPixels);
		Mat src = src0.reshape(1, nHeight);

		int height = src.size().height;
		int width = src.size().width;

		double min, max;
		minMaxLoc(src, &min, &max);
		//normalize(src, src, 0, max, NORM_MINMAX);
		int pad = int((width - height) / 2.0);
		copyMakeBorder(src, src, pad, pad, 0, 0, BORDER_CONSTANT, Scalar(0));
		src = src * 10;

		resize(src, src, Size(src.cols / 2, src.rows / 2), 0, 0, CV_INTER_NN);

		//gabor
		Mat dest;
		Mat src_f;
		src.convertTo(src_f, CV_32F);

		int kernel_size = 3;
		double sig = 0.3, th = pi / 4, lm = pi / 4, gm = 1, ps = 0;
		Mat kernel = getGaborKernel(Size(kernel_size, kernel_size), sig, th, lm, gm, ps);
		filter2D(src_f, dest, CV_32F, kernel);
		//cerr << dest(Rect(30, 30, 10, 10)) << endl; // peek into the data
		Mat viz;
		dest.convertTo(viz, CV_8U, 1.0 / 255.0);     // move to proper[0..255] range to show it


		//Custom Filtering

		float kernel_vec[] = { -1, 0, -1, 0, 0, 0, 1, 0, 1 };
		Mat Custom_kernel(3, 3, CV_32F, kernel_vec);

		filter2D(viz, result, CV_8U, Custom_kernel);

		//gaussian
		GaussianBlur(result, result, Size(kernel_size, kernel_size), 1);


		//Binary
		threshold(result, result, 52, 255, THRESH_BINARY);

		imshow("Result Image", result);
		imshow("original Image", src);


		//Sleep(100);
		int c = waitKey(100);

		vRanges.clear();
		vAngles.clear();
		vIntensities.clear();

		m_pSonar.SetCNT(m_nCNT);
		m_nCNT++;
	}
}