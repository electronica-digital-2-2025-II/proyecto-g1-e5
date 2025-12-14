#include <stdint.h>
#include "xparameters.h"
#include "xiic.h"
#include "xil_printf.h"

// ============================================================
//  "lcd.h" EMBEBIDO (prototipos)
// ============================================================
int  lcd_init(void);
void lcd_clear(void);
void lcd_print_line(uint8_t line, const char *s);

// ============================================================
//  Ajustes
// ============================================================
#define IIC_DEVICE_ID  0          // Solo tienes 1 AXI IIC -> ID 0
#define LCD_ADDR_7B    0x27       // Si no muestra nada, prueba 0x3F

// PCF8574 bits típicos:
// P0=RS, P1=RW, P2=EN, P3=BL, P4..P7=D4..D7
#define LCD_RS   (1u<<0)
#define LCD_RW   (1u<<1)
#define LCD_EN   (1u<<2)
#define LCD_BL   (1u<<3)

static XIic Iic;

// ============================================================
//  Delays simples
// ============================================================
static void delay_cycles(volatile uint32_t c){ while(c--) __asm__("nop"); }
static void delay_us(uint32_t us){ while(us--) delay_cycles(150); }
static void delay_ms(uint32_t ms){
    while(ms--){
        for(int i=0;i<1000;i++) delay_us(1);
    }
}

// ============================================================
//  Envío al PCF8574
// ============================================================
static int pcf_write(uint8_t data)
{
    int sent = XIic_Send(Iic.BaseAddress, LCD_ADDR_7B, &data, 1, XIIC_STOP);
    return (sent == 1) ? 0 : -1;
}

static void lcd_pulse(uint8_t data)
{
    pcf_write(data | LCD_EN);
    delay_us(2);
    pcf_write((uint8_t)(data & (uint8_t)~LCD_EN));
    delay_us(50);
}

static void lcd_write4(uint8_t nibble, uint8_t rs)
{
    uint8_t data = 0;
    data |= LCD_BL;                 // backlight ON
    if (rs) data |= LCD_RS;

    // nibble en P4..P7
    data |= (uint8_t)((nibble & 0x0Fu) << 4);

    pcf_write(data);
    lcd_pulse(data);
}

static void lcd_send(uint8_t byte, uint8_t rs)
{
    lcd_write4((uint8_t)(byte >> 4), rs);
    lcd_write4((uint8_t)(byte & 0x0F), rs);
}

static void lcd_cmd(uint8_t cmd)
{
    lcd_send(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) delay_ms(2);
}

static void lcd_data(uint8_t d)
{
    lcd_send(d, 1);
}

// ============================================================
//  API pública
// ============================================================
int lcd_init(void)
{
    XIic_Config *cfg = XIic_LookupConfig(IIC_DEVICE_ID);
    if (!cfg) {
        xil_printf("ERROR: XIic_LookupConfig(%d) fallo\r\n", IIC_DEVICE_ID);
        return -1;
    }

    int st = XIic_CfgInitialize(&Iic, cfg, cfg->BaseAddress);
    if (st != XST_SUCCESS) {
        xil_printf("ERROR: XIic_CfgInitialize fallo\r\n");
        return -1;
    }

    XIic_Start(&Iic);
    delay_ms(50);

    // Init LCD 4-bit (secuencia estándar)
    lcd_write4(0x03, 0); delay_ms(5);
    lcd_write4(0x03, 0); delay_us(150);
    lcd_write4(0x03, 0); delay_us(150);
    lcd_write4(0x02, 0); delay_us(150); // 4-bit mode

    lcd_cmd(0x28); // 4-bit, 2 líneas, 5x8 (16x2 OK)
    lcd_cmd(0x0C); // display ON, cursor OFF
    lcd_cmd(0x06); // entry mode
    lcd_cmd(0x01); // clear

    return 0;
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
}

static void lcd_set_cursor(uint8_t row, uint8_t col)
{
    // 16x2: fila 0 base 0x00, fila 1 base 0x40
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    addr += col;
    lcd_cmd((uint8_t)(0x80 | addr));
}

void lcd_print_line(uint8_t line, const char *s)
{
    uint8_t row = (line <= 1) ? 0 : 1;
    lcd_set_cursor(row, 0);

    // imprime 16 chars y rellena con espacios
    for (int i = 0; i < 16; i++) {
        char c = s[i];
        if (c == '\0') {
            for (; i < 16; i++) lcd_data(' ');
            break;
        }
        lcd_data((uint8_t)c);
    }
}
