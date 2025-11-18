/*
 * PiPin++ Performance Profiler
 *
 * Measures timing drift, scheduling jitter, and GPIO toggle throughput.
 * Outputs human-readable summaries and a CSV file for deeper analysis.
 *
 * Hardware:
 *   - Optional LED + 220Ω resistor on GPIO 17 to visualize toggles
 *   - No external equipment required for timing measurements
 */

#include "ArduinoCompat.hpp"
#include "pin.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <numeric>
#include <vector>

constexpr int PROFILER_PIN = 17;
constexpr unsigned long LOOP_INTERVAL_US = 1000;   // Target 1 kHz loop
constexpr unsigned long DELAY_TARGET_MS = 25;      // 25 ms timing test
constexpr int DELAY_SAMPLES = 25;
constexpr int LOOP_ITERATIONS = 1500;
constexpr int TOGGLE_CYCLES = 5000;

struct Summary
{
    long minValue{0};
    long maxValue{0};
    double average{0.0};
};

Summary summarize(const std::vector<long> &samples)
{
    Summary summary{};

    if (samples.empty())
    {
        return summary;
    }

    auto [minIt, maxIt] = std::minmax_element(samples.begin(), samples.end());
    summary.minValue = *minIt;
    summary.maxValue = *maxIt;
    summary.average = std::accumulate(samples.begin(), samples.end(), 0.0) /
        static_cast<double>(samples.size());

    return summary;
}

std::vector<long> runDelayDriftTest(unsigned long targetMs, int sampleCount)
{
    std::vector<long> drift;
    drift.reserve(sampleCount);

    for (int i = 0; i < sampleCount; ++i)
    {
        unsigned long start = millis();
        delay(targetMs);
        unsigned long stop = millis();
        long error = static_cast<long>(stop - start) - static_cast<long>(targetMs);
        drift.push_back(error);
    }

    return drift;
}

std::vector<long> runLoopJitterTest(unsigned long intervalMicros, int iterations)
{
    std::vector<long> jitter;
    jitter.reserve(iterations);

    unsigned long previous = micros();

    for (int i = 0; i < iterations; ++i)
    {
        delayMicroseconds(intervalMicros);
        unsigned long current = micros();
        long delta = static_cast<long>(current - previous) - static_cast<long>(intervalMicros);
        jitter.push_back(delta);
        previous = current;
    }

    return jitter;
}

struct ThroughputResult
{
    unsigned long durationMicros{0};
    double cycleRateHz{0.0};
    double writeTimeMicros{0.0};
};

ThroughputResult runGpioThroughputTest(int pinNumber, int cycles)
{
    Pin pin(pinNumber, PinDirection::OUTPUT);
    pin.write(LOW);

    unsigned long start = micros();
    for (int i = 0; i < cycles; ++i)
    {
        pin.write(HIGH);
        pin.write(LOW);
    }
    unsigned long end = micros();

    unsigned long duration = end - start;
    double seconds = duration / 1000000.0;

    ThroughputResult result;
    result.durationMicros = duration;
    result.cycleRateHz = seconds > 0.0 ? cycles / seconds : 0.0;
    result.writeTimeMicros = cycles > 0 ? duration / (cycles * 2.0) : 0.0;
    return result;
}

void exportCsv(const std::vector<long> &delayErrors,
               const std::vector<long> &loopJitter,
               const ThroughputResult &throughput)
{
    std::ofstream csv("performance_profile.csv");
    if (!csv)
    {
        std::cerr << "Failed to open performance_profile.csv for writing." << std::endl;
        return;
    }

    csv << "sample,delay_error_ms,loop_jitter_us\n";
    size_t maxSamples = std::max(delayErrors.size(), loopJitter.size());

    for (size_t i = 0; i < maxSamples; ++i)
    {
        long delaySample = i < delayErrors.size() ? delayErrors[i] : 0;
        long jitterSample = i < loopJitter.size() ? loopJitter[i] : 0;
        csv << i << ',' << delaySample << ',' << jitterSample << '\n';
    }

    csv << "\nmetric,value\n";

    auto previousFlags = csv.flags();
    auto previousPrecision = csv.precision();

    csv << "gpio_cycles_duration_us," << throughput.durationMicros << '\n';
    csv << std::fixed << std::setprecision(2);
    csv << "gpio_cycle_rate_hz," << throughput.cycleRateHz << '\n';
    csv << std::setprecision(3);
    csv << "avg_write_time_us," << throughput.writeTimeMicros << '\n';

    csv.flags(previousFlags);
    csv.precision(previousPrecision);
}

int main()
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                PiPin++ Performance Profiler                   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "Metrics collected:\n";
    std::cout << "  • delay() drift over " << DELAY_SAMPLES << " samples\n";
    std::cout << "  • delayMicroseconds() scheduling jitter at 1 kHz\n";
    std::cout << "  • GPIO toggle throughput on pin " << PROFILER_PIN << "\n";
    std::cout << "  • CSV export for spreadsheets or Python plots\n\n";

    try
    {
        auto delaySamples = runDelayDriftTest(DELAY_TARGET_MS, DELAY_SAMPLES);
        auto jitterSamples = runLoopJitterTest(LOOP_INTERVAL_US, LOOP_ITERATIONS);
        auto throughput = runGpioThroughputTest(PROFILER_PIN, TOGGLE_CYCLES);

        auto delaySummary = summarize(delaySamples);
        auto jitterSummary = summarize(jitterSamples);

        std::cout << "=== Delay Drift (target " << DELAY_TARGET_MS << " ms) ===\n";
        std::cout << "Avg drift : " << std::showpos << delaySummary.average << " ms\n";
        std::cout << "Min / Max : " << delaySummary.minValue << " ms / "
                  << delaySummary.maxValue << " ms\n" << std::noshowpos;

        std::cout << "\n=== Loop Jitter (" << LOOP_INTERVAL_US << " µs interval) ===\n";
        std::cout << "Avg jitter: " << std::showpos << jitterSummary.average << " µs\n";
        std::cout << "Min / Max : " << jitterSummary.minValue << " µs / "
                  << jitterSummary.maxValue << " µs\n" << std::noshowpos;

        std::cout << "\n=== GPIO Toggle Throughput (pin " << PROFILER_PIN << ") ===\n";
        auto coutFlags = std::cout.flags();
        auto coutPrecision = std::cout.precision();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Cycles executed : " << TOGGLE_CYCLES << " high/low pairs\n";
        std::cout << "Total duration : " << throughput.durationMicros << " µs\n";
        std::cout << "Cycle rate     : " << throughput.cycleRateHz << " Hz\n";
        std::cout << std::setprecision(3);
        std::cout << "Write cost     : " << throughput.writeTimeMicros << " µs per digitalWrite\n";
        std::cout.flags(coutFlags);
        std::cout.precision(coutPrecision);

        exportCsv(delaySamples, jitterSamples, throughput);
        std::cout << "\nCSV log saved to performance_profile.csv (delay drift + loop jitter).\n";
        std::cout << "Import it into your notebook or spreadsheet to visualize histograms.\n";

        std::cout << "\nProfiling complete. Try rerunning with different workload/background tasks\n"
              << "to see how Linux scheduling impacts jitter.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Profiler failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
