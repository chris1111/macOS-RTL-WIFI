//
//  CTimeout.h
//  AppleIntelWifiAdapter
//
//  Created by 钟先耀 on 2020/1/30.
//  Copyright © 2020 钟先耀. All rights reserved.
//

/*
* Copyright (C) 2020  钟先耀
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef CTimeout_h
#define CTimeout_h

/* RTWNet80211Prefix.hpp renames CTimeout/_fWorkloop/_fCommandGate to the
 * private RTWNet80211* names for this kext. It must be active in EVERY
 * translation unit that sees this class or the timeout API — otherwise the
 * compat symbols mangle under the plain CTimeout name here and under
 * RTWNet80211Timeout in the driver, which showed up on a real Mac build as
 * `broken import: __Z16timeout_add_usecPP18RTWNet80211Timeouti`. */
#include "RTWNet80211Prefix.hpp"

#include <IOKit/IOTimerEventSource.h>
#include <libkern/c++/OSObject.h>

class CTimeout : public OSObject {
    OSDeclareDefaultStructors(CTimeout)
    
public:
    static void timeoutOccurred(OSObject* owner, IOTimerEventSource* timer);
    
    static IOReturn timeout_add_msec(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);
    
    static IOReturn timeout_del(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);
    
    static IOReturn timeout_free(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);
    
    static IOReturn timeout_set(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);
    
    static IOReturn timeout_pending(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3);
    
public:
    IOTimerEventSource* tm = nullptr;
    void (*to_func)(void *) = nullptr;        /* function to call */
    void *to_arg = nullptr;                /* function argument */
    bool isPending = false;
};

#endif /* CTimeout_h */
