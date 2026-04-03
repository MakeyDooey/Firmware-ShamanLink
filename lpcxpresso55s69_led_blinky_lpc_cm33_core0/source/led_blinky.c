#include "board.h"
#include "app.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/*******************************************************************************
 * Definitions - Adding your new pins
 ******************************************************************************/
// Port 0 Pins
#define LED_YELLOW_PORT 0U
#define LED_YELLOW_PIN  15U

#define LED_1_PORT      0U
#define LED_1_PIN       1U

#define LED_17_PORT     0U
#define LED_17_PIN      17U

// Port 1 Pins
#define LED_P1_1_PORT   1U
#define LED_P1_1_PIN    1U

#define LED_P1_2_PORT   1U
#define LED_P1_2_PIN    2U

#define LED_P1_3_PORT   1U
#define LED_P1_3_PIN    3U

/*******************************************************************************
 * Variables & SysTick
 ******************************************************************************/
volatile uint32_t g_systickCounter;

void SysTick_Handler(void) {
    if (g_systickCounter != 0U) { g_systickCounter--; }
}

void SysTick_DelayTicks(uint32_t n) {
    g_systickCounter = n;
    while (g_systickCounter != 0U) { }
}

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitHardware();

    /* 1. Initialize GPIO Peripheral Clocks for both Port 0 and Port 1 */
    GPIO_PortInit(GPIO, 0U);
    GPIO_PortInit(GPIO, 1U);

    /* 2. Configure all pins as Outputs */
    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 1U}; // Start HIGH (OFF)

    // Port 0 initializations
    GPIO_PinInit(GPIO, LED_YELLOW_PORT, LED_YELLOW_PIN, &led_config);
    GPIO_PinInit(GPIO, LED_1_PORT,      LED_1_PIN,      &led_config);
    GPIO_PinInit(GPIO, LED_17_PORT,     LED_17_PIN,     &led_config);

    // Port 1 initializations
    GPIO_PinInit(GPIO, LED_P1_1_PORT,   LED_P1_1_PIN,   &led_config);
    GPIO_PinInit(GPIO, LED_P1_2_PORT,   LED_P1_2_PIN,   &led_config);
    GPIO_PinInit(GPIO, LED_P1_3_PORT,   LED_P1_3_PIN,   &led_config);

    /* 3. Start SysTick */
    SysTick_Config(SystemCoreClock / 1000U);

    while (1)
    {
        // Toggle sequence: Delay -> Toggle -> Repeat
        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_YELLOW_PORT, 1u << LED_YELLOW_PIN);

        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_1_PORT,      1u << LED_1_PIN);

        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_17_PORT,     1u << LED_17_PIN);

        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_P1_1_PORT,   1u << LED_P1_1_PIN);

        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_P1_2_PORT,   1u << LED_P1_2_PIN);

        SysTick_DelayTicks(1000U);
        GPIO_PortToggle(GPIO, LED_P1_3_PORT,   1u << LED_P1_3_PIN);
    }
}
