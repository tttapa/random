#include <atomic>     // atomic_flag
#include <cassert>    // assert
#include <cstdint>    // uint32_t
#include <iostream>   // cout
#include <stdexcept>  // runtime_error
#ifdef BAREMETAL
#include <ps7_cortexa9_1/include/sleep.h>
#else
#include <unistd.h>
#endif

// ================================== LOCK ================================== //

class ScopedLock {
  public:
    ScopedLock(volatile std::atomic_flag &lock) : lock{lock} {
        bool locked = true;
        for (size_t i = 0; i < NUM_RETRIES; ++i) {
            locked = lock.test_and_set(std::memory_order_acquire);
            std::cout << "locked = " << locked << std::endl;
            if (locked)
                usleep(WAIT_TIME);
            else
                break;
        }
        if (locked)
            throw std::runtime_error("Timeout: Could not acquire lock");
    }

    ~ScopedLock() {
        lock.clear(std::memory_order_release);
        std::cout << "released" << std::endl;
    }

  private:
    volatile std::atomic_flag &lock;
    constexpr static size_t NUM_RETRIES   = 10;
    constexpr static useconds_t WAIT_TIME = 50;
};

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

// ============================== TEST STRUCT =============================== //

#define atomic_flag32 std::atomic_flag __attribute__((aligned(4)))

struct TestStruct {
  private:
    mutable atomic_flag32 test_lock = ATOMIC_FLAG_INIT;
    uint32_t counter                = 0;

  public:
    uint32_t ack = 0;
    uint32_t testBaremetal2Linux;
    uint32_t testLinux2Baremetal;
    uint32_t magic = 0xAA55AA55;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x2000;

    void increment() volatile {
        {
            // ScopedLock lock(test_lock);
            uint32_t tmp = counter;
            usleep(40);
            counter = tmp + 1;
        }
        usleep(10);
    }

    uint32_t getCounter() const volatile {
        // ScopedLock lock(test_lock);
        return counter;
    }

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory
    static volatile TestStruct *init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(TestStruct));
        return new ((void *) address) TestStruct();
    }
#endif

  private:
    TestStruct() = default;
};

#ifndef BAREMETAL
// ================================= LINUX ================================== //

#include <SharedMem.hpp>  // BaremetalShared

int main() {
    BaremetalShared<TestStruct> baremetal;  // Maps the memory using mmap
    baremetal->testLinux2Baremetal = 0xBEEFDEAD;
    baremetal->ack                 = 1;

    // Wait for the baremetal application to start
    std::cout << "Waiting for Baremetal" << std::endl;
    while (baremetal->ack != 2)
        ;
    assert(baremetal->testBaremetal2Linux == 0xDEADBEEF);
    std::cout << "testBaremetal2Linux is correct\n";
    std::cout << "Go\n";

    // Increment the shared value many times
    for (size_t i = 0; i < 1'000; ++i)
        baremetal->increment();

    std::cout << std::endl << "Waiting for Baremetal to finish" << std::endl;
    // Wait for baremetal to finish
    while (baremetal->ack != 3)
        usleep(100);

    uint32_t result = baremetal->getCounter();
    std::cout << "Result = " << result << std::endl;
    assert(result == 2'000);
}

#else
// =============================== BARE-METAL =============================== //

#include "eagle_setup_ipc.hpp"

int main() {
    eagle_setup_ipc();  // Configure the inter-processor communication
    usleep(1'000);

    while (1) {
        try {
            // Baremetal always starts before Linux
            volatile TestStruct *sm = TestStruct::init();
            sm->testBaremetal2Linux = 0xDEADBEEF;
            std::cout << "TestStruct initialized at " << std::hex
                      << std::showbase << (uintptr_t) sm << std::dec
                      << std::noshowbase << std::endl;

            usleep(20'000'000);
            std::cout << "C++ Baremetal Started" << std::endl
                      << "Waiting for Linux" << std::endl;
            while (sm->ack != 1)
                usleep(100);
            assert(sm->testLinux2Baremetal == 0xBEEFDEAD);
            std::cout << "testLinux2Baremetal is correct" << std::endl;
            std::cout << "Sending ack to Linux" << std::endl;
            sm->ack = 2;

            for (size_t i = 0; i < 1'000; ++i)
                sm->increment();
            sm->ack = 3;

            std::cout << "Done" << std::endl;
        } catch (std::exception &e) {
            std::cout << "Baremetal Exception: " << e.what() << std::endl;
        }
        usleep(2'000'000);
    }
}
#endif
