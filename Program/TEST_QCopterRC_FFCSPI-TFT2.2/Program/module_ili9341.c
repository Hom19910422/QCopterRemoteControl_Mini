/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_spi.h"
#include "module_ili9341.h"
#include "module_fontlib.h"
#include "algorithm_string.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define LCD_CST   PAO(4)
#define LCD_DC    PBO(1)
#define LCD_RST   PCO(3)
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : ILI9341_Config
**功能 : 配置IO
**輸入 : None
**輸出 : None
**使用 : ILI9341_Config();
**=====================================================================================================*/
/*=====================================================================================================*/
void ILI9341_Config( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;

  /* Clk Init *************************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  /* CST PA4  */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* DC PB1  */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* RST PC3 */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  /* SCK PA5  */	/* MISO PA6  */	/* MOSI PA7  */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  LCD_CST = 1;

  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   // 雙線全雙工
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                        // 主模式
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                    // 數據大小8位
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                          // 時鐘極性，空閒時為低
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                         // 第1個邊沿有效，上升沿為采樣時刻
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                            // NSS信號由軟件產生
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;   // 8分頻，9MHz
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                   // 高位在前
  SPI_InitStruct.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStruct);

  SPI_Cmd(SPI1, ENABLE);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : ILI9341_LigConfig
