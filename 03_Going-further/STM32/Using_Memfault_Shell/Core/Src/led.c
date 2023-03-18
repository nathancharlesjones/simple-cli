#include "led.h"
#include "main.h"
#include "shell.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

static bool led_is_blinking = true;
static uint32_t prev_millis = 0;
static uint32_t on_time = 500, off_time = 500;

int on(int argc, char *argv[])
{
	led_is_blinking = true;
	return 0;
}

int off(int argc, char *argv[])
{
	led_is_blinking = false;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	return 0;
}

static void set_dc(float dc)
{
	uint32_t period = on_time + off_time;
	on_time = dc * period / 100.0;
	off_time = period - on_time;
}

int dc(int argc, char *argv[])
{
	if( argc == 1 )
	{
		char buffer[9] = {0};
		float dc = on_time*100.0/(on_time + off_time);

		// Hacky version of printing a floating-point value. Print out the 
		// integer portion first, then subtract the integer portion from
		// the float value to get the fractional portion. Multiply by 10
		// and convert to int to get the first decimal place.
		//
		sprintf(buffer, "%2d.%1d%%\r", (int)dc, (int)( ( dc - (int)dc ) * 10 ) );
		
		shell_put_line(buffer);
	}
	else
	{
		set_dc(atof(argv[1]));
	}
	return 0;
}

static void set_freq(float freq)
{
	float old_freq = 1000.0 / ( on_time + off_time );

	on_time = on_time * old_freq / freq;
	if( on_time < 1 ) on_time = 1;

	off_time = off_time * old_freq / freq;
	if( off_time < 1 ) off_time = 1;
}

int freq(int argc, char *argv[])
{
	if( argc == 1 )
	{
		char buffer[12] = {0};
		float freq = 1000.0/(on_time + off_time);

		// Hacky version of printing a floating-point value. Print out the 
		// integer portion first, then subtract the integer portion from
		// the float value to get the fractional portion. Multiply by 10
		// and convert to int to get the first decimal place.
		//
		sprintf((char*)buffer, "%3d.%1d Hz\r", (int)freq, (int)( ( freq - (int)freq ) * 10 ) );

		shell_put_line(buffer);
	}
	else
	{
		set_freq(atof(argv[1]));
	}
	return 0;
}

void led_task(void)
{
	// Non-blocking Blinky. If the "Blinky" task is enabled
    // then check if the LED is currently on or off.
    //
    if( led_is_blinking )
    {
      // If the LED is on, and has been on for at least <on_time> 
      // milliseconds, then turn the LED off.
      //
      if( HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET )
      {
        if( HAL_GetTick() - prev_millis > on_time )
        {
          prev_millis = HAL_GetTick();
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        }
      }
      
      // Otherwise check if it has been off for at least <off_time> 
      // milliseconds and, if it has, turn the LED on.
      //
      else
      {
        if( HAL_GetTick() - prev_millis > off_time )
        {
          prev_millis = HAL_GetTick();
          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        }
      }
    }
}