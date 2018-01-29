// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Concurrency/CriticalSectionMicrosoft.h
 * File contains Microsoft critical section implementation.
 */
#pragma once
#if !defined(GD_INSIDE_CRITICALSECTION_H)
#    error This file should be never directly included, please consider using <GoddamnEngine/Core/Concurrency/CriticalSection.h> instead.
#endif    // if !defined(GD_INSIDE_CRITICALSECTION_H)

#include <pthread.h>

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Apple critical section class.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    struct CriticalSectionApple : public CriticalSectionGeneric
    {
    private:
        mutable pthread_mutex_t m_Mutex;
        
    public:
        
        /*!
         * Initializes the Critical section.
         */
        GDINL CriticalSectionApple()
        {
            GD_VERIFY(pthread_mutex_init(&m_Mutex, nullptr) == 0);
        }
        GDINL ~CriticalSectionApple()
        {
            pthread_mutex_destroy(&m_Mutex);
        }
        
        /*!
         * Locks the Critical Section.
         */
        GDINL void Enter() const
        {
            pthread_mutex_lock(&m_Mutex);
        }
        
        /*!
         * Unlocks the Critical Section.
         */
        GDINL void Leave() const
        {
            pthread_mutex_unlock(&m_Mutex);
        }
        
    };    // struct CriticalSectionApple

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Cross-platform critical section class.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    using CriticalSection = CriticalSectionApple;

GD_NAMESPACE_END

