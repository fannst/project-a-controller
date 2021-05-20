#ifndef __PINS_H
#define __PINS_H

#define LD1_PIN               GPIO_PIN_0
#define LD2_PIN               GPIO_PIN_7
#define LD3_PIN               GPIO_PIN_14
#define LD123_GPIO            GPIOB

#define USART1_TX_PIN         GPIO_PIN_9
#define USART1_RX_PIN         GPIO_PIN_10
#define USART1_RX_TX_GPIO     GPIOA

#define STEPPER0_PU_PIN       GPIO_PIN_2
#define STEPPER0_EN_PIN       GPIO_PIN_4
#define STEPPER0_DIR_PIN      GPIO_PIN_5
#define STEPPER0_GPIO         GPIOE

#define STEPPER1_PU_PIN       GPIO_PIN_6
#define STEPPER1_EN_PIN       GPIO_PIN_1
#define STEPPER1_DIR_PIN      GPIO_PIN_3
#define STEPPER1_GPIO         GPIOE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
