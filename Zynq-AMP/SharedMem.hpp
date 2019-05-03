#pragma once

#include <ANSIColors.hpp>
#include <iomanip>
#include <iostream>

#include <cassert>
#include <cerrno>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const size_t PAGE_SIZE      = getpagesize();
const uintptr_t PAGE_MASK   = ~((uintptr_t) PAGE_SIZE - 1);
const uintptr_t OFFSET_MASK = (uintptr_t) PAGE_SIZE - 1;

class SharedMemReferenceCounter {
  public:
    SharedMemReferenceCounter() {
        if (count == 0)  // If this is the first instance
            openMem();   // the memory has to be opened
        ++count;
    }
    ~SharedMemReferenceCounter() {
        --count;
        if (count == 0)  // If this was the last instance
            closeMem();  // the memory has to be closed
    }
    // Delete copy constructor and copy assignment
    SharedMemReferenceCounter(const SharedMemReferenceCounter &) = delete;
    SharedMemReferenceCounter &
    operator=(const SharedMemReferenceCounter &) = delete;

    int getFileDescriptor() const { return mem_fd; }

  private:
    void openMem() {
        mem_fd = open(       //
            "/dev/mem",      // file path
            O_RDWR | O_SYNC  // flags
        );
        if (mem_fd < 0) {
            std::cerr << ANSIColors::redb << "open(/dev/mem) failed (" << errno
                      << ")" << ANSIColors::reset << std::endl;
            std::ostringstream oss;
            oss << "open(/dev/mem) failed (" << errno << ")";
            throw std::runtime_error(oss.str());
        }
    }

    void closeMem() { close(mem_fd); }

    static size_t count;
    static int mem_fd;
};

template <class T>
class BaremetalShared {
  public:
    BaremetalShared() {
        // Get the base address of the page, and the offset within the page.
        uintptr_t base   = T::address & PAGE_MASK;
        uintptr_t offset = T::address & OFFSET_MASK;
        // Make sure we don't cross the page boundary
        assert(offset + sizeof(T) <= PAGE_SIZE);

        std::cout << std::hex << std::showbase << "T::address = " << T::address
                  << ", base = " << base << ", offset = " << offset
                  << ", PAGE_SIZE = " << PAGE_SIZE
                  << ", PAGE_MASK = " << PAGE_MASK << std::dec
                  << std::noshowbase << std::endl;

        // Map the hardware address of the shared memory region into the virtual
        // address space of the program.
        // Offset should be aligned to a page, and size should be a multiple of
        // the page size.
        memmap = mmap(                      //
            nullptr,                        // address
            PAGE_SIZE,                      // length
            PROT_READ | PROT_WRITE,         // protection
            MAP_SHARED,                     // flags
            sharedMem.getFileDescriptor(),  // file descriptor
            base                            // offset
        );
        if (memmap == MAP_FAILED) {
            std::ostringstream oss;
            oss << "mmap(" << std::hex << std::showbase << base << std::dec
                << std::noshowbase << ") failed (" << errno << ")";
            std::cerr << ANSIColors::redb << "mmap(" << std::hex
                      << std::showbase << base << std::dec << std::noshowbase
                      << ") failed (" << errno << ")" << ANSIColors::reset
                      << std::endl;
            throw std::runtime_error(oss.str());
        }
        uintptr_t memmapp = reinterpret_cast<uintptr_t>(memmap);
        structdata        = reinterpret_cast<volatile T *>(memmapp + offset);

        std::cout << std::hex << std::showbase << "memmap = " << memmap
                  << std::endl;
        for (size_t i = 0; i < sizeof(T); ++i) {
            int data = reinterpret_cast<volatile uint8_t *>(structdata)[i];
            std::cout << data << " ";
        }
        std::cout << std::dec << std::noshowbase << std::endl;
    }

    BaremetalShared(const BaremetalShared &) = delete;
    BaremetalShared &operator=(const BaremetalShared &) = delete;

    ~BaremetalShared() { munmap(memmap, PAGE_SIZE); }

    volatile T *ptr() { return structdata; }
    volatile T *operator->() { return structdata; }

  private:
    volatile T *structdata;
    void *memmap;
    SharedMemReferenceCounter sharedMem;
};