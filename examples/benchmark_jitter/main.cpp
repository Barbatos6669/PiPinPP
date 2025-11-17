/*
 * PiPin++ PWM Timing Jitter Benchmark
 * 
 * Measures PWM timing accuracy and jitter for performance validation.
 * 
 * This benchmark:
 * 1. Measures actual PWM period vs expected period
 * 2. Calculates timing jitter (min/max/avg/stddev)
 * 3. Compares busy-loop PWM vs EventPWM
 * 4. Validates <10µs jitter target for EventPWM
 * 
 * Method:
 * - Use GPIO interrupt to timestamp rising edges
 * - Calculate period between edges
 * - Statistical analysis over 1000+ cycles
 * 
 * Expected Results:
 * - Busy-loop PWM: <5µs jitter (best case)
 * - EventPWM: <10µs jitter (acceptable for LEDs)
 * - Hardware PWM: 0µs jitter (reference)
 * 
 * Hardware Setup:
 * - Connect GPIO 17 (output) to GPIO 27 (input) with jumper wire
 * - This creates loopback for edge detection
 * 
 * Usage:
 *   sudo ./benchmark_jitter
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 */

#include "ArduinoCompat.hpp"
#include "pwm.hpp"
#include "event_pwm.hpp"
#include "interrupts.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <atomic>
#include <csignal>

// Configuration
const int OUTPUT_PIN = 17;  // PWM output
const int INPUT_PIN = 27;   // Loopback input for edge detection
const int TEST_FREQUENCY = 1000;  // 1kHz (1ms period)
const int SAMPLE_COUNT = 1000;    // Number of periods to measure

// Data collection
std::vector<long> edgeTimestamps;
std::atomic<bool> running(true);
std::atomic<int> edgeCount(0);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n\nShutting down...\n";
        running = false;
    }
}

/**
 * @brief Interrupt callback for rising edge detection
 */
void edgeDetected() {
    auto now = std::chrono::high_resolution_clock::now();
    long timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    
    if (edgeTimestamps.size() < SAMPLE_COUNT) {
        edgeTimestamps.push_back(timestamp);
        edgeCount++;
    }
}

/**
 * @brief Calculate statistics from period measurements
 */
struct JitterStats {
    double avgPeriod = 0.0;
    double minPeriod = 0.0;
    double maxPeriod = 0.0;
    double avgJitter = 0.0;
    double maxJitter = 0.0;
    double stddev = 0.0;
    int sampleCount = 0;
};

JitterStats calculateJitter(const std::vector<long>& timestamps, double expectedPeriodUs) {
    JitterStats stats;
    
    if (timestamps.size() < 2) {
        return stats;
    }
    
    // Calculate periods between edges
    std::vector<double> periods;
    for (size_t i = 1; i < timestamps.size(); i++) {
        long period = timestamps[i] - timestamps[i-1];
        periods.push_back(static_cast<double>(period));
    }
    
    stats.sampleCount = periods.size();
    
    // Calculate min/max/avg period
    stats.minPeriod = *std::min_element(periods.begin(), periods.end());
    stats.maxPeriod = *std::max_element(periods.begin(), periods.end());
    
    double sum = 0;
    for (double p : periods) {
        sum += p;
    }
    stats.avgPeriod = sum / periods.size();
    
    // Calculate jitter (deviation from expected)
    double jitterSum = 0;
    double maxJitter = 0;
    
    for (double p : periods) {
        double jitter = std::abs(p - expectedPeriodUs);
        jitterSum += jitter;
        if (jitter > maxJitter) {
            maxJitter = jitter;
        }
    }
    
    stats.avgJitter = jitterSum / periods.size();
    stats.maxJitter = maxJitter;
    
    // Calculate standard deviation
    double varianceSum = 0;
    for (double p : periods) {
        double diff = p - stats.avgPeriod;
        varianceSum += diff * diff;
    }
    stats.stddev = std::sqrt(varianceSum / periods.size());
    
    return stats;
}

/**
 * @brief Print statistics in formatted table
 */
