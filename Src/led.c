#include <stdint.h>
#include <stdio.h>
#include "led.h"

/* GPIOA addresses for Onboard LED LD2 (PA5) on NUCLEO-F446RE */
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830U)
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000U)
#define GPIOA_ODR       (*(volatile uint32_t*)0x40020014U)

void delay(uint32_t count)
{
	for (uint32_t i = 0; i < count; i++);
}

void led_init_all(void)
{
	/* Enable GPIOA clock */
	RCC_AHB1ENR |= (1 << 0);

	/* Configure PA5 as Output (01) for LD2 on NUCLEO-F446RE */
	GPIOA_MODER &= ~(3U << (2 * 5));
	GPIOA_MODER |=  (1U << (2 * 5));

	/* Turn off LED initially */
	GPIOA_ODR &= ~(1U << 5);

	printf("\n=========================================\n");
	printf("  STM32F446RE Task Scheduler Started!    \n");
	printf("  Output routed to ITM Data Console Port 0\n");
	printf("=========================================\n\n");
}

void led_on(uint8_t led_no)
{
	if (led_no == LED_GREEN)
	{
		/* Turn ON physical on-board LED LD2 (PA5) */
		GPIOA_ODR |= (1U << 5);
		printf("[Task 1] Running  ==> LED_GREEN (PA5) ON\n");
	}
	else if (led_no == LED_ORANGE)
	{
		printf("[Task 2] Running  ==> LED_ORANGE ON\n");
	}
	else if (led_no == LED_BLUE)
	{
		printf("[Task 3] Running  ==> LED_BLUE ON\n");
	}
	else if (led_no == LED_RED)
	{
		printf("[Task 4] Running  ==> LED_RED ON\n");
	}
}

void led_off(uint8_t led_no)
{
	if (led_no == LED_GREEN)
	{
		/* Turn OFF physical on-board LED LD2 (PA5) */
		GPIOA_ODR &= ~(1U << 5);
		printf("[Task 1] Yielding ==> LED_GREEN (PA5) OFF\n");
	}
	else if (led_no == LED_ORANGE)
	{
		printf("[Task 2] Yielding ==> LED_ORANGE OFF\n");
	}
	else if (led_no == LED_BLUE)
	{
		printf("[Task 3] Yielding ==> LED_BLUE OFF\n");
	}
	else if (led_no == LED_RED)
	{
		printf("[Task 4] Yielding ==> LED_RED OFF\n");
	}
}
