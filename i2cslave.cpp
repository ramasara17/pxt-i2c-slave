#include "pxt.h"

using namespace pxt;

static uint8_t rxBuf[32];
static uint8_t txBuf[32];
static uint8_t lastByte = 0;
static bool    gotData  = false;

static void twis_poll() {
    if (NRF_TWIS0->EVENTS_WRITE) {
        NRF_TWIS0->EVENTS_WRITE   = 0;
        NRF_TWIS0->TASKS_RESUME   = 1;
    }
    if (NRF_TWIS0->EVENTS_ERROR) {
        NRF_TWIS0->EVENTS_ERROR   = 0;
        NRF_TWIS0->ERRORSRC       = 0;
        NRF_TWIS0->TASKS_RESUME   = 1;
    }
    if (NRF_TWIS0->EVENTS_STOPPED) {
        NRF_TWIS0->EVENTS_STOPPED = 0;
        uint32_t amt = NRF_TWIS0->RXD.AMOUNT;
        if (amt > 0) {
            lastByte = rxBuf[0];
            gotData  = true;
            NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuf;
            NRF_TWIS0->RXD.MAXCNT = 32;
        }
    }
}

namespace i2cSlave {

    //%
    void initSlave(int addr) {
        NRF_TWI0->ENABLE  = 0;
        NRF_TWI1->ENABLE  = 0;

        NRF_GPIO->PIN_CNF[20] = 0x0000060C;
        NRF_GPIO->PIN_CNF[19] = 0x0000060C;

        NRF_TWIS0->PSEL.SDA   = 20;
        NRF_TWIS0->PSEL.SCL   = 19;
        NRF_TWIS0->ADDRESS[0] = (uint8_t)addr;
        NRF_TWIS0->CONFIG     = 1;

        NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuf;
        NRF_TWIS0->RXD.MAXCNT = 32;
        NRF_TWIS0->TXD.PTR    = (uint32_t)txBuf;
        NRF_TWIS0->TXD.MAXCNT = 32;

        NRF_TWIS0->SHORTS =
            (1 << 13) |
            (1 << 14);

        NRF_TWIS0->EVENTS_WRITE   = 0;
        NRF_TWIS0->EVENTS_READ    = 0;
        NRF_TWIS0->EVENTS_STOPPED = 0;
        NRF_TWIS0->EVENTS_ERROR   = 0;

        NRF_TWIS0->ENABLE = 9;
    }

    //%
    bool dataAvailable() {
        twis_poll();
        return gotData;
    }

    //%
    int readByte() {
        gotData = false;
        return (int)lastByte;
    }

    //%
    void writeByte(int val) {
        txBuf[0]              = (uint8_t)val;
        NRF_TWIS0->TXD.PTR    = (uint32_t)txBuf;
        NRF_TWIS0->TXD.MAXCNT = 1;
    }
}
