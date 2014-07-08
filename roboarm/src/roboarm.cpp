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

int sig_len = 1000;
std::vector<EEGSignal> signals {
	EEGSignal("ED_TIMESTAMP", ED_TIMESTAMP, sig_len),
	EEGSignal("ED_COUNTER", ED_COUNTER, sig_len),
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
	EEGSignal("ED_GYROX", ED_GYROX, sig_len),
	EEGSignal("ED_GYROY", ED_GYROY, sig_len),
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

void PutCentredText(const std::string& text, cv::Mat& img, int y, double font_scale = 4.0)
{
	cv::Size txt_sz = getTextSize(text, cv::FONT_HERSHEY_PLAIN, font_scale, 2, NULL);

	putText(img,
	        text,
	        cv::Point(img.cols / 2 - txt_sz.width / 2, y),
	        cv::FONT_HERSHEY_PLAIN,
	        font_scale,
	        cv::Scalar(255, 255, 255),
	        2);
}

int main()
{
	srand(time(NULL));

	ofstream ofs;
	EEGLogger::Open("/home/svetlin/Desktop/roboarm.log", ofs);
	EEGLogger::InitLog("roboarm", signals, ofs);


	char key;

	if(!Emotiv::Connect())
	{
		cout << "Unable to connect to the Emotiv." << endl;
		return 1;
	}

	Emotiv::Initialise();


	cv::Mat roboarm_init = cv::imread("./img/roboarm0.png");
	std::vector<cv::Mat> roboarm = {
		cv::imread("./img/roboarm1.png"),
		cv::imread("./img/roboarm2.png"),
		cv::imread("./img/roboarm3.png"),
		cv::imread("./img/roboarm4.png"),
		cv::imread("./img/roboarm5.png"),
	};
	cv::Mat screen = roboarm_init.clone();


	system_clock::time_point next_tp = system_clock::now(), curr_tp;

	duration<int, std::milli> fixation_dtn(500);

	const int WAIT_SECS = 5;
	const int SEQUENCES = 2; // 5
	const int MOVEMENTS = 5; // 10
	enum MovementState {WAITING, REACHING};
	MovementState curr_state = REACHING;

	int n_sequences = 0;
	while(true)
	{
		if(n_sequences == SEQUENCES) break;

		// Begin sequence with 5 sec count down
		int n_secs = -1;
		std::string progress_bar;
		while(true)
		{
			key = cv::waitKey(1);
			if(key == 'q') break;

			if(n_secs == WAIT_SECS) break;

			curr_tp = system_clock::now();
			if(curr_tp >= next_tp)
			{
				++n_secs;
				progress_bar = "";
				for(int i = 0; i < WAIT_SECS - n_secs; ++i)
					progress_bar += "_ ";

				next_tp += duration<int, std::milli>(1000);

				screen = roboarm_init.clone();
				PutCentredText(progress_bar, screen, 950);
				PutCentredText("+ ", screen, 800, 2.0);
			}

			int n_samples = Emotiv::GetData(signals);
			if(n_samples != 0)
			{
				EEGLogger::LogEEG(n_samples, signals, ofs);
			}

			imshow("EEG Experiment", screen);
		}
		if(key == 'q') break;

		next_tp = system_clock::now();
		int n_movements = 0;
		curr_state = REACHING;
		while(true)
		{
			if(key == 'q') break;

			key = cv::waitKey(1);

			curr_tp = system_clock::now();

			if(curr_tp >= next_tp)
			{
				if(n_movements == MOVEMENTS) break;

				curr_state = (MovementState)((curr_state + 1) % 2);
				switch(curr_state)
				{
					case WAITING:
						screen = roboarm_init.clone();

						PutCentredText("+ ", screen, 800, 2.0);
						next_tp += duration<int, std::milli>(1000);
						break;

					case REACHING:
						int m = rand() % 5;
						screen = roboarm[m].clone();

						PutCentredText("+ ", screen, 800, 2.0);
						next_tp += duration<int, std::milli>(1000);

						++n_movements;

						std::cout << "[roboarm.cpp] seq " << n_sequences + 1 << " mov " << n_movements << std::endl;

						EEGLogger::LogEvent("movement_stimulus", m + 1, ofs);
						break;
				}
			}

			int n_samples = Emotiv::GetData(signals);
			if(n_samples != 0)
			{
				EEGLogger::LogEEG(n_samples, signals, ofs);
			}

			imshow("EEG Experiment", screen);
		}
		if(key == 'q') break;
		++n_sequences;
	}

	EEGLogger::Close(ofs);
}
