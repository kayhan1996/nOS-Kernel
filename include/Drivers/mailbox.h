#ifndef MAILBOX_H
#define MAILBOX_H

#include "stdint.h"

int call_mailbox(uint32_t* message, uint8_t channel);

#endif