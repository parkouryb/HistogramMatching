#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>


using namespace std;
using namespace cv;

#define target "test.png"

void _count_pixel(Mat image, int hist[]) {
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}
	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			hist[(int)image.at<uchar>(i, j)] ++;
		}
	}
}

void histDisplay(Mat image, const char* name)
{
	int histogram[256];
	_count_pixel(image, histogram);
	int hist[256];
	for (int i = 0; i < 256; i++)
	{
		hist[i] = histogram[i];
	}
	// draw the histograms
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

	// find the maximum intensity element from histogram
	int max = hist[0];
	for (int i = 1; i < 256; i++) {
		if (max < hist[i]) {
			max = hist[i];
		}
	}

	// normalize the histogram between 0 and histImage.rows

	for (int i = 0; i < 256; i++) {
		hist[i] = ((double)hist[i] / max)*histImage.rows;
	}

	// draw the intensity line for histogram
	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point(bin_w*(i), hist_h), Point(bin_w*(i), hist_h - hist[i]), Scalar(0, 0, 0), 1, 8, 0);
	}

	// display histogram
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, histImage);
}



void _cdf(Mat image, double cdf[]) {
	int hist[256];
	double sumHist[256];
	double A = image.rows * image.cols;
	_count_pixel(image, hist);
	sumHist[0] = hist[0];
	cdf[0] = (double)(sumHist[0] / A);
	for (int i = 1; i < 256; ++i) {
		sumHist[i] = (double)(hist[i] + sumHist[i - 1]);
		cdf[i] = (double)(sumHist[i] / A);
	}
}

void hist_Equal(Mat image, Mat output) {
	double cdf[256];
	_cdf(image, cdf);
	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			output.at<uchar>(i, j) = min(255.0, 255 * cdf[image.at<uchar>(i, j) + 1]);
		}
	}
}

Mat Matching(Mat image_input, Mat image_target) {
	double PI[256], PJ[256];
	_cdf(image_input, PI);
	_cdf(image_target, PJ);
	Mat image_output = image_input.clone();
	int lut[256];
	for (int i = 0; i < 256; ++i) lut[i] = 0;
	for (int i = 0; i < 256; ++i) {
		for (int j = 1; j < 256; ++j) {
			if (PI[i] > PJ[j - 1] && PI[i] <= PJ[j]) lut[i] = j;
		}
	}
	for (int i = 0; i < image_input.rows; ++i) {
		for (int j = 0; j < image_input.cols; ++j) {
			image_output.at<uchar>(i, j) = lut[image_input.at<uchar>(i, j)];
		}
	}

	return image_output;
}

void ImageProcess() {
	freopen("list.txt", "r", stdin);
	string dir = "C:/Users/DELL/Documents/hahieu/HistogramMatching/build/imgsSrc/";
	string _dir = "C:/Users/DELL/Documents/hahieu/HistogramMatching/build/imgsDst/";
	string _dir_ = "C:/Users/DELL/Documents/hahieu/HistogramMatching/build/imgsDst_2/";
	string input = "CT_Image_Storage-_Chest_0068-257.png";
	string _Name[200];
	string s;
	int n = 0;
	while (getline(cin, s)) {
		++n;
		_Name[n] = s;
	}
	Mat imageTarget = imread("target.png", IMREAD_GRAYSCALE);
	
	for (int i = 1; i <= n; ++i) {
		Mat imageSrc = imread(dir + _Name[i], IMREAD_GRAYSCALE);
		Mat imageDst, imageDst_2;
		imageDst = Matching(imageSrc, imageTarget);
		//equalizeHist(imageSrc, imageDst_2);
		vector<int> compression_params;
		compression_params.push_back(IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);

		imwrite(_dir + _Name[i], imageDst, compression_params);
		//imwrite(_dir_ + _Name[i], imageDst_2, compression_params);
		cout << i << " : Done\n";
		
	}
	waitKey(0);
}

int main() {
	ImageProcess();
	return 0;
}

