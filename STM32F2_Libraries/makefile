
TOOLCHAIN = GCC_ARMCM3_STM32F2

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
SZ = arm-none-eabi-size

STMLIB_DIR = .

INCLUDEDIRS = \
$(STMLIB_DIR) \
$(STMLIB_DIR)/CMSIS/Include \
$(STMLIB_DIR)/CMSIS/Device/ST/STM32F2xx/Include \
$(STMLIB_DIR)/STM32F2xx_StdPeriph_Driver/inc

STSRC_DIR = $(STMLIB_DIR)/STM32F2xx_StdPeriph_Driver/src

GCCSRCDIR = . \
$(STSRC_DIR)

DEBUG = -ggdb3

OPTIM = -O1

AFLAGS = -ahls -mapcs-32

CFLAGS = \
$(DEBUG) \
-Wa,-adhlns=$(subst $(suffix $<),.lst,$<) \
$(patsubst %,-I%,$(INCLUDEDIRS)) \
-D $(TOOLCHAIN) \
-D inline= -mthumb -mcpu=cortex-m3 \
$(OPTIM) \
-ffunction-sections \
-fdata-sections \
-finline-functions \
-fomit-frame-pointer \
-Wunreachable-code \
-Wunused-variable \
-Wcast-align \
-Wimplicit \
-Wpointer-arith \
-Wswitch \
-Wreturn-type \
-Wshadow \
-Wl,--gc-sections
		
# main assembly source files that need to be Assembled
AM_SOURCE = \
startup_stm32f2xx.s

AM_OBJS = $(AM_SOURCE:.s=.o)
AM_LIST = $(AM_SOURCE:.s=.lst)

# main c source files that need to be compiled
CM_SOURCE = \
system_stm32f2xx.c \
$(STSRC_DIR)/misc.c \
$(STSRC_DIR)/stm32f2xx_gpio.c \
$(STSRC_DIR)/stm32f2xx_tim.c \
$(STSRC_DIR)/stm32f2xx_rcc.c \
$(STSRC_DIR)/stm32f2xx_usart.c \
$(STSRC_DIR)/stm32f2xx_iwdg.c \
$(STSRC_DIR)/stm32f2xx_syscfg.c \
$(STSRC_DIR)/stm32f2xx_can.c \
$(STSRC_DIR)/stm32f2xx_spi.c \
$(STSRC_DIR)/stm32f2xx_flash.c \
$(STSRC_DIR)/stm32f2xx_adc.c \
$(STSRC_DIR)/stm32f2xx_dma.c \
$(STSRC_DIR)/stm32f2xx_i2c.c \
$(STSRC_DIR)/stm32f2xx_pwr.c

CM_OBJS = $(CM_SOURCE:.c=.o)
CM_LIST = $(CM_SOURCE:.c=.lst)

all: $(CM_OBJS) $(AM_OBJS)
	 
$(CM_OBJS) : %.o : %.c $(LD_MAIN) makefile *.h
	@ echo ".compiling '$<'"
	$(CC) -c $(CFLAGS) $< -o $@
	@ echo
	
$(AM_OBJS) : %.o: %.s $(LD_MAIN) makefile *.h
	@ echo ".assembling $<'"
	$(AS) $(AFLAGS) -o $*.o $< > $*.lst
	@ echo

clean :
	$(foreach CDIR, $(GCCSRCDIR), rm -f $(CDIR)/*.o $(CDIR)/*.lst; )
