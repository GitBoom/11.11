#include <stdio.h>
#include <time.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

int main( int argc, char** argv )
{
	//声明IplImage指针
	IplImage* pFrame = NULL; //pFrame为视频截取的一帧,IplImage为一结构体，里面存储着画面信息
	IplImage* pFrame1 = NULL; //第一帧
	IplImage* pFrame2 = NULL;//第二帧
	IplImage* pFrame3 = NULL;//第三帧

	IplImage* pFrImg = NULL; //pFrImg为当前帧的灰度图
	IplImage* pBkImg = NULL; //pBkImg为当前背景的灰度图
	IplImage* pBkImgTran = NULL;//pBkImgTran为当前背景处理过的图像
	IplImage* pFrImgTran = NULL;//pFrImgTran为当前背景处理过的图像

	CvMat* pFrameMat = NULL; //pFrameMat为当前灰度矩阵
	CvMat* pFrMat = NULL; //pFrMat为当前前景图矩阵，当前帧减去背景图
	CvMat* bg1 = NULL;
	CvMat* bg2 = NULL;
	CvMat* bg3 = NULL;
	CvMat* pFrMatB = NULL; //pFrMatB为二值化（0,1）的前景图
	CvMat* pBkMat = NULL;
	CvMat* pZeroMat = NULL; //用于计算bg1 - bg2 的值
	CvMat* pZeroMatB = NULL;//用于计算 pZeroMat阈值化后来判断有多少个零的临时矩阵

	CvCapture* pCapture = NULL;

	int warningNum = 0; //检测到有异物入侵的次数
	int nFrmNum = 0;//帧计数
	int status = 0; //状态标志位

	//创建窗口
	cvNamedWindow("video", 0);
	cvNamedWindow("background", 0);
	//cvNamedWindow("foreground", 0);
	cvMoveWindow("video", 350, 50);
	cvMoveWindow("background", 800, 50);
	cvMoveWindow("foreground", 690, 0);

	//if (argc == 1)
	if (!(pCapture = cvCaptureFromCAM(0)))
	{
		fprintf(stderr, "Can not open camera./n");
		return -2;
	}
	while (pFrame = cvQueryFrame(pCapture))
	{
		nFrmNum++;
		if (nFrmNum == 1)
		{
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);//创建首地址并分配空间， 图像元素的位深度
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pBkImgTran = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImgTran = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);//创建矩阵，32字节有符号1个通道的矩阵
			pZeroMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMatB = cvCreateMat(pFrame->height, pFrame->width, CV_8UC1);
			pZeroMatB = cvCreateMat(pFrame->height, pFrame->width, CV_8UC1);
			pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			cvZero(pZeroMat);
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pBkMat);
		}
		else 
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);//pFrameMat为当前灰度矩阵
			cvConvert(pFrImg, pFrameMat);//pFrImg为当前帧的灰度图

			//pFrMat为前景图矩阵，当前帧减去背景图
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);
			cvThreshold(pFrMat, pFrMatB, 60, 1, CV_THRESH_BINARY);

			//将图像矩阵转化为图像格式，用以显示
			cvConvert(pBkMat, pBkImgTran);
			cvConvert(pFrMat, pFrImgTran);

			//显示图像
			cvShowImage("video", pFrame);
			cvShowImage("background", pBkImgTran); //显示背景
			//cvShowImage("foreground", pFrImgTran); //显示前景


			//以上是每抓取一帧都要做的工作，下面进行危险检测
			if (cvCountNonZero(pFrMatB) > 10000 && status == 0) //表示是第一帧的异物大于1W个像数点
			{/* 则需要将当前帧存储为第一帧 */
				pFrame1 = cvCloneImage(pFrame);
				bg1 = cvCloneMat(pFrMat);
				status = 1; //继续采集第2帧
			}
			else if (cvCountNonZero(pFrMatB) < 10000 && status == 1) // 表示第一帧的异物大于1W个像数点，而第二帧没有,则报警
			{
				printf("NO.%d 检测到异物!!/n/n", warningNum++);

				status = 0;
			}
			else if (cvCountNonZero(pFrMatB) > 10000 && status == 1)// 表示第一帧和第二帧的异物都大于1W个像数点
			{
				pFrame2 = cvCloneImage(pFrame);
				bg2 = cvCloneMat(pFrMat);

				cvAbsDiff(bg1, bg2, pZeroMat);
				cvThreshold(pZeroMat, pZeroMatB, 20, 1, CV_THRESH_BINARY);
				if (cvCountNonZero(pZeroMatB) > 3000) //表示他们不连续，这样的话要报警
				{
					printf("NO.%d 检测到异物!!/n/n", warningNum++);
					status = 0;
				}
				else
				{
					status = 2; 
				}
			}
			else if (cvCountNonZero(pFrMatB) < 10000 && status == 2)//表示第一帧和第二帧的异物都大于1W个像数点,而第三帧没有
			{
				printf("NO.%d w检测到异物!!/n/n", warningNum++);
				status = 0;
			}
			else if (cvCountNonZero(pFrMatB) > 10000 && status == 2)//表示连续3帧的异物都大于1W个像数点
			{
				pFrame3 = cvCloneImage(pFrame);
				bg3 = cvCloneMat(pFrMat);

				cvAbsDiff(bg2, bg3, pZeroMat);
				cvThreshold(pZeroMat, pZeroMatB, 20, 1, CV_THRESH_BINARY);
				if (cvCountNonZero(pZeroMatB) > 3000) //表示他们不连续，这样的话要报警
				{
					printf("NO.%d 检测到异物!!/n/n", warningNum++);
				}
				else //表示bg2,bg3连续
				{
					cvReleaseMat(&pBkMat);
					pBkMat = cvCloneMat(pFrameMat); //更新背景
				}
				status = 0; //进入下一次采集过程
			}

			if (cvWaitKey(2) >= 0)
				break;
		}
	}
	cvDestroyWindow("video");
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");

	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);
	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);
	cvReleaseCapture(&pCapture);
	return 0;
}