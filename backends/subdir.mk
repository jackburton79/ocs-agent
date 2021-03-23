################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../backends/DataBackend.cpp \
../backends/DMIDataBackend.cpp \
../backends/DMIDecodeBackend.cpp \
../backends/LSHWBackend.cpp \
../backends/MemInfoBackend.cpp \
../backends/UnameBackend.cpp \

OBJS += \
./backends/DataBackend.o \
./backends/DMIDataBackend.o \
./backends/DMIDecodeBackend.o \
./backends/LSHWBackend.o \
./backends/MemInfoBackend.o \
./backends/UnameBackend.o \

CPP_DEPS += \
./backends/DataBackend.d \
./backends/DMIDataBackend.d \
./backends/DMIDecodeBackend.d \
./backends/LSHWBackend.d \
./backends/MemInfoBackend.d \
./backends/UnameBackend.d \

# Each subdirectory must supply rules for building sources it contributes
backends/%.o: ../backends/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL $(CXXFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


