/***********************************************************************
��ֻ����ʶ�����
���ܣ�ͨ��ʶ��ֻͼƬ�жϳ�������һ�ִ���
��ֻ��Լ�װ�䴬��ɳ�����ʹ�����ж������
���� main.cpp
���������VS2012 , OpenCV 2.4.4
���ߣ�quinn
���ڣ�2014.05.01
***********************************************************************/
#include "cv.h"
#include "highgui.h"
#include<iostream>
#include <fstream>
#include<string>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
#define FEATURE_NUM 5 //���õ���������

CvSeq *GetAreaMaxContour(CvSeq *contour);//�ڸ�����contour���ҵ��������һ��������������ָ���������ָ��
float getlables(string imgPath);//ѵ��ʱ�����ݴ�ֻ������ӱ�ǩ	0:��װ�䴬 1:ɳ�� 2���ʹ� 3:��ж����
int getFeatureData(int i,char *path,float trainingData[][FEATURE_NUM]);//��ȡ�������ݺ���

int main( int argc, char** argv )
{
	IplImage *train;
	string buf;
	vector<string> img_train_path;
	ifstream img_train( "../SVM_Train.txt" ); //������ҪԤ���ͼƬ���ϣ�����ı����ŵ���ͼƬȫ·������Ҫ��ǩ
	while( img_train )
	{
			if( getline( img_train, buf ) )
			{
					img_train_path.push_back( buf );
			}
	}
	img_train.close();
	int nTrainImg=img_train_path.size();	//ѵ��ͼƬ���������Ա������������ռ�
	float (*trainingData)[FEATURE_NUM];	 //�洢ѵ���õ�����������
	trainingData=new float[nTrainImg][FEATURE_NUM];
	float *labels=new float[nTrainImg];
	cout<<"SVM Train:"<<endl;
	cout<<"	 ImagePath	 "<<"	Labels"<<endl;
	//���μ���ѵ��ͼƬ����ȡ��������

	for( string::size_type j = 0; j != nTrainImg; j++ )
	{
				train = cvLoadImage( img_train_path[j].c_str(), CV_LOAD_IMAGE_GRAYSCALE );
				if( train == NULL )
				{
						cout<<" can not load the image: "<<img_train_path[j].c_str()<<endl;
						continue;
				}
				labels[j]=getlables(img_train_path[j].c_str());//��ӱ�ǩ
				char *path=const_cast<char*>(img_train_path[j].c_str());

				cout<<img_train_path[j].c_str()<<"	"<< labels[j]<<endl;
				getFeatureData(j,path,trainingData);//ѵ������ȡ����
	}
	//SVM ѵ������
	// step 1:Array -> Matrix
	Mat labelsMat(nTrainImg, 1, CV_32FC1, labels);	//��ǩ����->����
	Mat trainingDataMat(nTrainImg, FEATURE_NUM, CV_32FC1, trainingData);	 //��������->��������
	// step 2:New a SVM
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::RBF;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	// step 3:Train 
	CvSVM SVM;
	SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);
	SVM.save("../SVM_Data.xml");
	// step 4	:Predict
	//	string buf;
	vector<string> img_tst_path;
	ifstream img_tst( "../SVM_Test.txt" ); //������ҪԤ���ͼƬ���ϣ�����ı����ŵ���ͼƬȫ·������Ҫ��ǩ
	while( img_tst )
	{
				if( getline( img_tst, buf ) )
				{
						img_tst_path.push_back( buf );
				}
	}
	img_tst.close();
	IplImage *tst;
	int nTestImg=img_tst_path.size();
	float testData[1][FEATURE_NUM];
	ofstream predict_txt( "../SVM_PREDICT.txt" );//��Ԥ�����洢������ı���
	cout<<endl<<endl<<"Vessels Detection:"<<endl;
	cout<<"labels: "<<"0:��װ�䴬 "<<"1:ɳ�� "<<"2���ʹ� "<<"3:��ж����"<<endl;
	predict_txt<<"labels:"<<endl<<"0:��װ�䴬 "<<"1:ɳ�� "<<"2���ʹ� "<<"3:��ж����"<<endl;
	for( string::size_type j = 0; j != nTestImg; j++ )
	{
				tst = cvLoadImage( img_tst_path[j].c_str(), CV_LOAD_IMAGE_GRAYSCALE );
				if( tst == NULL )
				{
					cout<<" can not load the image: "<<img_tst_path[j].c_str()<<endl;
					continue;
				}

				char *path=const_cast<char*>(img_tst_path[j].c_str());//string->char*
				getFeatureData(0,path,testData);
				Mat testDataMat(1, FEATURE_NUM, CV_32FC1, testData);
				float ret = SVM.predict( testDataMat );
				

				//	printf( line, "%s %d\r\n", img_tst_path[j].c_str(), ret );
				cout<<img_tst_path[j].c_str()<<"	"<<ret<<endl;
				predict_txt<<img_tst_path[j].c_str()<<"	"<<ret<<endl;
	}
	cvReleaseImage(&tst);
	cvWaitKey(0);
	system("pause");
	//	cvDestroyWindow( "Source" );
	//	cvDestroyWindow( "max" );
	//	cvReleaseImage(&dst);
	return 0;
}
float getlables(string imgPath)
{
			if(imgPath.find("��װ�䴬")!=imgPath.npos)
				return 0;

			else if(imgPath.find("��ж")!=imgPath.npos)
				return 3;

			else if(imgPath.find("ɳ��")!=imgPath.npos)
				return 1;

			else if(imgPath.find("�ʹ�")!=imgPath.npos)
				return 2;

			else
			{
				cout<<"ͼƬ·���ļ�����"<<endl;
				return -1;
			}
}
CvSeq *GetAreaMaxContour(CvSeq *contour)
{
		double contour_area_temp=0,contour_area_max=0;
		CvSeq * area_max_contour = 0 ;//ָ�������������
		CvSeq* c=0;
		//printf( "Total Contours Detected: %d\n", Nc );
		for(c=contour; c!=NULL; c=c->h_next )
		{//Ѱ�����������������ѭ������ʱ��area_max_contour
				contour_area_temp = fabs(cvContourArea( c, CV_WHOLE_SEQ )); //��ȡ��ǰ�������
				if( contour_area_temp > contour_area_max )
				{
						contour_area_max = contour_area_temp; //�ҵ������������
						area_max_contour = c;//��¼�����������
				}
		}
		return area_max_contour;
}


