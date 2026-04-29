declare namespace i2cSlave {
    //% shim=i2cSlave::initSlave
    function initSlave(addr: number): void;

    //% shim=i2cSlave::dataAvailable
    function dataAvailable(): boolean;

    //% shim=i2cSlave::readByte
    function readByte(): number;

    //% shim=i2cSlave::writeByte
    function writeByte(val: number): void;
}
