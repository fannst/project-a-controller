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

#include "stm32f7xx_hal.h"

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     current_position;
    int32_t     target_position;

    uint16_t    current_sps,
                min_sps,
                max_sps,
                sps_increment;

    unsigned    enabled         : 1;
    unsigned    moving          : 1;
    unsigned    auto_enabled    : 1;
    unsigned    reserved        : 5;
} Stepper_t;

#define STEPPER_NEW(MIN_SPS, MAX_SPS, SPS_INCREMENT, AUTOMATIC) \
    { \
        .current_position = 0, \
        .target_position = 0, \
        .current_sps = 0, \
        .min_sps = MIN_SPS, \
        .max_sps = MAX_SPS, \
        .sps_increment = SPS_INCREMENT, \
        .enabled = 0, \
        .moving = 0, \
        .auto_enabled = AUTOMATIC \
    }

#ifdef __cplusplus
}
#endif

#endif
