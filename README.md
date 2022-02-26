# Kria Kitchen Lifeguard
Xilinx Kria KV260 Kitchen Lifeguard

# Setting the Kria KV260

densebox_640_360

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
### Compile

```
g++ -std=c++17 -O2 -I. -o klifeguard klifeguard.cpp -lvitis_ai_library-facedetect -lvitis_ai_library-dpu_task -lvitis_ai_library-xnnpp -lvitis_ai_library-model_config -lvitis_ai_library-math -lvart-util -lxir -pthread -ljson-c -lglog -I/usr/include/opencv4 -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
```

## Execute

./klifeguard densebox_320_320 0 -t 8

./klifeguard densebox_640_360 0 -t 8


## Face Detection
The Kitchen ML Lifeguard uses the Face Detection library.
Face Detection uses the DenseBox neural network to detect human faces. The input is a picture with the faces you want to detect and the output is a vector containing the information of each detection box. 

