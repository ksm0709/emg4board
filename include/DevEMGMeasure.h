#pragma once
#include "serial.h"

typedef struct MEMORY_ACCESS_PACKET_HEADER
{
	unsigned char packet_type;
	unsigned char address;
	unsigned char length;
}MAP_HEADER;

typedef struct MEMORY_ACCESS_PACKET_MAPPING_TABLE 
{
	//WRITE
	float     TIME;			    //4     0
	float     EMG_raw[4];      //4x4    20
	short     command;

}MAP_MAPPINGTABLE;

#define MAP_MAXIMUM_ADDRESS 40
#define MAP_MAXIMUM_LENGTH  4 

#define MAP_PACKET_TYPE_READ  'R'
#define MAP_PACKET_TYPE_WRITE 'W'

#define FIFO_BUFFER_SIZE 1000


class CDevEMGMeasure
{
public:
	MAP_HEADER m_txHeader;
	MAP_HEADER m_rxHeader;
	int comm_fd; // File descriptor for Serial comm
	int cnt;
	char buf[1000];
	char port[30];

	unsigned char map_data[MAP_MAXIMUM_ADDRESS];
	
public:
	MAP_MAPPINGTABLE *map_table;
	CDevEMGMeasure();
	~CDevEMGMeasure();

	bool Serial_initialize(char* port_name);
	//void ReadWrite(unsigned char address, unsigned char length);
	int Read(unsigned char address, unsigned char length);
	void Write(unsigned char address, unsigned char length);
	char GetAddress(void * address);	
	void Lock();
	void UnLock();
	void Save(FILE *fp);
};

