################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../global_sum_openmp.c \
../hello_serial.c \
../hist_parallel.c \
../hist_serial.c \
../my_rand.c \
../odd_even_parallel.c \
../odd_even_parallel_for.c \
../odd_even_serial.c \
../series_sum.c \
../trap_openmp1.c \
../trap_openmp2.c \
../trap_openmp2_error.c \
../trap_openmp2_parallel_for.c \
../trap_serial.c 

OBJS += \
./global_sum_openmp.o \
./hello_serial.o \
./hist_parallel.o \
./hist_serial.o \
./my_rand.o \
./odd_even_parallel.o \
./odd_even_parallel_for.o \
./odd_even_serial.o \
./series_sum.o \
./trap_openmp1.o \
./trap_openmp2.o \
./trap_openmp2_error.o \
./trap_openmp2_parallel_for.o \
./trap_serial.o 

C_DEPS += \
./global_sum_openmp.d \
./hello_serial.d \
./hist_parallel.d \
./hist_serial.d \
./my_rand.d \
./odd_even_parallel.d \
./odd_even_parallel_for.d \
./odd_even_serial.d \
./series_sum.d \
./trap_openmp1.d \
./trap_openmp2.d \
./trap_openmp2_error.d \
./trap_openmp2_parallel_for.d \
./trap_serial.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-10.0/bin/nvcc -G -g -O0   -odir "." -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-10.0/bin/nvcc -G -g -O0 --compile  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


