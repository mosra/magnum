//
// Created by Matthew Good on 7/6/21.
//

#include "MotionEventSerializer.h"

MotionEventSerializer::IndexInfo::IndexInfo() {}

MotionEventSerializer::IndexInfo::IndexInfo(int pointerIndex, int segment_length, int reserved) {
    idx_pointerId = (pointerIndex*segment_length)+reserved;
    idx_ptrIndex = (pointerIndex*segment_length)+reserved+1;
    idx_active = (pointerIndex*segment_length)+reserved+2;
    idx_action = (pointerIndex*segment_length)+reserved+3;
    idx_moved = (pointerIndex*segment_length)+reserved+4;
    idx_x = (pointerIndex*segment_length)+reserved+5;
    idx_y = (pointerIndex*segment_length)+reserved+6;
    idx_pressure = (pointerIndex*segment_length)+reserved+7;
    idx_size = (pointerIndex*segment_length)+reserved+8;
}

void MotionEventSerializer::acquire(JNIEnv *jenv, jfloatArray motionEventData) {
    jboolean isCopy;
    data = jenv->GetFloatArrayElements(motionEventData, &isCopy);

    dataLength = data[idx_dataLength];
    reserved = data[idx_dataReserved];
    segment_length = data[idx_dataSegmentLength];
    maxSupportedPointers = data[idx_maxSupportedPointers];
    int c = pointerCount();
    indexInfo = new IndexInfo[c];
    for (int i = 0; i < c; i++) {
        indexInfo[i] = IndexInfo(i, segment_length, reserved);
    }
}

void MotionEventSerializer::release(JNIEnv *jenv, jfloatArray motionEventData) {
    delete[] indexInfo;
    jenv->ReleaseFloatArrayElements(motionEventData, data, 0);
    data = nullptr;
}

int MotionEventSerializer::pointerCount() {
    return data[idx_ptrCount];
}

int MotionEventSerializer::currentPointerIndex() {
    return data[idx_ptrCurrentIdx];
}

int MotionEventSerializer::getActionIndex() {
    return currentPointerIndex();
}

int MotionEventSerializer::getPointerId(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_pointerId];
}

bool MotionEventSerializer::isPointerActive(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_active] == MOTION_EVENT_ACTIVE;
}

int MotionEventSerializer::getAction(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_action];
}

bool MotionEventSerializer::didPointerMove(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_moved] == MOTION_EVENT_MOVED;
}

float MotionEventSerializer::getX(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_x];
}

float MotionEventSerializer::getY(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_y];
}

float MotionEventSerializer::getPressure(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_pressure];
}

float MotionEventSerializer::getSize(int pointerIndex) {
    return data[indexInfo[pointerIndex].idx_size];
}

int MotionEventSerializer::getButtonState() {
    return data[idx_buttonState];
}
