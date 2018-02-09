// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Critical section implementation.
 */
#include <GoddamnEngine/Core/Platform/CriticalSection.h>
#if GD_PLATFORM_API_MICROSOFT

#include <pthread.h>

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Critical section (Mutex) on Posix platforms.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL PosixPlatformCriticalSection final : public IPlatformCriticalSection
    {
    private:
        mutable pthread_mutex_t m_Mutex;
        
    public:
        GDINT PosixPlatformCriticalSection()
        {
            GD_VERIFY(pthread_mutex_init(&m_Mutex, nullptr) == 0);
        }
        
        GDINT virtual ~PosixPlatformCriticalSection()
        {
            pthread_mutex_destroy(&m_Mutex);
        }
        
        /*!
         * Locks the Critical Section.
         */
        GDINT virtual void Enter() const override final
        {
            pthread_mutex_lock(&m_Mutex);
        }
        
        /*!
         * Unlocks the Critical Section.
         */
        GDINT virtual void Leave() const override final
        {
            pthread_mutex_unlock(&m_Mutex);
        }
    };    // struct PosixPlatformCriticalSection

    /*!
     * Creates the critical section.
     */
    GDAPI IPlatformCriticalSection* IPlatformCriticalSection::Create()
    {
        return gd_new PosixPlatformCriticalSection();
    }

GD_NAMESPACE_END

#endif    // if GD_PLATFORM_API_MICROSOFT

