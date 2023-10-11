#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // DONE: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Robot seeking white ball");
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call DriveToTarget service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    
    // DONE: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    // Scan through image
    bool ballFound = false;
    for(int i=0; i < img.height; i++ ){
        for(float j=0.0; j < img.step; j++){
            int pos = i * img.step + j;
            if(img.data[pos] == white_pixel && img.data[pos + 1] == white_pixel && img.data[pos + 2] == white_pixel){
                ballFound = true;
                if(j/img.step < 0.33){
                    // turn left
                    drive_robot(0.0, 0.5);
                    ROS_INFO_STREAM("Turn left.");
                }
                else if(j/img.step > 0.67){
                    // turn right
                    drive_robot(0.0, -0.5);
                    ROS_INFO_STREAM("Turn right.");
                }
                else {
                    // keep straight
                    drive_robot(0.5, 0.0);
                    ROS_INFO_STREAM("Keep straight.");
                }
            }
        }
    }
    if(!ballFound){
        drive_robot(0.0, 0.0);
        ROS_INFO_STREAM("No ball found.");
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