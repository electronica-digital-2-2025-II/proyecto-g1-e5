#include <stdint.h>

#define GPIO_BASE      0x41200000U
#define OUT_DATA       (*(volatile uint32_t*)(GPIO_BASE + 0x8))

#define STEPPER_MASK   0x0Fu   // bits 0-3

#define STEPPER_DELAY_CYCLES  600000u

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles--) {
        __asm__("nop");
    }
}

// Secuencia de fases del motor (28BYJ-48 típico)
static const uint8_t step_seq[] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000
};

#define SEQ_LEN (sizeof(step_seq) / sizeof(step_seq[0]))

// -----------------------------
//   Giro a la DERECHA (sentido 1)
// -----------------------------
void motor_pasoapaso_derecha(uint32_t pasos)
{
    for (uint32_t i = 0; i < pasos; i++) {
        uint8_t pattern = step_seq[i % SEQ_LEN];

        OUT_DATA = (OUT_DATA & ~STEPPER_MASK) | (pattern & STEPPER_MASK);

        delay_cycles(STEPPER_DELAY_CYCLES);
    }
    // Apagar bobinas al final
    OUT_DATA &= ~STEPPER_MASK;
}

// -----------------------------
//   Giro a la IZQUIERDA (sentido contrario)
// -----------------------------
void motor_pasoapaso_izquierda(uint32_t pasos)
{
    for (uint32_t i = 0; i < pasos; i++) {
        // Recorremos la secuencia al revés: 0,3,2,1,0,3...
        uint32_t idx = i % SEQ_LEN;
        uint32_t rev_idx = (SEQ_LEN - idx) % SEQ_LEN;

        uint8_t pattern = step_seq[rev_idx];

        OUT_DATA = (OUT_DATA & ~STEPPER_MASK) | (pattern & STEPPER_MASK);

        delay_cycles(STEPPER_DELAY_CYCLES);
    }
    // Apagar bobinas al final
    OUT_DATA &= ~STEPPER_MASK;
}

