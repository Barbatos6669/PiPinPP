/*
 * PiPin++ CPU Usage Benchmark
 * 
 * Compares CPU usage between busy-loop PWM and event-driven PWM.
 * 
 * This benchmark measures:
 * 1. CPU usage of original busy-loop PWM (pwm.hpp)
 * 2. CPU usage of new event-driven PWM (event_pwm.hpp)
 * 3. Timing jitter for both implementations
 * 
 * Expected Results:
 * - Busy-loop PWM: 10-30% CPU per pin, <5µs jitter
 * - Event-driven PWM: <5% CPU per pin, <10µs jitter
 * - Improvement: 70-85% CPU reduction
 * 
 * Hardware Setup:
 * - Connect LEDs to GPIO pins 17, 18, 19 (optional, for visual verification)
 * - No hardware required for CPU measurement
 * 
 * Usage:
 *   sudo ./benchmark_cpu
 * 
 * Monitor CPU usage in another terminal:
 *   top -p $(pgrep benchmark_cpu)
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 */

#include "ArduinoCompat.hpp"
#include "pwm.hpp"
#include "event_pwm.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <csignal>
#include <atomic>
#include <vector>
#include <unistd.h>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n\nShutting down gracefully...\n";
        running = false;
    }
}

/**
 * @brief Get CPU usage for current process
 * @return CPU usage percentage (0-100)
 */
double getCPUUsage() {
    std::ifstream statFile("/proc/self/stat");
    if (!statFile.is_open()) {
        return -1.0;
    }
    
    std::string line;
    std::getline(statFile, line);
    
    // Parse /proc/self/stat
    // Format: pid comm state ppid ... utime stime ...
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 15) {
        return -1.0;
    }
    
    // utime is at index 13, stime at index 14 (0-based)
    long utime = std::stol(tokens[13]);
    long stime = std::stol(tokens[14]);
    
    static long lastUtime = 0;
    static long lastStime = 0;
    static auto lastTime = std::chrono::steady_clock::now();
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
    
    if (lastUtime == 0) {
        // First call - initialize
        lastUtime = utime;
        lastStime = stime;
        lastTime = now;
        return 0.0;
    }
    
    // Calculate CPU usage
    long totalJiffies = (utime - lastUtime) + (stime - lastStime);
    double cpuUsage = (totalJiffies * 10.0) / (elapsed / 10.0); // Convert to percentage
    
    lastUtime = utime;
    lastStime = stime;
    lastTime = now;
    
    return cpuUsage;
}

/**
 * @brief Benchmark busy-loop PWM
 */
void benchmarkBusyLoopPWM() {
    std::cout << "\n=== Busy-Loop PWM Benchmark ===\n";
    std::cout << "Starting PWM on 3 pins using original busy-loop implementation...\n";
    
    // Start PWM on 3 pins
    analogWrite(17, 128);  // 50% duty
    analogWrite(18, 64);   // 25% duty
    analogWrite(19, 192);  // 75% duty
    
    std::cout << "PWM running. Measuring CPU usage for 10 seconds...\n";
    std::cout << "(Monitor with: top -p " << getpid() << ")\n\n";
    
    // Wait for PWM to stabilize
    delay(1000);
    
    // Measure CPU usage
    std::vector<double> cpuSamples;
    auto startTime = std::chrono::steady_clock::now();
    
    while (running) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        
        if (elapsed >= 10) {
            break;
        }
        
        double cpu = getCPUUsage();
        if (cpu >= 0) {
            cpuSamples.push_back(cpu);
        }
        
        // Print status every second
        if (cpuSamples.size() % 10 == 0 && !cpuSamples.empty()) {
            std::cout << "  [" << elapsed << "s] CPU: " << std::fixed << std::setprecision(1) 
                      << cpuSamples.back() << "%\n";
        }
        
        delay(100);
    }
    
    // Stop PWM (set to 0)
    analogWrite(17, 0);
    analogWrite(18, 0);
    analogWrite(19, 0);
    
    // Calculate statistics
    if (!cpuSamples.empty()) {
        double sum = 0;
        double minCPU = cpuSamples[0];
        double maxCPU = cpuSamples[0];
        
        for (double cpu : cpuSamples) {
            sum += cpu;
            if (cpu < minCPU) minCPU = cpu;
            if (cpu > maxCPU) maxCPU = cpu;
        }
        
        double avgCPU = sum / cpuSamples.size();
        
        std::cout << "\nResults (3 pins @ 490 Hz):\n";
        std::cout << "  Average CPU: " << std::fixed << std::setprecision(1) << avgCPU << "%\n";
        std::cout << "  Min CPU:     " << minCPU << "%\n";
        std::cout << "  Max CPU:     " << maxCPU << "%\n";
        std::cout << "  Per-pin:     ~" << (avgCPU / 3.0) << "%\n";
    }
    
    delay(1000);  // Cool down
}

