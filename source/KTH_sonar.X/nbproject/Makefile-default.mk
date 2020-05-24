#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=georeference.c uplink.c downlink.c UART.c commands.c messages.c delay.c sonar_dsp.c sonar_math.c main.c memory.c modes.c setup.c flags.c time.c sonar.c detection.c listen.c pulse.c sampling.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/georeference.o ${OBJECTDIR}/uplink.o ${OBJECTDIR}/downlink.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/messages.o ${OBJECTDIR}/delay.o ${OBJECTDIR}/sonar_dsp.o ${OBJECTDIR}/sonar_math.o ${OBJECTDIR}/main.o ${OBJECTDIR}/memory.o ${OBJECTDIR}/modes.o ${OBJECTDIR}/setup.o ${OBJECTDIR}/flags.o ${OBJECTDIR}/time.o ${OBJECTDIR}/sonar.o ${OBJECTDIR}/detection.o ${OBJECTDIR}/listen.o ${OBJECTDIR}/pulse.o ${OBJECTDIR}/sampling.o
POSSIBLE_DEPFILES=${OBJECTDIR}/georeference.o.d ${OBJECTDIR}/uplink.o.d ${OBJECTDIR}/downlink.o.d ${OBJECTDIR}/UART.o.d ${OBJECTDIR}/commands.o.d ${OBJECTDIR}/messages.o.d ${OBJECTDIR}/delay.o.d ${OBJECTDIR}/sonar_dsp.o.d ${OBJECTDIR}/sonar_math.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/memory.o.d ${OBJECTDIR}/modes.o.d ${OBJECTDIR}/setup.o.d ${OBJECTDIR}/flags.o.d ${OBJECTDIR}/time.o.d ${OBJECTDIR}/sonar.o.d ${OBJECTDIR}/detection.o.d ${OBJECTDIR}/listen.o.d ${OBJECTDIR}/pulse.o.d ${OBJECTDIR}/sampling.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/georeference.o ${OBJECTDIR}/uplink.o ${OBJECTDIR}/downlink.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/commands.o ${OBJECTDIR}/messages.o ${OBJECTDIR}/delay.o ${OBJECTDIR}/sonar_dsp.o ${OBJECTDIR}/sonar_math.o ${OBJECTDIR}/main.o ${OBJECTDIR}/memory.o ${OBJECTDIR}/modes.o ${OBJECTDIR}/setup.o ${OBJECTDIR}/flags.o ${OBJECTDIR}/time.o ${OBJECTDIR}/sonar.o ${OBJECTDIR}/detection.o ${OBJECTDIR}/listen.o ${OBJECTDIR}/pulse.o ${OBJECTDIR}/sampling.o

