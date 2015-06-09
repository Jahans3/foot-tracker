#define _CRT_SECURE_NO_DEPRECATE

#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <Windows.h>
#include <tchar.h>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include "foot.h"

using namespace cv;

//Initial HSV values
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//Timer on/off values
//(cv::createButton appears broken so this is a quick fix)
int T_OFF = 0;
int T_ON = 2;

//Close program values
int P_ON = 0;
int P_OFF = 2;

//Window height and width
const int F_WIDTH = 640;
const int F_HEIGHT = 480;

//Max number of objects to prevent noise interferance
const int MAX_OBJECTS = 5;

//Min and max area of object
const int MIN_OBJ_AREA = 20 * 20;
const int MAX_OBJ_AREA = F_HEIGHT*F_WIDTH / 1.5;

//Window names
const string w_orig = "Original";
const string w_HSV = "HSV";
const string w_thres = "Thresholded Binary";
const string w_morph = "After Morphological Operations";
const string w_sliders = "HSV Sliders";

//Log file
ofstream logfile;

//Boolean to tell function when to snapshot coordinates
bool recordData = false;

//Convert an integer to a string
string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

//Empty function called whenever slider is adjusted
void on_trackbar(int, void*)
{}

//Create our adjustable HSV sliders
//One slider will activate the timer
void trackbars(){

	//Create window for sliders
	namedWindow(w_sliders, 0);

	//Assign memory for individual slider names
	char slider_name[50];
	sprintf(slider_name, "Hue min", H_MIN);
	sprintf(slider_name, "Hue max", H_MAX);
	sprintf(slider_name, "Sat. min", S_MIN);
	sprintf(slider_name, "Sat. max", S_MAX);
	sprintf(slider_name, "Value min", V_MIN);
	sprintf(slider_name, "Value max", V_MAX);
	sprintf(slider_name, "Timer", T_OFF); //Timer slider
	sprintf(slider_name, "Exit", P_ON); //Exit slider
	
	//Create the sliders   
	createTrackbar("Hue min", w_sliders, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("Hue max", w_sliders, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("Sat. min", w_sliders, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("Sat. max", w_sliders, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("Value min", w_sliders, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("Value max", w_sliders, &V_MAX, V_MAX, on_trackbar);
	createTrackbar("Timer", w_sliders, &T_OFF, T_ON, on_trackbar); //Timer slider
	createTrackbar("Exit", w_sliders, &P_ON, P_OFF, on_trackbar); //Exit slider
}

//Timer function
void timerOnOff(){

	
}

//Function to handle exit
void exitProgram(){

	
}

//Function to save foot data
void saveFootData(int leftX, int leftY, int rightX, int rightY, int distance){

	//Counter
	int counter = 0;

	//String for coordinates
	//Format: left X, left Y, right X, right Y, distance between feet
	//Save as CSV
	string coords = intToString(leftX) + ", " + intToString(leftY) + ", " + intToString(rightX) + ", " + intToString(rightY) + ", " + intToString(distance) + "\n";

	//If 1 minute has passed
	if (recordData == true){
			//Write coordinates to log file
			logfile << coords;

			//Increment counter
			counter++;
			//After 30 mins of recording exit program
			if (counter == 3){
				//Close the log file
				logfile.close();

				//Exit with no errors
				exit(0);
			}
			recordData = false;
	}
}

//Draw markers showing tracked objects
void drawObject(vector<Foot> feet, Mat &frame){

	//Unpack vector of feet
	for (int i = 0; i < feet.size(); i++){

		//Draw a circle over point
		circle(frame, Point(feet.at(i).getX(), feet.at(i).getY()), 20, Scalar(0, 255, 0), 2);

		//Write text at point displaying coordinates of foot
		putText(frame, intToString(feet.at(i).getX()) + "," + intToString(feet.at(i).getY()),
			Point(feet.at(i).getX(), feet.at(i).getY() + 30), 1, 1, Scalar(0, 255, 0), 2);
	}
}

//Apply dliation and erosion to thresholded binary image
void morphOps(Mat &thres){

	//Create structuring elements for dilation and erosion
	Mat erode_m = getStructuringElement(MORPH_RECT, Size(3, 3)); //3x3 square for erosion
	Mat dilate_m = getStructuringElement(MORPH_RECT, Size(8, 8)); //8x8 square for dilation

	erode(thres, thres, erode_m);
	dilate(thres, thres, dilate_m);
	//erode(thres, thres, erodeElement);
	//dilate(thres, thres, dilateElement);



}
void trackFilteredObject(int timer, int &x, int &y, Mat threshold, Mat &video){

	//Vector to store both feet in
	std::vector<Foot> feet;
	//Vector<Foot> feet;

	Mat temp;
	threshold.copyTo(temp);

	//For findContours output
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Find contours of filtered image
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	//Find our filtered object
	bool objectFound = false;
	if (hierarchy.size() > 0) {

		int numObjects = hierarchy.size();

		//If we aren't exceeding max number of objects
		if (numObjects<MAX_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//If the object is within the size limits
				//Objects within this limit are unlikely to be noise
				if (area>MIN_OBJ_AREA && area<MAX_OBJ_AREA){
					Foot foot;
					foot.setX(moment.m10 / area);
					foot.setY(moment.m01 / area);

					feet.push_back(foot);

					//right.setX(moment.m10 / area);
					//right.setY(moment.m01 / area);

					//x = moment.m10 / area;
					//y = moment.m01 / area;
					objectFound = true;
				}
				else objectFound = false;


			}

			//Draw our object
			if (objectFound == true){
				drawObject(feet, video);
			}

		}
		//Tell user to reduce noise if too many objects are detected
		else putText(video, "Reduce noise - adjust HSV values", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}

	//Get distance between feet
	//Gets first two tracked objects, assuming feet
	int x1, y1, x2, y2;
	int distance;

	//If timer is turned on begin recording coordinates
	if (timer > 1){
		//If more than one object is tracked get X and Y of 1st and 2nd tracked objects
		if (feet.size() > 1){
			x1 = feet.at(0).getX();
			y1 = feet.at(0).getY();
			x2 = feet.at(1).getX();
			y2 = feet.at(1).getY();

			//Pythagoras calculation to get distance
			distance = (int)sqrt((double)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)));

			//Convert distance to a string and print to window
			putText(video, intToString(distance), Point(50, 50), 1, 4, Scalar(255, 0, 0));

			//Save our coordinates and distance
			saveFootData(x1, y1, x2, y2, distance);
		}
	}
	
}
int main(int argc, char* argv[])
{
	//Matrices to store video, HSV and binary frames
	Mat video;
	Mat HSV;
	Mat threshold;

	//X and Y coordinates of tracked object
	int x = 0;
	int	y = 0;

	//Call sliders for HSV-binary thresholding
	trackbars();

	//Create and open the webcam stream
	VideoCapture stream;
	stream.open(0);

	//Set size of frames captured from stream
	stream.set(CV_CAP_PROP_FRAME_WIDTH, F_WIDTH);
	stream.set(CV_CAP_PROP_FRAME_HEIGHT, F_HEIGHT);

	//Open log file
	logfile.open("Log10.txt"); //Had to manually change file name

	//Get system time
	time_t t = time(0);
	time_t base = t;

	//Main loop
	while (true){

		//Read webcam stream
		stream.read(video);
		cvtColor(video, HSV, COLOR_BGR2HSV); //Convert webcam stream to HSV image

		//Filter HSV image and store result in threshold matrix
		inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
		
		//Dilation and erosion
		morphOps(threshold);

		//Object tracking
		trackFilteredObject(T_OFF, x, y, threshold, video);

		//show frames 
		imshow(w_thres, threshold);
		imshow(w_orig, video);
		imshow(w_HSV, HSV);

		//Snapshot coordinates and distance between feet
		if (base = base + 60){
			recordData = true;
			base = time(0);
		}

		//Delay so screen can refresh
		waitKey(30);

		//Exit function
		//exitProgram();
		if (P_ON > 1){
			//Close the log file
			logfile.close();

			//Exit with no errors
			exit(0);
		}
	}
	
	//return 0;
}