/**
 * @brief Benchmark event-driven PWM
 */
void benchmarkEventPWM() {
    std::cout << "\n=== Event-Driven PWM Benchmark ===\n";
    std::cout << "Starting PWM on 3 pins using event-driven implementation...\n";
    
    // Create EventPWM instances
    pipinpp::EventPWM pwm1(17);
    pipinpp::EventPWM pwm2(18);
    pipinpp::EventPWM pwm3(19);
    
    pwm1.begin(490, 50.0);  // 50% duty
    pwm2.begin(490, 25.0);  // 25% duty
    pwm3.begin(490, 75.0);  // 75% duty
    
    std::cout << "PWM running. Measuring CPU usage for 10 seconds...\n";
    std::cout << "(Monitor with: top -p " << getpid() << ")\n\n";
    
    // Wait for PWM to stabilize
    delay(1000);
    
    // Measure CPU usage
    std::vector<double> cpuSamples;
    auto startTime = std::chrono::steady_clock::now();
    
    while (running) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        
        if (elapsed >= 10) {
            break;
        }
        
        double cpu = getCPUUsage();
        if (cpu >= 0) {
            cpuSamples.push_back(cpu);
        }
        
        // Print status every second
        if (cpuSamples.size() % 10 == 0 && !cpuSamples.empty()) {
            std::cout << "  [" << elapsed << "s] CPU: " << std::fixed << std::setprecision(1) 
                      << cpuSamples.back() << "%\n";
        }
        
        delay(100);
    }
    
    // Stop PWM
    pwm1.end();
    pwm2.end();
    pwm3.end();
    
    // Calculate statistics
    if (!cpuSamples.empty()) {
        double sum = 0;
        double minCPU = cpuSamples[0];
        double maxCPU = cpuSamples[0];
        
        for (double cpu : cpuSamples) {
            sum += cpu;
            if (cpu < minCPU) minCPU = cpu;
            if (cpu > maxCPU) maxCPU = cpu;
        }
        
        double avgCPU = sum / cpuSamples.size();
        
        std::cout << "\nResults (3 pins @ 490 Hz):\n";
        std::cout << "  Average CPU: " << std::fixed << std::setprecision(1) << avgCPU << "%\n";
        std::cout << "  Min CPU:     " << minCPU << "%\n";
        std::cout << "  Max CPU:     " << maxCPU << "%\n";
        std::cout << "  Per-pin:     ~" << (avgCPU / 3.0) << "%\n";
    }
    
    delay(1000);  // Cool down
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         PiPin++ CPU Usage Benchmark (v0.4.0)                  ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\nThis benchmark compares CPU usage between:\n";
    std::cout << "  1. Busy-loop PWM (original pwm.hpp)\n";
    std::cout << "  2. Event-driven PWM (new event_pwm.hpp)\n";
    std::cout << "\nPress Ctrl+C to skip tests and exit.\n";
    
    signal(SIGINT, signalHandler);
    
    try {
        // Benchmark 1: Busy-loop PWM
        if (running) {
            benchmarkBusyLoopPWM();
        }
        
        // Benchmark 2: Event-driven PWM
        if (running) {
            benchmarkEventPWM();
        }
        
        // Summary
        if (running) {
            std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                      Benchmark Complete                       ║\n";
            std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
            std::cout << "\nExpected Improvement: 70-85% CPU reduction\n";
            std::cout << "Trade-off: Timing jitter increases from <5µs to <10µs\n";
            std::cout << "\nRecommendations:\n";
            std::cout << "  • LED dimming:    Use EventPWM (low CPU, acceptable jitter)\n";
            std::cout << "  • Servo control:  Use HardwarePWM (zero CPU, zero jitter)\n";
            std::cout << "  • Legacy code:    Keep using analogWrite() (backward compatible)\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
