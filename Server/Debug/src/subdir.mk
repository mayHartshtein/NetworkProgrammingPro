################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ChatRoom.cpp \
../src/Login.cpp \
../src/ServerManager.cpp \
../src/User.cpp \
../src/main.cpp 

OBJS += \
./src/ChatRoom.o \
./src/Login.o \
./src/ServerManager.o \
./src/User.o \
./src/main.o 

CPP_DEPS += \
./src/ChatRoom.d \
./src/Login.d \
./src/ServerManager.d \
./src/User.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/user/Desktop/The project/SocketUtils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


