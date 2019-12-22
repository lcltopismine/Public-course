################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CCalGlobalSum_pthreads_busywait_error.c \
../CCalGlobalSum_pthreads_error.c \
../CCalGlobalSum_pthreads_metex.c \
../CCalGlobalSum_pthreads_semaphore.c \
../CCalGlobalSum_pthreads_treesum_barrier_mutex.c \
../CCalGlobalSum_pthreads_treesum_barrier_sem.c \
../CCalGlobalSum_pthreads_treesum_barrier_sem_in_class.c \
../CCalGlobalSum_pthreads_treesum_sem_in_class.c \
../multi_thread_naive.c \
../practise1.c \
../practise_2.c \
../practise_3.c \
../series_sum.c \
../test22.c \
../testDataRace.c 

OBJS += \
./CCalGlobalSum_pthreads_busywait_error.o \
./CCalGlobalSum_pthreads_error.o \
./CCalGlobalSum_pthreads_metex.o \
./CCalGlobalSum_pthreads_semaphore.o \
./CCalGlobalSum_pthreads_treesum_barrier_mutex.o \
./CCalGlobalSum_pthreads_treesum_barrier_sem.o \
./CCalGlobalSum_pthreads_treesum_barrier_sem_in_class.o \
./CCalGlobalSum_pthreads_treesum_sem_in_class.o \
./multi_thread_naive.o \
./practise1.o \
./practise_2.o \
./practise_3.o \
./series_sum.o \
./test22.o \
./testDataRace.o 

C_DEPS += \
./CCalGlobalSum_pthreads_busywait_error.d \
./CCalGlobalSum_pthreads_error.d \
./CCalGlobalSum_pthreads_metex.d \
./CCalGlobalSum_pthreads_semaphore.d \
./CCalGlobalSum_pthreads_treesum_barrier_mutex.d \
./CCalGlobalSum_pthreads_treesum_barrier_sem.d \
./CCalGlobalSum_pthreads_treesum_barrier_sem_in_class.d \
./CCalGlobalSum_pthreads_treesum_sem_in_class.d \
./multi_thread_naive.d \
./practise1.d \
./practise_2.d \
./practise_3.d \
./series_sum.d \
./test22.d \
./testDataRace.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


