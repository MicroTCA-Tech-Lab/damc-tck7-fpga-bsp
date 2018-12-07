/*
 * Copyright (c) 2018 Deutsches Elektronen-Synchrotron DESY
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


// to compile:
// g++ -Wall -Wextra -pedantic App.cpp
//

#include <iostream>
#include <iomanip>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


//=============================================================================

struct con_colors {
    static const std::string RED;
    static const std::string GREEN;
    static const std::string ENDC;
};

const std::string con_colors::RED = "\033[31m";
const std::string con_colors::GREEN = "\033[32m";
const std::string con_colors::ENDC = "\033[39m";

//=============================================================================
// register map

constexpr uint32_t ADDR_GPIO_DDR3 = 0x0;
constexpr uint32_t SIZE_GPIO_DDR3 = 0x1000;

constexpr uint32_t ADDR_AXI_DMA = 0x1000;
constexpr uint32_t SIZE_AXI_DMA = 0x1000;

constexpr uint32_t ADDR_AXI_TR_GEN = 0x10000;
constexpr uint32_t SIZE_AXI_TR_GEN = 0x10000;

//=============================================================================
/// implements LFSR as described in "AXI Traffic Generator v3.0"
class AxiTrafficGenLfsr {
public:
    AxiTrafficGenLfsr(uint16_t seed) : val{seed} {};

    /// advance LFSR by one iteration, return new value
    uint16_t advance() {
        int bit_sel[] = {0, 1, 3, 12};
        uint16_t new_bit = 0;
        for (auto bi : bit_sel) {
            new_bit ^= (val >> bi) & 0x1;
        }

        // xnor from xor
        new_bit ^= 0x1;

        val = (new_bit << 15) | (val >> 1);
        return val;
    }

    /// get value without advancing the LFSR
    uint16_t get() {
        return val;
    }

private:
    uint16_t val;
};

//=============================================================================
/// interface to Xilinx DMA driver
class XdmaIsr {
public:
    XdmaIsr(std::string event_file) {
        fd = open(event_file.c_str(), O_RDWR | O_SYNC);
        if (fd < 0) {
            throw std::runtime_error("could not open event file");
        }
    }

    ~XdmaIsr() {
        close(fd);
    }

    /// blocks until IRQ is received
    void wait_for_irq() {
        uint32_t evnt;
        int rc = read(fd, &evnt, sizeof(evnt));
        if (rc != 4) {
            throw std::runtime_error("unexpected return code from event file");
        }
        std::cout << "XdmaIsr: received an event, value: " << evnt << "\n";
    }

private:
    int fd;
};


//=============================================================================
/// general interface for register access (using wr32 and rd32 methods)
class XdmaRegAccess {
public:
    XdmaRegAccess(std::string user_file, uint32_t mmap_size, uint32_t mmap_offset):
        mmap_size{mmap_size}
    {
        fd = open(user_file.c_str(), O_RDWR | O_SYNC);
        if (fd < 0) {
            throw std::runtime_error("could not open user file");
        }

        mmap_base = mmap(0, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mmap_offset);
        if (mmap_base == MAP_FAILED) {
            throw std::runtime_error("mmap failed");
        }
    }

    ~XdmaRegAccess() {
        munmap(mmap_base, mmap_size);
        close(fd);
    }

    /// 32-bit write
    void wr32(uint32_t byte_addr, uint32_t data) {
        *(uint32_t*)((char*)mmap_base + byte_addr) = data;
    }

    /// 32-bit read
    uint32_t rd32(uint32_t byte_addr) {
        return *(uint32_t*)((char*)mmap_base + byte_addr);
    }

private:
    int fd;
    void* mmap_base;
    uint32_t mmap_size;
};


//=============================================================================
/// AXI DMA controller
class HwAxiDmaControl : XdmaRegAccess {
public:
    HwAxiDmaControl(std::string user_file, uint32_t mmap_size, uint32_t mmap_offset):
        XdmaRegAccess{user_file, mmap_size, mmap_offset} {};

    void init() {
        wr32(ADDR_S2MM_DMACR, S2MM_DMACR_RS | S2MM_DMACR_IOC_IRQEN);
        uint32_t status = rd32(ADDR_S2MM_DMASR);
        std::cout << "HwAxiDmaControl: status after run bit set = " <<
            std::hex << status << std::dec << "\n";
    }

    /// arm (or trigger) the DMA
    void arm(uint32_t byte_len) {
        wr32(ADDR_S2MM_LENGTH, byte_len);
    }

    /// return number of bytes transfered
    uint32_t get_bytes_trans() {
        return rd32(ADDR_S2MM_LENGTH);
    }

    /// acknowledge the interrupt request
    void ack_irq() {
        std::cout << "HwAxiDmaControl: status = " <<
            std::hex << rd32(ADDR_S2MM_DMASR) << std::dec << "\n";

        wr32(ADDR_S2MM_DMASR, S2MM_DMASR_IOC_IRQ);

        std::cout << "HwAxiDmaControl: status = " <<
            std::hex << rd32(ADDR_S2MM_DMASR) << std::dec << "\n";
    }

private:
    static constexpr uint32_t ADDR_S2MM_DMACR = 0x30;
    static constexpr uint32_t ADDR_S2MM_DMASR = 0x34;
    static constexpr uint32_t ADDR_S2MM_DA = 0x48;
    static constexpr uint32_t ADDR_S2MM_DA_MSB = 0x4C;
    static constexpr uint32_t ADDR_S2MM_LENGTH = 0x58;

    static constexpr uint32_t S2MM_DMACR_RS = 1 << 0;
    static constexpr uint32_t S2MM_DMACR_RESET = 1 << 2;
    static constexpr uint32_t S2MM_DMACR_IOC_IRQEN = 1 << 12;

    static constexpr uint32_t S2MM_DMASR_HALTED = 1 << 0;
    static constexpr uint32_t S2MM_DMASR_IDLE = 1 << 1;
    static constexpr uint32_t S2MM_DMASR_IOC_IRQ = 1 << 12;

};

//=============================================================================
/// data consumer

class DataConsumer{
public:
    DataConsumer(std::string c2h_file,
            uint16_t lfsr_seed, uint32_t max_buf_size) : lfsr{lfsr_seed}
    {
        fd = open(c2h_file.c_str(), O_RDWR | O_SYNC);
        if (fd < 0) {
            throw std::runtime_error("could not open c2h file");
        }

        buf = malloc(max_buf_size);
        if (buf == NULL) {
            throw std::runtime_error("malloc failed");
        }
    };

    ~DataConsumer() {
        close(fd);
        free(buf);
    }

    /// get the data from on-board DDR3 and compare it to reference LFSR impl
    void check(uint32_t bytes_len) {
        std::cout << "DataConsumer::check, bytes_len = " << bytes_len << "\n";

        // read from DDR3 (over PCIe)
        lseek(fd, 0x80000000UL, SEEK_SET);
        int rc = read(fd, buf, bytes_len);
        if (rc < 0) {
            throw std::runtime_error("read returned error");
        }

        // compare received data with expected
        uint16_t* buf_ptr = reinterpret_cast<uint16_t*>(buf);
        unsigned int nr_samples_ok = 0;
        unsigned int nr_samples_tot = 0;

        for (unsigned int i = 0; i < bytes_len / 4; i++) {
            bool dbg_print = (i < 5) || (i > bytes_len / 4 - 5);
            uint16_t sampl_expect = lfsr.get();
            lfsr.advance();

            if (dbg_print) {
                std::cout << "at " << std::setw(4) << i << " expect " <<
                    std::hex << std::setw(4) << sampl_expect << ", got: ";
            }

            for (int j = 0; j < 32 / 16; j++) {
                uint16_t sampl = buf_ptr[i*2+j];
                if (sampl == sampl_expect) {
                    nr_samples_ok++;
                }
                nr_samples_tot++;

                if (dbg_print) {
                    std::cout << std::setw(4) << sampl << " | ";
                }
            }

            if (dbg_print) {
                std::cout << "\b\n";
            }
        }

        // print results
        float ok_percent = nr_samples_ok * 100.0 / nr_samples_tot;
        if (nr_samples_ok == nr_samples_tot) {
            std::cout << con_colors::GREEN;
        } else {
            std::cout << con_colors::RED;
        }

        std::cout << "DataConsumer: check, ok = " << nr_samples_ok <<
            ", tot = " << nr_samples_tot << " (" <<
            std::setprecision(4) << ok_percent << "%)\n";

        std::cout << con_colors::ENDC;
    }

private:
    AxiTrafficGenLfsr lfsr;
    void *buf;
    int fd;
};

//=============================================================================
/// application capturing data and checking data integrity
int main() {

    uint16_t LFSR_SEED = 0xABCD;
    uint32_t MAX_BYTES_TO_CAPT = 0x2000;

    auto isr = XdmaIsr{"/dev/xdma/card0/events0"};
    auto dma_ctrl = HwAxiDmaControl{"/dev/xdma/card0/user", ADDR_AXI_DMA, SIZE_AXI_DMA};
    auto data_consumer = DataConsumer("/dev/xdma/card0/c2h0", LFSR_SEED, MAX_BYTES_TO_CAPT);

    dma_ctrl.init();

    while(1) {
        dma_ctrl.arm(MAX_BYTES_TO_CAPT);

        isr.wait_for_irq();
        dma_ctrl.ack_irq();

        uint32_t bytes_len = dma_ctrl.get_bytes_trans();
        data_consumer.check(bytes_len);
    }
}

