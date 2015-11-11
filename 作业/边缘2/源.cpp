#include "cv.h"
#include "highgui.h"

using namespace cv;

int main(int argc, char* argv[])
{
	Mat src = imread("pp.jpg");
	Mat dst;

	//����ͼ��
	//���ͼ��
	//����ͼ����ɫͨ����
	//x�������
	//y�������
	Sobel(src, dst, src.depth(), 1, 1);
	imwrite("sobel.jpg", dst);

	//����ͼ��
	//���ͼ��
	//����ͼ����ɫͨ����
	Laplacian(src, dst, src.depth());
	imwrite("laplacian.jpg", dst);

	//����ͼ��
	//���ͼ��
	//��ɫת�Ҷ�
	cvtColor(src, src, CV_BGR2GRAY);  //cannyֻ����Ҷ�ͼ

	//����ͼ��
	//���ͼ��
	//����ֵ
	//����ֵ��opencv�����ǵ���ֵ��3��
	//�ڲ�sobel�˲�����С
	Canny(src, dst, 50, 150, 3);
	imwrite("canny.jpg", dst);

	imshow("dst", dst);
	waitKey();

	return 0;
}