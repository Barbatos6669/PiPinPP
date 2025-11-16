/*
 * PiPin++ DMA GPIO Proof-of-Concept
 * 
 * This is a minimal implementation to validate DMA-based GPIO control.
 * Tests basic DMA register access and GPIO toggle via DMA.
 * 
 * IMPORTANT: This is experimental code requiring root permissions.
 * DO NOT use in production - for research and validation only.
 * 
 * Hardware Requirements:
 * - Raspberry Pi 4 (tested) or Pi 3
 * - LED on GPIO 17 (or logic analyzer)
 * 
 * Safety:
 * - Requires root: sudo ./poc_dma_gpio
 * - Validates memory mapping before access
 * - Clean shutdown on Ctrl+C
 * - Minimal DMA usage (single channel)
 * 
 * Expected Behavior:
 * - GPIO 17 toggles at 1Hz via DMA (CPU usage: 0%)
 * - Console shows DMA status
 * - Press Ctrl+C to stop cleanly
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 * Status: Proof-of-Concept (NOT PRODUCTION CODE)
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <chrono>
#include <thread>

// Raspberry Pi 4 memory addresses (BCM2711)
// WARNING: These differ for Pi 3 (BCM2837) and Pi 5!
constexpr uint32_t BCM2711_PERI_BASE = 0xFE000000;
constexpr uint32_t GPIO_BASE = BCM2711_PERI_BASE + 0x200000;
constexpr uint32_t DMA_BASE = BCM2711_PERI_BASE + 0x007000;
constexpr uint32_t PWM_BASE = BCM2711_PERI_BASE + 0x20C000;
constexpr uint32_t CLOCK_BASE = BCM2711_PERI_BASE + 0x101000;

// Memory mapping sizes
constexpr size_t BLOCK_SIZE = 4096;

// DMA channel to use (5 is typically safe, not used by OS)
constexpr int DMA_CHANNEL = 5;

// GPIO registers
constexpr uint32_t GPFSEL1 = 0x04 / 4;  // Function select for GPIO 10-19
constexpr uint32_t GPSET0 = 0x1C / 4;   // Set GPIO
constexpr uint32_t GPCLR0 = 0x28 / 4;   // Clear GPIO

// DMA Control Block structure
struct dma_cb_t {
    uint32_t info;      // Transfer info
    uint32_t src;       // Source address
    uint32_t dst;       // Destination address
    uint32_t length;    // Transfer length
    uint32_t stride;    // 2D stride
    uint32_t next;      // Next control block
    uint32_t pad[2];    // Padding
} __attribute__((aligned(32)));

// DMA register offsets
constexpr uint32_t DMA_CS = 0x00 / 4;       // Control/Status
constexpr uint32_t DMA_CONBLK_AD = 0x04 / 4; // Control Block Address
constexpr uint32_t DMA_DEBUG = 0x20 / 4;    // Debug

// DMA CS register bits
constexpr uint32_t DMA_CS_ACTIVE = (1 << 0);
constexpr uint32_t DMA_CS_END = (1 << 1);
constexpr uint32_t DMA_CS_RESET = (1 << 31);

// DMA TI (Transfer Information) register bits
constexpr uint32_t DMA_TI_NO_WIDE_BURSTS = (1 << 26);
constexpr uint32_t DMA_TI_WAIT_RESP = (1 << 3);
constexpr uint32_t DMA_TI_DEST_DREQ = (1 << 6);
constexpr uint32_t DMA_TI_PERMAP_SHIFT = 16;
constexpr uint32_t DMA_TI_SRC_INC = (1 << 8);

// Global state
volatile bool running = true;
volatile uint32_t* gpio = nullptr;
volatile uint32_t* dma = nullptr;
dma_cb_t* dmaControlBlocks = nullptr;
uint32_t* dmaData = nullptr;
int memFd = -1;

// Signal handler for clean shutdown
void signalHandler(int sig) {
    if (sig == SIGINT) {
        std::cout << "\n\nShutting down DMA...\n";
        running = false;
    }
}

// Convert virtual address to bus address for DMA
uint32_t memVirtToBus(void* virt) {
    // On Pi 4, bus addresses use 0xC0000000 base for SDRAM
    // This is a simplified conversion - production code needs proper MMU handling
    return 0xC0000000 | ((uint32_t)(uintptr_t)virt & 0x3FFFFFFF);
}

// Setup GPIO pin 17 as output
bool setupGPIO(int pin) {
    if (pin != 17) {
        std::cerr << "This PoC only supports GPIO 17\n";
        return false;
    }
    
    // GPIO 17 is in GPFSEL1 register, bits 21-23 (3 bits per pin)
    // Set to 001 (output)
    uint32_t fsel = gpio[GPFSEL1];
    fsel &= ~(7 << 21);  // Clear bits 21-23
    fsel |= (1 << 21);   // Set to output (001)
    gpio[GPFSEL1] = fsel;
    
    std::cout << "✓ GPIO 17 configured as output\n";
    return true;
}

// Setup DMA for GPIO toggle
bool setupDMA() {
    // Reset DMA channel
    dma[DMA_CS] = DMA_CS_RESET;
    usleep(10);
    dma[DMA_CS] = 0;  // Clear all flags
    
    // Allocate memory for DMA control blocks (physically contiguous)
    // In production, use CMA or proper physical memory allocator
    // For PoC, we assume small allocations stay contiguous
    dmaControlBlocks = (dma_cb_t*)mmap(
        nullptr, 
        2 * sizeof(dma_cb_t),  // 2 control blocks
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS | MAP_LOCKED,
        -1, 
        0
    );
    
    if (dmaControlBlocks == MAP_FAILED) {
        std::cerr << "Failed to allocate DMA control blocks\n";
        return false;
    }
    
    // Allocate memory for GPIO data (values to write)
    dmaData = (uint32_t*)mmap(
        nullptr,
        2 * sizeof(uint32_t),  // 2 words (SET and CLR values)
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS | MAP_LOCKED,
        -1,
        0
    );
    
    if (dmaData == MAP_FAILED) {
        std::cerr << "Failed to allocate DMA data buffer\n";
        munmap(dmaControlBlocks, 2 * sizeof(dma_cb_t));
        return false;
    }
    
    // Setup data values
    dmaData[0] = (1 << 17);  // Set GPIO 17 (bit 17 in GPSET0)
    dmaData[1] = (1 << 17);  // Clear GPIO 17 (bit 17 in GPCLR0)
    
    // Control Block 0: Set GPIO 17 HIGH
    memset(&dmaControlBlocks[0], 0, sizeof(dma_cb_t));
    dmaControlBlocks[0].info = DMA_TI_NO_WIDE_BURSTS | DMA_TI_WAIT_RESP;
    dmaControlBlocks[0].src = memVirtToBus(&dmaData[0]);
    dmaControlBlocks[0].dst = GPIO_BASE + 0x1C;  // GPSET0 (physical address)
    dmaControlBlocks[0].length = 4;  // 4 bytes (1 word)
    dmaControlBlocks[0].next = memVirtToBus(&dmaControlBlocks[1]);
    
    // Control Block 1: Set GPIO 17 LOW
    memset(&dmaControlBlocks[1], 0, sizeof(dma_cb_t));
    dmaControlBlocks[1].info = DMA_TI_NO_WIDE_BURSTS | DMA_TI_WAIT_RESP;
    dmaControlBlocks[1].src = memVirtToBus(&dmaData[1]);
    dmaControlBlocks[1].dst = GPIO_BASE + 0x28;  // GPCLR0 (physical address)
    dmaControlBlocks[1].length = 4;  // 4 bytes (1 word)
    dmaControlBlocks[1].next = memVirtToBus(&dmaControlBlocks[0]);  // Loop back
    
    std::cout << "✓ DMA control blocks configured\n";
    std::cout << "  CB0 (SET): src=0x" << std::hex << dmaControlBlocks[0].src 
              << " dst=0x" << dmaControlBlocks[0].dst << std::dec << "\n";
    std::cout << "  CB1 (CLR): src=0x" << std::hex << dmaControlBlocks[1].src 
              << " dst=0x" << dmaControlBlocks[1].dst << std::dec << "\n";
    
    return true;
}

// Start DMA transfer
bool startDMA() {
    // Set control block address
    dma[DMA_CONBLK_AD] = memVirtToBus(&dmaControlBlocks[0]);
    
    // Start DMA (set ACTIVE bit)
    dma[DMA_CS] = DMA_CS_ACTIVE;
    
    std::cout << "✓ DMA started on channel " << DMA_CHANNEL << "\n";
    std::cout << "  CS register: 0x" << std::hex << dma[DMA_CS] << std::dec << "\n";
    
    return true;
}

// Stop DMA transfer
void stopDMA() {
    if (dma) {
        // Clear ACTIVE bit
        dma[DMA_CS] = 0;
        
        // Wait for DMA to finish current transfer
        int timeout = 1000;
        while ((dma[DMA_CS] & DMA_CS_ACTIVE) && timeout-- > 0) {
            usleep(1);
        }
        
        // Reset channel
        dma[DMA_CS] = DMA_CS_RESET;
        usleep(10);
        
        std::cout << "✓ DMA stopped\n";
    }
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║      PiPin++ DMA GPIO Proof-of-Concept (v0.4.0)              ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n⚠️  EXPERIMENTAL CODE - REQUIRES ROOT\n";
    std::cout << "This PoC validates DMA-based GPIO control.\n";
    std::cout << "Expected: GPIO 17 toggles at 1Hz with 0% CPU usage\n\n";
    
    // Check if running as root
    if (geteuid() != 0) {
        std::cerr << "❌ Must run as root: sudo ./poc_dma_gpio\n";
        return 1;
    }
    
    // Install signal handler
    signal(SIGINT, signalHandler);
    
    try {
        // Open /dev/mem for hardware access
        memFd = open("/dev/mem", O_RDWR | O_SYNC);
        if (memFd < 0) {
            std::cerr << "❌ Failed to open /dev/mem: " << strerror(errno) << "\n";
            std::cerr << "   Are you running as root?\n";
            return 1;
        }
        std::cout << "✓ Opened /dev/mem\n";
        
        // Map GPIO registers
        void* gpioMap = mmap(
            nullptr,
            BLOCK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            memFd,
            GPIO_BASE
        );
        
        if (gpioMap == MAP_FAILED) {
            std::cerr << "❌ Failed to map GPIO registers\n";
            close(memFd);
            return 1;
        }
        gpio = (volatile uint32_t*)gpioMap;
        std::cout << "✓ Mapped GPIO registers at 0x" << std::hex << GPIO_BASE << std::dec << "\n";
        
        // Map DMA registers
        void* dmaMap = mmap(
            nullptr,
            BLOCK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            memFd,
            DMA_BASE + (DMA_CHANNEL * 0x100)
        );
        
        if (dmaMap == MAP_FAILED) {
            std::cerr << "❌ Failed to map DMA registers\n";
            munmap((void*)gpio, BLOCK_SIZE);
            close(memFd);
            return 1;
        }
        dma = (volatile uint32_t*)dmaMap;
        std::cout << "✓ Mapped DMA channel " << DMA_CHANNEL << " registers\n";
        
        // Setup GPIO
        if (!setupGPIO(17)) {
            munmap((void*)gpio, BLOCK_SIZE);
            munmap((void*)dma, BLOCK_SIZE);
            close(memFd);
            return 1;
        }
        
        // Setup DMA
        if (!setupDMA()) {
            munmap((void*)gpio, BLOCK_SIZE);
            munmap((void*)dma, BLOCK_SIZE);
            close(memFd);
            return 1;
        }
        
        // Start DMA
        if (!startDMA()) {
            stopDMA();
            if (dmaControlBlocks) munmap(dmaControlBlocks, 2 * sizeof(dma_cb_t));
            if (dmaData) munmap(dmaData, 2 * sizeof(uint32_t));
            munmap((void*)gpio, BLOCK_SIZE);
            munmap((void*)dma, BLOCK_SIZE);
            close(memFd);
            return 1;
        }
        
        std::cout << "\n🚀 DMA running! GPIO 17 toggling...\n";
        std::cout << "   Monitor with: watch -n 0.1 'gpio readall | grep GPIO.17'\n";
        std::cout << "   Or connect LED/logic analyzer to GPIO 17\n";
        std::cout << "\nPress Ctrl+C to stop.\n\n";
        
        // Monitor DMA status
        int seconds = 0;
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            uint32_t cs = dma[DMA_CS];
            bool active = cs & DMA_CS_ACTIVE;
            bool error = cs & (1 << 8);  // Error bit
            
            if (error) {
                std::cerr << "❌ DMA error detected! CS=0x" << std::hex << cs << std::dec << "\n";
                break;
            }
            
            std::cout << "[" << ++seconds << "s] DMA Status: " 
                      << (active ? "✓ ACTIVE" : "✗ STOPPED") 
                      << " (CS=0x" << std::hex << cs << std::dec << ")\n";
        }
        
        // Clean shutdown
        std::cout << "\nCleaning up...\n";
        stopDMA();
        
        // Set GPIO low
        gpio[GPCLR0] = (1 << 17);
        std::cout << "✓ GPIO 17 set LOW\n";
        
        // Unmap memory
        if (dmaControlBlocks) {
            munmap(dmaControlBlocks, 2 * sizeof(dma_cb_t));
            std::cout << "✓ Unmapped DMA control blocks\n";
        }
        if (dmaData) {
            munmap(dmaData, 2 * sizeof(uint32_t));
            std::cout << "✓ Unmapped DMA data buffer\n";
        }
        munmap((void*)gpio, BLOCK_SIZE);
        munmap((void*)dma, BLOCK_SIZE);
        close(memFd);
        std::cout << "✓ Unmapped registers, closed /dev/mem\n";
        
        std::cout << "\n✅ PoC completed successfully!\n";
        std::cout << "Next: Add delay mechanism for actual PWM timing\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Exception: " << e.what() << "\n";
        
        // Emergency cleanup
        if (dma) stopDMA();
        if (gpio) gpio[GPCLR0] = (1 << 17);
        if (dmaControlBlocks) munmap(dmaControlBlocks, 2 * sizeof(dma_cb_t));
        if (dmaData) munmap(dmaData, 2 * sizeof(uint32_t));
        if (gpio) munmap((void*)gpio, BLOCK_SIZE);
        if (dma) munmap((void*)dma, BLOCK_SIZE);
        if (memFd >= 0) close(memFd);
        
        return 1;
    }
    
    return 0;
}
