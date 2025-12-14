#include <stdint.h>
#include "xparameters.h"
#include "xil_printf.h"

// -----------------------------------------------------------
//  Direcciones GPIO (AXI GPIO en 0x41200000)
// -----------------------------------------------------------
#define GPIO_BASE      0x41200000U

// Canal 1: entradas (inductivo, humedad digital, color OUT, infrarrojo si lo usas)
#define GPIO_CH1_TRI   (*(volatile uint32_t*)(GPIO_BASE + 0x4))

// Canal 2: salidas (motor, servo, LCD, S2, S3, etc.)
#define OUT_TRI        (*(volatile uint32_t*)(GPIO_BASE + 0x0C))

// Bits de canal 1 (entradas)
#define INDUCTIVE_BIT  0u   // GPIO_0_tri_i[0] -> inductivo (JC1 V15)
#define HUM_BIT        1u   // GPIO_0_tri_i[1] -> humedad  (JC2 W15)
#define COLOR_BIT      2u   // GPIO_0_tri_i[2] -> OUT TCS3200 (JC3 T11)
// #define IR_BIT      3u   // GPIO_0_tri_i[3] -> infrarrojo (JC? U12) si lo vuelves a usar

// -----------------------------------------------------------
//  Delays locales
// -----------------------------------------------------------
#define DELAY_MS_FACTOR  150000u

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles--) {
        __asm__("nop");
    }
}

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delay_cycles(DELAY_MS_FACTOR);
    }
}

// -----------------------------------------------------------
//  PROTOTIPOS (otros .c)
// -----------------------------------------------------------

// Sensores
int  sensor_inductivo_leer(void);    // inductivo.c
int  sensor_humedad_digital(void);   // humedad.c

// COLOR (TCS3200) - color.c
void     color_init(void);
void     color_medir_CRGC(uint32_t *C, uint32_t *R, uint32_t *G, uint32_t *B);

// Actuadores
void motor_pasoapaso_derecha(uint32_t pasos);   // paso_a_paso.c
void motor_pasoapaso_izquierda(uint32_t pasos); // paso_a_paso.c
void servo_180_ida_vuelta(void);                // servo.c

// LCD (lcd.c autocontenido)
int  lcd_init(void);
void lcd_clear(void);
void lcd_print_line(uint8_t line, const char *s);

// Paso para 120°
#define STEPS_PER_REV      2048u
#define DEG_PER_MOVE       120u
#define STEPS_FOR_120_DEG  ((STEPS_PER_REV * DEG_PER_MOVE + 180u) / 360u)

// -----------------------------------------------------------
//  Clasificador: BLANCO (bolsa plástica)
//  (Tus rangos típicos: C≈20–30, R≈16–23, G≈18–25, B≈57–80)
// -----------------------------------------------------------
static int es_color_plastico(uint32_t C,
                             uint32_t R,
                             uint32_t G,
                             uint32_t B)
{
    // RANGOS NUEVOS (según tu captura)
    // C: 7-8, R: 6-8, G: 6-7, B: ~21
    if (C < 6u || C > 10u) return 0;
    if (R < 5u || R > 10u) return 0;
    if (G < 5u || G > 9u)  return 0;
    if (B < 18u || B > 26u) return 0;

    // Extra: asegurar que B sea claramente mayor que R y G
    // (evita falsos positivos si todo sube/baja parecido)
    if (B <= R + 8u) return 0;
    if (B <= G + 8u) return 0;

    return 1;
}


// -----------------------------------------------------------
//  MAIN
// -----------------------------------------------------------
int main(void)
{
    xil_printf("Sistema iniciado...\r\n");

    // -------------------------------------------------------
    // Configuración GPIO:
    // Canal 1 bits 0,1,2 como entrada
    // Canal 2 todo salida
    // -------------------------------------------------------
    GPIO_CH1_TRI |= ( (1u << INDUCTIVE_BIT) |
                      (1u << HUM_BIT)       |
                      (1u << COLOR_BIT) );

    OUT_TRI = 0x00000000u;   // todo salida

    // Inicializar Color
    color_init();

    // Inicializar LCD
    if (lcd_init() == 0) {
        lcd_clear();
        lcd_print_line(1, "TRASHBOT :D");
        lcd_print_line(2, "INICIADO <3");
    } else {
        xil_printf("OJO: lcd_init() fallo\r\n");
    }

    delay_ms(800);

    // Estados previos para flancos
    uint8_t prev_ind   = (uint8_t)sensor_inductivo_leer();
    uint8_t prev_hum   = (uint8_t)sensor_humedad_digital();
    uint8_t prev_plast = 0u;

    uint32_t dbg = 0;

    while (1) {

        // 1) Lecturas
        int ind_now = sensor_inductivo_leer();
        int hum_now = sensor_humedad_digital();

        uint32_t C=0, R=0, G=0, B=0;
        color_medir_CRGC(&C, &R, &G, &B);

        int plast_now = es_color_plastico(C, R, G, B);

        // 2) Flancos de subida (respuesta inmediata al detectar)
        uint8_t rising_ind   = (ind_now   && !prev_ind);
        uint8_t rising_hum   = (hum_now   && !prev_hum);
        uint8_t rising_plast = (plast_now && !prev_plast);

        // 3) Acciones + LCD
        if (rising_ind) {
            xil_printf("EVENTO: METAL\r\n");

            lcd_clear();
            lcd_print_line(1, "RESIDUO:");
            lcd_print_line(2, "METAL");

            // Ejemplo: mover a METAL y depositar
            motor_pasoapaso_derecha(STEPS_FOR_120_DEG);
            delay_ms(500);
            servo_180_ida_vuelta();
            motor_pasoapaso_izquierda(STEPS_FOR_120_DEG);
        }
        else if (rising_hum) {
            xil_printf("EVENTO: ORGANICO\r\n");

            lcd_clear();
            lcd_print_line(1, "RESIDUO:");
            lcd_print_line(2, "ORGANICO °~°");

            motor_pasoapaso_izquierda(STEPS_FOR_120_DEG);
            delay_ms(500);
            servo_180_ida_vuelta();
            motor_pasoapaso_derecha(STEPS_FOR_120_DEG);
        }
        else if (rising_plast) {
            xil_printf("EVENTO: PLASTICO(BLANCO)\r\n");
            xil_printf("RGB -> C=%u R=%u G=%u B=%u\r\n",
                       (unsigned)C,(unsigned)R,(unsigned)G,(unsigned)B);

            lcd_clear();
            lcd_print_line(1, "RESIDUO:");
            lcd_print_line(2, "RECICLABLE");

            // Para tu prueba: "apenas detecta blanco mueva el servo"
            servo_180_ida_vuelta();

            // Si también quieres mover motor en plástico, descomenta:
            // motor_pasoapaso_derecha(STEPS_FOR_120_DEG);
            // delay_ms(300);
            // motor_pasoapaso_izquierda(STEPS_FOR_120_DEG);
        }

        // 4) Update previos
        prev_ind   = (uint8_t)ind_now;
        prev_hum   = (uint8_t)hum_now;
        prev_plast = (uint8_t)plast_now;

        // 5) Debug periódico de color (para ver valores siempre)
        dbg++;
        if (dbg >= 5) {
            dbg = 0;
            xil_printf("DBG COLOR -> C=%u R=%u G=%u B=%u plast=%d\r\n",
                       (unsigned)C,(unsigned)R,(unsigned)G,(unsigned)B, plast_now);
        }

        delay_ms(200);
    }

    return 0;
}