# Source Files
SOURCEFILES=georeference.c uplink.c downlink.c UART.c commands.c messages.c delay.c sonar_dsp.c sonar_math.c main.c memory.c modes.c setup.c flags.c time.c sonar.c detection.c listen.c pulse.c sampling.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MZ2048EFH064
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/georeference.o: georeference.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/georeference.o.d 
	@${RM} ${OBJECTDIR}/georeference.o 
	@${FIXDEPS} "${OBJECTDIR}/georeference.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/georeference.o.d" -o ${OBJECTDIR}/georeference.o georeference.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/uplink.o: uplink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uplink.o.d 
	@${RM} ${OBJECTDIR}/uplink.o 
	@${FIXDEPS} "${OBJECTDIR}/uplink.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/uplink.o.d" -o ${OBJECTDIR}/uplink.o uplink.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/downlink.o: downlink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/downlink.o.d 
	@${RM} ${OBJECTDIR}/downlink.o 
	@${FIXDEPS} "${OBJECTDIR}/downlink.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/downlink.o.d" -o ${OBJECTDIR}/downlink.o downlink.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/commands.o.d 
	@${RM} ${OBJECTDIR}/commands.o 
	@${FIXDEPS} "${OBJECTDIR}/commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/commands.o.d" -o ${OBJECTDIR}/commands.o commands.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/messages.o: messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/messages.o.d 
	@${RM} ${OBJECTDIR}/messages.o 
	@${FIXDEPS} "${OBJECTDIR}/messages.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/messages.o.d" -o ${OBJECTDIR}/messages.o messages.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/delay.o: delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/delay.o.d 
	@${RM} ${OBJECTDIR}/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/delay.o.d" -o ${OBJECTDIR}/delay.o delay.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar_dsp.o: sonar_dsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar_dsp.o.d 
	@${RM} ${OBJECTDIR}/sonar_dsp.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar_dsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar_dsp.o.d" -o ${OBJECTDIR}/sonar_dsp.o sonar_dsp.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar_math.o: sonar_math.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar_math.o.d 
	@${RM} ${OBJECTDIR}/sonar_math.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar_math.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar_math.o.d" -o ${OBJECTDIR}/sonar_math.o sonar_math.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/memory.o: memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/memory.o.d 
	@${RM} ${OBJECTDIR}/memory.o 
	@${FIXDEPS} "${OBJECTDIR}/memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/memory.o.d" -o ${OBJECTDIR}/memory.o memory.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/modes.o: modes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/modes.o.d 
	@${RM} ${OBJECTDIR}/modes.o 
	@${FIXDEPS} "${OBJECTDIR}/modes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/modes.o.d" -o ${OBJECTDIR}/modes.o modes.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/setup.o: setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/setup.o.d 
	@${RM} ${OBJECTDIR}/setup.o 
	@${FIXDEPS} "${OBJECTDIR}/setup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/setup.o.d" -o ${OBJECTDIR}/setup.o setup.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/flags.o: flags.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flags.o.d 
	@${RM} ${OBJECTDIR}/flags.o 
	@${FIXDEPS} "${OBJECTDIR}/flags.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/flags.o.d" -o ${OBJECTDIR}/flags.o flags.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/time.o: time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/time.o.d 
	@${RM} ${OBJECTDIR}/time.o 
	@${FIXDEPS} "${OBJECTDIR}/time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/time.o.d" -o ${OBJECTDIR}/time.o time.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar.o: sonar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar.o.d 
	@${RM} ${OBJECTDIR}/sonar.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar.o.d" -o ${OBJECTDIR}/sonar.o sonar.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/detection.o: detection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/detection.o.d 
	@${RM} ${OBJECTDIR}/detection.o 
	@${FIXDEPS} "${OBJECTDIR}/detection.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/detection.o.d" -o ${OBJECTDIR}/detection.o detection.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/listen.o: listen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/listen.o.d 
	@${RM} ${OBJECTDIR}/listen.o 
	@${FIXDEPS} "${OBJECTDIR}/listen.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/listen.o.d" -o ${OBJECTDIR}/listen.o listen.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/pulse.o: pulse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pulse.o.d 
	@${RM} ${OBJECTDIR}/pulse.o 
	@${FIXDEPS} "${OBJECTDIR}/pulse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/pulse.o.d" -o ${OBJECTDIR}/pulse.o pulse.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sampling.o: sampling.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sampling.o.d 
	@${RM} ${OBJECTDIR}/sampling.o 
	@${FIXDEPS} "${OBJECTDIR}/sampling.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sampling.o.d" -o ${OBJECTDIR}/sampling.o sampling.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/georeference.o: georeference.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/georeference.o.d 
	@${RM} ${OBJECTDIR}/georeference.o 
	@${FIXDEPS} "${OBJECTDIR}/georeference.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/georeference.o.d" -o ${OBJECTDIR}/georeference.o georeference.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/uplink.o: uplink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uplink.o.d 
	@${RM} ${OBJECTDIR}/uplink.o 
	@${FIXDEPS} "${OBJECTDIR}/uplink.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/uplink.o.d" -o ${OBJECTDIR}/uplink.o uplink.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/downlink.o: downlink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/downlink.o.d 
	@${RM} ${OBJECTDIR}/downlink.o 
	@${FIXDEPS} "${OBJECTDIR}/downlink.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/downlink.o.d" -o ${OBJECTDIR}/downlink.o downlink.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/commands.o: commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/commands.o.d 
	@${RM} ${OBJECTDIR}/commands.o 
	@${FIXDEPS} "${OBJECTDIR}/commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/commands.o.d" -o ${OBJECTDIR}/commands.o commands.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/messages.o: messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/messages.o.d 
	@${RM} ${OBJECTDIR}/messages.o 
	@${FIXDEPS} "${OBJECTDIR}/messages.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/messages.o.d" -o ${OBJECTDIR}/messages.o messages.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/delay.o: delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/delay.o.d 
	@${RM} ${OBJECTDIR}/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/delay.o.d" -o ${OBJECTDIR}/delay.o delay.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar_dsp.o: sonar_dsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar_dsp.o.d 
	@${RM} ${OBJECTDIR}/sonar_dsp.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar_dsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar_dsp.o.d" -o ${OBJECTDIR}/sonar_dsp.o sonar_dsp.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar_math.o: sonar_math.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar_math.o.d 
	@${RM} ${OBJECTDIR}/sonar_math.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar_math.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar_math.o.d" -o ${OBJECTDIR}/sonar_math.o sonar_math.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/memory.o: memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/memory.o.d 
	@${RM} ${OBJECTDIR}/memory.o 
	@${FIXDEPS} "${OBJECTDIR}/memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/memory.o.d" -o ${OBJECTDIR}/memory.o memory.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/modes.o: modes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/modes.o.d 
	@${RM} ${OBJECTDIR}/modes.o 
	@${FIXDEPS} "${OBJECTDIR}/modes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/modes.o.d" -o ${OBJECTDIR}/modes.o modes.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/setup.o: setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/setup.o.d 
	@${RM} ${OBJECTDIR}/setup.o 
	@${FIXDEPS} "${OBJECTDIR}/setup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/setup.o.d" -o ${OBJECTDIR}/setup.o setup.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/flags.o: flags.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flags.o.d 
	@${RM} ${OBJECTDIR}/flags.o 
	@${FIXDEPS} "${OBJECTDIR}/flags.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/flags.o.d" -o ${OBJECTDIR}/flags.o flags.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/time.o: time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/time.o.d 
	@${RM} ${OBJECTDIR}/time.o 
	@${FIXDEPS} "${OBJECTDIR}/time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/time.o.d" -o ${OBJECTDIR}/time.o time.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sonar.o: sonar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sonar.o.d 
	@${RM} ${OBJECTDIR}/sonar.o 
	@${FIXDEPS} "${OBJECTDIR}/sonar.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sonar.o.d" -o ${OBJECTDIR}/sonar.o sonar.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/detection.o: detection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/detection.o.d 
	@${RM} ${OBJECTDIR}/detection.o 
	@${FIXDEPS} "${OBJECTDIR}/detection.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/detection.o.d" -o ${OBJECTDIR}/detection.o detection.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/listen.o: listen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/listen.o.d 
	@${RM} ${OBJECTDIR}/listen.o 
	@${FIXDEPS} "${OBJECTDIR}/listen.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/listen.o.d" -o ${OBJECTDIR}/listen.o listen.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/pulse.o: pulse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pulse.o.d 
	@${RM} ${OBJECTDIR}/pulse.o 
	@${FIXDEPS} "${OBJECTDIR}/pulse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/pulse.o.d" -o ${OBJECTDIR}/pulse.o pulse.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/sampling.o: sampling.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sampling.o.d 
	@${RM} ${OBJECTDIR}/sampling.o 
	@${FIXDEPS} "${OBJECTDIR}/sampling.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"C:/Program Files (x86)/Microchip/xc32/v1.44/pic32mx/include" -Wall -MMD -MF "${OBJECTDIR}/sampling.o.d" -o ${OBJECTDIR}/sampling.o sampling.c    -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../libs/LIBQ_Library.X.a ../../libs/dsp_pic32mz_ef_fpu.a  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_ICD3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\libs\LIBQ_Library.X.a ..\..\libs\dsp_pic32mz_ef_fpu.a      -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x37F   -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_ICD3=1,--defsym=_min_heap_size=1000,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../libs/LIBQ_Library.X.a ../../libs/dsp_pic32mz_ef_fpu.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\libs\LIBQ_Library.X.a ..\..\libs\dsp_pic32mz_ef_fpu.a      -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=1000,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/KTH_sonar.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
