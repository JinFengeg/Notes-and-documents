/***************************************************************
Copyright ? ALIENTEK Co., Ltd. 1998-2021. All rights reserved.
文件名 : uart_test.c
作者 : 邓涛
版本 : V1.0
描述 : 串口在原始模式下进行数据传输--应用程序示例代码
其他 : 无
论坛 : www.openedv.com
日志 : 初版 V1.0 2021/7/20 邓涛创建
***************************************************************/
#define _GNU_SOURCE //在源文件开头定义_GNU_SOURCE 宏
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
typedef struct uart_hardware_cfg {
    unsigned int baudrate; /* 波特率 */
 unsigned char dbit; /* 数据位 */
 char parity; /* 奇偶校验 */
 unsigned char sbit; /* 停止位 */
 int num;
} uart_cfg_t;
 uart_cfg_t cfg = {0};//申请一个全局变量
static struct termios old_cfg; //用于保存终端的配置参数
static int fd; //串口终端对应的文件描述符
/**
** 串口初始化操作
** 参数 device 表示串口终端的设备节点
**/
static int uart_init(const char *device)
{
 /* 打开串口终端 */
 fd = open(device, O_RDWR | O_NOCTTY);
 if (0 > fd) {
 fprintf(stderr, "open error: %s: %s\n", device, strerror(errno));
 return -1;
 }
 /* 获取串口当前的配置参数 */
 if (0 > tcgetattr(fd, &old_cfg)) {
 fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
 close(fd);
 return -1;
 }
 return 0;
}
/**
** 串口配置
** 参数 cfg 指向一个 uart_cfg_t 结构体对象
**/
static int uart_cfg(const uart_cfg_t *cfg)
{
 struct termios new_cfg = {0}; //将 new_cfg 对象清零
 speed_t speed;
 /* 设置为原始模式 */
 cfmakeraw(&new_cfg);
 /* 使能接收 */
 new_cfg.c_cflag |= CREAD;
 /* 设置波特率 */
 switch (cfg->baudrate) {
 case 1200: speed = B1200;
 break;
 case 1800: speed = B1800;
 break;
 case 2400: speed = B2400;
 break;
 case 4800: speed = B4800;
 break;
 case 9600: speed = B9600;
 break;
 case 19200: speed = B19200;
 break;
 case 38400: speed = B38400;
 break;
 case 57600: speed = B57600;
 break;
 case 115200: speed = B115200;
 break;
 case 230400: speed = B230400;
 break;
 case 460800: speed = B460800;
 break;
 case 500000: speed = B500000;
 break;
 default: //默认配置为 115200
 speed = B115200;
 printf("default baud rate: 115200\n");
 break;
 }
 if (0 > cfsetspeed(&new_cfg, speed)) {
 fprintf(stderr, "cfsetspeed error: %s\n", strerror(errno));
 return -1;
 }
 /* 设置数据位大小 */
 new_cfg.c_cflag &= ~CSIZE; //将数据位相关的比特位清零
 switch (cfg->dbit) {
 case 5:
 new_cfg.c_cflag |= CS5;
 break;
 case 6:
 new_cfg.c_cflag |= CS6;
 break;
 case 7:
 new_cfg.c_cflag |= CS7;
 break;
 case 8:
 new_cfg.c_cflag |= CS8;
 break;
 default: //默认数据位大小为 8
 new_cfg.c_cflag |= CS8;
 printf("default data bit size: 8\n");
 break;
 }
 /* 设置奇偶校验 */
 switch (cfg->parity) {
 case 'N': //无校验
 new_cfg.c_cflag &= ~PARENB;
 new_cfg.c_iflag &= ~INPCK;
 break;
 case 'O': //奇校验
 new_cfg.c_cflag |= (PARODD | PARENB);
 new_cfg.c_iflag |= INPCK;
 break;
 case 'E': //偶校验
 new_cfg.c_cflag |= PARENB;
 new_cfg.c_cflag &= ~PARODD; /* 清除 PARODD 标志，配置为偶校验 */
 new_cfg.c_iflag |= INPCK;
 break;
 default: //默认配置为无校验
 new_cfg.c_cflag &= ~PARENB;
 new_cfg.c_iflag &= ~INPCK;
 printf("default parity: N\n");
 break;
 }
 /* 设置停止位 */
 switch (cfg->sbit) {
    case 1: //1 个停止位
 new_cfg.c_cflag &= ~CSTOPB;
 break;
 case 2: //2 个停止位
 new_cfg.c_cflag |= CSTOPB;
 break;
 default: //默认配置为 1 个停止位
 new_cfg.c_cflag &= ~CSTOPB;
 printf("default stop bit size: 1\n");
 break;
 }
 /* 将 MIN 和 TIME 设置为 0 */
 new_cfg.c_cc[VTIME] = 0;
 new_cfg.c_cc[VMIN] = 0;
 /* 清空缓冲区 */
 if (0 > tcflush(fd, TCIOFLUSH)) {
 fprintf(stderr, "tcflush error: %s\n", strerror(errno));
 return -1;
 }
 /* 写入配置、使配置生效 */
 if (0 > tcsetattr(fd, TCSANOW, &new_cfg)) {
 fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
 return -1;
 }
 /* 配置 OK 退出 */
 return 0;
}
/***
--dev=/dev/ttymxc2
--brate=115200
--dbit=8
--parity=N
--sbit=1
--type=read
***/
/**
** 打印帮助信息
**/
static void show_help(const char *app)
{
 printf("Usage: %s [选项]\n"
 "\n 必选选项:\n"
 " --dev=DEVICE 指定串口终端设备名称, 譬如--dev=/dev/ttymxc2\n"
 " --type=TYPE 指定操作类型, 读串口还是写串口, 譬如--type=read(read 表示读、write 表示写、其它值无效)\n"
 "\n 可选选项:\n"
 " --brate=SPEED 指定串口波特率, 譬如--brate=115200\n"
 " --dbit=SIZE 指定串口数据位个数, 譬如--dbit=8(可取值为: 5/6/7/8)\n"
 " --parity=PARITY 指定串口奇偶校验方式, 譬如--parity=N(N 表示无校验、O 表示奇校验、E 表示偶校验)\n"
 " --sbit=SIZE 指定串口停止位个数, 譬如--sbit=1(可取值为: 1/2)\n"
 " --help 查看本程序使用帮助信息\n\n", app);
}
/**
** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
**/
static void io_handler(int sig, siginfo_t *info, void *context)
{
	float data[8][12] ={0};
 unsigned char buf[100] = {0};
 int ret;
 int n;
 if(SIGRTMIN != sig)
 return;
 /* 判断串口是否有数据可读 */
 if (POLL_IN == info->si_code) {
 ret = read(fd, buf, 29); //一次最多读 8 个字节数据
if(ret !=29)
{
	printf("read error: %d\n",cfg.num);
	perror("");
}
//数据转换
//加速度
		data[cfg.num][0] = (float)((short)((buf[3] << 8) | buf[4])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)
		data[cfg.num][1] = (float)((short)((buf[5] << 8) | buf[6])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)
		data[cfg.num][2] = (float)((short)((buf[7] << 8) | buf[8])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)																		   
		//角速度
		data[cfg.num][3] = (float)((short)((buf[9] << 8) | buf[10])) / 32768 * 2000;
		data[cfg.num][4] = (float)((short)((buf[11] << 8) | buf[12])) / 32768 * 2000;
		data[cfg.num][5] = (float)((short)((buf[13] << 8) | buf[14])) / 32768 * 2000;
		//磁场
		data[cfg.num][6] = (float)((short)((buf[15] << 8) | buf[16]));
		data[cfg.num][7] = (float)((short)((buf[17] << 8) | buf[18]));
		data[cfg.num][8] = (float)((short)((buf[19] << 8) | buf[20]));
		//角度
		data[cfg.num][9] = (float)((short)((buf[21] << 8) | buf[22])) / 32768 * 180;
		data[cfg.num][10] = (float)((short)((buf[23] << 8) | buf[24])) / 32768 * 180;
		data[cfg.num][11] = (float)((short)((buf[25] << 8) | buf[26])) / 32768 * 180;


 /*printf("[ ");
 for (n = 0; n < ret; n++)
 printf("0x%hhx ", buf[n]);
 printf("]\n");*/


	printf("第%d个IMU:加x=%f,加y=%f,加z=%f,\
角x=%f,角y=%f,角z=%f,\
磁场x=%f,磁场y=%f,磁场z=%f,\
角度x=%f,角度y =%f,角度z=%f\r\n",\
	cfg.num,data[cfg.num][0],data[cfg.num][1],data[cfg.num][2],data[cfg.num][3],data[cfg.num][4],data[cfg.num][5]\
	,data[cfg.num][6],data[cfg.num][7],data[cfg.num][8],data[cfg.num][9],data[cfg.num][10],data[cfg.num][11]);
	

 }
}
/**
** 异步 I/O 初始化函数
**/

