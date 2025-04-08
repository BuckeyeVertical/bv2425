#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <ctime>

int main()
{
    cv::VideoCapture cap(0);
    
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }
    
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(cv::CAP_PROP_FPS);
    
    // Use 30 fps if the camera doesn't report a valid fps value
    if (fps <= 0) fps = 30.0;
    
    // Generate a filename with timestamp
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char buffer[128];
    strftime(buffer, sizeof(buffer), "recording_%Y-%m-%d_%H-%M.mp4", now);
    std::string filename = buffer;
    
    // Define the codec and create VideoWriter object
    cv::VideoWriter writer(
        filename,                              // Filename
        cv::VideoWriter::fourcc('a','v','c','1'), // H.264 codec
        fps,                                   // Frames per second
        cv::Size(frame_width, frame_height)    // Frame size
    );

    if (!writer.isOpened()) {
        std::cerr << "Error: Could not open the output video file for writing." << std::endl;
        return -1;
    }
    
    std::cout << "Recording started. Press 'q' to stop and save the video." << std::endl;
    
    cv::Mat frame;
    while (true) {
        // Capture frame-by-frame
        cap >> frame;
        
        // Check if frame is empty
        if (frame.empty()) {
            std::cerr << "Error: Blank frame grabbed." << std::endl;
            break;
        }
        
        // Write the frame to the output file
        writer.write(frame);
        
        // Display the frame
        cv::imshow("Recording", frame);
        
        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            std::cout << "Stopping recording..." << std::endl;
            break;
        }
    }
    
    // Release resources
    cap.release();
    writer.release();
    cv::destroyAllWindows();
    
    std::cout << "Video saved as: " << filename << std::endl;
    
    return 0;
}



