#ifndef EMOTIV_H
#define EMOTIV_H

#include <vector>
#include <ostream>
#include "edk.h"
#include "EEGSignal.h"

class Emotiv
{
public:
	static bool Connect(bool remote = false);
	static void Initialise();
	static int GetData(std::vector<EEGSignal>& channels);
	static void Disconnect();

private:
	Emotiv() {};
	~Emotiv() {};

	static DataHandle data_hndl;
};
#endif

