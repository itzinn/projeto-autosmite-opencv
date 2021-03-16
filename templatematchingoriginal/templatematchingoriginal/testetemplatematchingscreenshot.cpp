#include <fstream>
#include <memory>
#include <string>
#include <iostream>
#include <strstream>
#include <functional>
#include <Windows.h>
#include <iostream>
#include <string>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/calib3d.hpp"

#include <vector>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <chrono>

#include <thread>

using namespace std;
using namespace cv;

int xpos, ypos;

cv::Mat subImage;

double countertest=0;
int iter = 0;

class Timer {
public:
    Timer() {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        Stop();
    }

    void Stop() {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        auto duration = end - start;
        double ms = duration * 0.001;

        countertest += ms;
        iter++;
    }

private:
    std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;
};

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
}

Mat hwnd2mat(HWND hwnd) {

    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
    
    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom;  //change this to whatever size you want to resize to
    width = windowsize.right;

    src.create(height, width, CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

    return src;
}

void pegasubImage(Point left_top, Point right_bottom, Mat image) {
    cv::Range cols(left_top.x, right_bottom.x);
    cv::Range rows(left_top.y, right_bottom.y);
    subImage = image(rows, cols);
}


int pegaLife(Mat subimage) {

    string outText;  
    Mat im = subImage; 
    Mat imBigger;
    
    cv::resize(im, imBigger, cv::Size(im.cols * 4.28, im.rows * 4.28), 0, 0, INTER_LINEAR);

    Mat imGray;

    cvtColor(imBigger, imGray, COLOR_BGR2GRAY);

    //cv::imshow("imGray", imGray);
    //cv::waitKey(1);

    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    ocr->Init(NULL, "eng", tesseract::OEM_DEFAULT);

    ocr->SetPageSegMode(tesseract::PageSegMode(8)); // psm_auto é o melhor?

    ocr->SetImage(imGray.data, imGray.cols, imGray.rows, 1, imGray.step);

    ocr->SetVariable("debug_file", "NUL");

    outText = string(ocr->GetUTF8Text()); //get the output text utilizing tesseract api

    //cout << outText;

    int life = std::stoi(outText);

    //cout << life << endl;

    ocr->End(); //destroy the tesseract object

    return life;
}


void NMultipleTemplateMatching(Mat mInput, Mat mTemplate, float Threshold, float Closeness, vector<Point2f>& List_Matches)
{
    Timer timer;

    Mat mResult;
    Size szTemplate = mTemplate.size();
    Size szTemplateCloseRadius((szTemplate.width / 2) * Closeness, (szTemplate.height / 2) * Closeness);

    matchTemplate(mInput, mTemplate, mResult, TM_CCOEFF_NORMED);
    threshold(mResult, mResult, Threshold, 1.0, THRESH_TOZERO);
    
    double minval, maxval;
    Point minloc, maxloc;
    minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

    if (maxval >= Threshold)
    {
        List_Matches.push_back(maxloc);
        rectangle(mResult, Point2f(maxloc.x - szTemplateCloseRadius.width, maxloc.y - szTemplateCloseRadius.height), Point2f(maxloc.x + szTemplateCloseRadius.width, maxloc.y + szTemplateCloseRadius.height), Scalar(0), -1);
        
    }

}

void smita(int x, int y, HWND hWND) {
    // Get total screen coordinates
    int screen_x = GetSystemMetrics(SM_CXSCREEN);
    int screen_y = GetSystemMetrics(SM_CYSCREEN);

    WINDOWPLACEMENT wp;
    GetWindowPlacement(hWND, &wp);

    long xConverted = (65535 * (x + wp.rcNormalPosition.left)) / screen_x;
    long yConverted = (65535 * (y + wp.rcNormalPosition.top)) / screen_y;

    INPUT iNPUT = { 0 };
    iNPUT.type = INPUT_MOUSE;
    iNPUT.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);
    iNPUT.mi.dx = xConverted;
    iNPUT.mi.dy = yConverted;
    SendInput(1, &iNPUT, sizeof(iNPUT));

    INPUT Input = { 0 };
    Input.type = INPUT_KEYBOARD;
    //Input.ki.wVk = VkKeyScanA('a');
    Input.ki.wVk = NULL;
    Input.ki.wScan = 33; //scanCode 'F'
    Input.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &Input, sizeof(Input));
    ZeroMemory(&Input, sizeof(Input));
    Input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &Input, sizeof(Input));

    //release

    INPUT Input2 = { 0 };
    Input2.type = INPUT_KEYBOARD;
    Input2.ki.wVk = NULL;
    Input2.ki.wScan = 33; //scanCode 'F'
    Input2.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &Input2, sizeof(INPUT));
    
}

