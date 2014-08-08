/* Filename: test_main.cpp
 */

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <queue>
#include <deque>
#include <string>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "BlackLib.h"
#include "SensorSignal.h"


/* PREPROCESSOR CONSTANTS
 */
#define PRELIM   false       // Record ALL video
#define DEBUG    true
#define PRETIME  40          // 5 SEC (Before Collision)              // 100
#define POSTTIME 80          // 35 SEC TOTAL (30 SEC After Collision) // 700
#define FPS      20
#define X_RESOLUTION  1280   // 320
#define Y_RESOLUTION  720    // 240
#define WINDOW_SIZE 0        //!! DUSTIN
#define GROUND_THRESHOLD 0   //!! DUSTIN
#define TEST_THRESHOLD 1000  // For testing purposes. To be removed later

using namespace cv;
using namespace std;



/* HELPER FUNCTION PROTOTYPES
 */
void create_directory(const char *path, struct stat &st);
string create_id(const char *path);


int main()
{
    Mat frame;
    queue <Mat> frames;
    deque <int> sensor_signal;
    deque <int> averaged_signal; //!! DUSTIN

    struct stat st;
    string vid_id;

    int limit = PRETIME;
    int save = 0;

    float average_signal;
    float normal_signal;

    BlackADC* test_adc = new BlackADC(AIN4);
    const char * path = "/home/ubuntu/AngryBirds/SDCard/videos/";

    int test_count = 0;

    VideoCapture input_cap(0);

    input_cap.set(CV_CAP_PROP_FRAME_WIDTH, X_RESOLUTION);
    input_cap.set(CV_CAP_PROP_FRAME_HEIGHT, Y_RESOLUTION);
    
    if (!input_cap.isOpened())
    {
        cout << "!!! Input video could not be opened" << endl;
        return -1;
    }

    while(input_cap.read(frame))
    {
        if(frames.size() >= limit)
        {
            frames.pop();
            frames.push(frame.clone());
        }
        else
        {
            frames.push(frame.clone());
        }

        if (test_count >= 40)
        {
            cout << "Event detected!" << endl;
            save = 1;
            limit = POSTTIME;
        }

        cout << "test_count: " << test_count << endl;
        cout << "frames.size(): " << frames.size() << endl;
        cout << "save: " << save << endl;

        // Collision detected, save queue to write to file
        if((save == 1) && (frames.size() >= limit))
        {
            cout << "\n\nEVENT TRIGGERED!\n\n" << endl;
            
            create_directory(path, st);

            cout << "\n\nCREATING VIDEO\n\n" << endl;

            vid_id = create_id(path);

            VideoWriter output_cap(vid_id,
                                   CV_FOURCC('M','J','P','G'),
                                   FPS,
                                   Size(X_RESOLUTION, Y_RESOLUTION),
                                   true);
                                   
            if(!output_cap.isOpened())
            {
                cout << "!!! Output video could not be opened" << endl;
                return -1;
            }

            cout << "\n\nPUSHING FRAMES\n\n" << endl;

            int frame_count = 0;
            
            while(!frames.empty())
            {
                cout << "WRITING FRAME: " << frame_count << endl;
                output_cap.write(frames.front());
                frames.pop();
                frame_count += 1;
            }

            cout << "\n\nDONE WRITING\n\n" << endl;

            output_cap.release();
            save = 0;
            limit = PRETIME;
            test_count = 0;
        }
        test_count++;
    }
    input_cap.release();
}



/* FUNCTION DEFINITIONS
 */

/* Creates a new directory to store footage if it doesn't
 * exist
 */
void create_directory(const char *path, struct stat &st)
{
    if(stat(path, &st) != 0)
    {
        if(errno == ENOENT)
        {
         cout << "Creating a new video directory" << endl;
            if(mkdir(path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) == 0)
            {
                perror("mkdir");
                }
            }
        }
}

/* Creates a new ID to name output video file
 * in format "Year-Month-Day Hour_Minute_Second"
 */
string create_id(const char *path)
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    string vid_id;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%F %H_%M_%S", timeinfo);
    string new_id = string(buffer);
    vid_id = path + new_id + ".avi";

    return vid_id;
}

//-----EOF-----



