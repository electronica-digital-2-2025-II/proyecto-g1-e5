#include <stdint.h>
#define GPIO_BASE      0x41200000U
#define GPIO_CH1_DATA  (*(volatile uint32_t*)(GPIO_BASE + 0x0))

#define DHT_BIT  1u
#define DHT_MASK (1u << DHT_BIT)

int sensor_humedad_digital(void)
{
    return ( (GPIO_CH1_DATA & DHT_MASK) ? 1 : 0 );
}
