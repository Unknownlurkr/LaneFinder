//change from x86 to x64 since we are using x64 openCv
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv; //cv-computer vision 

//min and max HSV filter volumes 
//changed useding trackbars 
int MinH = 0;
int MaxH = 179; 
int MinS = 0; 
int MaxS = 255; 
int MinV = 0;
int MaxV = 255;
int MinC = 120;
int MaxC = 150;

//window names
const String windowName = "Original";
const String windowName1 = "HSV Image";
const String windowName2 = "Threashold Image";
const String adjustmentWindow = "Fine Tuning";

bool Road = true;

/*functions before main*/
//fine tuning window
void on_trackbar(int, void*) {};// called when trackbar has moved 

//Hue values 
void setRoad(int MinHue = 70, int MaxHue = 179, int MinSat = 0, int MaxSat = 87, int MinValue = 0, int MaxValue = 77, int MinCanny = 50, int MaxCanny = 150) 
{
	MinH = MinHue;
	MaxH = MaxHue;
	MinS = MinSat;
	MaxS = MaxSat;
	MinV = MinValue;
	MaxV = MaxValue;
	MinC = MinCanny;
	MaxC = MaxCanny;
}

//
void createTrackBars()
{
	if (Road == true) 
	{
		setRoad();
	}
	namedWindow(adjustmentWindow, 0);

	//creating memory to store track bar names in the window
	char TrackBarName[50];

	//creating track bars into window- fine tune values(open adj window)
	sprintf(TrackBarName, "MinH", MinH);
	sprintf(TrackBarName, "MaxH", MaxH);
	sprintf(TrackBarName, "MinS", MinS);
	sprintf(TrackBarName, "MaxS", MaxS);
	sprintf(TrackBarName, "MinV", MinV);
	sprintf(TrackBarName, "MaxV", MaxV);
	sprintf(TrackBarName, "MinC", MinC);
	sprintf(TrackBarName, "MaxC", MaxC);

	//3 params that are address var. that changes when tracking is changed
	//min and max tracking values
	//function is called once bars are moved

	//title,window,&min/&max, on_trackbar
	createTrackbar("MinH", adjustmentWindow, &MinH, MaxH, on_trackbar);
	createTrackbar("MaxH", adjustmentWindow, &MaxH, MaxH, on_trackbar);
	createTrackbar("MinS", adjustmentWindow, &MinS, MaxS, on_trackbar);
	createTrackbar("MaxS", adjustmentWindow, &MaxS, MaxS, on_trackbar);
	createTrackbar("MinV", adjustmentWindow, &MinV, MaxV, on_trackbar);
	createTrackbar("MaxV", adjustmentWindow, &MaxV, MaxV, on_trackbar);
	createTrackbar("MinC", adjustmentWindow, &MinC, MaxC, on_trackbar);
	createTrackbar("MinC", adjustmentWindow, &MaxC, MaxC, on_trackbar);
}

//main
int main()
{
	//VideoCapture cv lib-capture function call
	VideoCapture capture("roadLane.mp4");//assign vid val into capture
	
	if(!capture.isOpened()) 
	{
		cout << "error";
		return -1;
	}

	//call createTrackBars function
	createTrackBars();

	Mat threshold, capt_Image, HSV_Img, dst;
	while (1)
	{
		//1.get img/vid/camera feed
		capture >> capt_Image;

		//resize the img on the screen-condense img file 500x640
		resize(capt_Image, capt_Image, Size(500,640));

		//definging the region of interest
		//box(x, y, b, c);
		//(x,y)=(100,295) define x and y
		//second corner 
		//(x + b, y + c) = (100 + 220,295 + 185)
		Rect const box(100, 295, 320, 185);

		//2.defing the box-putting box into map 
		Mat ROI = capt_Image(box);//region of interest (ROI)

		//3.Edge detection using canny detector
		GaussianBlur(ROI,dst,Size(3,3),1.5,1.5);

		//second Mat with MORPH_RECT
		Mat k = getStructuringElement(MORPH_RECT, Size(3,3));
		//opencv function-morphologyEx:Preforms advanced Morphlogical Transformations.
		morphologyEx(ROI,ROI,MORPH_CLOSE, k);

		//Canny params-detect edges on the road
		Canny(ROI, dst, MinC, MaxC, 3);//help opencv with edge dection

		//4.convert image to HSV 
		//cvtColor == convert color
		//CONVERT IMG TO HSV
		cvtColor(ROI, HSV_Img, COLOR_RGB2HSV);

		inRange(ROI, Scalar(70, 179, 0),Scalar(87,0,77),threshold);

		//5.use a vector to HoughLinesP
		//Hough transform to DETECT lines in image 
		vector<Vec4i>lines;
		double rho = 1;
		double theta = CV_PI / 180;
		int threshold = 50;
		double minLineLength = 10;
		double maxLineGap = 300;
		HoughLinesP(dst, lines,rho,theta,threshold,minLineLength,maxLineGap);
		//drawing the lines on the screen over the road
		int linethickness = 10;
		int lineType = LINE_4;

		//CYCLING THROUGH
		for(size_t i = 0; i < lines.size(); i++) 
		{
			Vec4i l = lines[i];
			//color for the lines 
			Scalar greenColor = Scalar(0, 250, 0); //green = 200, blue = 0, red = 0
			//causes an error 
			line(ROI, Point(l[0],l[1]), Point(l[2], l[3]), greenColor, linethickness, lineType);
		}

		imshow(windowName, capt_Image);

		if(waitKey(20) == 27)
		{
			cout << "esc key";
			break;
		}

	}
}
