//% color="#AA278D" weight=100 icon="\uf085"
//% block="I2C Slave"
namespace i2cSlave {

    /**
     * Initialise I2C slave at given address
     * @param addr I2C address e.g. 117 for 0x75
     */
    //% blockId="i2cSlave_init"
    //% block="init I2C slave at address %addr"
    //% addr.defl=117
    export function initSlave(addr: number): void {
        return;
    }

    /**
     * Returns true if new data received from master
     */
    //% blockId="i2cSlave_dataAvailable"
    //% block="I2C slave data available"
    export function dataAvailable(): boolean {
        return false;
    }

    /**
     * Read last received byte from master
     */
    //% blockId="i2cSlave_readByte"
    //% block="I2C slave read byte"
    export function readByte(): number {
        return 0;
    }

    /**
     * Write response byte to master
     * @param val byte value to send
     */
    //% blockId="i2cSlave_writeByte"
    //% block="I2C slave write byte %val"
    //% val.defl=0
    export function writeByte(val: number): void {
        return;
    }
}
