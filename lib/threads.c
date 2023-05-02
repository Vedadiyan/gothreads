#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>

typedef HANDLE handle;

extern void callback(int id);

static inline handle Create(int id) {
    HANDLE hThread;
    DWORD dwThreadId;
    DWORD WINAPI thread_func(LPVOID lpParam)
    {
        callback((int)(uintptr_t)lpParam);
        return 0;
    }
    hThread = CreateThread(NULL, 0, thread_func, (LPVOID)(uintptr_t)id, 0, &dwThreadId);
    if (hThread == NULL) {
        printf("Failed to create thread!\n");
    }
    return hThread;
}

static inline void SetAttributes() {
    // nothing to do here
}

static inline void Terminate(handle handle) {
    TerminateThread(handle, 0);
    CloseHandle(handle);
}

static inline void Close(handle handle) {
    CloseHandle(handle);
}

static inline void Join(handle handle) {
    WaitForSingleObject(handle, INFINITE);
}

static inline void JoinWithTimeout(handle handle, DWORD timeout) {
    WaitForSingleObject(handle, timeout);
}

static inline void Suspend(handle handle) {
    SuspendThread(handle);
}

static inline void Resume(handle handle) {
    ResumeThread(handle);
}

#else
#include <pthread.h>
#include <unistd.h>

typedef pthread_t handle;

extern void callback(int id);

static inline handle Create(int id) {
    pthread_t thread;
    pthread_create(&thread, NULL, (void* (*)(void*))callback, (void*)(uintptr_t)id);
    return thread;
}

static inline void SetAttributes() {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
}

static inline void Terminate(handle thread) {
    pthread_cancel(thread);
}

static inline void Close(handle thread) {
    // nothing to do here
    // pthread_t handles are not explicitly closed
}


static inline void Suspend(handle handle)
{

}

static inline void Resume(handle handle)
{
   
}

#endif