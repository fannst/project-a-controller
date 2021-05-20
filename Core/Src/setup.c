#include "setup.h"
#include "pins.h"
#include "main.h"
#include "stepper.h"

//////////////////////////////////////////////////////////////////////////
// Static Variables
//////////////////////////////////////////////////////////////////////////

//// UARTs

UART_HandleTypeDef s_DebugUARTHandle = {
  .Instance = USART1,
  .Init.BaudRate = 460800,
  .Init.WordLength = UART_WORDLENGTH_8B,
  .Init.StopBits = UART_STOPBITS_1,
  .Init.Parity = UART_PARITY_NONE,
  .Init.HwFlowCtl = UART_HWCONTROL_NONE,
  .Init.Mode = UART_MODE_TX_RX
};

/// Steppers

TIM_HandleTypeDef g_Tim3 = {
  .Instance = TIM3,
  .Init = {
    .Prescaler = 54,
    .CounterMode = TIM_COUNTERMODE_UP,
    .Period = 0,
    .ClockDivision = TIM_CLOCKDIVISION_DIV1,
    .RepetitionCounter = 0,
    .AutoReloadPreload = 0
  }
};

TIM_HandleTypeDef g_Tim2 = {
  .Instance = TIM2,
  .Init = {
    .Prescaler = 54,
    .CounterMode = TIM_COUNTERMODE_UP,
    .Period = 0,
    .ClockDivision = TIM_CLOCKDIVISION_DIV1,
    .RepetitionCounter = 0,
    .AutoReloadPreload = 0
  }
};

TIM_HandleTypeDef g_Tim4 = {
  .Instance = TIM4,
  .Init = {
    .Prescaler = 54,
    .CounterMode = TIM_COUNTERMODE_UP,
    .Period = 0,
    .ClockDivision = TIM_CLOCKDIVISION_DIV1,
    .RepetitionCounter = 0,
    .AutoReloadPreload = 0
  }
};

TIM_HandleTypeDef g_Tim5 = {
  .Instance = TIM5,
  .Init = {
    .Prescaler = 54,
    .CounterMode = TIM_COUNTERMODE_UP,
    .Period = 0,
    .ClockDivision = TIM_CLOCKDIVISION_DIV1,
    .RepetitionCounter = 0,
    .AutoReloadPreload = 0
  }
};

Stepper_t g_Stepper0 = STEPPER_NEW (200, 2000, 2, 0, STEPPER0_GPIO, STEPPER0_EN_PIN, STEPPER0_PU_PIN, STEPPER0_DIR_PIN, &g_Tim3);
Stepper_t g_Stepper1 = STEPPER_NEW (200, 4000, 2, 0, STEPPER1_GPIO, STEPPER1_EN_PIN, STEPPER1_PU_PIN, STEPPER1_DIR_PIN, &g_Tim2);
Stepper_t g_Stepper2 = STEPPER_NEW (200, 4000, 2, 0, STEPPER2_GPIO, STEPPER2_EN_PIN, STEPPER2_PU_PIN, STEPPER2_DIR_PIN, &g_Tim4);
Stepper_t g_Stepper3 = STEPPER_NEW (200, 4000, 2, 0, STEPPER3_GPIO, STEPPER3_EN_PIN, STEPPER3_PU_PIN, STEPPER3_DIR_PIN, &g_Tim5);
Stepper_t g_Stepper4 = {};
Stepper_t g_Stepper5 = {};

uint32_t g_StepperCount = 6;
Stepper_t *g_Steppers[6] = {
  &g_Stepper0,
  &g_Stepper1,
  &g_Stepper2,
  &g_Stepper3,
  &g_Stepper4,
  &g_Stepper5
};

//////////////////////////////////////////////////////////////////////////
// Setup Methods
//////////////////////////////////////////////////////////////////////////

/// Enables all required clock sources.
void Setup_RCC (void) {
    ////
    //    GPIO's
    ////

    __GPIOB_CLK_ENABLE ();
    __GPIOA_CLK_ENABLE ();
    __GPIOC_CLK_ENABLE ();
    __GPIOE_CLK_ENABLE ();

    ////
    //    UART's
    ////

    __USART1_CLK_ENABLE ();

    ////
    //    TIM's
    ////

    __TIM2_CLK_ENABLE ();
    __TIM3_CLK_ENABLE ();
    __TIM4_CLK_ENABLE ();
    __TIM5_CLK_ENABLE ();
}

/// Configures all the GPIO's.
void Setup_GPIO (void) {
    GPIO_InitTypeDef GPIO_InitStruct;
  
  ////
  //    User LED's
  ////
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

  GPIO_InitStruct.Pin = LD1_PIN | LD2_PIN | LD3_PIN;
  HAL_GPIO_Init (LD123_GPIO, &GPIO_InitStruct);

  ////
  //    USART1 RX/TX
  ////

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = 7;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = USART1_TX_PIN | USART1_RX_PIN;
  HAL_GPIO_Init (USART1_RX_TX_GPIO, &GPIO_InitStruct);

  ////
  //    Steppers
  ////

  Stepper_Init (&g_Stepper0, &GPIO_InitStruct);
  Stepper_Init (&g_Stepper1, &GPIO_InitStruct);
  Stepper_Init (&g_Stepper2, &GPIO_InitStruct);
  Stepper_Init (&g_Stepper3, &GPIO_InitStruct);
}

/// Configures all the UART's.
void Setup_UART (void) {
  ////
  //    USART1
  ////

  if (HAL_UART_Init (&s_DebugUARTHandle) != HAL_OK)
    Error_Handler ();
}

/// Configures the steppers.
void Setup_Steppers (void) {

}


/// Configures the TIMs.
void Setup_TIMs (void) {
  if (HAL_TIM_Base_Init (&g_Tim2) != HAL_OK)
    Error_Handler ();

  if (HAL_TIM_Base_Init (&g_Tim3) != HAL_OK)
    Error_Handler ();

  if (HAL_TIM_Base_Init (&g_Tim4) != HAL_OK)
    Error_Handler ();

  if (HAL_TIM_Base_Init (&g_Tim5) != HAL_OK)
    Error_Handler ();
}

/// Configures the NVIC.
void Setup_NVIC (void) {
  NVIC_EnableIRQ (TIM2_IRQn);
  NVIC_EnableIRQ (TIM3_IRQn);
  NVIC_EnableIRQ (TIM4_IRQn);
  NVIC_EnableIRQ (TIM5_IRQn);
}
