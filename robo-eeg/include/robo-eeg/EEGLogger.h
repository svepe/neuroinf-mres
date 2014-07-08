#ifndef EEG_LOGGER_H
#define EEG_LOGGER_H

#include <string>
#include <vector>
#include <chrono>
#include <fstream>

#include "EEGSignal.h"

class EEGLogger
{
public:
	static bool Open(const std::string& filename, std::ofstream& ofs);
	static void Close(std::ofstream& ofs);

	static void InitLog(const std::string& task_name, const std::vector<EEGSignal>& sig, std::ofstream& ofs);
	static void LogEEG(int n_samples, const std::vector<EEGSignal>& sig, std::ofstream& ofs);

	static void LogEvent(const std::string& type, const std::string& data, std::ofstream& ofs);
	static void LogEvent(const std::string& type, const int data, std::ofstream& ofs);


	static long long GetTimestampNow();

private:
	EEGLogger() {};
	~EEGLogger() {};
};

#endif

