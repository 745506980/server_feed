#ifndef CLIENT_LIGHT_RGB_PTHREAD__H
#define CLIENT_LIGHT_RGB_PTHREAD__H

/* RGB小灯处理线程，其中自动亮灯是由驱动进行控制，后期完善RBGLED驱动，对中断进行控制 */


#define RGBLED_DRIVER "/dev/my_rgb_led"

#define CMD_R _IOW('M', 0, int)
#define CMD_G _IOW('M', 1, int)
#define CMD_B _IOW('M', 2, int)
#define CMD_W _IOW('M', 3, int)

#endif

