# Kria Kitchen Lifeguard
Xilinx Kria KV260 Kitchen Lifeguard project for [Hackster.io Adaptive Computing Challenge 2021 with AMD-Xilinx ](https://www.hackster.io/contests/xilinxadaptivecomputing2021)

Smart device that monitors unattended kitchen cooking appliances using computer vision with Machine Learning.

See project blog at hackster.io: [KV260 Kitchen ML Lifeguard](https://www.hackster.io/javagoza/kria-kitchen-ml-lifeguard-d038f7)

## Setting the Kria KV260
You have to install the Vitis AI Runtime on your own image. The version of the board image should be 2021.2 or above.


### Step 1: Installing a Board Image
A system images for the KV260 starter kit can be downloaded from [here](https://login.xilinx.com/app/xilinxinc_f5awsprod_1/exknv8ms950lm0Ldh0x7/sso/saml)

### Step 2: Flash OS Image to the SD card
You can use etcher (https://etcher.io/)


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
The build script will call g++

```
g++ -std=c++17 -O2 -I. -o klifeguard klifeguard.cpp -lvitis_ai_library-facedetect -lvitis_ai_library-dpu_task -lvitis_ai_library-xnnpp -lvitis_ai_library-model_config -lvitis_ai_library-math -lvart-util -lxir -pthread -ljson-c -lglog -I/usr/include/opencv4 -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
```

## Execute
To test the program with a USB camera as input, run the following command:

```./klifeguard densebox_320_320 0 -t 8```
or
```./klifeguard densebox_640_360 0 -t 8```

Here, 0 is the first USB camera device node. If you have multiple USB cameras, the value is 1,2,3, etc., where, -t is the number of threads.

Important: Enable X11 forwarding with the following command (suppose in this example that the host machine IP address is 192.168.2.16) when logging in to the board using an SSH terminal because the video examples require a Linux windows system to work properly.

```export DISPLAY=192.168.2.48:0.0```


## Execute with TCPServer

To start both person detection and the tcpserver to get the date and time of the last person detection.

```sh klifeguard.sh```



## Face Detection
The Kitchen ML Lifeguard uses the Face Detection library.
Face Detection uses the DenseBox neural network to detect human faces. The input is a picture with the faces you want to detect and the output is a vector containing the information of each detection box. 

<img src="images/facedetect.jpg" alt="Face Detection" width="150"/>


vitis::ai::FaceDetect
Base class for detecting the position of faces in the input image (cv::Mat).
Input is an image (cv::Mat).

Output is a vector of position and score for faces in the input image.

Sample code:

```
auto image = cv::imread("sample_facedetect.jpg");
auto network = vitis::ai::FaceDetect::create(
               "densebox_640_360",
               true);
auto result = network->run(image);
for (const auto &r : result.rects) {
   auto score = r.score;
   auto x = r.x * image.cols;
   auto y = r.y * image.rows;
   auto width = r.width * image.cols;
   auto height = r.height * image.rows;
}
```

### Project blog

[Kria Kitchen ML Lifeguard](https://www.hackster.io/javagoza/kria-kitchen-ml-lifeguard-d038f7) 



