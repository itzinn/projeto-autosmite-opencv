/*#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

#include <vector>
#include <Windows.h>

using namespace std;
using namespace cv;
bool use_mask;
cv::Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;

void MatchingMethod(int, void*);

string type2str(int type) {
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
    }

    r += "C";
    r += (chans + '0');

    return r;
/*}

cv::Mat getMat(HWND hWND) {

    HDC deviceContext = GetDC(hWND);
    HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

    RECT windowRect;
    GetClientRect(hWND, &windowRect);

    int height = windowRect.bottom;
    int width = windowRect.right;

    HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);

    SelectObject(memoryDeviceContext, bitmap);

    //copy data into bitmap
    BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);


    //specify format by using bitmapinfoheader!
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0; //because no compression
    bi.biXPelsPerMeter = 1; //we
    bi.biYPelsPerMeter = 2; //we
    bi.biClrUsed = 3; //we ^^
    bi.biClrImportant = 4; //still we

    cv::Mat mat = cv::Mat(height, width, CV_8UC3); // 8 bit unsigned ints 4 Channels -> RGBA

    //transform data and store into mat.data
    GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //clean up!
    DeleteObject(bitmap);
    DeleteDC(memoryDeviceContext); //delete not release!
    ReleaseDC(hWND, deviceContext);

    return mat;
}

int main(int argc, char** argv)
{*/
    /*
    if (argc < 3)
    {
        cout << "Not enough parameters" << endl;
        cout << "Usage:\n" << argv[0] << " <image_name> <template_name> [<mask_name>]" << endl;
        return -1;
    }
    */
    /*LPCWSTR window_title = L"League of Legends (TM) Client";
    HWND hWND = FindWindow(NULL, window_title);
    //cv::namedWindow("output", cv::WINDOW_NORMAL);

    templ = imread("healthbar2.png", IMREAD_COLOR);
    if (argc > 3) {
        use_mask = true;
        mask = imread(argv[3], IMREAD_COLOR);
    }
    //img = imread("red3.png", IMREAD_COLOR);

    //cout << "img before convert: " << type2str(img.type()) << endl;
    //cvtColor(img, img, cv::COLOR_BGR2BGRA);
    //cout << "img after convert: " << type2str(img.type()) << endl;
    //cvtColor(img, img, cv::COLOR_BGRA2BGR);

    /*if (img.empty() || templ.empty() || (use_mask && mask.empty()))
    {
        cout << "Can't read one of the images" << endl;
        return EXIT_FAILURE;
    }*//*
    namedWindow(image_window, WINDOW_AUTOSIZE);
    //namedWindow(result_window, WINDOW_AUTOSIZE);
    int key = 0;
    
    while (key != 27)
    {
        img = getMat(hWND);
        MatchingMethod(0, 0);

        key = cv::waitKey(30);
    }
}

void MatchingMethod(int, void*)
{
        cout << "img after convert: " << type2str(img.type()) << endl;
        int result_cols = img.cols - templ.cols + 1;
        int result_rows = img.rows - templ.rows + 1;
        result.create(result_rows, result_cols, CV_32FC1);
        bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
        //if (use_mask && method_accepts_mask)
        //{
        //    matchTemplate(img, templ, result, match_method, mask);
        //}
        //else
        //{
        matchTemplate(img, templ, result, match_method);
        //}
        normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        Point matchLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
        if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
        {
            matchLoc = minLoc;
        }
        else
        {
            matchLoc = maxLoc;
        }
        rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
        //rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
        imshow(image_window, img);
        //imshow(result_window, result);
        return;
    }*/