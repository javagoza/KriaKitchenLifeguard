# Kria Kitchen Lifeguard
Xilinx Kria KV260 Kitchen Lifeguard

## Install Vitis AI Library User Guide

[Vitis AI Library User Guide (UG1354)](https://docs.xilinx.com/r/en-US/ug1354-xilinx-ai-sdk/Introduction)

## Clone repository
```
git clone --recurse-submodules https://github.com/javagoza/KriaKitchenLifeguard kriaklifeguard
```
## Build
```
cd kriaklifeguard/
```
```
bash -x build.sh
```
## Include libraries

```
g++ -std=c++17 -O2 -I. -o klifeguard klifeguard.cpp -lvitis_ai_library-facedetect -lvitis_ai_library-dpu_task -lvitis_ai_library-xnnpp -lvitis_ai_library-model_config -lvitis_ai_library-math -lvart-util -lxir -pthread -ljson-c -lglog -I/usr/include/opencv4 -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
```
