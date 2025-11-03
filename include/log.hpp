/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <iostream>

// Logging macros - controlled by CMake options
// By default, logging is disabled in Release builds
// Enable with: cmake -DPIPINPP_ENABLE_LOGGING=ON

#ifdef PIPINPP_ENABLE_LOGGING
    #ifndef PIPINPP_LOG_LEVEL
        #define PIPINPP_LOG_LEVEL 1  // Default: INFO level
    #endif
    
    // Log levels: 0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR
    #define PIPINPP_LOG_DEBUG(msg) \
        do { if (PIPINPP_LOG_LEVEL <= 0) std::cerr << "[DEBUG] " << msg << std::endl; } while(0)
    
    #define PIPINPP_LOG_INFO(msg) \
        do { if (PIPINPP_LOG_LEVEL <= 1) std::cerr << "[INFO] " << msg << std::endl; } while(0)
    
    #define PIPINPP_LOG_WARNING(msg) \
        do { if (PIPINPP_LOG_LEVEL <= 2) std::cerr << "[WARNING] " << msg << std::endl; } while(0)
    
    #define PIPINPP_LOG_ERROR(msg) \
        do { if (PIPINPP_LOG_LEVEL <= 3) std::cerr << "[ERROR] " << msg << std::endl; } while(0)
#else
    // Logging disabled - macros expand to nothing (zero overhead)
    #define PIPINPP_LOG_DEBUG(msg) do {} while(0)
    #define PIPINPP_LOG_INFO(msg) do {} while(0)
    #define PIPINPP_LOG_WARNING(msg) do {} while(0)
    #define PIPINPP_LOG_ERROR(msg) do {} while(0)
#endif
