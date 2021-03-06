#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

#include <ros/console.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // DONE: Request a service and pass the velocities to it to drive the robot
    ROS_INFO("lin_x: %f", lin_x);
    ROS_INFO("ang_z: %f", ang_z);

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    ROS_INFO("processing_image");
    
    ROS_INFO("img.step: %i", img.step);
    ROS_INFO("img.height: %i", img.height);
    ROS_INFO("img.width: %i", img.width);
    
    int white_pixel = 255;

    bool red_pixel = false;
    bool green_pixel = false;
    bool blue_pixel = false;

   // DONE: Loop through each pixel in the image and check if there's a bright white one
   // Then, identify if this pixel falls in the left, mid, or right side of the image
   // Depending on the white ball position, call the drive_bot function and pass velocities to it
   // Request a stop when there's no white ball seen by the camera

   bool found_ball = false;
   int left_cutoff = img.step / 3;
   int right_cutoff = img.step / 3 * 2;

   // Loop through each pixel in the image and check if its equal to the first one
   for (int i = 0; i < img.height * img.step; i=i+3) {

       if ( (img.data[i] - white_pixel == 0) && (img.data[i+1] - white_pixel == 0) && (img.data[i+2] - white_pixel == 0) )

       {
           ROS_INFO("found white");
           found_ball = true;

           int row_position = i % img.step;
           ROS_INFO("rowPosition: %i", row_position);

           if (row_position < left_cutoff)
           {
               ROS_INFO("turning left");
	       drive_robot(0.5, 0.5);
           }

           if ((row_position > left_cutoff) && (row_position < right_cutoff))
           {
               ROS_INFO("go straight");
	       drive_robot(0.5, 0);
           }

 
           if (row_position > right_cutoff)
           {
               ROS_INFO("turning right");
	       drive_robot(0.5, -0.5);
           }
	
           break;
       }
    }

    if (found_ball == false)
    {
        ROS_INFO("stop");
	drive_robot(0., 0.);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
