
/*
  日期:2018.5.3
  IDE 版本:1.8.6
  功能：采集压力传感器数据，通过zigbee模块发送，PC端通过zigbee接收转串口采集模拟数据。
  Author:He
*/

int fsrPin = 0;
int ledPin = 13;
short fsrReading;
char data;
int dataSize = 9;
char array[2];

/*
    pressData[]: 按照DL-LN3X协议格式组包

   包头 2byte 包头由FE和包的长度组成，长度为包的数据长度加4。
   源端口号 1byte 包的源端口号。
   目的端口号 1byte 包的目的端口号。
   远程地址 2byte 远程模块的地址，当发送数据给模块时，远程地址为目标模块的地址，目标地址为0x0000表示此包发给本模块，目标地址为0xFFFF的包会被发往本网络中的所有节点。当从模块接收数据时，远程地址是数据的源地址。
   数据 可变 想要传输的数据。
   包尾 1byte 固定为FF表示一个包传输完成。
*/
unsigned char pressData[10] = {0xfe, 0x06, 0x91, 0x90, 0x00, 0x08, 0x00, 0x00, 0xff, 0xff,};

void setup() {
  Serial.begin(9600); //USB
  Serial1.begin(115200); //RX TX serial port
  pinMode(ledPin, OUTPUT);
}

void loop() {
  fsrReading = analogRead(fsrPin);

  //fsrReading最大值是1023（0x3FF）
  if (fsrReading > 10) {
    //fsrReading = 0xfe;
    Serial.print("analogReading = ");
    Serial.print("0x");
    Serial.print(fsrReading, HEX);
    Serial.print(" = ");
    Serial.print(fsrReading);

    //fsrReading最大值0x3ff,所以高位不需要转义
    data = fsrReading >> 8;
    pressData[6] = data;

    data = (char)(fsrReading & 0xff);
    pressData[7] = data;

    pressData[8] = 0xff;
    if (pressData[7] == 0xfe) {
      pressData[7] = 0xfe;
      pressData[8] = 0xfc;
      dataSize = 10;
    }
    else if (pressData[7] == 0xff) {
      pressData[7] = 0xfe;
      pressData[8] = 0xfd;
      dataSize = 10;
    }
    else {}

    Serial1.write(pressData, dataSize);

    if (!digitalRead(ledPin))
    {
      digitalWrite(ledPin, HIGH);
    }
  } else {
    if (digitalRead(ledPin))
    {
      digitalWrite(ledPin, LOW);
    }
  }
  if (fsrReading < 10) {
    // Serial.println(" - No pressure");
  } else if (fsrReading < 200) {
    Serial.println(" - Light touch");
  } else if (fsrReading < 500) {
    Serial.println(" - Light squeeze");
  } else if (fsrReading < 800) {
    Serial.println(" - Medium squeeze");
  } else {
    Serial.println(" - Big squeeze");
  }
  //delay(100);
}

/*
   Function:按协议转换特殊字符，返回转换后字符个数
   Date: 2018-5-15
   Author:He
*/
void transConvert(char data, char arr[2]) {
  arr[0] = 0xFE;
  if (data == 0xFF) {
    arr[1] = 0xFD;
  }
  if (data == 0xFE) {
    arr[1] = 0xFC;
  }
}

/*
   Function:判断是否需要字符转义
   Date: 2018-5-15
   Author:He
*/
int needConvert(char data) {
  if (data == 0xFF) {
    return 1;
  }
  if (data == 0xFE) {
    return 2;
  }
  return 0;
}


