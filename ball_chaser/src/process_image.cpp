#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Directing the robot to the target");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the drive_to_target and pass the requested velocities
    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive_to_target");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int target = -1;
    int left_ball_side = -1;
    int right_ball_side = -1;

    for (int i = 0; i < img.height * img.step - 2; i+=3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
        {
            target = i % img.step;
            break;    
        }
    }

    int left_border = img.step / 6;
    int right_border = img.step - img.step / 6;


    ROS_INFO_STREAM("Step: " + std::to_string(img.step) + " Right: " + std::to_string(right_border) + " Left: " + std::to_string(left_border) + " Target: " + std::to_string(target));

    if (target == -1)
    {
        drive_robot(0, 0);
    } else    // Left side
    if (target < left_border)
    {
        drive_robot(0.2, 0.4);
    } else    // Center
    if (target >= left_border && target <= right_border)
    {
        drive_robot(0.5, 0);
    } else 
    // Right side
    if (target > right_border)
    {
        drive_robot(0.2, -0.4);
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