#include <QImage>
#include <QDebug>
#include <QQueue>

#include "imagequeue.h"
#include "simpletracer.h"

ImageQueue::ImageQueue(QObject *parent, int capacity) :
    QObject(parent),
    m_capacity(capacity)
{
    // https://stackoverflow.com/questions/11706985/win32-thread-safe-queue-implementation-using-native-windows-api
    // http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
    // HANDLE hSem1, hSem2, hLock;
    // CreateSemaphore(security, init val, fina val, anonym)
    // semaphore enqueue
    hSem1 = CreateSemaphore(NULL, capacity, capacity, NULL);
    // semaphore dequeue
    hSem2 = CreateSemaphore(NULL, capacity, capacity, NULL);

    // CreateMutex(security, owned, unnamed)
    hLock = CreateMutex(NULL, FALSE, NULL);
}

ImageQueue::~ImageQueue()
{
    CloseHandle(hSem1);
    CloseHandle(hSem2);
    CloseHandle(hLock);
   // queue.removeAll();
}

void ImageQueue::enqueue(QImage *item)
{
    DWORD waitResult;
    // Attendre
    WaitForSingleObject(hSem1, INFINITE);
    waitResult = WaitForSingleObject(hLock, INFINITE);

    if (waitResult == WAIT_OBJECT_0) {
         queue.enqueue(item);
         // tracer la taille de la file lorsqu'elle change
         SimpleTracer::writeEvent(this, 0);
         // Liberer mutex (release ownership)
         ReleaseMutex(hLock);
         // Incrementer valeur du semaphore
         ReleaseSemaphore(hSem1, 1, NULL);
    }
}

QImage *ImageQueue::dequeue()
{
    DWORD waitResult;
    // Attendre
    WaitForSingleObject(hSem2, INFINITE);
    waitResult = WaitForSingleObject(hLock, INFINITE);

    if (waitResult == WAIT_OBJECT_0) {
         queue.dequeue();
         // tracer la taille de la file lorsqu'elle change
         SimpleTracer::writeEvent(this, 0);
         // Liberer mutex (release ownership)
         ReleaseMutex(hLock);
         // Incrementer valeur du semaphore
         ReleaseSemaphore(hSem2, 1, NULL);
    }

    // tracer la taille de la file lorsqu'elle change
    // SimpleTracer::writeEvent(this, 0);
    return NULL;
}
