#pragma once
#include <stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void io_wait();   


void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

void io_wait(uint64_t us);

// memory mapped io stuff

void mOutb(uint64_t address, uint8_t value);
uint8_t mInb(uint64_t address);

void mOutw(uint64_t address, uint16_t value);
uint16_t mInw(uint64_t address);

void mOutl(uint64_t address, uint32_t value);
uint32_t mInl(uint64_t address);

void mOutq(uint64_t address, uint64_t value);
uint64_t mInq(uint64_t address);
