//
// Created by Matthew Good on 7/6/21.
//

#ifndef CRAFTER_MOTIONEVENTSERIALIZER_H
#define CRAFTER_MOTIONEVENTSERIALIZER_H

#include <jni.h>

class MotionEventSerializer {
private:
    class IndexInfo {
    public:
        int idx_pointerId;
        int idx_ptrIndex;
        int idx_active;
        int idx_action;
        int idx_moved;
        int idx_x;
        int idx_y;
        int idx_pressure;
        int idx_size;
        IndexInfo();
        IndexInfo(int pointerIndex, int segment_length, int reserved);
    };

    IndexInfo * indexInfo;

    int reserved;
    int segment_length;
    int maxSupportedPointers;
    int dataLength;
    float * data = nullptr;
    // THESE MUSE BE KEPT IN SYNC WITH JAVA SIDE
    static const int idx_dataLength = 0;
    static const int idx_dataReserved = 1;
    static const int idx_dataSegmentLength = 2;
    static const int idx_maxSupportedPointers = 3;
    static const int idx_ptrCount = 4;
    static const int idx_ptrCurrentIdx = 5;
    static const int idx_buttonState = 6;
public:
    // THESE MUSE BE KEPT IN SYNC WITH JAVA SIDE
    static const int MOTION_EVENT_ACTIVE = 1;
    static const int MOTION_EVENT_NOT_ACTIVE = 2;
    static const int MOTION_EVENT_ACTION_UNKNOWN = -1;
    static const int MOTION_EVENT_ACTION_DOWN = 1;
    static const int MOTION_EVENT_ACTION_UP = 2;
    static const int MOTION_EVENT_ACTION_MOVE = 3;
    static const int MOTION_EVENT_MOVED = 1;
    static const int MOTION_EVENT_DID_NOT_MOVE = 2;

    /**
     * Constants that identify buttons that are associated with motion events.
     * Refer to the documentation on the MotionEvent class for descriptions of each button.
     */
    enum {
        /** primary */
        MOTION_EVENT_BUTTON_PRIMARY = 1 << 0,
        /** secondary */
        MOTION_EVENT_BUTTON_SECONDARY = 1 << 1,
        /** tertiary */
        MOTION_EVENT_BUTTON_TERTIARY = 1 << 2,
        /** back */
        MOTION_EVENT_BUTTON_BACK = 1 << 3,
        /** forward */
        MOTION_EVENT_BUTTON_FORWARD = 1 << 4,
        MOTION_EVENT_BUTTON_STYLUS_PRIMARY = 1 << 5,
        MOTION_EVENT_BUTTON_STYLUS_SECONDARY = 1 << 6,
    };

    void acquire(JNIEnv *jenv, jfloatArray motionEventData);
    void release(JNIEnv *jenv, jfloatArray motionEventData);

    int pointerCount();
    int currentPointerIndex();

    bool isPointerActive(int pointerIndex);
    bool didPointerMove(int pointerIndex);

    int getPointerId(int pointerIndex);
    int getAction(int pointerIndex);
    int getActionIndex();
    float getX(int pointerIndex);
    float getY(int pointerIndex);
    float getPressure(int pointerIndex);
    float getSize(int pointerIndex);

    int getButtonState();
};

#endif //CRAFTER_MOTIONEVENTSERIALIZER_H
