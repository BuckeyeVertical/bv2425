#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

//#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>

// Function to split a frame into 1080p chunks with overlap
std::vector<cv::Mat> splitFrameIntoChunks(const cv::Mat& frame, int chunkWidth, int chunkHeight, float overlapPercent) {
    std::vector<cv::Mat> chunks;
    
    // Calculate the step size (how much to move for each new chunk) considering overlap
    int stepX = static_cast<int>(chunkWidth * (1.0 - overlapPercent));
    int stepY = static_cast<int>(chunkHeight * (1.0 - overlapPercent));
    
    // Make sure step size is at least 1 pixel
    stepX = std::max(1, stepX);
    stepY = std::max(1, stepY);
    
    // Iterate through the frame, extracting chunks
    for (int y = 0; y <= frame.rows - chunkHeight; y += stepY) {
        for (int x = 0; x <= frame.cols - chunkWidth; x += stepX) {
            // Extract ROI (Region of Interest)
            cv::Rect roi(x, y, chunkWidth, chunkHeight);
            chunks.push_back(frame(roi).clone());
        }
    }
    
    // Handle any remaining pixels at the edges
    // Right edge
    if (frame.cols % stepX != 0) {
        int x = frame.cols - chunkWidth;
        for (int y = 0; y <= frame.rows - chunkHeight; y += stepY) {
            if (x >= 0) {
                cv::Rect roi(x, y, chunkWidth, chunkHeight);
                chunks.push_back(frame(roi).clone());
            }
        }
    }
    
    // Bottom edge
    if (frame.rows % stepY != 0) {
        int y = frame.rows - chunkHeight;
        for (int x = 0; x <= frame.cols - chunkWidth; x += stepX) {
            if (y >= 0) {
                cv::Rect roi(x, y, chunkWidth, chunkHeight);
                chunks.push_back(frame(roi).clone());
            }
        }
    }
    
    // Bottom-right corner
    if (frame.rows % stepY != 0 && frame.cols % stepX != 0) {
        int x = frame.cols - chunkWidth;
        int y = frame.rows - chunkHeight;
        if (x >= 0 && y >= 0) {
            cv::Rect roi(x, y, chunkWidth, chunkHeight);
            chunks.push_back(frame(roi).clone());
        }
    }
    
    return chunks;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <video_path> [output_directory] [overlap_percent]" << std::endl;
        return -1;
    }
    
    // Parse command line arguments
    std::string videoPath = argv[1];
    std::string outputDir = (argc >= 3) ? argv[2] : "output_chunks";
    float overlapPercent = (argc >= 4) ? std::stof(argv[3]) : 0.2f; // Default 20% overlap
    
    // Create output directory if it doesn't exist
    //std::filesystem::create_directories(outputDir);
    system(("mkdir -p " + outputDir).c_str());
    
    // Open the video file
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the video file." << std::endl;
        return -1;
    }
    
    // Get video properties
    int frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    
    std::cout << "Video dimensions: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "Total frames: " << totalFrames << std::endl;
    
    // Define 1080p chunk dimensions (1920x1080)
    const int chunkWidth = 1920;
    const int chunkHeight = 1080;
    
    // Check if video is smaller than chunk size
    if (frameWidth < chunkWidth || frameHeight < chunkHeight) {
        std::cerr << "Warning: Video dimensions are smaller than 1080p. Will use original size." << std::endl;
    }
    
    // Calculate actual chunk dimensions (handle smaller videos)
    int actualChunkWidth = std::min(frameWidth, chunkWidth);
    int actualChunkHeight = std::min(frameHeight, chunkHeight);
    
    // Process each frame
    cv::Mat frame;
    int frameCount = 0;
    
    while (true) {
        // Read next frame
        cap >> frame;
        
        // Break if no more frames
        if (frame.empty()) {
            break;
        }
        
        // Split frame into chunks
        std::vector<cv::Mat> chunks = splitFrameIntoChunks(frame, actualChunkWidth, actualChunkHeight, overlapPercent);
        
        std::cout << "Frame " << frameCount << ": Split into " << chunks.size() << " chunks." << std::endl;
        
        // Save each chunk
        for (size_t i = 0; i < chunks.size(); i++) {
            std::string chunkFileName = outputDir + "/frame_" + std::to_string(frameCount) + 
                                       "_chunk_" + std::to_string(i) + ".png";
            cv::imwrite(chunkFileName, chunks[i]);
        }
        
        // Display progress
        if (frameCount % 10 == 0) {
            std::cout << "Processed " << frameCount << " frames..." << std::endl;
        }
        
        frameCount++;
        
        // Optional: Press 'q' to quit early
        if (cv::waitKey(1) == 'q') {
            std::cout << "Processing stopped by user." << std::endl;
            break;
        }
    }
    
    cap.release();
    std::cout << "Completed! Processed " << frameCount << " frames." << std::endl;
    
    return 0;
}