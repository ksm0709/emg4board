#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"

void callback_emg(const std_msgs::Float32MultiArray::ConstPtr& arr);

FILE *fp = fopen("emgdata.txt","w");

int main(int argc, char **argv)
{
	ros::init(argc,argv,"datasaver");
	ros::NodeHandle nh;

	ros::Subscriber sub = nh.subscribe<std_msgs::Float32MultiArray>("rawEMG",1,callback_emg);

	ROS_INFO("Start to write emg data!");

	ros::spin();

	fclose(fp);

	return 0;
}

void callback_emg(const std_msgs::Float32MultiArray::ConstPtr& arr)
{
	int i = 0;

	for(std::vector<float>::const_iterator it = arr->data.begin(); it != arr->data.end(); ++it )
	{

		fprintf(fp, "%.3f", *it);

		if( it == arr->data.end()-1 )
			fprintf(fp,"\n");
		else
			fprintf(fp," ");

		i++;
	}

	return;
}

