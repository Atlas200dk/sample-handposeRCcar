
/**
 * @brief: compute RC car command
 */
class HandposeDecode {
public:
    /**
    * @brief Constructor
    */
    HandposeDecode();

    /**
    * @brief Destructor
    */
    //~HandposeDecode();
    /**
    * @brief: validate keypoints
    * @return: true: valid
    *          false: invalid
    */
    int validate();

    /**
    * @brief: calculate top of finger keypoint relative to bottom
    * @return: value proportional to angle of fingers
    */
    int get_fingers_angle_scale();

    /**
    * @brief: thumb pointing inward or outward
    * @return: true: inward
    *          false: outward
    */
    int thumb_status();
    /**
    * @brief: compute rc_command based on keypoint coordinates
    * @return: 0: stop
    *          1: forward
    *          2: backward
    *          3: left
    *          4: right
    */
    unsigned char get_rc_command();

private:
    /**
    * @brief: find index of minimum value in array
    */
    int min_index(int arr[]);

    /**
    * @brief: find maximum value in array
    */
    int max(int arr[]);

    /**
    * @brief: find minimum value in array
    */
    int min(int arr[]);

public:
    int x_arr [21];
    int y_arr [21];

private:
    const int x_max_threshold = 1200;
    const int x_min_threshold = 50;
    const int thumb_threshold = 5;//threshold for thumb-index finger angle: for backward decision
    const unsigned char cmd_stop = 0;
    const unsigned char cmd_forward = 1;
    const unsigned char cmd_backward = 2;
    const unsigned char cmd_left = 3;
    const unsigned char cmd_right = 4;
    const int left_thresh = -500;
    const int right_thresh = 500;
};