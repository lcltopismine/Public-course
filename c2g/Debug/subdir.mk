################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../color2gray.c \
../color2gray_mpi.c \
../color2gray_mpi_sr.c \
../color2gray_openmp.c 

OBJS += \
./color2gray.o \
./color2gray_mpi.o \
./color2gray_mpi_sr.o \
./color2gray_openmp.o 

C_DEPS += \
./color2gray.d \
./color2gray_mpi.d \
./color2gray_mpi_sr.d \
./color2gray_openmp.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


