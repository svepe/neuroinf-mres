#include <vector>
#include <chrono>
#include <iostream>

using std::cout;
using std::endl;
using std::ofstream;
using std::chrono::duration;
using std::chrono::system_clock;

// usleep
#include <unistd.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// RoboEEG
#include <robo-eeg/Emotiv.h>
#include <robo-eeg/EEGSignal.h>
#include <robo-eeg/EEGLogger.h>

const int TRIALS = 40;

int sig_len = 1000;
std::vector<EEGSignal> signals {
	// EEGSignal("ED_COUNTER", ED_COUNTER, sig_len),
	EEGSignal("ED_AF3", ED_AF3, sig_len),
	EEGSignal("ED_F7", ED_F7, sig_len),
	EEGSignal("ED_F3", ED_F3, sig_len),
	EEGSignal("ED_FC5", ED_FC5, sig_len),
	EEGSignal("ED_T7", ED_T7, sig_len),
	EEGSignal("ED_P7", ED_P7, sig_len),
	EEGSignal("ED_O1", ED_O1, sig_len),
	EEGSignal("ED_O2", ED_O2, sig_len),
	EEGSignal("ED_P8", ED_P8, sig_len),
	EEGSignal("ED_T8", ED_T8, sig_len),
	EEGSignal("ED_FC6", ED_FC6, sig_len),
	EEGSignal("ED_F4", ED_F4, sig_len),
	EEGSignal("ED_F8", ED_F8, sig_len),
	EEGSignal("ED_AF4", ED_AF4, sig_len),
	// EEGSignal("ED_GYROX", ED_GYROX, sig_len),
	// EEGSignal("ED_GYROY", ED_GYROY, sig_len),
	// EEGSignal("ED_TIMESTAMP", ED_TIMESTAMP, sig_len),
	// EEGSignal("ED_FUNC_ID", ED_FUNC_ID, sig_len),
	// EEGSignal("ED_FUNC_VALUE", ED_FUNC_VALUE, sig_len),
	// EEGSignal("ED_MARKER", ED_MARKER, sig_len),
	// EEGSignal("ED_SYNC_SIGNAL", ED_SYNC_SIGNAL, sig_len)
};

void PlotSignals(const std::vector<EEGSignal>& sig)
{
	cv::Mat canvas(900, 1000, CV_8UC3);
	rectangle(canvas,
			cv::Point(0, 0),
			cv::Point(canvas.cols, canvas.rows),
			cv::Scalar(0, 0, 0),
			CV_FILLED);

	for(size_t s = 0; s < signals.size(); ++s)
			signals[s].PlotSignal(canvas, cv::Point(0, (s + 1) * 60), 0.2);

	imshow("EEG data", canvas);
}

void PutCentredText(const std::string& text, cv::Mat& img)
{
	cv::Size txt_sz = getTextSize(text, cv::FONT_HERSHEY_PLAIN, 4.0, 2, NULL);

	putText(img,
	        text,
	        cv::Point(img.cols / 2 - txt_sz.width / 2, img.rows / 2 - txt_sz.height / 2),
	        cv::FONT_HERSHEY_PLAIN,
	        4.0,
	        cv::Scalar(255, 255, 255),
	        2);
}

int main()
{
	srand(time(NULL));
	// std::cout << "[flanker.cpp]  " << EEGLogger::GetTimestampNow() << std::endl;

	ofstream ofs;
	EEGLogger::Open("/home/svetlin/Desktop/exp.log", ofs);
	EEGLogger::InitLog("flanker", signals, ofs);

	char key;
	// double data[2];
	// int i = 0;

	if(!Emotiv::Connect())
	{
		cout << "Unable to connect to the Emotiv." << endl;
		return 1;
	}

	Emotiv::Initialise();

	cv::Mat screen = cv::Mat::zeros(1000, 1920, CV_8UC3);

	duration<int, std::milli> fixation_dtn(500);
	duration<int, std::milli> stimulus_dtn(100);
	duration<int, std::milli> waiting_dtn(1000);
	duration<int, std::milli> resting_dtn(500);

	enum ExperimentState {FIXATION = 0, STIMULUS, WAITING, RESTING};
	ExperimentState current_state = RESTING;
	bool update = false;
	system_clock::time_point next_tp = system_clock::now(), curr_tp, waiting_tp;

	std::vector<std::string> stimuli {"<<<<<", ">>>>>", ">><>>", "<<><<"};
	std::string waiting_str;

	int n_trials = 0;

	while(true)
	{
		key = cv::waitKey(1);
		if(key == ' ') break;

		PutCentredText("+", screen);
		imshow("EEG Experiment", screen);
	}

	while(true)
	{
		key = cv::waitKey(1);
		if(key == 'q') break;

		if(n_trials == TRIALS) break;

		curr_tp = system_clock::now();
		if(current_state != WAITING && curr_tp >= next_tp)
		{
			current_state = (ExperimentState)((current_state + 1) % 4);
			update = true;
		}
		if(current_state == WAITING && (key == 81 || key == 83)) // 81 left arrow; 83 right arrow
		{
			EEGLogger::LogEvent("user_input", (key == 81) ? "left" : "right", ofs);
			current_state = (ExperimentState)((current_state + 1) % 4);
			update = true;
		}

		switch(current_state)
		{
			case FIXATION:
			{
				if(!update) break;
				update = false;

				++n_trials;

				screen = cv::Mat::zeros(1000, 1920, CV_8UC3);
				PutCentredText("+", screen);

				next_tp = curr_tp + fixation_dtn;
				break;
			}

			case STIMULUS:
			{
				if(!update) break;
				update = false;

				screen = cv::Mat::zeros(1000, 1920, CV_8UC3);

				int s = rand() % 4;

				PutCentredText(stimuli[s], screen);

				next_tp = curr_tp + stimulus_dtn;

				EEGLogger::LogEvent("flanker_stimulus", stimuli[s], ofs);
				break;
			}

			case WAITING:
				if(update)
				{
					waiting_tp = curr_tp;
					waiting_str = "";
					update = false;
				}

				if(curr_tp >= waiting_tp)
				{
					screen = cv::Mat::zeros(1000, 1920, CV_8UC3);

					waiting_str += std::string("-");
					PutCentredText(waiting_str, screen);
					waiting_tp = curr_tp + waiting_dtn;
				}

				break;

			case RESTING:
				if(!update) break;
				update = false;

				screen = cv::Mat::zeros(1000, 1920, CV_8UC3);
				next_tp = curr_tp + resting_dtn;
				break;
		}


		int n_samples = Emotiv::GetData(signals);
		if(n_samples != 0)
		{
			EEGLogger::LogEEG(n_samples, signals, ofs);
			// PlotSignals(signals);
		}

		imshow("EEG Experiment", screen);
	}

	EEGLogger::Close(ofs);
}
