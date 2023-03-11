#ifndef LED_H
#define LED_H

void led_task(void);

int on(int argc, char *argv[]);
int off(int argc, char *argv[]);
int dc(int argc, char *argv[]);
int freq(int argc, char *argv[]);

#endif // LED_H