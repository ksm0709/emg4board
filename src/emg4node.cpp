#include "ros/ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Float32MultiArray.h"

#include "DevEMGMeasure.h" 

int main(int argc, char **argv)
{
	ros::init(argc,argv,"emg4_node");
	ros::NodeHandle nh;

	ros::Publisher pub = nh.advertise<std_msgs::Float32MultiArray>("rawEMG",10);
	ros::Rate loop_rate(500);

	std_msgs::Float32MultiArray pub_msg;
	CDevEMGMeasure* board;
	int cnt[10] = {NULL}, stat[10] = {NULL}, i;
	int board_num;
	char port_name[30];

	// publishing 되는 데이터의 규격 설정
//	pub_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
//	pub_msg.layout.dim[0].size = 4;
//	pub_msg.layout.dim[0].stride = 1;
//	pub_msg.layout.dim[0].label = "emg4_raw";

	if( argc > 1 )
	{
		board = new CDevEMGMeasure[argc-1];
		board_num = argc-1;

		for(i = 1; i < argc; i++ )
			if(	!board[i-1].Serial_initialize(argv[i]) )
			{
				ROS_INFO("Serial Initialization FAIL!! [ %s ]", argv[i]); 	
				goto EXIT;
			}
			else
			{
				ROS_INFO("Serial Initialization done! [ %s ]", argv[i]);
			}
	}
	else
	{
		board = new CDevEMGMeasure;
		board_num = 1;

		if( !board[0].Serial_initialize("/dev/ttyUSB0") )
		{
			ROS_INFO("Serial Initialization FAIL!! [ /dev/ttyUSB0 ]"); 	
			goto EXIT;
		}
	}

	while( ros::ok() )
	{
		pub_msg.data.clear();

		for( i=0 ; i < board_num; i++ )
		{
			board[i].Lock();
			stat[i] = board[i].Read(4, 16);			
			board[i].UnLock();

			pub_msg.data.push_back( board[i].map_table->EMG_raw[0] / 300.0f * 1000.0f );
			pub_msg.data.push_back( board[i].map_table->EMG_raw[1] / 300.0f * 1000.0f );
			pub_msg.data.push_back( board[i].map_table->EMG_raw[2] / 300.0f * 1000.0f );
			pub_msg.data.push_back( board[i].map_table->EMG_raw[3] / 300.0f * 1000.0f );

			if( ++cnt[i] > 499 )
			{
				if( stat[i] > 0 )
					ROS_INFO("%s : OK [ PACKET COUNT : %d ]",board[i].port,stat[i]);
				else
				{
					ROS_INFO("%s : FAIL !!",board[i].port);
					goto EXIT;
				}

				cnt[i] = 0;
			}	
		}

		pub.publish( pub_msg );

		loop_rate.sleep();
		ros::spinOnce();
	}

EXIT:

	if( board != NULL )
	{
		if( board_num > 1 )
			delete[] board;
		else
			delete board;
	}

	return 0;
}

