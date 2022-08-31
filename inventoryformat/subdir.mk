################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LOCAL_CPP_SRCS = \
../inventoryformat/InventoryFormat.cpp \
../inventoryformat/InventoryFormatOCS.cpp \


LOCALOBJECTS= $(addprefix ./inventoryformat/, $(notdir $(LOCAL_CPP_SRCS:.cpp=.o)))
LOCALDEPS= $(addprefix ./inventoryformat/, $(notdir $(LOCALOBJECTS:.o=.d)))

SOURCES+= $(LOCAL_CPP_SRCS)
OBJS += $(LOCALOBJECTS)
DEPS += $(LOCALDEPS)


# Each subdirectory must supply rules for building sources it contributes
inventoryformat/%.o: ../inventoryformat/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DTIXML_USE_STL $(CXXFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


