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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define FileName "data.dat"

void report_and_exit(const char* msg);
int writeLastTimeSeen(const char* msg);

cv::Mat process_result(cv::Mat &m1, const vitis::ai::FaceDetectResult &result,
                       bool is_jpeg) {
  cv::Mat image;
  char timeStrBuffer[256];
  char unattendedTimeStrBuffer[256];
  
  static time_t lastTimeSeenPeople;
  
  cv::resize(m1, image, cv::Size{result.width, result.height});
  
  time_t actualTime = time(NULL);
  
  struct tm lastTimeTm = *localtime(&lastTimeSeenPeople); 
  struct tm timeTm = *localtime(&actualTime);
  
 
  
  cv::rectangle(image,
                  cv::Rect{cv::Point(0, result.height - 24),
                           cv::Size{result.width, 24}},
                  cv::Scalar(30,30,30),-1);
  
  strftime(timeStrBuffer, sizeof(timeStrBuffer), "Time: %H:%M:%S", &timeTm);
  cv::putText(image,timeStrBuffer,cv::Point(1,result.height - 9),cv::FONT_HERSHEY_PLAIN ,1,cv::Scalar(255,255,255),1,false);
  
  strftime(unattendedTimeStrBuffer, sizeof(timeStrBuffer), "Unatt. since: %H:%M:%S", &lastTimeTm);
  cv::putText(image,unattendedTimeStrBuffer,cv::Point(380,result.height - 9),cv::FONT_HERSHEY_PLAIN ,1,cv::Scalar(0,255,0),1,false);

  for (const auto &r : result.rects) {
    lastTimeSeenPeople = actualTime;
    strftime(unattendedTimeStrBuffer, sizeof(timeStrBuffer), "%Y-%m-%d %H:%M:%S", &lastTimeTm);
    writeLastTimeSeen(unattendedTimeStrBuffer);
    LOG_IF(INFO, is_jpeg) << " " << r.score << " "  //
                          << r.x << " "             //
                          << r.y << " "             //
                          << r.width << " "         //
                          << r.height << " time:" << timeStrBuffer;
    
    cv::rectangle(image,
                  cv::Rect{cv::Point(r.x * image.cols, r.y * image.rows),
                           cv::Size{(int)(r.width * image.cols),
                                    (int)(r.height * image.rows)}},
                  0xff, 2);

  }

  return image;
}

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1); /* EXIT_FAILURE */
}

int writeLastTimeSeen(const char* msg) {
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive versus shared) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */

  int fd; /* file descriptor to identify a file within a process */
  if ((fd = open(FileName, O_RDWR | O_CREAT, 0666)) < 0)  /* -1 signals an error */
    report_and_exit("open failed...");

  if (fcntl(fd, F_SETLK, &lock) < 0) /** F_SETLK doesn't block, F_SETLKW does **/
    report_and_exit("fcntl failed to get lock...");
  else {
    write(fd, msg, strlen(msg)); /* populate data file */
  }

  /* Now release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");

  close(fd); /* close the file: would unlock if needed */
  return 0;  /* terminating the process would unlock as well */
}
