################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Agent.cpp \
../Configuration.cpp \
../Inventory.cpp \
../Logger.cpp \
../LoggedUsers.cpp \
../Machine.cpp \
../NetworkInterface.cpp \
../NetworkRoster.cpp \
../ProcReader.cpp \
../RunningProcessesList.cpp \
../Screens.cpp \
../Softwares.cpp \
../Storages.cpp \
../Support.cpp \
../VolumeReader.cpp \
../XML.cpp \
../ZLibCompressor.cpp \
../edid-decode.c

OBJS += \
./Agent.o \
./Configuration.o \
./Inventory.o \
./Logger.o \
./LoggedUsers.o \
./Machine.o \
./NetworkInterface.o \
./NetworkRoster.o \
./ProcReader.o \
./RunningProcessesList.o \
./Screens.o \
./Softwares.o \
./Storages.o \
./Support.o \
./VolumeReader.o \
./XML.o \
./ZLibCompressor.o \
./edid-decode.o 

CPP_DEPS += \
./Agent.d \
./Configuration.d \
./Drives.d \
./Inventory.d \
./Logger.d \
./LoggedUsers.d \
./Machine.d \
./NetworkInterface.d \
./NetworkRoster.d \
./ProcReader.d \
./RunningProcessesList.d \
./Screens.d \
./Sofwares.d \
./Storages.d \
./Support.d \
./VolumeReader.d \
./XML.d \
./ZLibCompressor.d \
./edid-decode.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL $(CXXFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


