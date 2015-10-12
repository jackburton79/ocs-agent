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
../NetworkInterface.cpp \
../NetworkRoster.cpp \
../ProcReader.cpp \
../RunningProcessesList.cpp \
../Screens.cpp \
../Storages.cpp \
../Support.cpp \
../VolumeReader.cpp \
../XML.cpp \
../edid-decode.c 	\
../main.cpp 

OBJS += \
./Agent.o \
./Configuration.o \
./IfConfigReader.o \
./Inventory.o \
./LoggedUsers.o \
./Machine.o \
./NetworkInterface.o \
./NetworkRoster.o \
./ProcReader.o \
./RunningProcessesList.o \
./Screens.o \
./Storages.o \
./Support.o \
./VolumeReader.o \
./XML.o \
./edid-decode.o \
./main.o 

CPP_DEPS += \
./Agent.d \
./Configuration.d \
./Drives.d \
./IfConfigReader.d \
./Inventory.d \
./LoggedUsers.d \
./Machine.d \
./NetworkInterface.d \
./NetworkRoster.d \
./ProcReader.d \
./RunningProcessesList.d \
./Screens.d \
./Storages.d \
./Support.d \
./VolumeReader.d \
./XML.d \
./edid-decode.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


