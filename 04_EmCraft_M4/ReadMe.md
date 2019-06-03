#add template to create group project
    <group>
        <name>board</name>
    </group>
    <group>
        <name>CMSIS</name>
    </group>
    <group>
        <name>drivers</name>
    </group>
    <group>
        <name>startup</name>
    </group>
    <group>
        <name>utilities</name>
    </group>
    <group>
        <name>source</name>
    </group>
    
# setting
## General Options
* Devices: NXP MIMX8MQ6_M4

## C/C++ Compiler
* Preprocessor:
$PROJ_DIR$/board
$PROJ_DIR$/drivers
$PROJ_DIR$/source
$PROJ_DIR$/startup
$PROJ_DIR$/utilities
$PROJ_DIR$/CMSIS/include

CPU_MIMX8MQ6DVAJZ
DEBUG

## Output Converter
* Output Format: Raw Binary

## Linker
* Override default: $PROJ_DIR$/MIMX8MQ6xxxJZ_cm4_ram.icf

## Debugger
* Driver: J-Link/J-Trace
