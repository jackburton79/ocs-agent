################################################################################
# Automatically-generated file. Do not edit!
################################################################################

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include tinyxml2/subdir.mk
-include http/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif


# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: OcsInventory-ng-agent urltest

# Tool invocations
OcsInventory-ng-agent: $(OBJS) $(USER_OBJS) main.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "ocsinventory-agent" $(OBJS) $(USER_OBJS) main.o $(LDFLAGS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

urltest: $(OBJS) $(USER_OBJS) test/urltest.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "test/urltest" $(OBJS) $(USER_OBJS) test/urltest.o $(LDFLAGS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) main.o test/urltest.o ocsinventory-agent test/urltest
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

