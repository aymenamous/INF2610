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
    // semaphore enqueue
    hSem1 = CreateSemaphore(NULL, capacity, capacity, NULL);
    // semaphore dequeue
    hSem2 = CreateSemaphore(NULL, 0, capacity, NULL);
    //Initialisation du tab
    queue = QQueue<QImage*> ();
}

ImageQueue::~ImageQueue()
{
    CloseHandle(hSem1);
    CloseHandle(hSem2);
}

void ImageQueue::enqueue(QImage *item)
{
    int waitResult=WaitForSingleObject(hSem1, INFINITE);
    //Si la semaphore est signalee
    if (waitResult == WAIT_OBJECT_0) {
         queue.enqueue(item);
         // tracer la taille de la file lorsqu'elle change
         SimpleTracer::writeEvent(this, queue.size());
         // Incrementer valeur de l'autre semaphore
         ReleaseSemaphore(hSem2, 1, NULL);
    }
}

QImage *ImageQueue::dequeue() {

    int waitResult=WaitForSingleObject(hSem2, INFINITE);
    //Si la semaphore est signalee
    if (waitResult == WAIT_OBJECT_0) {
         QImage* image = queue.dequeue();
         // tracer la taille de la file lorsqu'elle change
         SimpleTracer::writeEvent(this, queue.size());
         // Incrementer l'autre semaphore
         ReleaseSemaphore(hSem1, 1, NULL);
    return image;
    }
    return NULL;
}
