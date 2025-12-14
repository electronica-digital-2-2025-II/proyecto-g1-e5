#include <stdint.h>

// Mismos registros y base
#define GPIO_BASE      0x41200000U
#define GPIO_CH1_DATA  (*(volatile uint32_t*)(GPIO_BASE + 0x0))

#define INDUCTIVE_BIT  0u
#define INDUCTIVE_MASK (1u << INDUCTIVE_BIT)

// Devuelve 0 o 1
int sensor_inductivo_leer(void)
{
    return (GPIO_CH1_DATA & INDUCTIVE_MASK) ? 1 : 0;
}
