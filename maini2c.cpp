#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libi2c.hpp"

int main(argc, argv)
     int argc;
     char *argv[];
{
  double temp;

  adt7420_init();

  temp = adt7420_read_temp();

  printf("TEMP=%.2f oC\n", temp);

  adt7420_close();

  exit(0);
}