static void async_io_init(void)
{
    struct sigaction sigatn;
 int flag;
 /* 使能异步 I/O */
 flag = fcntl(fd, F_GETFL); //使能串口的异步 I/O 功能
 flag |= O_ASYNC;
 fcntl(fd, F_SETFL, flag);
 /* 设置异步 I/O 的所有者 */
 fcntl(fd, F_SETOWN, getpid());
 /* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
 fcntl(fd, F_SETSIG, SIGRTMIN);
 /* 为实时信号 SIGRTMIN 注册信号处理函数 */
 sigatn.sa_sigaction = io_handler; //当串口有数据可读时，会跳转到 io_handler 函数
 sigatn.sa_flags = SA_SIGINFO;//采用实时信号
 sigemptyset(&sigatn.sa_mask);//不设置信号掩码
 sigaction(SIGRTMIN, &sigatn, NULL);
}

int main(int argc, char *argv[])
{

 char *device = NULL;
 int rw_flag = -1;
 unsigned char w_buf[5][8] = {{0x50,0x03,0x00,0x34,0x00,0x0C,0x09,0x80},\
	 {0x51,0x03,0x00,0x34,0x00,0x0C,0x08,0x51},\
	 {0x52,0x03,0x00,0x34,0x00,0x0C,0x08,0x62},\
	 {0x53,0x03,0x00,0x34,0x00,0x0C,0x09,0xB3},\
	 {0x54,0x03,0x00,0x34,0x00,0x0C,0x08,0x04}}; //通过串口发送出去的数据
 int n;
 /* 解析出参数 */
 for (n = 1; n < argc; n++) {
 if (!strncmp("--dev=", argv[n], 6))
 device = &argv[n][6];
 else if (!strncmp("--brate=", argv[n], 8))
 cfg.baudrate = atoi(&argv[n][8]);
 else if (!strncmp("--dbit=", argv[n], 7))
 cfg.dbit = atoi(&argv[n][7]);
 else if (!strncmp("--parity=", argv[n], 9))
 cfg.parity = argv[n][9];
 else if (!strncmp("--sbit=", argv[n], 7))
 cfg.sbit = atoi(&argv[n][7]);
 else if (!strncmp("--type=", argv[n], 7)) {
 if (!strcmp("read", &argv[n][7]))
 rw_flag = 0; //读
 else if (!strcmp("write", &argv[n][7]))
 rw_flag = 1; //写
 }
 else if (!strcmp("--help", argv[n])) {
 show_help(argv[0]); //打印帮助信息
 exit(EXIT_SUCCESS);
 }
 }
 if (NULL == device || -1 == rw_flag) {
 fprintf(stderr, "Error: the device and read|write type must be set!\n");
 show_help(argv[0]);
 exit(EXIT_FAILURE);
 }
 /* 串口初始化 */
 if (uart_init(device))
 exit(EXIT_FAILURE);
 /* 串口配置 */
 if (uart_cfg(&cfg)) {
 tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
 close(fd);
 exit(EXIT_FAILURE);
 }
 /* 读|写串口 */
 //switch (rw_flag) {
 //case 0: //读串口数据
 async_io_init(); //我们使用异步 I/O 方式读取串口的数据，调用该函数去初始化串口的异步 I/O
 //for ( ; ; )
 //sleep(1); //进入休眠、等待有数据可读，有数据可读之后就会跳转到 io_handler()函数
 //break;
 //case 1: //向串口写入数据
 cfg.num=0;
 for ( ; ; ) { //循环向串口写入数据

 //一次向串口写入 8 个字节
 if(8 != write(fd, w_buf[cfg.num], 8))
 {
	printf("\r\nwrite error :%d",cfg.num);
	perror("");
 }
 sleep(3); //间隔 1 秒钟
 cfg.num++;
 if(cfg.num == 5)
 {
	cfg.num =0;
 }
 }
 //break;
 //}
 /* 退出 */
 tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
 close(fd);
 exit(EXIT_SUCCESS);
}