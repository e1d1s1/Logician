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
	g++ -DPOSIX -D__UNIX__ -D__WXGTK__ -I/usr/include/libxml2 -I/usr/include/wx-2.8 -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8 -I/usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-release-2.8 -O0 -g3 -Wall -c -fmessage-length=0 --std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


