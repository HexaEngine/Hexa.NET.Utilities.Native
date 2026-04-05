#include "ReaderWriterLock.h"

#include <atomic>
#include <cstddef>
#include <new>
#include <type_traits>

struct ReaderWriterLockCpp
{
    using TState = std::conditional_t<std::atomic<size_t>::is_always_lock_free, size_t, uint32_t>;
    static_assert(std::atomic<TState>::is_always_lock_free);
    static constexpr size_t valueBitCount = sizeof(TState) * 8;
    static constexpr TState writerBit = static_cast<TState>(static_cast<TState>(1) << (valueBitCount - 1));
    static constexpr TState readerMask = ~writerBit;

    std::atomic<TState> state{0};

    int TryLockRead()
    {
        auto value = state.load(std::memory_order_acquire);

        if ((value & writerBit) != 0)
        {
            return 0;
        }

        auto wanted = value + 1;
        if (wanted > readerMask)
        {
            return -1;
        }

        if (state.compare_exchange_strong(value, wanted, std::memory_order_release, std::memory_order_relaxed))
        {
            return 1;
        }
        return 0;
    }

    int LockRead()
    {
        auto value = state.load(std::memory_order_acquire);
        while (true) 
        {
            if ((value & writerBit) != 0)
            {
                do 
                {
                    state.wait(value, std::memory_order_relaxed);
                    value = state.load(std::memory_order_acquire);
                } while ((value & writerBit) != 0);
            }

            auto wanted = value + 1;
            if (wanted > readerMask)
            {
                return -1;
            }
            if (state.compare_exchange_weak(value, wanted, std::memory_order_release, std::memory_order_acquire))
            {
                return 1;
            }
        }
    }

    void UnlockRead()
    {
        state.fetch_sub(1, std::memory_order_release);
        state.notify_all();
        // We sadly cannot use notify_one here because if a writer is waiting and a reader too then the reader could steal the signal from the writer causing a deadlock.
    }

    int TryLockWrite(bool preserveWriterFairness = true)
    {
        TState oldValue;
        oldValue = state.fetch_or(writerBit, std::memory_order_acq_rel);
        if ((oldValue & writerBit) != 0)
        {
            return 0;
        }
        
        if (preserveWriterFairness)
        {
            // note we don't early return here or it wouldn't be fair for the writers and can get starved.
            while (((oldValue = state.load(std::memory_order_acquire)) & readerMask) != 0) 
            {
                state.wait(oldValue, std::memory_order_relaxed);
            }
        }
        else if (((oldValue = state.load(std::memory_order_acquire)) & readerMask) != 0)
        {
            state.fetch_and(readerMask, std::memory_order_release);
            state.notify_all();
            return 0;
        }

        return 1;
    }

    void LockWrite()
    {
        TState oldValue;
        while (true) 
        {
            oldValue = state.fetch_or(writerBit, std::memory_order_acq_rel);
            if ((oldValue & writerBit) == 0)
            {
                break;
            }
            state.wait(oldValue, std::memory_order_relaxed);
        }

        while (((oldValue = state.load(std::memory_order_acquire)) & readerMask) != 0) 
        {
            state.wait(oldValue, std::memory_order_relaxed);
        }
    }

    void UnlockWrite()
    {
        state.fetch_and(readerMask, std::memory_order_release);
        state.notify_all();
    }
};

static_assert(sizeof(ReaderWriterLockCpp) <= sizeof(ReaderWriterLock));
static_assert(alignof(ReaderWriterLockCpp) <= alignof(ReaderWriterLock));

HEXA_UTILS_API_INTERNAL(void) ReaderWriterLock_Init(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    new (lock) ReaderWriterLockCpp();
}

HEXA_UTILS_API_INTERNAL(int) ReaderWriterLock_LockRead(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    return lock->LockRead();
}

HEXA_UTILS_API_INTERNAL(int) ReaderWriterLock_TryLockRead(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    return lock->TryLockRead();
}

HEXA_UTILS_API_INTERNAL(void) ReaderWriterLock_UnlockRead(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    lock->UnlockRead();
}

HEXA_UTILS_API_INTERNAL(void) ReaderWriterLock_LockWrite(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    lock->LockWrite();
}

HEXA_UTILS_API_INTERNAL(int) ReaderWriterLock_TryLockWrite(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    return lock->TryLockWrite();
}

HEXA_UTILS_API_INTERNAL(void) ReaderWriterLock_UnlockWrite(ReaderWriterLock* cLock)
{
    ReaderWriterLockCpp* lock = reinterpret_cast<ReaderWriterLockCpp*>(cLock);
    lock->UnlockWrite();
}