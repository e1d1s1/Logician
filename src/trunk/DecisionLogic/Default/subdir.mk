################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CodeControl.cpp \
../DecisionLogic.cpp \
../EditorPrefs.cpp \
../FindReplaceDialog.cpp \
../GUIClass.cpp \
../LogicTable.cpp \
../MDIChild.cpp \
../ProjectManager.cpp \
../WorkerClass.cpp \
../stdafx.cpp \
../utilities.cpp 

OBJS += \
./CodeControl.o \
./DecisionLogic.o \
./EditorPrefs.o \
./FindReplaceDialog.o \
./GUIClass.o \
./LogicTable.o \
./MDIChild.o \
./ProjectManager.o \
./WorkerClass.o \
./stdafx.o \
./utilities.o 

CPP_DEPS += \
./CodeControl.d \
./DecisionLogic.d \
./EditorPrefs.d \
./FindReplaceDialog.d \
./GUIClass.d \
./LogicTable.d \
./MDIChild.d \
./ProjectManager.d \
./WorkerClass.d \
./stdafx.d \
./utilities.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


