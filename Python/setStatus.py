import serial.tools.list_ports
import time
import sys

reply_time = 0.1
unknown_txt = "unknown"
unknown_no = -1
is_running = False
is_checking = True
serial_reply = None
system_output = ""
activeID = "Unkown"
write_to_chip_flag = False
loopfinished = False

# Serial Interface Definition
# 1: Start Identifier
# 2: Msg Type
# 3: Sender ID
# 4: PayLoad 1
# 5: PayLoad 2
# 6: PayLoad 3
# 7: PayLoad 4
# 8: End Identifier
#
#


def hexToByte(inputObject):
    return (inputObject).to_bytes(1, byteorder="big")


frameLength = 8
payLoadLength = 4
startIdentifier = 0x0F
endIdentfier = 0xF0

writeInformation = 1
getInformation = 2
findDevice = 3


class msgFrame:
    buffer = [0, 0, 0, 0, 0, 0, 0, 0]

    def __init__(self, msgType, senderId, payLoad):

        if type(payLoad) is not list:
            payLoad = [payLoad]

        if len(payLoad) < payLoadLength:
            requiredStubLength = payLoadLength - len(payLoad)

            for addElement in range(requiredStubLength):
                payLoad.append(0)

        self.buffer[0] = startIdentifier
        self.buffer[1] = msgType
        self.buffer[2] = senderId
        for index, information in enumerate(payLoad):
            self.buffer[3 + index] = information
        self.buffer[7] = endIdentfier

    def __str__(self):
        newLine = "\n"
        stringBuffer = (
            newLine
            + "Start Identifier : "
            + str(self.buffer[0])
            + newLine
            + "Msg Type         : "
            + str(self.buffer[1])
            + newLine
            + "Sender ID        : "
            + str(self.buffer[2])
            + newLine
            + "PayLoad 1        : "
            + str(self.buffer[3])
            + newLine
            + "PayLoad 2        : "
            + str(self.buffer[4])
            + newLine
            + "PayLoad 3        : "
            + str(self.buffer[5])
            + newLine
            + "PayLoad 4        : "
            + str(self.buffer[6])
            + newLine
            + "End Identifier   : "
            + str(self.buffer[7])
        )

        return stringBuffer

    def getBuffer(self):
        return self.buffer

    def getBytesBuffer(self):
        return bytes(self.getBuffer())


debug = True

foundDevice = False

def connect_to_hardware():
    msg = msgFrame(findDevice, 0, 0)
    ports = serial.tools.list_ports.comports()

    for port in ports:
        port = ports[-1]
        if debug:
            print(port)
            # print(msg.getBytesBuffer())

        try:
            SerialConnection = serial.Serial(port.device, timeout=1, baudrate=115200)
            time.sleep(2)
        except:
            if debug:
                print("Error")  # time.sleep(0.1)
                continue

        for element in msg.getBuffer():
            SerialConnection.write(int(element).to_bytes(1, "big"))
        time.sleep(0.1)
        
        returnValue = SerialConnection.read(frameLength)

        if returnValue == msg.getBytesBuffer():
            print("OK!")
            print("Expected     : " + str(msg.getBytesBuffer()))
            print("App. Returned: " + str(returnValue))
            print("Connect to port: " + str(port))
            foundDevice = True
            break
        
        else:
            print("ERROR!")
            print("Expected     : " + str(msg.getBytesBuffer()))
            print("App. Returned: " + str(returnValue))
            foundDevice = False

    if not foundDevice:
        exit()

    while True:
        setting = 0
        try:
            setting = int(input("Select: "))
        except:
            print("Only numbers are valid!")

        msg = msgFrame(writeInformation, 0, setting)
        for element in msg.getBuffer():
            SerialConnection.write(int(element).to_bytes(1, "big"))

        returnValue = SerialConnection.read(frameLength)

        if returnValue == msg.getBytesBuffer():
            print("OK!")
            print("Expected     : " + str(msg.getBytesBuffer()))
            print("App. Returned: " + str(returnValue))
        
        else:
            print("ERROR!")
            print("Expected     : " + str(msg.getBytesBuffer()))
            print("App. Returned: " + str(returnValue))

        
connect_to_hardware()
