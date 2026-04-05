#ifndef HEXA_UTILS_H
#define HEXA_UTILS_H

#include "common.h"
#include <stdio.h>


typedef struct ReaderWriterLock_t
{
    size_t storage;
} ReaderWriterLock;


HEXA_UTILS_API(void) ReaderWriterLock_Init(ReaderWriterLock* cLock);
HEXA_UTILS_API(int) ReaderWriterLock_LockRead(ReaderWriterLock* cLock);
HEXA_UTILS_API(int) ReaderWriterLock_TryLockRead(ReaderWriterLock* cLock);
HEXA_UTILS_API(void) ReaderWriterLock_UnlockRead(ReaderWriterLock* cLock);
HEXA_UTILS_API(void) ReaderWriterLock_LockWrite(ReaderWriterLock* cLock);
HEXA_UTILS_API(int) ReaderWriterLock_TryLockWrite(ReaderWriterLock* cLock);
HEXA_UTILS_API(void) ReaderWriterLock_UnlockWrite(ReaderWriterLock* cLock);

#endif