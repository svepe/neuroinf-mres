#include <vector>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::system_clock;

int main(int argc, char const *argv[])
{
	if(argc != 2)
	{
		std::cout << "Usage: logs-converter [filename] " << std::endl;
		return 1;
	}
	std::string filename(argv[1], strcspn(argv[1], ".")); // Remove .log extension

	std::string log_filename(filename);
	log_filename += ".log";
	ifstream log_ifs(log_filename);
	if(!log_ifs.is_open())
	{
		std::cout << "[converter.cpp] Unable to open file: " << log_filename << std::endl;
		return 1;
	}

	std::string eeg_filename(filename);
	eeg_filename += ".eeg";
	ofstream eeg_ofs(eeg_filename);
	if(!eeg_ofs.is_open())
	{
		std::cout << "[converter.cpp] Unable to open file: " << eeg_filename << std::endl;
		return 1;
	}

	std::string event_filename(filename);
	event_filename += ".event";
	ofstream event_ofs(event_filename);
	if(!event_ofs.is_open())
	{
		std::cout << "[converter.cpp] Unable to open file: " << event_filename << std::endl;
		return 1;
	}

	long long timestamp, initial_timestamp;
	std::string type;

	log_ifs >> timestamp;
	initial_timestamp = timestamp;

	log_ifs >> type;
	if(type != "Description")
	{
		std::cout << "[converter.cpp] Corrupted log file. " << std::endl;
		return 1;
	}

	std::string exp_name;
	log_ifs >> exp_name;

	std::cout << "[converter.cpp] Processing experiment: " << exp_name << std::endl;

	std::string line;
	std::getline(log_ifs, line);
	std::stringstream line_ss(line);

	eeg_ofs << "TIMESTAMP ";

	while(!line_ss.eof())
	{
		int signal_id;
		line_ss >> signal_id;

		std::string signal_name;
		line_ss >> signal_name;

		eeg_ofs << signal_name << " ";

		std::cout << "[converter.cpp] " << signal_name << std::endl;
	}

	eeg_ofs << std::endl;

	unsigned long long sample_time =
		duration_cast<duration<long long, std::micro>>(duration<int,std::ratio<1,128>>(1)).count();

	while(!log_ifs.eof())
	{
		log_ifs >> timestamp;
		if(!log_ifs) break;

		timestamp -= initial_timestamp;

		log_ifs >> type;

		if(type == "EEG")
		{
			int samples;
			log_ifs >> samples;
			log_ifs.ignore(); // ignore the new line after the number of samples

			for(int i = samples - 1; i >= 0; --i)
			{
				eeg_ofs << timestamp - i * sample_time << " ";

				std::getline(log_ifs, line);
				eeg_ofs << line << std::endl;
			}
		}
		else if(type == "Event")
		{
			std::getline(log_ifs, line);
			std::stringstream ss(line);
			std::string event_type, event_data;
			ss >> event_type >> event_data;

			event_ofs << timestamp << " ";
			if(event_type == "flanker_stimulus")
			{
				event_ofs << "0 ";
				if(event_data == "<<<<<")
					event_ofs << "0";
				else if(event_data == ">>>>>")
					event_ofs << "1";
				else if(event_data == ">><>>")
					event_ofs << "2";
				else if(event_data == "<<><<")
					event_ofs << "3";
			}
			else if(event_type == "user_input")
			{
				event_ofs << "1 ";
				if(event_data == "left")
					event_ofs << "0";
				else if(event_data == "right")
					event_ofs << "1";
			}
			else if (event_type == "movement_stimulus")
			{
				event_ofs << "2 " << event_type;
			}

			event_ofs << std::endl;
		}
	}

	return 0;
}
