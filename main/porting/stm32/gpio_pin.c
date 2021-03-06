/**
 * gpio_pin.c
 */

#include <string.h>
#include "generic_hal.h"
#include "gpio_pin.h"

int gpio_read_pin(pin_t pin)
{
    return HAL_GPIO_ReadPin(pin.port, pin.pin);
}

void gpio_write_pin(pin_t pin, int value)
{
    HAL_GPIO_WritePin(pin.port, pin.pin, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_set_output_pushpull(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = pin.pin;
    init_param.Speed = GPIO_SPEED_FREQ_HIGH;
    init_param.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(pin.port, &init_param);
}

void gpio_set_output_opendrain(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = pin.pin;
    init_param.Speed = GPIO_SPEED_FREQ_HIGH;
    init_param.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(pin.port, &init_param);
}

void gpio_set_input_floating(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = pin.pin;
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin.port, &init_param);
}

void gpio_set_input_pullup(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = pin.pin;
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(pin.port, &init_param);
}

void gpio_set_input_pulldown(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = pin.pin;
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(pin.port, &init_param);
}