**功能 : 配置調光 IO
**輸入 : None
**輸出 : None
**使用 : ILI9341_LigConfig();
**=====================================================================================================*/
/*=====================================================================================================*/
void ILI9341_LigConfig( void )
{
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_WriteCmd
**功能 : Write Command or Address
**輸入 : WriteCmd
**輸出 : None
**使用 : LCD_WriteCmd(0xCB);
**=====================================================================================================*/
/*=====================================================================================================*/
static void LCD_WriteCmd( u8 WriteCmd )
{
  LCD_CST = 0;
  LCD_DC = 0;
  SPI_RW(SPI1, WriteCmd);
  LCD_CST = 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_WriteData
**功能 : Write Data
**輸入 : WriteData
**輸出 : None
**使用 : LCD_WriteData(Byte8H(Color));
**=====================================================================================================*/
/*=====================================================================================================*/
static void LCD_WriteData( u16 WriteData )
{
  LCD_CST = 0;
  LCD_DC = 1;
  SPI_RW(SPI1, WriteData);
  LCD_CST = 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_WriteColor
**功能 : Write Color
**輸入 : Color
**輸出 : None
**使用 : LCD_WriteColor(BLACK);
**=====================================================================================================*/
/*=====================================================================================================*/
static void LCD_WriteColor( u16 Color )
{
  LCD_CST = 0;
  LCD_DC = 1;
  SPI_RW(SPI1, Byte8H(Color));
  SPI_RW(SPI1, Byte8L(Color));
  LCD_CST = 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : ILI9341_Init
**功能 : 初始化 ILI9341
**輸入 : None
**輸出 : None
**使用 : ILI9341_Init();
**=====================================================================================================*/
/*=====================================================================================================*/
void ILI9341_Init( void )
{
  // Reset
	LCD_RST = 0;
	Delay_1ms(100);
	LCD_RST = 1;
	Delay_1ms(50);

  LCD_WriteCmd(0xCB);
  LCD_WriteData(0x39);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x00);
  LCD_WriteData(0x34);
  LCD_WriteData(0x02);

  LCD_WriteCmd(0xCF);
  LCD_WriteData(0x00);
  LCD_WriteData(0XC1);
  LCD_WriteData(0X30);

  LCD_WriteCmd(0xE8);
  LCD_WriteData(0x85);
  LCD_WriteData(0x00);
  LCD_WriteData(0x78);

  LCD_WriteCmd(0xEA);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);

  LCD_WriteCmd(0xED);
  LCD_WriteData(0x64);
  LCD_WriteData(0x03);
  LCD_WriteData(0X12);
  LCD_WriteData(0X81);

  LCD_WriteCmd(0xF7);
  LCD_WriteData(0x20);

  LCD_WriteCmd(0xC0);   // Power control
  LCD_WriteData(0x23);  // VRH[5:0]

  LCD_WriteCmd(0xC1);   // Power control
  LCD_WriteData(0x10);  // SAP[2:0];BT[3:0]

  LCD_WriteCmd(0xC5);   // VCM control
  LCD_WriteData(0x3e);  // 對比度調節
  LCD_WriteData(0x28); 

  LCD_WriteCmd(0xC7);   // VCM control2
  LCD_WriteData(0x86);  // --

  LCD_WriteCmd(0x36);   // Memory Access Control
#ifdef H_VIEW
  LCD_WriteData(0xE8);  //C8  // 48 68豎屏 // 28 E8 橫屏
#else
  LCD_WriteData(0x48);
#endif

  LCD_WriteCmd(0x3A);
  LCD_WriteData(0x55);

  LCD_WriteCmd(0xB1);
  LCD_WriteData(0x00);
  LCD_WriteData(0x18);

  LCD_WriteCmd(0xB6);   // Display Function Control
  LCD_WriteData(0x08);
  LCD_WriteData(0x82);
  LCD_WriteData(0x27);

  LCD_WriteCmd(0xF2);   // 3Gamma Function Disable
  LCD_WriteData(0x00);

  LCD_WriteCmd(0x26);   // Gamma curve selected
  LCD_WriteData(0x01);

  LCD_WriteCmd(0xE0);   // Set Gamma
  LCD_WriteData(0x0F);
  LCD_WriteData(0x31);
  LCD_WriteData(0x2B);
  LCD_WriteData(0x0C);
  LCD_WriteData(0x0E);
  LCD_WriteData(0x08);
  LCD_WriteData(0x4E);
  LCD_WriteData(0xF1);
  LCD_WriteData(0x37);
  LCD_WriteData(0x07);
  LCD_WriteData(0x10);
  LCD_WriteData(0x03);
  LCD_WriteData(0x0E);
  LCD_WriteData(0x09);
  LCD_WriteData(0x00);

  LCD_WriteCmd(0XE1);   // Set Gamma
  LCD_WriteData(0x00);
  LCD_WriteData(0x0E);
  LCD_WriteData(0x14);
  LCD_WriteData(0x03);
  LCD_WriteData(0x11);
  LCD_WriteData(0x07);
  LCD_WriteData(0x31);
  LCD_WriteData(0xC1);
  LCD_WriteData(0x48);
  LCD_WriteData(0x08);
  LCD_WriteData(0x0F);
  LCD_WriteData(0x0C);
  LCD_WriteData(0x31);
  LCD_WriteData(0x36);
  LCD_WriteData(0x0F);

  LCD_WriteCmd(0x11);   // Exit Sleep
  Delay_1ms(120);

  LCD_WriteCmd(0x29);   // Display on
  LCD_WriteCmd(0x2C);

  LCD_Clear(BLACK);

//  LCD_SetBackLight(BLIGHT_DEFAULT);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_Clear
**功能 : Clear Windows
**輸入 : Color
**輸出 : None
**使用 : LCD_Clear(BLACK);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_Clear( u16 Color )
{
  u32 Point = LCD_W*LCD_H;

  LCD_SetWindow(0, 0, LCD_W-1, LCD_H-1);

	while(Point--)
		LCD_WriteColor(Color);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_SetCursor
**功能 : Set Cursor
**輸入 : CoordiX, CoordiY
**輸出 : None
**使用 : LCD_SetCursor(StartX, StartY);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_SetCursor( u16 CoordiX, u16 CoordiY )
{
	LCD_WriteCmd(0x2A);
  LCD_WriteData(Byte8H(CoordiX));
	LCD_WriteData(Byte8L(CoordiX));
	LCD_WriteCmd(0x2B);
  LCD_WriteData(Byte8H(CoordiY));
	LCD_WriteData(Byte8L(CoordiY));
	LCD_WriteCmd(0x2C);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_SetWindow
**功能 : Set Window
**輸入 : StartX, StartY, EndX, EndY
**輸出 : None
**使用 : LCD_SetWindow(X0, Y0, X1, Y1);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_SetWindow( u16 StartX, u16 StartY, u16 EndX, u16 EndY )
{
	LCD_WriteCmd(0x2A);
  LCD_WriteData(Byte8H(StartX));
	LCD_WriteData(Byte8L(StartX));
  LCD_WriteData(Byte8H(EndX));
	LCD_WriteData(Byte8L(EndX));
	LCD_WriteCmd(0x2B);
  LCD_WriteData(Byte8H(StartY));
	LCD_WriteData(Byte8L(StartY));
  LCD_WriteData(Byte8H(EndY));
	LCD_WriteData(Byte8L(EndY));
	LCD_WriteCmd(0x2C);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_SetBackLight
**功能 : Set LCD BackLight
**輸入 : BackLight
**輸出 : None
**使用 : LCD_SetBackLight(BLIGHT_DEFAULT);
**=====================================================================================================*/
/*=====================================================================================================*/
//void LCD_SetBackLight( u16 BackLight )
//{
//	if(BackLight <= 0)
//    LCD_LIG = BLIGHT_MIN;
//	else if (BackLight >= BLIGHT_MAX)
//    LCD_LIG = BLIGHT_MAX;
//  else
//    LCD_LIG = BackLight;
//}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawPixel
**功能 : Draw a Pixel
**輸入 : CoordiX, CoordiY, Color
**輸出 : None
**使用 : LCD_DrawPixel(CoordiX, CoordiY, Color);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawPixel( u16 CoordiX, u16 CoordiY, u16 Color )
{
  LCD_SetCursor(CoordiX, CoordiY);
  LCD_WriteColor(Color);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawLineX
**功能 : Draw X-Axis Line
**輸入 : CoordiX, CoordiY, Length, Color
**輸出 : None
**使用 : LCD_DrawLine(CoordiX, CoordiY, Length, Color)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawLineX( u16 CoordiX, u16 CoordiY, u16 Length, u16 Color )
{
	u16 i;

	for(i=0; i<Length; i++)
		LCD_DrawPixel(CoordiX+i, CoordiY, Color);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawLineY
**功能 : Draw Y-Axis Line
**輸入 : CoordiX, CoordiY, Length, Color
**輸出 : None
**使用 : LCD_DrawLine(CoordiX, CoordiY, Length, Color)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawLineY( u16 CoordiX, u16 CoordiY, u16 Length, u16 Color )
{
	u16 i;

	for(i=0; i<Length; i++)
		LCD_DrawPixel(CoordiX, CoordiY+i, Color);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawRectangle
**功能 : Draw Rectangle
**輸入 : CoordiX, CoordiY, Length, Width, Color
**輸出 : None
**使用 : LCD_DrawRectangle(CoordiX, CoordiY, Width, Height, Color)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawRectangle( u16 CoordiX, u16 CoordiY, u16 Width, u16 Height, u16 Color )
{
	u16 i;

	for(i=0; i<=Height; i++) {
		LCD_DrawPixel(CoordiX+i, CoordiY, Color);
		LCD_DrawPixel(CoordiX+i, CoordiY+Width, Color);
	}
	for(i=1; i<Width; i++) {
		LCD_DrawPixel(CoordiX, CoordiY+i, Color);
		LCD_DrawPixel(CoordiX+Height, CoordiY+i, Color);
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawRectangleFill
**功能 : Draw Rectangle
**輸入 : CoordiX, CoordiY, Length, Width, Color
**輸出 : None
**使用 : LCD_DrawRectangleFill(CoordiX, CoordiY, Width, Height, Color)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawRectangleFill( u16 CoordiX, u16 CoordiY, u16 Width, u16 Height, u16 Color )
{
	u16 i, j;

	for(i=0; i<Width; i++)
		for(j=0; j<Height; j++)
			LCD_DrawPixel(CoordiX+j, CoordiY+i, Color);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawCircle
**功能 : Draw Circle
**輸入 : CoordiX, CoordiY, Radius, Color
**輸出 : None
**使用 : LCD_DrawCircle(CoordiX, CoordiY, Radius, Color)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawCircle( u16 CoordiX, u16 CoordiY, u16 Radius, u16 Color )
{
	int a,b;
	int di;

	a=0;
	b=Radius;
	di=3-(Radius<<1);

	while(a<=b) {
		LCD_DrawPixel(CoordiX-b, CoordiY-a, Color); //3
		LCD_DrawPixel(CoordiX+b, CoordiY-a, Color); //0
		LCD_DrawPixel(CoordiX-a, CoordiY+b, Color); //1
		LCD_DrawPixel(CoordiX-b, CoordiY-a, Color); //7
		LCD_DrawPixel(CoordiX-a, CoordiY-b, Color); //2
		LCD_DrawPixel(CoordiX+b, CoordiY+a, Color); //4
		LCD_DrawPixel(CoordiX+a, CoordiY-b, Color); //5
		LCD_DrawPixel(CoordiX+a, CoordiY+b, Color); //6
		LCD_DrawPixel(CoordiX-b, CoordiY+a, Color);
		a++;
		// 使用Bresenham算法畫圓
		if(di<0)
			di += 4*a+6;
		else {
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPixel(CoordiX+a, CoordiY+b, Color);
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_DrawPicture
**功能 : Draw Picture
**輸入 : CoordiX, CoordiY, Length, Width, Pic
**輸出 : None
**使用 : LCD_DrawPicture(CoordiX, CoordiY, Width, Height, Pic);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_DrawPicture( u16 CoordiX, u16 CoordiY, u16 Width, u16 Height, uc8 *Pic )
{
  u16* readPixel = (u16*)Pic;
  u32 i = 0, j = Height*Width;

  LCD_SetWindow(CoordiX, CoordiY, CoordiX+Width-1, CoordiY+Height-1);

  for(i=0; i<j; i++)
    LCD_WriteColor(readPixel[i]);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_PutChar1206
**功能 : Put Char
**輸入 : CoordiX, CoordiY, ChWord, FontColor, BackColor
**輸出 : None
**使用 : LCD_PutChar1206(x, y, "C", WHITE, BLACK)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_PutChar1206( u16 CoordiX, u16 CoordiY, u8* ChWord, u16 FontColor, u16 BackColor )
{
	u8 Tmp_Char = 0;
	u16 i = 0, j = 0;

	for(i=0; i<12; i++) {
		Tmp_Char = ASCII_12x6[*ChWord-32][i];
		for(j=0; j<6; j++) {
			if(((Tmp_Char>>j)&0x01) == 0x01)
				LCD_DrawPixel(CoordiX+j, CoordiY+i, FontColor);	// 字符顏色
			else
				LCD_DrawPixel(CoordiX+j, CoordiY+i, BackColor);	// 背景顏色
		}
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_PutChar1608
**功能 : Put Char
**輸入 : CoordiX, CoordiY, ChWord, FontColor, BackColor
**輸出 : None
**使用 : LCD_PutChar1608(x, y, "C", WHITE, BLACK)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_PutChar1608( u16 CoordiX, u16 CoordiY, u8* ChWord, u16 FontColor, u16 BackColor )
{
	u8 Tmp_Char = 0;
	u16 i = 0, j = 0;

	for(i=0; i<16; i++) {
		Tmp_Char = ASCII_16x8[*ChWord-32][i];
		for(j=0; j<8; j++) {
			if(((Tmp_Char>>(7-j))&0x01) == 0x01)
				LCD_DrawPixel(CoordiX+j, CoordiY+i, FontColor);	// 字符顏色
			else
				LCD_DrawPixel(CoordiX+j, CoordiY+i, BackColor);	// 背景顏色
		}
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_PutChar1608_
**功能 : Put Char
**輸入 : CoordiX, CoordiY, ChWord, FontColor, BackColor
**輸出 : None
**使用 : LCD_PutChar1608(x, y, "C", WHITE, BLACK)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_PutChar1608_( u16 CoordiX, u16 CoordiY, u8* ChWord, u16 FontColor, u16 BackColor )
{
	u8 Tmp_Char = 0;
	u16 i = 0, j = 0;

	for(i=0; i<16; i++) {
		Tmp_Char = ASCII_16x8_[*ChWord-32][i];
		for(j=0; j<8; j++) {
			if(((Tmp_Char>>j)&0x01) == 0x01)
				LCD_DrawPixel(CoordiX+j, CoordiY+i, FontColor);	// 字符顏色
			else
				LCD_DrawPixel(CoordiX+j, CoordiY+i, BackColor);	// 背景顏色
		}
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_PutStr
**功能 : Put String
**輸入 : CoordiX, CoordiY, ChWord, FontStyle, FontColor, BackColor
**輸出 : None
**使用 : LCD_PutStr(10, 10, (u8*)"TFT LCD TEST ... ", ASCII1608, WHITE, BLACK);
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_PutStr( u16 CoordiX, u16 CoordiY, u8 *ChWord, u8 FontStyle, u16 FontColor, u16 BackColor )
{
	u16 OffsetX = 0;

	switch(FontStyle) {
		case ASCII1206:
			while(*ChWord) {
				LCD_PutChar1206(CoordiX+OffsetX, CoordiY, ChWord, FontColor, BackColor);
				ChWord++;
				OffsetX += 6;
			}
			break;
		case ASCII1608:
			while(*ChWord) {
				LCD_PutChar1608(CoordiX+OffsetX, CoordiY, ChWord, FontColor, BackColor);
				ChWord++;
				OffsetX += 8;
			}
			break;
		case ASCII1608_:
			while(*ChWord) {
				LCD_PutChar1608_(CoordiX+OffsetX, CoordiY, ChWord, FontColor, BackColor);
				ChWord++;
				OffsetX += 8;
			}
			break;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_PutNum
**功能 : Put Number
**輸入 : oordiX, CoordiY, Type, Length, NumData, FontColor, BackColor
**輸出 : None
**使用 : LCD_PutNum(CoordiX, CoordiY, Type_D, Length, NumData, WHITE, BLACK)
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_PutNum( u16 CoordiX, u16 CoordiY, u8 Type, u8 Length, u32 NumData, u16 FontColor, u16 BackColor )
{
	u8 TmpNumber[16] = {0};

	Str_NumToChar(Type, Length, TmpNumber, NumData);
	LCD_PutStr(CoordiX, CoordiY, TmpNumber, ASCII1608, FontColor, BackColor);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : LCD_TestColoBar
**功能 : Draw Color Bar
**輸入 : None
**輸出 : None
**使用 : LCD_TestColoBar();
**=====================================================================================================*/
/*=====================================================================================================*/
void LCD_TestColoBar( void )
{
  u32 i = 0, j = 0;
  u16 drawColor[20] = {
    WHITE,
    RED,
    GREEN,
    BLUE,
    MAGENTA,
    GRED,
    GBLUE,
    BRED,
    BROWN,
    BRRED,
    CYAN,
    GRAY,
    YELLOW,
    DARKBLUE,
    LIGHTBLUE,
    GRAYBLUE,
    LIGHTGREEN,
    LGRAY,
    LGRAYBLUE,
    LBBLUE
  };

	LCD_SetWindow(0, 0, LCD_W-1, LCD_H-1);

  for(i=0; i<20; i++) {
    j = 16*LCD_W;
    while(j--)
      LCD_WriteColor(drawColor[i]);
  }
}
/*=====================================================================================================*/
/*=====================================================================================================*/
