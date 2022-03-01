#define DEV_PATH 	"/dev/i2c-1"
#define DEV_ID 		0x4b

#define T_MSB  0x00
#define T_LSB  0x01
#define STATUS 0x02
#define CONFIG 0x03
#define IDREG  0x0b
#define RESET  0x2f

void adt7420_init();
double adt7420_read_temp();
void adt7420_close();

void delay();

char i2c_smbus_read_byte_data(int, char);
int i2c_smbus_write_byte_data(int, char, char);
int i2c_smbus_read_2byte_data(int, char);