vector<Point2f> retornaPontos(Mat img, Mat temp) {

    Mat mTemplate_Bgr, mTemplate_Gray;
    mTemplate_Bgr = temp;

    Mat mSource_Gray, mResult_Bgr = img.clone();

    vector<Point2f> List_Matches;

    float Threshold = 0.7;
    float Closeness = 0.7;

    cvtColor(img, mSource_Gray, COLOR_BGR2GRAY);
    cvtColor(mTemplate_Bgr, mTemplate_Gray, COLOR_BGR2GRAY);

    //imshow("IMAGE", mSource_Gray);
    //imshow("TEMPLATE", mTemplate_Gray);

    //cv::waitKey(0);

    NMultipleTemplateMatching(mSource_Gray, mTemplate_Gray, Threshold, Closeness, List_Matches);

    return List_Matches;
}

int main(int argc, char** argv)
{
    int life;

    vector<Point2f> List_Matches;
    vector<Point2f> List_Ones;

    LPCWSTR window_title = L"League of Legends (TM) Client";
    HWND hDesktopWnd = FindWindow(NULL, window_title);
    Mat boxLifeTemp = imread("template.png", 1);
    //cout << "temp1 type: " << type2str(boxLifeTemp.type()) << endl;
    //Mat oneTemp = imread("1.png", 1);
    //cout << "temp2 type: " << type2str(oneTemp.type()) << endl;

    namedWindow("Final Results", WINDOW_AUTOSIZE);

    int key = 0;
    while (key != 27) {
        Mat mScreenShot = hwnd2mat(hDesktopWnd);
        //cout << "img1 type: " << type2str(mScreenShot.type()) << endl;

        List_Matches = retornaPontos(mScreenShot, boxLifeTemp);
        if (std::size(List_Matches) != 0) {
            Point left_top = Point(List_Matches[0].x+0.41*boxLifeTemp.cols, List_Matches[0].y+0.5*boxLifeTemp.rows);
            Point right_bottom = Point(List_Matches[0].x + 0.61*boxLifeTemp.cols, List_Matches[0].y + 0.85*boxLifeTemp.rows);

            //DESENHA NA SCREENSHOT
            //rectangle(mScreenShot, left_top, right_bottom, Scalar(0, 255, 0), 1);

            //circle(mScreenShot, left_top, 1, (0, 0, 255), -1);
            //circle(mScreenShot, right_bottom, 1, (0, 0, 255), -1);

            pegasubImage(left_top, right_bottom, mScreenShot);

            life = pegaLife(subImage);

            //cv::imshow("SUBIMAGE", subImage);
            //cv::imshow("Screen Gray Shot", mSource_Gray);

            if (life <= 390 ) {
                xpos = List_Matches[0].x + (boxLifeTemp.cols / 2);
                ypos = List_Matches[0].y + (boxLifeTemp.rows * 3);
                smita(xpos, ypos, hDesktopWnd);
            }
            cv::imshow("Final Results", mScreenShot);
            key = cv::waitKey(1);
        }
    }

    std::cout << "media tempo: " <<(countertest / iter) << std::endl;

    return 0;
}