void printStats(const std::string& name, const JitterStats& stats, double expectedPeriodUs) {
    std::cout << "\n" << name << " Results:\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Samples:           " << stats.sampleCount << "\n";
    std::cout << "  Expected Period:   " << expectedPeriodUs << " µs\n";
    std::cout << "  Average Period:    " << stats.avgPeriod << " µs\n";
    std::cout << "  Min Period:        " << stats.minPeriod << " µs\n";
    std::cout << "  Max Period:        " << stats.maxPeriod << " µs\n";
    std::cout << "  Average Jitter:    " << stats.avgJitter << " µs";
    
    // Pass/fail indication
    if (stats.avgJitter < 5.0) {
        std::cout << " ✅ (Excellent)";
    } else if (stats.avgJitter < 10.0) {
        std::cout << " ✅ (Good)";
    } else if (stats.avgJitter < 50.0) {
        std::cout << " ⚠️  (Acceptable)";
    } else {
        std::cout << " ❌ (Poor)";
    }
    std::cout << "\n";
    
    std::cout << "  Max Jitter:        " << stats.maxJitter << " µs\n";
    std::cout << "  Std Deviation:     " << stats.stddev << " µs\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

/**
 * @brief Benchmark busy-loop PWM timing
 */
JitterStats benchmarkBusyLoopJitter() {
    std::cout << "\n=== Busy-Loop PWM Timing Benchmark ===\n";
    std::cout << "Setting up loopback: GPIO " << OUTPUT_PIN << " → GPIO " << INPUT_PIN << "\n";
    
    // Reset data
    edgeTimestamps.clear();
    edgeCount = 0;
    
    // Setup input pin with interrupt
    pinMode(INPUT_PIN, INPUT);
    attachInterrupt(INPUT_PIN, edgeDetected, RISING);
    
    // Start PWM on output pin
    std::cout << "Starting PWM at " << TEST_FREQUENCY << " Hz (50% duty)...\n";
    analogWrite(OUTPUT_PIN, 128);
    
    // Wait for samples
    std::cout << "Collecting " << SAMPLE_COUNT << " samples";
    std::cout.flush();
    
    auto startTime = std::chrono::steady_clock::now();
    while (edgeTimestamps.size() < SAMPLE_COUNT && running) {
        delay(100);
        
        // Progress indicator
        if (edgeCount % 100 == 0) {
            std::cout << ".";
            std::cout.flush();
        }
        
        // Timeout after 30 seconds
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        if (elapsed > 30) {
            std::cout << "\n⚠️  Timeout - only collected " << edgeTimestamps.size() << " samples\n";
            break;
        }
    }
    std::cout << " Done!\n";
    
    // Stop PWM
    analogWrite(OUTPUT_PIN, 0);
    detachInterrupt(INPUT_PIN);
    
    // Calculate statistics
    double expectedPeriodUs = 1000000.0 / TEST_FREQUENCY;
    return calculateJitter(edgeTimestamps, expectedPeriodUs);
}

/**
 * @brief Benchmark EventPWM timing
 */
JitterStats benchmarkEventPWMJitter() {
    std::cout << "\n=== Event-Driven PWM Timing Benchmark ===\n";
    std::cout << "Setting up loopback: GPIO " << OUTPUT_PIN << " → GPIO " << INPUT_PIN << "\n";
    
    // Reset data
    edgeTimestamps.clear();
    edgeCount = 0;
    
    // Setup input pin with interrupt
    pinMode(INPUT_PIN, INPUT);
    attachInterrupt(INPUT_PIN, edgeDetected, RISING);
    
    // Start EventPWM on output pin
    std::cout << "Starting EventPWM at " << TEST_FREQUENCY << " Hz (50% duty)...\n";
    pipinpp::EventPWM pwm(OUTPUT_PIN);
    pwm.begin(TEST_FREQUENCY, 50.0);
    
    // Wait for samples
    std::cout << "Collecting " << SAMPLE_COUNT << " samples";
    std::cout.flush();
    
    auto startTime = std::chrono::steady_clock::now();
    while (edgeTimestamps.size() < SAMPLE_COUNT && running) {
        delay(100);
        
        // Progress indicator
        if (edgeCount % 100 == 0) {
            std::cout << ".";
            std::cout.flush();
        }
        
        // Timeout after 30 seconds
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        if (elapsed > 30) {
            std::cout << "\n⚠️  Timeout - only collected " << edgeTimestamps.size() << " samples\n";
            break;
        }
    }
    std::cout << " Done!\n";
    
    // Stop PWM
    pwm.end();
    detachInterrupt(INPUT_PIN);
    
    // Calculate statistics
    double expectedPeriodUs = 1000000.0 / TEST_FREQUENCY;
    return calculateJitter(edgeTimestamps, expectedPeriodUs);
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       PiPin++ PWM Timing Jitter Benchmark (v0.4.0)           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n⚠️  HARDWARE REQUIRED:\n";
    std::cout << "  Connect GPIO " << OUTPUT_PIN << " to GPIO " << INPUT_PIN << " with jumper wire\n";
    std::cout << "  This creates loopback for edge detection\n";
    std::cout << "\nPress Ctrl+C to abort.\n";
    
    signal(SIGINT, signalHandler);
    
    try {
        // Test 1: Busy-loop PWM
        JitterStats busyLoopStats;
        if (running) {
            busyLoopStats = benchmarkBusyLoopJitter();
            printStats("Busy-Loop PWM", busyLoopStats, 1000.0);
        }
        
        delay(1000);  // Cool down
        
        // Test 2: EventPWM
        JitterStats eventStats;
        if (running) {
            eventStats = benchmarkEventPWMJitter();
            printStats("Event-Driven PWM", eventStats, 1000.0);
        }
        
        // Comparison
        if (running && busyLoopStats.sampleCount > 0 && eventStats.sampleCount > 0) {
            std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                        Comparison                             ║\n";
            std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
            std::cout << std::fixed << std::setprecision(2);
            
            double jitterRatio = eventStats.avgJitter / busyLoopStats.avgJitter;
            std::cout << "\nJitter Comparison:\n";
            std::cout << "  Busy-Loop:  " << busyLoopStats.avgJitter << " µs\n";
            std::cout << "  EventPWM:   " << eventStats.avgJitter << " µs\n";
            std::cout << "  Ratio:      " << jitterRatio << "x\n";
            
            if (eventStats.avgJitter < 10.0) {
                std::cout << "\n✅ EventPWM meets <10µs jitter target!\n";
            } else {
                std::cout << "\n⚠️  EventPWM exceeds 10µs jitter target\n";
            }
            
            std::cout << "\nRecommendations:\n";
            if (busyLoopStats.avgJitter < 5.0 && eventStats.avgJitter < 10.0) {
                std::cout << "  ✅ Both implementations suitable for LED control\n";
                std::cout << "  ✅ Use EventPWM to save 70-85% CPU\n";
            }
            std::cout << "  ⚠️  For servo control, use HardwarePWM (zero jitter)\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        std::cerr << "\nTroubleshooting:\n";
        std::cerr << "  1. Check GPIO " << OUTPUT_PIN << " and " << INPUT_PIN << " are connected\n";
        std::cerr << "  2. Run with sudo for GPIO access\n";
        std::cerr << "  3. Verify no other programs using these pins\n";
        return 1;
    }
    
    return 0;
}
