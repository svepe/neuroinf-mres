#include "EEGLogger.h"
#include <chrono>
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

bool EEGLogger::Open(const std::string& filename, std::ofstream& ofs)
{
	ofs.open(filename);

	return ofs.is_open();
}

void EEGLogger::Close(std::ofstream& ofs)
{
	ofs.close();
}

void EEGLogger::InitLog(const std::string& task_name, const std::vector<EEGSignal>& sig, std::ofstream& ofs)
{
	ofs << GetTimestampNow() << " ";
	ofs << "Description ";
	ofs << task_name << " ";
	for(size_t s = 0; s < sig.size(); ++s)
		ofs << sig[s].GetChannelID() << " " <<  sig[s].GetChannelName() << " ";
	ofs << std::endl;
}

void EEGLogger::LogEvent(const std::string& type, const std::string& data, std::ofstream& ofs)
{
	ofs << GetTimestampNow() << " ";
	ofs << "Event " << type << " " << data << std::endl;
}

void EEGLogger::LogEEG(int n_samples, const std::vector<EEGSignal>& sig, std::ofstream& ofs)
{
	ofs << GetTimestampNow() << " ";
	ofs << "EEG ";
	ofs << n_samples << std::endl;
	for(int n = 0; n < n_samples; ++n)
	{
		for(size_t s = 0; s < sig.size(); ++s)
			ofs << sig[s].GetSampleRev(n) << " ";
		ofs << std::endl;
	}
}

long long EEGLogger::GetTimestampNow()
{
	return duration_cast<duration<long long, std::micro>>(system_clock::now().time_since_epoch()).count();
}


