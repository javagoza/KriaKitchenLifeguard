#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libi2c.hpp"

int fd;



void adt7420_init()
{

  /* open i2c comms */
  if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
    perror("Unable to open i2c device");
    exit(1);
  }

  /* configure i2c slave */
  if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
    perror("Unable to configure i2c slave device");
    close(fd);
    exit(1);
  }

i2c_smbus_write_byte_data(fd, CONFIG, 0x80); // 16bit cts conversions

}


double adt7420_read_temp()
{

  int32_t t_raw  = i2c_smbus_read_2byte_data(fd, T_MSB);
 
  // ADT74x0 puts MSB first so flip order
  int16_t lo = (t_raw & 0xff00) >> 8;
  int16_t hi = (t_raw & 0x00ff);

  int16_t t128 = hi << 8 | lo;

  /* Calculate ambient temperature */
  double T_DegC = t128 / 128.0;

  return(T_DegC);
}

void adt7420_close()
{
  close(fd);
}

void delay(int t){
  usleep(t * 1000);
}

char i2c_smbus_read_byte_data(int fd, char reg)
{
  char data;
  char reg0;

  reg0 = reg;

  write(fd, &reg0, 1);
  read(fd, &data, 1);

  return(data);
}

int i2c_smbus_write_byte_data(int fd, char reg, char data) {
  char config[2];

  config[0] = reg;
  config[1] = data;
  write(fd, config, 2);

  return(0);
}

int i2c_smbus_read_2byte_data(int fd, char reg)
{
  short data;
  char reg0;
  int *res;

  reg0 = reg;

  write(fd, &reg0, 1);
  if(read(fd, &data, 2) != 2) {
	printf("I2C 2-byte error\n");
  }
 
  return((int)data);
}
