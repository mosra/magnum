#include "WindowlessAndroidApplication.h"

#include <android/native_window.h>
#include <pthread.h>
#include <Magnum/GL/DefaultFramebuffer.h>

#include "Magnum/GL/Version.h"

#include "Magnum/Platform/Implementation/Egl.h"

#define M_LOG(TYPE, func_name) TYPE{Corrade::Utility::TYPE::Flags::Type::NoSpace} << "Platform::WindowlessAndroidApplication::" << #func_name << "(): "
#define CHECK_EGL(name, func_name) \
{ \
    EGLint e = eglGetError(); \
    if (e == EGL_SUCCESS) { \
        M_LOG(Debug, func_name) << name << " succeeded"; \
    } else { \
        M_LOG(Error, func_name) << "cannot create EGL context: " << name << \
        " returned error: " << Magnum::Platform::Implementation::eglErrorString(e); \
    } \
}
#define CHECK_PTR(ptr) (ptr == 0 ? "(nullptr)" : ptr)

namespace Magnum { namespace Platform {

WindowlessAndroidApplication::WindowlessAndroidApplication(JNIEnv *jenv, jobject classInstance, jstring name, jstring signature) {
    if (!jvmManager.getJVM(jenv)) {
        Error{} << "failed to get JavaVM";
        return;
    }
    if (!jvmManager.attachJVM()) {
        Error() << "failed to attach JVM";
        return;
    }
    jObject = jvmManager.globalRef(jenv, classInstance);
    jClass = jvmManager.globalRef(jenv, jenv->GetObjectClass(jObject));
    jboolean isCopy1, isCopy2;

    if (name == nullptr) {
        Error{} << "cannot use a null name (0x0)";
        return;
    }

    if (signature == nullptr) {
        Error{} << "cannot use a null signature (0x0)";
        return;
    }

    const char * n = jvmManager.jenv->GetStringUTFChars(name, &isCopy1);
    if (n == nullptr) {
        Error{} << "cannot get UTF chars from name";
        return;
    }

    const char * s = jvmManager.jenv->GetStringUTFChars(signature, &isCopy2);
    if (s == nullptr) {
        Error{} << "cannot get UTF chars from signature";
        jvmManager.jenv->ReleaseStringUTFChars(name, n);
        return;
    }

    jSwapBuffers = jvmManager.jenv->GetMethodID(jClass, n, s);

    jvmManager.jenv->ReleaseStringUTFChars(name, n);
    jvmManager.jenv->ReleaseStringUTFChars(name, s);

    if (jSwapBuffers == nullptr) {
        Error{} << "cannot find method with name '" << name << "', "
                << "and signature '" << signature << "'";
    }
}

WindowlessAndroidApplication::~WindowlessAndroidApplication() {
    jvmManager.jenv->DeleteGlobalRef(jClass);
    jvmManager.jenv->DeleteGlobalRef(jObject);
    jvmManager.detachJVM();
}

void WindowlessAndroidApplication::swapBuffers() {
    if (jSwapBuffers != nullptr) {
        jvmManager.jenv->CallVoidMethod(jObject, jSwapBuffers);
    }
}

enum class WindowlessAndroidApplication::Flag: UnsignedByte {
    Redraw = 1 << 0
};

void WindowlessAndroidApplication::redraw() {
    _flags |= Flag::Redraw;
}

void WindowlessAndroidApplication::onDraw() {
    if (
        magnum_context != nullptr &&
        Platform::GLContext::hasCurrent() &&
        &Platform::GLContext::current() == &magnum_context->context->current()
    ) {
        if(_flags & Flag::Redraw) {
            _flags &= ~Flag::Redraw;
            drawEvent();
        }
    }
}

WindowlessAndroidApplication::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 8}, _depthBufferSize{24}, _stencilBufferSize{0} {}

Vector2i WindowlessAndroidApplication::framebufferSize() const {
    return {width,height};
}

void WindowlessAndroidApplication::surfaceChanged(int w, int h) {
    width = w;
    height = h;
    ViewportEvent e{{w, h}};
    viewportEvent(e);
}

void WindowlessAndroidApplication::viewportEvent(ViewportEvent& event) {
    static_cast<void>(event);
}

static MotionEventSerializer motionEvent;

bool WindowlessAndroidApplication::onTouchEvent(JNIEnv *jenv, jfloatArray motionEventData) {
    motionEvent.acquire(jenv, motionEventData);
    bool r;
    int action = motionEvent.getAction(0);
    switch (action) {
        case MotionEventSerializer::MOTION_EVENT_ACTION_DOWN:
        case MotionEventSerializer::MOTION_EVENT_ACTION_UP: {
            /* On a touch screen move events aren't reported when the
               finger is moving above (of course), so remember the position
               always */
            float x = motionEvent.getX(0);
            float y = motionEvent.getX(0);
            _previousMouseMovePosition = {Int(x), Int(y)};
            MouseEvent e(motionEvent, x, y, motionEvent.getButtonState());
            if (action == MotionEventSerializer::MOTION_EVENT_ACTION_DOWN) {
                mousePressEvent(e);
            } else {
                mouseReleaseEvent(e);
                // reset the relative position
                // if the relative position is not reset, then
                // the relative position of mouse press will be
                // relative to the last location of mouse release
                // which differs from desktop behaviour
                _previousMouseMovePosition = Vector2i{-1};
            }
            r = e.isAccepted();
            break;
        }

        case MotionEventSerializer::MOTION_EVENT_ACTION_MOVE: {
            float x = motionEvent.getX(0);
            float y = motionEvent.getY(0);
            Vector2i position{Int(x), Int(y)};
            MouseMoveEvent e{motionEvent, x, y, motionEvent.getButtonState(),
                             _previousMouseMovePosition == Vector2i{-1} ? Vector2i{} :
                             position - _previousMouseMovePosition};
            _previousMouseMovePosition = position;
            mouseMoveEvent(e);
            r = e.isAccepted();
            break;
        }
        default:
            r = false;
            break;
    }

    /** @todo Implement also other input events */

    motionEvent.release(jenv, motionEventData);
    return r;
}

void WindowlessAndroidApplication::mousePressEvent(MouseEvent&) {}
void WindowlessAndroidApplication::mouseReleaseEvent(MouseEvent&) {}
void WindowlessAndroidApplication::mouseMoveEvent(MouseMoveEvent&) {}

WindowlessAndroidApplication::JVM_MANAGER::JVM_MANAGER() {
    jvm = nullptr;
    jenv = nullptr;
    needsToDetach = false;
    jvmArgs.version = JNI_VERSION_1_6;
}

bool WindowlessAndroidApplication::JVM_MANAGER::getJVM(JNIEnv *env) {
    return env->GetJavaVM(&jvm) == JNI_OK;
}

bool WindowlessAndroidApplication::JVM_MANAGER::attachJVM() {
    // checks if current env needs attaching or it is already attached
    jint res = jvm->GetEnv((void**)&jenv, JNI_VERSION_1_6);
    if (res == JNI_EDETACHED) {
        // Supported but not attached yet, needs to call AttachCurrentThread
        res = jvm->AttachCurrentThread(&jenv, &jvmArgs);
        if (res == JNI_OK) {
            needsToDetach = true;
        } else {
            // Failed to attach, cancel
            return false;
        }
    } else if (JNI_OK == res) {
        // Current thread already attached, do not attach 'again' (just to save the attachedHere flag)
        // We make sure to keep attached = 0
        needsToDetach = false;
    } else {
        // JNI_EVERSION, specified version is not supported cancel this..
        return false;
    }
    return true;
}

void WindowlessAndroidApplication::JVM_MANAGER::detachJVM() {
    if (needsToDetach) {
        jvm->DetachCurrentThread();
    }
}
}}
