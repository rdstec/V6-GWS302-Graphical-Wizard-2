
# using FreeRTOS V8.2.1
# using GCC 4.9.3

TARGET_MAIN = GWS302XXX

TOOLCHAIN = GCC_ARMCM3_STM32F2

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
SZ = arm-none-eabi-size

ODFLAGS = -x --syms

STMLIB_DIR = ../STM32F2_Libraries
OTGDRIVER_DIR = $(STMLIB_DIR)/STM32_USB_OTG_Driver
OTGDEVICE_DIR = $(STMLIB_DIR)/STM32_USB_Device_Library
STMPLB_DIR = $(STMLIB_DIR)/STM32F2xx_StdPeriph_Driver
STMPIN_DIR = $(STMPLB_DIR)/inc
STMPSR_DIR = $(STMPLB_DIR)/src
INCLUDE_DIR = ./include
FREERTOS_DIR = ./freertos
TEST_DIR = ./testtasks

INCLUDEDIRS = . \
$(STMLIB_DIR) \
$(STMLIB_DIR)/CMSIS/Include \
$(STMLIB_DIR)/CMSIS/Device/ST/STM32F2xx/Include \
$(STMPIN_DIR) \
$(OTGDRIVER_DIR)/inc \
$(OTGDEVICE_DIR)/core/inc \
$(OTGDEVICE_DIR)/class/cdc/inc \
$(INCLUDE_DIR) \
$(FREERTOS_DIR) \
$(TEST_DIR)

GCCSRCDIR = . \
$(FREERTOS_DIR) \
$(STSRC_DIR) \
$(TEST_DIR)

LD_MAIN = stm32f2xx_flash_main.ld

MATH_LIB = -lm

LF_MAIN = -nostartfiles $(MATH_LIB) -Xlinker -o$(TARGET_MAIN).axf -Xlinker -M -Xlinker -Map=$(TARGET_MAIN).map

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
AM_SOURCE = 

AM_OBJS = $(AM_SOURCE:.s=.o)
AM_LIST = $(AM_SOURCE:.s=.lst)

# main c source files that need to be compiled
CM_SOURCE = \
$(FREERTOS_DIR)/list.c \
$(FREERTOS_DIR)/queue.c \
$(FREERTOS_DIR)/tasks.c \
$(FREERTOS_DIR)/port.c \
$(FREERTOS_DIR)/heap_2.c \
$(FREERTOS_DIR)/syscalls.c \
main.c \
bm_sym.c \
bm_num.c \
bm_f1all.c \
bm_f1eur.c \
bm_f1cyr.c \
functions.c \
operate.c \
display.c \
screens.c \
COMtask.c \
CANtask.c \
timers.c \
TWItask.c \
interrupts.c \
power.c \
strings.c \
$(TEST_DIR)/BlockQ.c \
$(TEST_DIR)/GenQTest.c \
$(TEST_DIR)/integer.c \
$(TEST_DIR)/PollQ.c \
$(TEST_DIR)/QPeek.c \
$(TEST_DIR)/recmutex.c \
$(TEST_DIR)/semtest.c \
$(TEST_DIR)/timertest.c

CM_OBJS = $(CM_SOURCE:.c=.o)
CM_LIST = $(CM_SOURCE:.c=.lst)

# library object
LB_OBJS = \
$(STMLIB_DIR)/startup_stm32f2xx.o \
$(STMLIB_DIR)/system_stm32f2xx.o \
$(STMPSR_DIR)/misc.o \
$(STMPSR_DIR)/stm32f2xx_gpio.o \
$(STMPSR_DIR)/stm32f2xx_tim.o \
$(STMPSR_DIR)/stm32f2xx_rcc.o \
$(STMPSR_DIR)/stm32f2xx_usart.o \
$(STMPSR_DIR)/stm32f2xx_iwdg.o \
$(STMPSR_DIR)/stm32f2xx_syscfg.o \
$(STMPSR_DIR)/stm32f2xx_can.o \
$(STMPSR_DIR)/stm32f2xx_flash.o \
$(STMPSR_DIR)/stm32f2xx_spi.o \
$(STMPSR_DIR)/stm32f2xx_exti.o \
$(STMPSR_DIR)/stm32f2xx_adc.o \
$(STMPSR_DIR)/stm32f2xx_dma.o \
$(STMPSR_DIR)/stm32f2xx_i2c.o \
$(STMPSR_DIR)/stm32f2xx_pwr.o

all: $(TARGET_MAIN).bin
	 
$(TARGET_MAIN).bin : $(TARGET_MAIN).axf
	$(CP) $(TARGET_MAIN).axf -O binary $(TARGET_MAIN).bin
	$(OD) $(ODFLAGS) $(TARGET_MAIN).axf > $(TARGET_MAIN).dmp
	$(SZ) -x -B $(TARGET_MAIN).axf	
	padsum $(TARGET_MAIN).bin common.h

$(TARGET_MAIN).axf : $(AM_OBJS) $(CM_OBJS) $(LB_OBJS) $(LD_MAIN) makefile *.h
	@ echo ".linking "
	$(CC) $(CFLAGS) -T$(LD_MAIN) $(AM_OBJS) $(CM_OBJS) $(LB_OBJS) $(LF_MAIN)
	@ echo

$(CM_OBJS) : %.o : %.c $(LD_MAIN) $(FREERTOS_DIR)/FreeRTOSConfig.h makefile *.h
	@ echo ".compiling '$<'"
	$(CC) -c $(CFLAGS) $< -o $@
	@ echo

$(AM_OBJS) : %.o: %.s $(LD_MAIN) $(FREERTOS_DIR)/FreeRTOSConfig.h makefile *.h
	@ echo ".assembling $<'"
	$(AS) $(AFLAGS) -o $*.o $< > $*.lst
	@ echo
		
clean :
	$(foreach CDIR, $(GCCSRCDIR), rm -f $(CDIR)/*.o $(CDIR)/*.lst; )
	-rm -f ./sim/*.obj ./sim/*.~*  ./sim/*.tds
	-rm -f *.bak $(TARGET_MAIN).axf $(TARGET_MAIN).map $(TARGET_MAIN).dmp
