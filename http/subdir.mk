################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../http/HTTP.cpp \
../http/HTTPDefines.cpp \
../http/HTTPHeader.cpp \
../http/HTTPRequestHeader.cpp \
../http/HTTPResponseHeader.cpp 

OBJS += \
./http/HTTP.o \
./http/HTTPDefines.o \
./http/HTTPHeader.o \
./http/HTTPRequestHeader.o \
./http/HTTPResponseHeader.o 

CPP_DEPS += \
./http/HTTP.d \
./http/HTTPDefines.d \
./http/HTTPHeader.d \
./http/HTTPRequestHeader.d \
./http/HTTPResponseHeader.d 


# Each subdirectory must supply rules for building sources it contributes
http/%.o: ../http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


