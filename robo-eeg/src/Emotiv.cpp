#include "Emotiv.h"

#include <iostream>

DataHandle Emotiv::data_hndl;

bool Emotiv::Connect(bool remote)
{
	if(remote)
	{
		if (EE_EngineRemoteConnect("127.0.0.1", 1726) != EDK_OK)
		{
			return false;
		}
		return true;
	}
	else
	{
		if (EE_EngineConnect() != EDK_OK)
		{
	    	return false;
		}
		return true;
	}
}

void Emotiv::Initialise()
{
	EmoEngineEventHandle eEvent = EE_EmoEngineEventCreate();

	bool enable = false;

	std::cout << "Initialising..." << std::endl;
	do
	{
		int state = EE_EngineGetNextEvent(eEvent);
		if(state == EDK_OK)
		{
			std::cout << "EDK_OK" << std::endl;
			unsigned int userID;
			EE_Event_t eventType = EE_EmoEngineEventGetType(eEvent);
			EE_EmoEngineEventGetUserId(eEvent, &userID);

			// Log the EmoState if it has been updated
			if (eventType == EE_UserAdded)
			{
				std::cout << "Enabled..." << std::endl;
				EE_DataAcquisitionEnable(userID,true);
				enable = true;
			}
		}
	}
	while(!enable);

	data_hndl = EE_DataCreate();
	EE_DataSetBufferSizeInSec(0);
}

int Emotiv::GetData(std::vector<EEGSignal>& channels)
{
	std::vector<std::vector<double>> ret;
	std::vector<double> ch;

	EE_DataUpdateHandle(0, data_hndl);

	unsigned int n_samples = 0;

	EE_DataGetNumberOfSample(data_hndl, &n_samples);

    if (n_samples != 0)
    {

	    double *samples = new double[n_samples];
	    ret.clear();

       	for (size_t c = 0; c < channels.size(); ++c)
       	{
        	EE_DataGet(data_hndl, (EE_DataChannel_t)channels[c].GetChannelID(), samples, n_samples);
            channels[c].AddData(samples, n_samples);
        }

        delete[] samples;
    }

    return n_samples;
}

void Emotiv::Disconnect()
{
	EE_EngineDisconnect();
}
