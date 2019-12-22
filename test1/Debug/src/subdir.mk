################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../src/vectorAdd.cu 

OBJS += \
./src/vectorAdd.o 

CU_DEPS += \
./src/vectorAdd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda/bin/nvcc -I"/usr/local/cuda-10.0/samples/0_Simple" -I"/usr/local/cuda-10.0/samples/common/inc" -I"/home/ubuntu/cuda-workspace/test1" -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_35,code=sm_35 -gencode arch=compute_60,code=sm_60 -gencode arch=compute_61,code=sm_61 -gencode arch=compute_62,code=sm_62 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda/bin/nvcc -I"/usr/local/cuda-10.0/samples/0_Simple" -I"/usr/local/cuda-10.0/samples/common/inc" -I"/home/ubuntu/cuda-workspace/test1" -G -g -O0 --compile --relocatable-device-code=false -gencode arch=compute_35,code=compute_35 -gencode arch=compute_60,code=compute_60 -gencode arch=compute_61,code=compute_61 -gencode arch=compute_62,code=compute_62 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_60,code=sm_60 -gencode arch=compute_61,code=sm_61 -gencode arch=compute_62,code=sm_62 -m64 -ccbin aarch64-linux-gnu-g++  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


