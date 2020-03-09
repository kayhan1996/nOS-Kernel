#include "Drivers/timer.h"
#include "Libraries/printx.h"
#include "Libraries/utils.h"
#include "Processes/process.h"
#include "Processes/exceptions.h"

enum Exception_State { Synchronouse = 0, IRQ = 1, FIQ = 2, SError = 3 };

void timer_tick() {
    set_next_time_arm();
    schedule();
}

void exception_handler() {
    timer_tick();
}

void mmu_fault(uint64_t iss, uint64_t far) {
    switch (iss) {
    case 0b000000:
        printf("Address size fault, level 0 of translation or translation "
               "table base register\n");
        break;
    case 0b000001:
        printf("Address size fault, level 1\n");
        break;
    case 0b000010:
        printf("Address size fault, level 2\n");
        break;
    case 0b000011:
        printf("Address size fault, level 3\n");
        break;
    case 0b000100:
        printf("Translation fault, level 0\n");
        break;
    case 0b000101:
        printf("Translation fault, level 1\n");
        break;
    case 0b000110:
        printf("Translation fault, level 2\n");
        break;
    case 0b000111:
        printf("Translation fault, level 3\n");
        break;
    case 0b001001:
        printf("Access flag fault, level 1\n");
        break;
    case 0b001010:
        printf("Access flag fault, level 2\n");
        break;
    case 0b001011:
        printf("Access flag fault, level 3\n");
        break;
    case 0b001101:
        printf("Permission fault, level 1\n");
        break;
    case 0b001110:
        printf("Permission fault, level 2\n");
        break;
    case 0b001111:
        printf("Permission fault, level 3\n");
        break;
    case 0b010000:
        printf("Synchronous External about\n");
        break;
    default:
        break;
    }
}

void error_handler(uint64_t FAR, uint64_t ESR, uint64_t ELR) {
    uint32_t ec = GET_BITS(ESR, 31, 26);

    switch (ec) {
    case 0b0100001:
        printf("Exception Class: INSTRUCTION ABORT FROM SAME LEVEL/ MMU FAULT\n");
        mmu_fault(GET_BITS(ESR, 5, 0), FAR);
        break;
    case 0b0100100:
        printf("Exception Class: DATA ABORT FROM LOWER EXCEPTION LEVEL/ MMU FAULT\n");
        mmu_fault(GET_BITS(ESR, 5, 0), FAR);
        break;
    case 0b0100101:
        printf("Exception Class: DATA ABORT/ MMU FAULT\n");
        mmu_fault(GET_BITS(ESR, 5, 0), FAR);
        break;
    default:
        printf("Exception Class: %d\n", ec);
        break;
    }

    printf("Exception occurred at instruction 0x%x\n", ELR);
    printf("Error occured while accessing address 0x%x\n", FAR);

    while (1) {
    };
}