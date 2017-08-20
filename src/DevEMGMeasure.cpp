#include "DevEMGMeasure.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include <stdio.h>

static std::vector<char>  vecRecv;
static std::vector<char>  vecBuffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int ShowError(long lError, char* lptszMessage)
{
	printf("%s\n(error code %d)", lptszMessage, lError);
	return 1;
}

CDevEMGMeasure::CDevEMGMeasure()
{	
	map_table = (MAP_MAPPINGTABLE *)map_data;

	cnt = 0;
}


CDevEMGMeasure::~CDevEMGMeasure()
{
}


bool CDevEMGMeasure::Serial_initialize(char* port_name)
{
	long    lLastError = 0;

	// Attempt to open the serial port
	comm_fd = open_serial(port_name,460800,10,1);

	if (comm_fd < 0)
	{
		printf(("Unable to open Serial-port\n"));
		return false;
	}
	sprintf(port, "%s", port_name);

	vecRecv.assign(1000, 0);

	return true;
}

void CDevEMGMeasure::Write(unsigned char address, unsigned char length)
{

	unsigned char	dwRead;
	m_txHeader.packet_type = MAP_PACKET_TYPE_WRITE;
	m_txHeader.address = address;
	m_txHeader.length = length;
	long    lLastError = 0;

	memcpy(buf, &m_txHeader, sizeof(MAP_HEADER));
	memcpy(buf + sizeof(MAP_HEADER), &map_data[m_txHeader.address], m_txHeader.length);

	lLastError = write(comm_fd, buf, m_txHeader.length + sizeof(MAP_HEADER));	
	
	if (lLastError < 0)
	{
		//printf("[Write:MAP ERROR]Unable to send data\n");
		return;
	}

	while (1)
	{
		dwRead = read(comm_fd, &vecRecv[0], sizeof(MAP_HEADER));		
		//int dwRead = 1;

		if (dwRead == 0)
		{
			printf("[Write:MAP ERROR]Time out\n");
			return;
		}
		int size = vecBuffer.size();
		if (dwRead == 1)
		{
			vecBuffer.push_back(vecRecv[0]);
		}
		else
		{
			vecBuffer.insert(vecBuffer.end(), vecRecv.begin(), vecRecv.begin() + (dwRead));
		}
		while (vecBuffer.size() >= sizeof(MAP_HEADER))
		{
			memcpy(&m_rxHeader, &vecBuffer[0], sizeof(MAP_HEADER));
			if (m_txHeader.address == m_rxHeader.address && m_txHeader.length == m_rxHeader.length)
			{
				//	printf("[Write:MAP ERROR, ]PACKET ok\n");
				vecBuffer.erase(vecBuffer.begin(), vecBuffer.begin() + (sizeof(MAP_HEADER)));
				return;
			}
			else
			{
				vecBuffer.erase(vecBuffer.begin(), vecBuffer.begin() + 1);
				printf("[Write:MAP ERROR]PACKET error\n");
			}
		}
	}


}
int CDevEMGMeasure::Read(unsigned char address, unsigned char length)
{
	m_txHeader.packet_type = MAP_PACKET_TYPE_READ;
	m_txHeader.address = address;
	m_txHeader.length = length;

	long    lLastError = 0;
	unsigned char dwRead;

	lLastError = write(comm_fd,&m_txHeader, sizeof(MAP_HEADER));

	if (lLastError < 0 )
	{
		printf(("[MAP ERROR]Unable to send data\n"));

		return 0;
	}

	while (1)
	{
		dwRead = read(comm_fd,&vecRecv[0], m_txHeader.length + sizeof(MAP_HEADER));

		if (dwRead == 0)
		{
			printf("[MAP ERROR]Time out\n");
			return 0;
		}
		else if (dwRead == 1)
		{
			vecBuffer.push_back(vecRecv[0]);
		}
		else
		{
			vecBuffer.insert(vecBuffer.end(), vecRecv.begin(), vecRecv.begin() + (dwRead));
		}


		while (vecBuffer.size() >= m_txHeader.length + sizeof(MAP_HEADER))
		{
			memcpy(&m_rxHeader, &vecBuffer[0], sizeof(MAP_HEADER));
			if (m_txHeader.address == m_rxHeader.address && m_txHeader.length == m_rxHeader.length)
			{
				memcpy(&map_data[m_rxHeader.address], &vecBuffer[sizeof(MAP_HEADER)], m_rxHeader.length);
				vecBuffer.erase(vecBuffer.begin(), vecBuffer.begin() + (m_txHeader.length + sizeof(MAP_HEADER)));


				cnt++;

				return cnt;
			}
			else
			{
				vecBuffer.erase(vecBuffer.begin(), vecBuffer.begin() + 1);
			}
		}
	}
}

char CDevEMGMeasure::GetAddress(void *address)
{
//	return (int)address - (int)map_table;
return 0;
}
void CDevEMGMeasure::Lock()
{
	pthread_mutex_lock(&mutex);
}

void CDevEMGMeasure::UnLock()
{
	pthread_mutex_unlock(&mutex);
}
void CDevEMGMeasure::Save(FILE *fp)
{

}
