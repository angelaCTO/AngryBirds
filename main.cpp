#include <cv.h>
#include <highgui.h>
#include <queue>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture input_cap(0);

    if (!input_cap.isOpened())
    {
        cout << "!!! Input video could not be opened" << endl;
        return -1;
    }

    queue <Mat> frames;
    Mat frame;
    int i = 0;                             //27=ESC, 101=e
    int limit = 30;                        //Number of frames queue will hold at once
    int save = 0;

    while (i!=27)                          //Default procedure
    {

        if (!input_cap.read(frame))
        {
            break;
        }
        if(frames.size()>=limit)          //Refresh queue if trying to add more frames than limit
        {
            frames.pop();
            frames.push(frame.clone());
        }
        else
        {
            frames.push(frame.clone());
        }
        if(i == 101)
        {
            limit = 60;
            save = 1;
        }
        if((save == 1)&&(frames.size()>=limit))
        {
            VideoWriter output_cap("video.avi", CV_FOURCC('M','J','P','G'), 10.0, Size(input_cap.get(CV_CAP_PROP_FRAME_WIDTH), input_cap.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
            if (!output_cap.isOpened()){
                cout << "!!! Output video could not be opened" << endl;
                return -1;
            }
            printf("number of frames saved = %d\r\n", frames.size());
            while(!frames.empty()){
                output_cap.write(frames.front());
                imshow("record", frames.front());
                waitKey(10);
                frames.pop();
            }
            output_cap.release();
            save = 0;
        }
        imshow("FrontCam", frame);
        i = waitKey(10);
    }
    input_cap.release();
}
