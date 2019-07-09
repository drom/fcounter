// frequency counter

//
#define GPIO_OEN     (*(volatile int *) 0x10012008)
#define GPIO_VAL     (*(volatile int *) 0x1001200C)
#define GPIO_IOF_EN  (*(volatile int *) 0x10012038)
#define GPIO_IOF_SEL (*(volatile int *) 0x1001203C)

#define SPI1_DIV (*(volatile int *) 0x10034000)
#define SPI1_FMT (*(volatile int *) 0x10034040)
#define SPI1_TXD (*(volatile int *) 0x10034048)
#define SPI1_RXD (*(volatile int *) 0x1003404C)
#define SPI1_TXC (*(volatile int *) 0x10034050)
#define SPI1_IP  (*(volatile int *) 0x10034074)


// HiFive1 Rev.B
#define RED   0x00400000 /* 22 LED Red */
#define GREEN 0x00080000 /* 19 LED Green */
#define BLUE  0x00200000 /* 21 LED Blue */
#define BLK   0x00000001 /* 0  IPS Backlight Active High */
#define DC    0x00000002 /* 1  IPS Data/Command */
#define RST   0x00000004 /* 2  IPS Reset Active Low */
#define SDA1  0x00000008 /* 3  MOSI */
#define SCL1  0x00000020 /* 5  SCK */
#define ALL   (RED | GREEN | BLUE | BLK | DC | RST | SDA1 | SCL1)

// ST7789
#define ST77_SWRESET 0x01
#define ST77_SLPIN   0x10
#define ST77_SLPOUT  0x11
#define ST77_DISPOFF 0x28
#define ST77_DISPON  0x29
#define ST77_CASET   0x2A
#define ST77_RASET   0x2B
#define ST77_RAMWR   0x2C
#define ST77_COLMOD  0x3A
#define ST77_RGB565  0x55

void sleep(int t) {
  while(t--) {
    asm("nop");
  }
}

void writeSPI1 (int val) {
  while (SPI1_TXD); // wait for non-full
  SPI1_TXD = val;
  while(!(SPI1_IP & 1));
  // sleep(100);
}

int main() {
  GPIO_OEN = ALL;
  SPI1_TXC = 1;
  // HW reset
  GPIO_VAL = ALL ^ RST ^ RED ^ GREEN;
  sleep(2000000);

  // SPI1_DIV = 100; // F=?
  // SPI1_FMT = /* Single, MSB */ 0x00080000 /* len=8 */ | 0x08/* dir=TX */ ;
  GPIO_VAL = ALL;
  GPIO_IOF_SEL = 0x00000000; // IOF0
  GPIO_IOF_EN  = SCL1 | SDA1;
  sleep(2000000);

  // SW reset
  GPIO_VAL = ALL ^ DC; writeSPI1(ST77_SWRESET); GPIO_VAL = ALL;
  sleep(2000000);

  // Sleep Out
  GPIO_VAL = ALL ^ DC; writeSPI1(ST77_SLPOUT); GPIO_VAL = ALL;
  sleep(2000000);

  // Display On
  GPIO_VAL = ALL ^ DC; writeSPI1(ST77_DISPON); GPIO_VAL = ALL;
  writeSPI1(ST77_RGB565);

  // // column set
  // GPIO_VAL = ALL ^ DC; writeSPI1(ST77_CASET); GPIO_VAL = ALL;
  // writeSPI1(31 >> 8); writeSPI1(31 & 0xFF); // x0
  // writeSPI1(63 >> 8); writeSPI1(63 & 0xFF); // x1
  //
  // // row set
  // GPIO_VAL = ALL ^ DC; writeSPI1(ST77_RASET); GPIO_VAL = ALL;
  // writeSPI1(31 >> 8); writeSPI1(31 & 0xFF); // y0
  // writeSPI1(63 >> 8); writeSPI1(63 & 0xFF); // y1

  // ram write
  // for (int i = 0; i < 100; i++) {
  //     GPIO_VAL = ALL ^ DC; writeSPI1(ST77_RAMWR); GPIO_VAL = ALL;
  //     writeSPI1(0b11111111); writeSPI1(0b11111111);
  //     //          R----G--               ---B----
  // }

  while (1) {
    sleep(2000000); GPIO_VAL = ALL ^ RED;
    sleep(2000000); GPIO_VAL = ALL ^ GREEN;
    sleep(2000000); GPIO_VAL = ALL ^ BLUE;
  }
  return 0;
}
