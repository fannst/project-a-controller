import os.path as p

flags = [
    '-I./Core/Inc/',
    '-I./Drivers/CMSIS/Include',
    '-I./Drivers/CMSIS/Device/ST/STM32F7xx/Include/',
    '-I./Drivers/STM32F7xx_HAL_Driver/Inc/',
    '-I./LWIP/App/',
    '-I./LWIP/Target/',
    '-I./Middlewares/Third_Party/LwIP/src/include/',
    '-I./Middlewares/Third_Party/LwIP/system/arch/',
    '-DSTM32F767xx'
]

def Settings (**kwargs):
    return {
        'flags': flags,
        'include_paths_relative_to_dir': './'
    }
