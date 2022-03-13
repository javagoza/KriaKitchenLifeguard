/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include<time.h>

cv::Mat process_result(cv::Mat &m1, const vitis::ai::FaceDetectResult &result,
                       bool is_jpeg) {
  cv::Mat image;
  std::string legend;
  char timeStrBuffer[256];
  
  cv::resize(m1, image, cv::Size{result.width, result.height});
  
  time_t t = time(NULL);                                                        
  struct tm time = *localtime(&t);  
  
  

  strftime(timeStrBuffer, sizeof(timeStrBuffer), "%a %b %d %H:%M:%S %Y", &time);
  
  legend = "Unattended since:";
  cv::putText(image,legend,cv::Point(1,50),cv::FONT_HERSHEY_COMPLEX_SMALL,1,cv::Scalar(0,255,0),1,false);
  cv::putText(image,timeStrBuffer,cv::Point(1,60),cv::FONT_HERSHEY_COMPLEX_SMALL,1,cv::Scalar(0,255,255),1,false);

  for (const auto &r : result.rects) {
    LOG_IF(INFO, is_jpeg) << " " << r.score << " "  //
                          << r.x << " "             //
                          << r.y << " "             //
                          << r.width << " "         //
                          << r.height;
    cv::rectangle(image,
                  cv::Rect{cv::Point(r.x * image.cols, r.y * image.rows),
                           cv::Size{(int)(r.width * image.cols),
                                    (int)(r.height * image.rows)}},
                  0xff);

  }

  return image;
}
