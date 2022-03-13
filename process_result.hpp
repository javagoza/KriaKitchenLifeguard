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

cv::Mat process_result(cv::Mat &m1, const vitis::ai::FaceDetectResult &result,
                       bool is_jpeg) {
  cv::Mat image;
  char timeStrBuffer[256];
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
  
  strftime(timeStrBuffer, sizeof(timeStrBuffer), "Unatt. since: %H:%M:%S", &lastTimeTm);
  cv::putText(image,timeStrBuffer,cv::Point(380,result.height - 9),cv::FONT_HERSHEY_PLAIN ,1,cv::Scalar(0,255,0),1,false);

  for (const auto &r : result.rects) {
    lastTimeSeenPeople = actualTime;
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

int writeLastTimeSeen() {
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */

  int fd; /* file descriptor to identify a file within a process */
  if ((fd = open(FileName, O_RDONLY)) < 0)  /* -1 signals an error */
    report_and_exit("open to read failed...");

  /* If the file is write-locked, we can't continue. */
  fcntl(fd, F_GETLK, &lock); /* sets lock.l_type to F_UNLCK if no write lock */
  if (lock.l_type != F_UNLCK)
    report_and_exit("file is still write locked...");

  lock.l_type = F_RDLCK; /* prevents any writing during the reading */
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("can't get a read-only lock...");

  /* Read the bytes (they happen to be ASCII codes) one at a time. */
  int c; /* buffer for read bytes */
  while (read(fd, &c, 1) > 0)    /* 0 signals EOF */
    write(STDOUT_FILENO, &c, 1); /* write one byte to the standard output */

  /* Release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");

  close(fd);
  return 0;
}
