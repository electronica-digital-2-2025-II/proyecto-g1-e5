#include <stdint.h>
#include "xparameters.h"

// -------------------------------------------------------------
//  Direcciones GPIO (mismo AXI GPIO del proyecto)
// -------------------------------------------------------------
#define GPIO_BASE       0x41200000U

// Canal 1: ENTRADAS (OUT del TCS3200 en JC3)
#define GPIO_CH1_DATA   (*(volatile uint32_t*)(GPIO_BASE + 0x0))

// Canal 2: SALIDAS (JD + JE)
#define OUT_DATA        (*(volatile uint32_t*)(GPIO_BASE + 0x8))
#define OUT_TRI         (*(volatile uint32_t*)(GPIO_BASE + 0xC))

// -------------------------------------------------------------
//  Bits usados para COLOR
// -------------------------------------------------------------
#define COLOR_BIT       2u                   // OUT en JC3 → GPIO_0_tri_i[2]
#define COLOR_MASK      (1u << COLOR_BIT)

// JE2 y JE3 mapeados a GPIO2_0_tri_o[6] y [7]
#define S2_BIT          6u                   // JE2
#define S3_BIT          7u                   // JE3

#define S2_MASK         (1u << S2_BIT)
#define S3_MASK         (1u << S3_BIT)

// -------------------------------------------------------------
//  Delays locales
// -------------------------------------------------------------
#define DELAY_US_FACTOR 150u

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles--) {
        __asm__("nop");
    }
}

static void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        delay_cycles(DELAY_US_FACTOR);
    }
}

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (ms * 1000u); i++) {
        delay_cycles(DELAY_US_FACTOR);
    }
}

// -------------------------------------------------------------
//  Lectura simple del pin OUT del TCS3200
// -------------------------------------------------------------
static uint8_t color_read_level(void)
{
    return (GPIO_CH1_DATA & COLOR_MASK) ? 1u : 0u;
}

// -------------------------------------------------------------
//  ENUM de filtros
// -------------------------------------------------------------
typedef enum {
    COLOR_FILTER_RED = 0,
    COLOR_FILTER_BLUE,
    COLOR_FILTER_CLEAR,
    COLOR_FILTER_GREEN
} color_filter_t;

// -------------------------------------------------------------
//  Inicialización del TCS3200
// -------------------------------------------------------------
void color_init(void)
{
    // Filtro inicial CLEAR (mide luz total)
    OUT_DATA &= ~S2_MASK;     // S2 = 0
    OUT_DATA |=  S3_MASK;     // S3 = 1
}

// -------------------------------------------------------------
//  Selección de filtro (S2, S3)
// -------------------------------------------------------------
void color_set_filter(color_filter_t f)
{
    switch (f) {
        case COLOR_FILTER_RED:
            // S2=0, S3=0
            OUT_DATA &= ~(S2_MASK | S3_MASK);
            break;

        case COLOR_FILTER_BLUE:
            // S2=1, S3=0
            OUT_DATA |=  S2_MASK;
            OUT_DATA &= ~S3_MASK;
            break;

        case COLOR_FILTER_CLEAR:
            // S2=0, S3=1
            OUT_DATA &= ~S2_MASK;
            OUT_DATA |=  S3_MASK;
            break;

        case COLOR_FILTER_GREEN:
        default:
            // S2=1, S3=1
            OUT_DATA |=  (S2_MASK | S3_MASK);
            break;
    }
}

// -------------------------------------------------------------
//  Medir pulsos del OUT por una ventana de tiempo (ms)
// -------------------------------------------------------------
uint32_t color_measure_pulses(uint32_t ventana_ms)
{
    uint32_t pulses = 0;
    uint32_t total_us = ventana_ms * 1000u;

    uint8_t prev = color_read_level();

    while (total_us--) {
        uint8_t now = color_read_level();
        if (now && !prev) {
            pulses++;
        }
        prev = now;
        delay_us(1u);
    }

    return pulses;
}

// -------------------------------------------------------------
//  Lectura simple (compatibilidad con tu main viejo)
// -------------------------------------------------------------
int sensor_color_leer(void)
{
    return color_read_level() ? 1 : 0;
}

// -------------------------------------------------------------
//  MEDIR C, R, G, B DE UN MISMO OBJETO
// -------------------------------------------------------------
void color_medir_CRGC(uint32_t *C, uint32_t *R, uint32_t *G, uint32_t *B)
{
    // CLEAR (luminosidad total)
    color_set_filter(COLOR_FILTER_CLEAR);
    delay_ms(2);
    *C = color_measure_pulses(10);

    // RED
    color_set_filter(COLOR_FILTER_RED);
    delay_ms(2);
    *R = color_measure_pulses(10);

    // GREEN
    color_set_filter(COLOR_FILTER_GREEN);
    delay_ms(2);
    *G = color_measure_pulses(10);

    // BLUE
    color_set_filter(COLOR_FILTER_BLUE);
    delay_ms(2);
    *B = color_measure_pulses(10);

    // Regresar a CLEAR
    color_set_filter(COLOR_FILTER_CLEAR);
}
