#include <stdio.h>
#include <time.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

int main( int argc, char** argv )
{
	//����IplImageָ��
	IplImage* pFrame = NULL; //pFrameΪ��Ƶ��ȡ��һ֡,IplImageΪһ�ṹ�壬����洢�Ż�����Ϣ
	IplImage* pFrame1 = NULL; //��һ֡
	IplImage* pFrame2 = NULL;//�ڶ�֡
	IplImage* pFrame3 = NULL;//����֡

	IplImage* pFrImg = NULL; //pFrImgΪ��ǰ֡�ĻҶ�ͼ
	IplImage* pBkImg = NULL; //pBkImgΪ��ǰ�����ĻҶ�ͼ
	IplImage* pBkImgTran = NULL;//pBkImgTranΪ��ǰ�����������ͼ��
	IplImage* pFrImgTran = NULL;//pFrImgTranΪ��ǰ�����������ͼ��

	CvMat* pFrameMat = NULL; //pFrameMatΪ��ǰ�ҶȾ���
	CvMat* pFrMat = NULL; //pFrMatΪ��ǰǰ��ͼ���󣬵�ǰ֡��ȥ����ͼ
	CvMat* bg1 = NULL;
	CvMat* bg2 = NULL;
	CvMat* bg3 = NULL;
	CvMat* pFrMatB = NULL; //pFrMatBΪ��ֵ����0,1����ǰ��ͼ
	CvMat* pBkMat = NULL;
	CvMat* pZeroMat = NULL; //���ڼ���bg1 - bg2 ��ֵ
	CvMat* pZeroMatB = NULL;//���ڼ��� pZeroMat��ֵ�������ж��ж��ٸ������ʱ����

	CvCapture* pCapture = NULL;

	int warningNum = 0; //��⵽���������ֵĴ���
	int nFrmNum = 0;//֡����
	int status = 0; //״̬��־λ

	//��������
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
			pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);//�����׵�ַ������ռ䣬 ͼ��Ԫ�ص�λ���
			pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pBkImgTran = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
			pFrImgTran = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

			pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);//��������32�ֽ��з���1��ͨ���ľ���
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
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);//pFrameMatΪ��ǰ�ҶȾ���
			cvConvert(pFrImg, pFrameMat);//pFrImgΪ��ǰ֡�ĻҶ�ͼ

			//pFrMatΪǰ��ͼ���󣬵�ǰ֡��ȥ����ͼ
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);
			cvThreshold(pFrMat, pFrMatB, 60, 1, CV_THRESH_BINARY);

			//��ͼ�����ת��Ϊͼ���ʽ��������ʾ
			cvConvert(pBkMat, pBkImgTran);
			cvConvert(pFrMat, pFrImgTran);

			//��ʾͼ��
			cvShowImage("video", pFrame);
			cvShowImage("background", pBkImgTran); //��ʾ����
			//cvShowImage("foreground", pFrImgTran); //��ʾǰ��


			//������ÿץȡһ֡��Ҫ���Ĺ������������Σ�ռ��
			if (cvCountNonZero(pFrMatB) > 10000 && status == 0) //��ʾ�ǵ�һ֡���������1W��������
			{/* ����Ҫ����ǰ֡�洢Ϊ��һ֡ */
				pFrame1 = cvCloneImage(pFrame);
				bg1 = cvCloneMat(pFrMat);
				status = 1; //�����ɼ���2֡
			}
			else if (cvCountNonZero(pFrMatB) < 10000 && status == 1) // ��ʾ��һ֡���������1W�������㣬���ڶ�֡û��,�򱨾�
			{
				printf("NO.%d ��⵽����!!/n/n", warningNum++);

				status = 0;
			}
			else if (cvCountNonZero(pFrMatB) > 10000 && status == 1)// ��ʾ��һ֡�͵ڶ�֡�����ﶼ����1W��������
			{
				pFrame2 = cvCloneImage(pFrame);
				bg2 = cvCloneMat(pFrMat);

				cvAbsDiff(bg1, bg2, pZeroMat);
				cvThreshold(pZeroMat, pZeroMatB, 20, 1, CV_THRESH_BINARY);
				if (cvCountNonZero(pZeroMatB) > 3000) //��ʾ���ǲ������������Ļ�Ҫ����
				{
					printf("NO.%d ��⵽����!!/n/n", warningNum++);
					status = 0;
				}
				else
				{
					status = 2; 
				}
			}
			else if (cvCountNonZero(pFrMatB) < 10000 && status == 2)//��ʾ��һ֡�͵ڶ�֡�����ﶼ����1W��������,������֡û��
			{
				printf("NO.%d w��⵽����!!/n/n", warningNum++);
				status = 0;
			}
			else if (cvCountNonZero(pFrMatB) > 10000 && status == 2)//��ʾ����3֡�����ﶼ����1W��������
			{
				pFrame3 = cvCloneImage(pFrame);
				bg3 = cvCloneMat(pFrMat);

				cvAbsDiff(bg2, bg3, pZeroMat);
				cvThreshold(pZeroMat, pZeroMatB, 20, 1, CV_THRESH_BINARY);
				if (cvCountNonZero(pZeroMatB) > 3000) //��ʾ���ǲ������������Ļ�Ҫ����
				{
					printf("NO.%d ��⵽����!!/n/n", warningNum++);
				}
				else //��ʾbg2,bg3����
				{
					cvReleaseMat(&pBkMat);
					pBkMat = cvCloneMat(pFrameMat); //���±���
				}
				status = 0; //������һ�βɼ�����
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