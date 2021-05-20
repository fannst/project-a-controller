/*
Copyright 2021 Luke A.C.A. Rieff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __STEPPER_H
#define __STEPPER_H

#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Stepper_Forwards,
    Stepper_Backwards
} StepperDir_t;

typedef struct {
    StepperDir_t dir;

    int32_t     current_position;
    int32_t     target_position;
    
    /* Current Operation */
    int32_t     total_steps;
    uint32_t    stop_speedup_at;
    uint32_t    start_slowdown_at;
    uint32_t    done_steps;
    
    uint16_t    current_sps,
                min_sps,
                max_sps,
                sps_increment;

    unsigned    enabled         : 1;
    unsigned    moving          : 1;
    unsigned    auto_enabled    : 1;
    unsigned    reserved        : 5;

    GPIO_TypeDef *GPIO;
    uint32_t      GPIO_EN_Pin,
                  GPIO_PU_Pin,
                  GPIO_DIR_Pin;

    TIM_HandleTypeDef  *TIM;
} Stepper_t;

#define STEPPER_NEW(MIN_SPS, MAX_SPS, SPS_INCREMENT, AUTOMATIC, GPIOn, EN, PU, DIR, TIMn) \
    { \
        .dir = Stepper_Forwards, \
        .current_position = 0, \
        .target_position = 0, \
        .current_sps = 0, \
        .min_sps = MIN_SPS, \
        .max_sps = MAX_SPS, \
        .sps_increment = SPS_INCREMENT, \
        .enabled = 0, \
        .moving = 0, \
        .auto_enabled = AUTOMATIC, \
        .GPIO = GPIOn, \
        .GPIO_EN_Pin = EN, \
        .GPIO_PU_Pin = PU, \
        .GPIO_DIR_Pin = DIR, \
        .TIM = TIMn, \
    }

#define Stepper_SetAuto(STEPPER, AUTO_ENABLED) \
    (STEPPER)->auto_enabled = (AUTO_ENABLED)

/// Gets called on an stepper interrupt.
void Stepper_ISRHandler (Stepper_t *stepper);

/// Initializes an stepper.
void Stepper_Init (Stepper_t *stepper, GPIO_InitTypeDef *GPIO_InitStruct);

/// Changes an stepper dir.
void Stepper_SetDir (Stepper_t *stepper, StepperDir_t dir);

/// Moves an stepper.
void Stepper_Move (Stepper_t *stepper, int32_t pos);

/// Sets an stepper enabled or disabled.
void Stepper_SetEnabled (Stepper_t *stepper, bool enabled);

/// Locks the current thread until stepper done moving.
void Stepper_LockThread (Stepper_t *stepper);

#ifdef __cplusplus
}
#endif

#endif