int getFeatureData(int i,char *path,float trainingData[][FEATURE_NUM])
{
			IplImage* src;
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* contour = 0;
			CvSeq* max_contour = 0;
			CvScalar color = CV_RGB( 0, 255,255 );

			src=cvLoadImage(path,CV_LOAD_IMAGE_GRAYSCALE);//�ԻҶȷ�ʽ����ͼƬ

			//ƽ����ֵ�˲�
			cvSmooth(src, src, CV_MEDIAN, 3, 0,0,0 );

			//��̬ѧ���������븯ʴ
			if(src==NULL)return 0;
			//cvNamedWindow("Morph:Dilatee and Erod", 1);
			IplConvKernel*kernal=cvCreateStructuringElementEx(1,1,0,0,CV_SHAPE_RECT);
			IplConvKernel*kernal1=cvCreateStructuringElementEx(1,1,0,0,CV_SHAPE_RECT);
			IplConvKernel*kernal2=cvCreateStructuringElementEx(1,1,0,0,CV_SHAPE_RECT);
			cvDilate(src,src,kernal);
			cvErode(src,src,kernal2);
			cvDilate(src,src,kernal);
			cvErode(src,src,kernal2);
			cvReleaseStructuringElement(&kernal);
			cvReleaseStructuringElement(&kernal1);
			cvReleaseStructuringElement(&kernal2);

			IplImage* dst = cvCreateImage( cvGetSize(src), 8, 3 );

			//cvThreshold( src, src,51, 255, CV_THRESH_BINARY );//��ֵ��
			cvAdaptiveThreshold( src,src, 255,0,1,91,5 ); //����Ӧ��ֵ��
			//cvNamedWindow( "Source", 1 );
			//cvShowImage( "Source", src );
			//��ȡ����
			cvFindContours( src, storage, &contour, sizeof(CvContour), 0, CV_CHAIN_APPROX_SIMPLE );
			cvZero( dst );//�������

			max_contour =GetAreaMaxContour(contour);//�ҳ����պ����������

			cvDrawContours( dst, max_contour, color, color, -1, 1, 8 );//�����ⲿ���ڲ�������
			//	cvNamedWindow( "contour", 1 );
			//	cvShowImage( "contour", dst );
			//	Rect r0 = boundingRect(max_contour);
			//rectangle(result,r0,Scalar(0),2);
			//��С��Ӿ���
			CvBox2D rect = cvMinAreaRect2(max_contour);
			float height,width;
			height=rect.size.height;
			width=rect.size.width;
			float r=width/height;//����
			//	W_L_Ratio[i]=r;
			trainingData[i][0]=r;
			//�����Բ
			CvBox2D ellipse = cvFitEllipse2(max_contour);//��С���˷�����Բ���
			float focal_len=abs(ellipse.center.x-ellipse.center.y);//����
			float e=focal_len/2/ellipse.size.width;//������
			//	eccentricity[i]=e;
			trainingData[i][1]=e;
			/*�ܳ������
			float s,l;
			for(contour;contour!=NULL;contour=contour->h_next)
			{
			s=cvContourArea(contour,CV_WHOLE_SEQ);
			l=cvArcLength(contour,CV_WHOLE_SEQ,-1);//�������0��ʾ�������պϣ�������ʾ�պϣ�������ʾ����������ɵ��������ȡ�Ľǵ���list��ʽʱ���ø�����

			}
			trainingData[i][5]=l/s;
			//trainingData[i][10]=s;*/

			//����OpenCV������Hu��
			CvMoments moments;
			CvHuMoments hu_moments;
			cvMoments(max_contour, &moments, 0); //���� Opencv �ĺ���ͨ����������ͼ��Ŀռ�غ����ľ�
			cvGetHuMoments(&moments, &hu_moments);//ͨ�����ľ����ͼ���Hu�ش�ŵ�hu�ṹ����

			trainingData[i][2]=(float) hu_moments.hu1;
			trainingData[i][3]=(float)hu_moments.hu2;
			trainingData[i][4]=(float)hu_moments.hu3;
			//trainingData[i][5]=(float)hu_moments.hu4;
			//trainingData[i][6]=(float)hu_moments.hu5;
			//trainingData[i][7]=(float)hu_moments.hu6;
			//trainingData[i][8]=(float)hu_moments.hu7;
			return 0;
}
