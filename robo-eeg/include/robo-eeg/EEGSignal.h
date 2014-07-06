#ifndef EEG_SIGNAL_H
#define EEG_SIGNAL_H

#include <list>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class EEGSignal
{
private:
	// Signal data
	std::string name;
	int id;
	int length;
	std::list<double> data;

	// Signal characteristics
	std::list<double> window;
	double min;
	double max;
	double avg;
	double sum;

	// Plot variables
	cv::Scalar clr;

public:
    EEGSignal(std::string sig_name, int ch_id, int len);
    void AddData(double* new_data, int new_data_len);
    void PlotSignal(cv::Mat& canvas, cv::Point offset = cv::Point(0, 0), double y_scale = 1.0);
    ~EEGSignal();

    int GetChannelID() const {return id;}
    std::string GetChannelName() const {return name;}
	double GetSample(int idx) const;
    double GetSampleRev(int idx) const;
};

#endif
