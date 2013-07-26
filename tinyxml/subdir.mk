################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tinyxml/tinystr.cpp \
../tinyxml/tinyxml.cpp \
../tinyxml/tinyxmlerror.cpp \
../tinyxml/tinyxmlparser.cpp 

OBJS += \
./tinyxml/tinystr.o \
./tinyxml/tinyxml.o \
./tinyxml/tinyxmlerror.o \
./tinyxml/tinyxmlparser.o 

CPP_DEPS += \
./tinyxml/tinystr.d \
./tinyxml/tinyxml.d \
./tinyxml/tinyxmlerror.d \
./tinyxml/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
tinyxml/%.o: ../tinyxml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


