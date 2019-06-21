################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Agent.cpp \
../Configuration.cpp \
../Inventory.cpp \
../Logger.cpp \
../Machine.cpp \
../NetworkInterface.cpp \
../NetworkRoster.cpp \
../Processors.cpp \
../ProcReader.cpp \
../ProcessRoster.cpp \
../Screens.cpp \
../Softwares.cpp \
../StorageRoster.cpp \
../Support.cpp \
../UsersRoster.cpp \
../VolumeRoster.cpp \
../XML.cpp \
../ZLibCompressor.cpp \
../edid-decode.c

OBJS += \
./Agent.o \
./Configuration.o \
./Inventory.o \
./Logger.o \
./Machine.o \
./NetworkInterface.o \
./NetworkRoster.o \
./Processors.o \
./ProcReader.o \
./ProcessRoster.o \
./Screens.o \
./Softwares.o \
./StorageRoster.o \
./Support.o \
./UsersRoster.o \
./VolumeRoster.o \
./XML.o \
./ZLibCompressor.o \
./edid-decode.o 

CPP_DEPS += \
./Agent.d \
./Configuration.d \
./Inventory.d \
./Logger.d \
./Machine.d \
./NetworkInterface.d \
./NetworkRoster.d \
./Processors.d \
./ProcReader.d \
./ProcessRoster.d \
./Screens.d \
./Sofwares.d \
./StorageRoster.d \
./Support.d \
./UsersRoster.d \
./VolumeRoster.d \
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


