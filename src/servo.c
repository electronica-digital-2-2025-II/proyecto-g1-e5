#include <stdint.h>

#define GPIO_BASE      0x41200000U
#define OUT_DATA       (*(volatile uint32_t*)(GPIO_BASE + 0x8))

#define SERVO_BIT      4u
#define SERVO_MASK     (1u << SERVO_BIT)

#define SERVO_PERIOD_TICKS      1000u
#define SERVO_PULSE_0_TICKS       20u
#define SERVO_PULSE_180_TICKS    120u
#define SERVO_TICK_DELAY_CYCLES  2000u
#define DELAY_MS_FACTOR          150000u

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

static void servo_run(uint8_t go_180, uint32_t num_periods)
{
    uint32_t period_ticks = SERVO_PERIOD_TICKS;
    uint32_t pulse_ticks  = go_180 ? SERVO_PULSE_180_TICKS : SERVO_PULSE_0_TICKS;

    for (uint32_t p = 0; p < num_periods; p++) {
        for (uint32_t cnt = 0; cnt < period_ticks; cnt++) {

            if (cnt < pulse_ticks) {
                OUT_DATA |= SERVO_MASK;
            } else {
                OUT_DATA &= ~SERVO_MASK;
            }

            delay_cycles(SERVO_TICK_DELAY_CYCLES);
        }
    }
}

void servo_180_ida_vuelta(void)
{
    servo_run(1, 100u);   // ir a 180°
    delay_ms(500u);
    servo_run(0, 100u);   // volver a 0°
}
