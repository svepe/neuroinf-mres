#include "EEGSignal.h"

#include <cstdlib>
#include <iterator>

EEGSignal::EEGSignal(std::string sig_name, int ch_id, int len)
{
	// Signal variables
	name = sig_name;
	length = len;
	id = ch_id;

	// Plot variables
	clr = cv::Scalar(rand() % 255, rand() % 255, rand() % 255);

	sum = 0;
}

EEGSignal::~EEGSignal()
{

}

void EEGSignal::AddData(double* new_data, int new_data_len)
{
	// Forget old signal values
	while((int)data.size() + new_data_len > length)
	{
		sum -= data.front();
		data.pop_front();
	}

	for(int i = 0; i < new_data_len; ++i)
	{
		data.push_back(new_data[i]);
		sum += data.back();
	}

	if(data.size() != 0)
		avg = sum / data.size();
}

void EEGSignal::PlotSignal(cv::Mat& canvas, cv::Point offset, double y_scale)
{

	int x = 1;

	auto it_old = data.begin();
	auto it_new = data.begin();
	++it_new;

	while(it_new != data.end())
	{
		cv::line(canvas,
			cv::Point(offset.x + x - 1, offset.y + y_scale * (*it_old - avg)),
			cv::Point(offset.x + x, offset.y + y_scale * (*it_new - avg)),
			clr, 1);
		++x;
		++it_old;
		++it_new;
	}
}

double EEGSignal::GetSample(int idx) const
{
	auto rit = data.begin();
	std::advance(rit, idx);
	return *rit;
}

double EEGSignal::GetSampleRev(int idx) const
{
	auto rit = data.rbegin();
	std::advance(rit, idx);
	return *rit;
}
