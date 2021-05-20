#include "stepper.h"
#include "pins.h"

/// Gets called on an stepper interrupt.
void Stepper_ISRHandler (Stepper_t *stepper) {
    if (__HAL_TIM_GET_FLAG (stepper->TIM, TIM_SR_UIF)) {
        __HAL_TIM_CLEAR_FLAG (stepper->TIM, TIM_SR_UIF);

        if (stepper->done_steps >= stepper->total_steps) {
            if (stepper->auto_enabled)
                Stepper_SetEnabled (stepper, false);

            stepper->moving = 0;
            stepper->current_sps = 0;
            HAL_TIM_Base_Stop_IT (stepper->TIM);
            return;
        }

        HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_PU_Pin, GPIO_PIN_RESET);
        for (uint16_t i = 0; i < 100; ++i)
            __asm__ ("nop\n\t");
        HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_PU_Pin, GPIO_PIN_SET);

        if (stepper->done_steps < stepper->stop_speedup_at) {
            stepper->current_sps += stepper->sps_increment;
            __HAL_TIM_SET_AUTORELOAD (stepper->TIM, 1000000 / stepper->current_sps);
        } else if (stepper->done_steps >= stepper->start_slowdown_at) {
            stepper->current_sps -= stepper->sps_increment;
            __HAL_TIM_SET_AUTORELOAD (stepper->TIM, 1000000 / stepper->current_sps);
        }

        ++stepper->done_steps;
        stepper->current_position = (stepper->dir == Stepper_Forwards) ? stepper->current_position + 1 : stepper->current_position - 1;        
    }
}

/// Initializes an stepper.
void Stepper_Init (Stepper_t *stepper, GPIO_InitTypeDef *GPIO_InitStruct) {
  //
  // Initializes the GPIO.
  //
  
  GPIO_InitStruct->Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct->Pull = GPIO_PULLDOWN;
  GPIO_InitStruct->Alternate = 0;
  GPIO_InitStruct->Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct->Pin = stepper->GPIO_PU_Pin | stepper->GPIO_EN_Pin | stepper->GPIO_DIR_Pin;
  HAL_GPIO_Init (stepper->GPIO, GPIO_InitStruct);

  HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_PU_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_DIR_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_EN_Pin, GPIO_PIN_RESET);

  
}

/// Changes an stepper dir.
void Stepper_SetDir (Stepper_t *stepper, StepperDir_t dir) {
    stepper->dir = dir;

    switch (dir) {
    case Stepper_Forwards:
        HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_DIR_Pin, GPIO_PIN_SET);
        break;
    case Stepper_Backwards:
        HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_DIR_Pin, GPIO_PIN_RESET);
        break;
    default:
        break;
    }
}

/// Moves an stepper.
void Stepper_Move (Stepper_t *stepper, int32_t pos) {
    if (stepper->moving)
        return;

    stepper->moving = 1;
    stepper->target_position = pos;

    if (pos > stepper->current_position) {
        stepper->total_steps = pos - stepper->current_position;
        Stepper_SetDir (stepper, Stepper_Forwards);
    } else {
        stepper->total_steps = stepper->current_position - pos;
        Stepper_SetDir (stepper, Stepper_Backwards);
    }

    if ((stepper->total_steps % 2) != 0) {
        ++stepper->total_steps;
        pos = (stepper->dir == Stepper_Forwards) ? pos + 1 : pos - 1;
    }

    uint32_t rampStepSize = (stepper->max_sps - stepper->min_sps) / stepper->sps_increment;
    if (rampStepSize > (stepper->total_steps / 2)) {
        stepper->stop_speedup_at = stepper->start_slowdown_at = stepper->total_steps / 2;
    } else {
        stepper->stop_speedup_at = rampStepSize;
        stepper->start_slowdown_at = stepper->total_steps - rampStepSize;
    }

    stepper->done_steps = 0;
    stepper->current_sps = stepper->min_sps;

    if (stepper->auto_enabled)
        Stepper_SetEnabled (stepper, true);

    __HAL_TIM_SET_COUNTER (stepper->TIM, 0);
    __HAL_TIM_SET_AUTORELOAD (stepper->TIM, 1000000 / (uint32_t) stepper->current_sps);
    if (HAL_TIM_Base_Start_IT (stepper->TIM) != HAL_OK)
        Error_Handler ();
}

/// Sets an stepper enabled or disabled.
void Stepper_SetEnabled (Stepper_t *stepper, bool enabled) {
    stepper->enabled = enabled;
    HAL_GPIO_WritePin (stepper->GPIO, stepper->GPIO_EN_Pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/// Locks the current thread until stepper done moving.
void Stepper_LockThread (Stepper_t *stepper) {
    while (stepper->moving);
}
