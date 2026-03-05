#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void bluetooth_init()
{
    SerialBT.begin("EARA_DEVICE");
}

void bluetooth_send(String msg)
{
    SerialBT.println(msg);
}

String bluetooth_read()
{
    if (SerialBT.available())
    {
        return SerialBT.readString();
    }
    return "";
}