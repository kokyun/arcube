// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <opencv2\opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/calib3d/calib3d_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

void main()
{
    int CHECKERBOARD[2]{ 6, 9 };

    vector<vector<cv::Point3f> > objpoints;			// 각 이미지에 대해 3D 좌표를 저장하는 벡터 선언
    vector<vector<cv::Point2f> > imgpoints;			// 각 이미지에 대해 2D 좌표를 저장하는 벡터 선언
    vector<cv::Point3f> objp;
    for (int i = 0; i < CHECKERBOARD[1]; i++)		// CHECKERBOARD[1] = 6
    {
        for (int j = 0; j < CHECKERBOARD[0]; j++)	// CHECKERBOARD[0] = 9
        {
            objp.push_back(cv::Point3f(j * 3, i * 3, 0));
        }
    }
	vector<cv::String> images;
	string path = "chessboard.png";
    cv::Mat frame, gray;
    vector<cv::Point2f> corner_pts;		// 검출된 check board corner 포인트(2D 좌표)를 담을 벡터 선언
	bool success;						          // findChessboardCorners 되었는지 안 되었는지를 확인하기 위한 용도


	frame = cv::imread("chessboard.png");	// images로부터 한 장씩 받아서 frame으로 읽어옴
	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);		// COLOR 니까 GRAY로 바꾸기 위해 cvtColor 함수를 사용해 변경

	success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

	// bool findChessboardCorners(InputArray image, Size patternSize, OutputArray corners, int flags = CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE )

	/* If desired number of vcorner are detected, we refine the pixel coordinates and display them on the images of checker board*/
	if (success)
	{
		cv::TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);

		cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);	// 주어진 2D point에 대해 더 정제시켜 업데이트

		cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);		// Chessboard Corner 그리기

		objpoints.push_back(objp);
		imgpoints.push_back(corner_pts);
	}
	
	Mat view, rview, map1, map2,imgpts;
	cv::Mat cameraMatrix, distCoeffs, rvec, tvec;	// 파라미터를 구하기 위한 Mat 객체 선언

	cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, rvec, tvec);

	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, gray.size(), 1, gray.size(), 0), gray.size(), CV_16SC2, map1, map2 );
	remap(gray, rview, map1, map2, INTER_LINEAR);
	float tt[8][3] = { 
		{0, 0, 0},{0, 3, 0},{3, 3, 0},{3, 0, 0},
		{0, 0, -3},{0, 3, -3},{3, 3, -3},{3, 0, -3} };
	cv::Mat axis(8, 3, CV_32F, tt);
	//projectPoints(objpoints, rvec, tvec, cameraMatrix, distCoeffs,imgpoints,imgpts);
	cv::drawFrameAxes(frame, cameraMatrix, distCoeffs, rvec, tvec, 23);
	for (int i = 0; i < 49;i++) {
		if (i == 0 || i == 5 || i == 48)
			for (int j = 5; j < 54; j++) {
				if(j == 5 || j == 53 || j == 48)
					cv::line(frame, corner_pts[i], corner_pts[j], Scalar(i*10,i*20,200), 3);
			}
	}
	cv::imshow("Image", rview);			// 해당 번째의 이미지에 대해 CHESSBOARD CORNER 그린걸 창에 띄움
	cv::imwrite("find_corner.png", frame);
	cv::waitKey(0);						// 특별한 입력 있을 때까지 대기


	// extract rotation & translation matrix
	
}
