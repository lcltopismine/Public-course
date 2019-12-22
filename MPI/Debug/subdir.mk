################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CGreetingMPIPassingDataError1.c \
../CGreetingMPIPassingDataError2.c \
../CMPI_changeCommunicator.c \
../Greeting_api.c \
../Linear_chain.c \
../MPI_ranking.c \
../MPI_ranking_bkup.c \
../MPI_ranking_error.c \
../P2_1a.c \
../P2_1b.c \
../P2_1c.c \
../compare_bcast.c \
../mpi_get_buff.c \
../mpi_global_sum.c \
../mpi_global_sum_collect.c \
../mpi_histogram.c \
../mpi_histogram_collect.c \
../mpi_histogram_correct.c \
../my_rand.c \
../ping_pong.c \
../ring.c 

OBJS += \
./CGreetingMPIPassingDataError1.o \
./CGreetingMPIPassingDataError2.o \
./CMPI_changeCommunicator.o \
./Greeting_api.o \
./Linear_chain.o \
./MPI_ranking.o \
./MPI_ranking_bkup.o \
./MPI_ranking_error.o \
./P2_1a.o \
./P2_1b.o \
./P2_1c.o \
./compare_bcast.o \
./mpi_get_buff.o \
./mpi_global_sum.o \
./mpi_global_sum_collect.o \
./mpi_histogram.o \
./mpi_histogram_collect.o \
./mpi_histogram_correct.o \
./my_rand.o \
./ping_pong.o \
./ring.o 

C_DEPS += \
./CGreetingMPIPassingDataError1.d \
./CGreetingMPIPassingDataError2.d \
./CMPI_changeCommunicator.d \
./Greeting_api.d \
./Linear_chain.d \
./MPI_ranking.d \
./MPI_ranking_bkup.d \
./MPI_ranking_error.d \
./P2_1a.d \
./P2_1b.d \
./P2_1c.d \
./compare_bcast.d \
./mpi_get_buff.d \
./mpi_global_sum.d \
./mpi_global_sum_collect.d \
./mpi_histogram.d \
./mpi_histogram_collect.d \
./mpi_histogram_correct.d \
./my_rand.d \
./ping_pong.d \
./ring.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


