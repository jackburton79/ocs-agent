################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Agent.cpp \
../Configuration.cpp \
../IfConfigReader.cpp \
../Inventory.cpp \
../LoggedUsers.cpp \
../Machine.cpp \
../ProcReader.cpp \
../RunningProcessesList.cpp \
../Support.cpp \
../VolumeReader.cpp \
../edid-decode.c \
../main.cpp 

OBJS += \
./Agent.o \
./Configuration.o \
./IfConfigReader.o \
./Inventory.o \
./LoggedUsers.o \
./Machine.o \
./ProcReader.o \
./RunningProcessesList.o \
./Support.o \
./VolumeReader.o \
./edid-decode.o \
./main.o 

CPP_DEPS += \
./Agent.d \
./Configuration.d \
./IfConfigReader.d \
./Inventory.d \
./LoggedUsers.d \
./Machine.d \
./ProcReader.d \
./RunningProcessesList.d \
./Support.d \
./VolumeReader.d \
./edid-decode.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


