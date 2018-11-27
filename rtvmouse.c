#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

// デバイスドライバの情報
MODULE_AUTHOR("AtsushiSaito");
MODULE_DESCRIPTION("RaspberryPi Mouse Simulator Device Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

/* --- Device Numbers --- */
#define NUM_DEV_LED 4
#define NUM_DEV_SWITCH 3
#define NUM_DEV_SENSOR 1
#define NUM_DEV_BUZZER 1
#define NUM_DEV_MOTORRAWR 1
#define NUM_DEV_MOTORRAWL 1
#define NUM_DEV_MOTOREN 1
#define NUM_DEV_MOTOR 1
#define NUM_DEV_CNTR 1
#define NUM_DEV_CNTL 1
#define NUM_DEV_CNT 1

/* --- Device Names --- */
#define DEVNAME_LED "rtled"
#define DEVNAME_SWITCH "rtswitch"
#define DEVNAME_BUZZER "rtbuzzer"
#define DEVNAME_MOTORRAWR "rtmotor_raw_r"
#define DEVNAME_MOTORRAWL "rtmotor_raw_l"
#define DEVNAME_MOTOREN "rtmotoren"
#define DEVNAME_MOTOR "rtmotor"
#define DEVNAME_SENSOR "rtlightsensor"
#define DEVNAME_CNTR "rtcounter_r"
#define DEVNAME_CNTL "rtcounter_l"
#define DEVNAME_CNT "rtcounter"

#define NUM_BUFFER 256

#define NUM_DEV_TOTAL                                                          \
	(NUM_DEV_LED + NUM_DEV_SWITCH + NUM_DEV_BUZZER + NUM_DEV_MOTORRAWR +   \
	 NUM_DEV_MOTORRAWL + NUM_DEV_MOTOREN + NUM_DEV_SENSOR +                \
	 NUM_DEV_MOTOR + NUM_DEV_CNTR + NUM_DEV_CNTL + NUM_DEV_CNT)

/* --- Device Major and Minor Numbers --- */
#define DEV_MAJOR 0
#define DEV_MINOR 0

static int _major_led = DEV_MAJOR;
static int _minor_led = DEV_MINOR;

static int _major_switch = DEV_MAJOR;
static int _minor_switch = DEV_MINOR;

static int _major_sensor = DEV_MAJOR;
static int _minor_sensor = DEV_MINOR;

static int _major_buzzer = DEV_MAJOR;
static int _minor_buzzer = DEV_MINOR;

static int _major_motorrawr = DEV_MAJOR;
static int _minor_motorrawr = DEV_MINOR;

static int _major_motorrawl = DEV_MAJOR;
static int _minor_motorrawl = DEV_MINOR;

static int _major_motoren = DEV_MAJOR;
static int _minor_motoren = DEV_MINOR;

static int _major_motor = DEV_MAJOR;
static int _minor_motor = DEV_MINOR;

static int _major_cntr = DEV_MAJOR;
static int _minor_cntr = DEV_MINOR;

static int _major_cntl = DEV_MAJOR;
static int _minor_cntl = DEV_MINOR;

static int _major_cnt = DEV_MAJOR;
static int _minor_cnt = DEV_MINOR;

/* --- Device Class --- */
static struct cdev *cdev_array = NULL;
static struct class *class_led = NULL;
static struct class *class_buzzer = NULL;
static struct class *class_switch = NULL;
static struct class *class_sensor = NULL;
static struct class *class_motorrawr = NULL;
static struct class *class_motorrawl = NULL;
static struct class *class_motoren = NULL;
static struct class *class_motor = NULL;
static struct class *class_cntr = NULL;
static struct class *class_cntl = NULL;
static struct class *class_cnt = NULL;

/* --- 変数 --- */
static int cdev_index = 0;

struct Device_Data {
	int minor;
	int major;
	int flag;
};

static char value_led[NUM_DEV_LED][NUM_BUFFER];
static char value_switch[NUM_DEV_SWITCH][NUM_BUFFER];
static char value_buzzer[NUM_DEV_BUZZER][NUM_BUFFER];
static char value_motor_raw_l[NUM_DEV_MOTORRAWL][NUM_BUFFER];
static char value_motor_raw_r[NUM_DEV_MOTORRAWR][NUM_BUFFER];
static char value_motoren[NUM_DEV_MOTOREN][NUM_BUFFER];
static char value_motor[NUM_DEV_MOTOR][NUM_BUFFER];
static char value_sensor[NUM_DEV_SENSOR][NUM_BUFFER];
static char value_cnt[NUM_DEV_CNT][NUM_BUFFER];
static char value_cntl[NUM_DEV_CNTL][NUM_BUFFER];
static char value_cntr[NUM_DEV_CNTR][NUM_BUFFER];

/* --- Open Function --- */
static int device_open(struct inode *inode, struct file *file){
	struct Device_Data *data = kmalloc(sizeof(struct Device_Data), GFP_KERNEL);

	data->minor = iminor(inode);
	data->major = imajor(inode);
	data->flag = 1;
	file->private_data = data;

	return 0;
}

/* --- Write Function --- */
static ssize_t device_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
	struct Device_Data *data = file->private_data;

	if(data->flag == 1){
		/* ---- LED ---- */
		if(_major_led == data->major){
			memset(value_led[data->minor], '\0', sizeof(value_led[data->minor]));
			if (copy_from_user(value_led[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- Switch ---- */
		if(_major_switch == data->major){
			memset(value_switch[data->minor], '\0', sizeof(value_switch[data->minor]));
			if (copy_from_user(value_switch[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- Buzzer ---- */
		if(_major_buzzer == data->major){
			memset(value_buzzer[data->minor], '\0', sizeof(value_buzzer[data->minor]));
			if(copy_from_user(value_buzzer[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- MOTORRAWR ---- */
		if(_major_motorrawr == data->major){
			memset(value_motor_raw_r[data->minor], '\0', sizeof(value_motor_raw_r[data->minor]));
			if(copy_from_user(value_motor_raw_r[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- MOTORRAWL ---- */
		if(_major_motorrawl == data->major){
			memset(value_motor_raw_l[data->minor], '\0', sizeof(value_motor_raw_l[data->minor]));
			if(copy_from_user(value_motor_raw_l[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- MOTOREN ---- */
		if(_major_motoren == data->major){
			memset(value_motoren[data->minor], '\0', sizeof(value_motoren[data->minor]));
			if(copy_from_user(value_motoren[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- MOTOR ---- */
		if(_major_motor == data->major){
			memset(value_motor[data->minor], '\0', sizeof(value_motor[data->minor]));
			if(copy_from_user(value_motor[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- SENSOR ---- */
		if(_major_sensor == data->major){
			memset(value_sensor[data->minor], '\0', sizeof(value_sensor[data->minor]));
			if(copy_from_user(value_sensor[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- CNTR ---- */
		if(_major_cntr == data->major){
			memset(value_cntr[data->minor], '\0', sizeof(value_cntr[data->minor]));
			if(copy_from_user(value_cntr[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- CNTL ---- */
		if(_major_cntl == data->major){
			memset(value_cntl[data->minor], '\0', sizeof(value_cntl[data->minor]));
			if(copy_from_user(value_cntl[data->minor], buf, count) != 0) return -EFAULT;
		}

		/* ---- CNT ---- */
		if(_major_cnt == data->major){
			memset(value_cnt[data->minor], '\0', sizeof(value_cnt[data->minor]));
			if(copy_from_user(value_cnt[data->minor], buf, count) != 0) return -EFAULT;
		}
		data->flag = 0;
		return count;
	}else{
		return 0;
	}

}

/* --- Read Function --- */
static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos){
	int size = 0;
	struct Device_Data *data = file->private_data;
	if(data->flag == 1){

		/* ---- LED ---- */
		if(_major_led == data->major){
			size = sizeof(value_led[data->minor]);
			if(copy_to_user(buf, value_led[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- Switch ---- */
		if(_major_switch == data->major){
			size = sizeof(value_switch[data->minor]);
			if(copy_to_user(buf, value_switch[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- Buzzer ---- */
		if(_major_buzzer == data->major){
			size = sizeof(value_buzzer[data->minor]);
			if(copy_to_user(buf, value_buzzer[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- MOTORRAWR ---- */
		if(_major_motorrawr == data->major){
			size = sizeof(value_motor_raw_r[data->minor]);
			if(copy_to_user(buf, value_motor_raw_r[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- MOTORRAWL ---- */
		if(_major_motorrawl == data->major){
			size = sizeof(value_motor_raw_l[data->minor]);
			if(copy_to_user(buf, value_motor_raw_l[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- SENSOR ---- */
		if(_major_sensor == data->major){
			size = sizeof(value_sensor[data->minor]);
			if(copy_to_user(buf, value_sensor[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- MOTOREN ---- */
		if(_major_motoren == data->major){
			size = sizeof(value_motoren[data->minor]);
			if(copy_to_user(buf, value_motoren[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- MOTOR ---- */
		if(_major_motor == data->major){
			size = sizeof(value_motor[data->minor]);
			if(copy_to_user(buf, value_motor[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- CNTR ---- */
		if(_major_cntr == data->major){
			size = sizeof(value_cntr[data->minor]);
			if(copy_to_user(buf, value_cntr[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- CNTL ---- */
		if(_major_cntl == data->major){
			size = sizeof(value_cntl[data->minor]);
			if(copy_to_user(buf, value_cntl[data->minor], size) != 0) return -EFAULT;
		}

		/* ---- CNT ---- */
		if(_major_cnt == data->major){
			size = sizeof(value_cnt[data->minor]);
			if(copy_to_user(buf, value_cnt[data->minor], size) != 0) return -EFAULT;
		}
		data->flag = 0;
    return size;
	}else{
		return 0;
	}
}

/* --- Operation List --- */
static struct file_operations device_fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.write = device_write,
	.read = device_read,
};

/* --- Led Register --- */
static int led_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_LED, DEVNAME_LED); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_led = MAJOR(dev);

	class_led = class_create(THIS_MODULE, DEVNAME_LED);
	if(IS_ERR(class_led)) return PTR_ERR(class_led);

	for(i=0;i<NUM_DEV_LED;i++){
		devno = MKDEV(_major_led, _minor_led + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_led + i);
		}else{
			device_create(class_led, NULL, devno, NULL, DEVNAME_LED "%u", _minor_led + i);
		}
		cdev_index++;
	}
	return 0;
}

static int buzzer_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_BUZZER, DEVNAME_BUZZER); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_buzzer = MAJOR(dev);

	class_buzzer = class_create(THIS_MODULE, DEVNAME_BUZZER);
	if(IS_ERR(class_buzzer)) return PTR_ERR(class_buzzer);

	for(i=0;i<NUM_DEV_BUZZER;i++){
		devno = MKDEV(_major_buzzer, _minor_buzzer + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_buzzer + i);
		}else{
			device_create(class_buzzer, NULL, devno, NULL, DEVNAME_BUZZER "%u", _minor_buzzer + i);
		}
		cdev_index++;
	}
	return 0;
}

static int switch_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_SWITCH, DEVNAME_SWITCH); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_switch = MAJOR(dev);

	class_switch = class_create(THIS_MODULE, DEVNAME_SWITCH);
	if(IS_ERR(class_switch)) return PTR_ERR(class_switch);

	for(i=0;i<NUM_DEV_SWITCH;i++){
		devno = MKDEV(_major_switch, _minor_switch + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_switch + i);
		}else{
			device_create(class_switch, NULL, devno, NULL, DEVNAME_SWITCH "%u", _minor_switch + i);
		}
		cdev_index++;
	}
	return 0;
}

static int sensor_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_SENSOR, DEVNAME_SENSOR); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_sensor = MAJOR(dev);

	class_sensor = class_create(THIS_MODULE, DEVNAME_SENSOR);
	if(IS_ERR(class_sensor)) return PTR_ERR(class_sensor);

	for(i=0;i<NUM_DEV_SENSOR;i++){
		devno = MKDEV(_major_sensor, _minor_sensor + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_sensor + i);
		}else{
			device_create(class_sensor, NULL, devno, NULL, DEVNAME_SENSOR "%u", _minor_sensor + i);
		}
		cdev_index++;
	}
	return 0;
}

static int motorrawr_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_MOTORRAWR, DEVNAME_MOTORRAWR); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_motorrawr = MAJOR(dev);

	class_motorrawr = class_create(THIS_MODULE, DEVNAME_MOTORRAWR);
	if(IS_ERR(class_motorrawr)) return PTR_ERR(class_motorrawr);

	for(i=0;i<NUM_DEV_MOTORRAWR;i++){
		devno = MKDEV(_major_motorrawr, _minor_motorrawr + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_motorrawr + i);
		}else{
			device_create(class_motorrawr, NULL, devno, NULL, DEVNAME_MOTORRAWR "%u", _minor_motorrawr + i);
		}
		cdev_index++;
	}
	return 0;
}

static int motorrawl_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_MOTORRAWL, DEVNAME_MOTORRAWL); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_motorrawl = MAJOR(dev);

	class_motorrawl = class_create(THIS_MODULE, DEVNAME_MOTORRAWL);
	if(IS_ERR(class_motorrawl)) return PTR_ERR(class_motorrawl);

	for(i=0;i<NUM_DEV_MOTORRAWL;i++){
		devno = MKDEV(_major_motorrawl, _minor_motorrawl + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_motorrawl + i);
		}else{
			device_create(class_motorrawl, NULL, devno, NULL, DEVNAME_MOTORRAWL "%u", _minor_motorrawr + i);
		}
		cdev_index++;
	}
	return 0;
}

static int motor_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_MOTOR, DEVNAME_MOTOR); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_motor = MAJOR(dev);

	class_motor = class_create(THIS_MODULE, DEVNAME_MOTOR);
	if(IS_ERR(class_motor)) return PTR_ERR(class_motor);

	for(i=0;i<NUM_DEV_MOTOR;i++){
		devno = MKDEV(_major_motor, _minor_motor + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_motor + i);
		}else{
			device_create(class_motor, NULL, devno, NULL, DEVNAME_MOTOR "%u", _minor_motor + i);
		}
		cdev_index++;
	}
	return 0;
}

static int motoren_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_MOTOREN, DEVNAME_MOTOREN); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_motoren = MAJOR(dev);

	class_motoren = class_create(THIS_MODULE, DEVNAME_MOTOREN);
	if(IS_ERR(class_motoren)) return PTR_ERR(class_motoren);

	for(i=0;i<NUM_DEV_MOTOREN;i++){
		devno = MKDEV(_major_motoren, _minor_motoren + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_motoren + i);
		}else{
			device_create(class_motoren, NULL, devno, NULL, DEVNAME_MOTOREN "%u", _minor_motoren + i);
		}
		cdev_index++;
	}
	return 0;
}

static int countr_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_CNTR, DEVNAME_CNTR); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_cntr = MAJOR(dev);

	class_cntr = class_create(THIS_MODULE, DEVNAME_CNTR);
	if(IS_ERR(class_cntr)) return PTR_ERR(class_cntr);

	for(i=0;i<NUM_DEV_CNTR;i++){
		devno = MKDEV(_major_cntr, _minor_cntr + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_cntr + i);
		}else{
			device_create(class_cntr, NULL, devno, NULL, DEVNAME_CNTR "%u", _minor_cntr + i);
		}
		cdev_index++;
	}
	return 0;
}

static int countl_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_CNTL, DEVNAME_CNTL); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_cntl = MAJOR(dev);

	class_cntl = class_create(THIS_MODULE, DEVNAME_CNTL);
	if(IS_ERR(class_cntl)) return PTR_ERR(class_cntl);

	for(i=0;i<NUM_DEV_CNTL;i++){
		devno = MKDEV(_major_cntl, _minor_cntl + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_cntl + i);
		}else{
			device_create(class_cntl, NULL, devno, NULL, DEVNAME_CNTL "%u", _minor_cntl + i);
		}
		cdev_index++;
	}
	return 0;
}

static int count_register_dev(void){
	int retval, i;
	dev_t dev, devno;
	retval = alloc_chrdev_region(&dev, DEV_MINOR, NUM_DEV_CNT, DEVNAME_CNT); /* マイナー番号をカーネルに登録 */

	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	_major_cnt = MAJOR(dev);

	class_cnt = class_create(THIS_MODULE, DEVNAME_CNT);
	if(IS_ERR(class_cnt)) return PTR_ERR(class_cnt);

	for(i=0;i<NUM_DEV_CNT;i++){
		devno = MKDEV(_major_cnt, _minor_cnt + i);
		cdev_init(&(cdev_array[cdev_index]), &device_fops);

		cdev_array[cdev_index].owner = THIS_MODULE;
		if(cdev_add(&(cdev_array[cdev_index]), devno, 1) < 0) {
			printk(KERN_ERR "cdev_add failed minor = %d\n", _minor_cnt + i);
		}else{
			device_create(class_cnt, NULL, devno, NULL, DEVNAME_CNT "%u", _minor_cnt + i);
		}
		cdev_index++;
	}
	return 0;
}

static int init_mod(void){
	int retval;
	size_t size;

	size = sizeof(struct cdev) * NUM_DEV_TOTAL;
	cdev_array = (struct cdev *)kmalloc(size, GFP_KERNEL);

	/* ---- LED Device ---- */
	retval = led_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " led driver register failed.\n");
		return retval;
	}

	/* ---- Buzzer Device ---- */
	retval = buzzer_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " buzzer driver register failed.\n");
		return retval;
	}

	/* ---- Switch Device ---- */
	retval = switch_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " switch driver register failed.\n");
		return retval;
	}

	/* ---- Sensor Device ---- */
	retval = sensor_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " sensor driver register failed.\n");
		return retval;
	}

	/* ---- MotorRawR Device ---- */
	retval = motorrawr_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " motorrawr driver register failed.\n");
		return retval;
	}

	/* ---- MotorRawL Device ---- */
	retval = motorrawl_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " motorrawl driver register failed.\n");
		return retval;
	}

	/* ---- Motor Device ---- */
	retval = motor_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " motor driver register failed.\n");
		return retval;
	}

	/* ---- Motoren Device ---- */
	retval = motoren_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " motoren driver register failed.\n");
		return retval;
	}

	/* ---- counterR Device ---- */
	retval = countr_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " counterR driver register failed.\n");
		return retval;
	}

	/* ---- counterL Device ---- */
	retval = countl_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " counterL driver register failed.\n");
		return retval;
	}

	/* ---- counter Device ---- */
	retval = count_register_dev();
	if(retval != 0){
		printk(KERN_ALERT " counter driver register failed.\n");
		return retval;
	}

	return 0;
}

static void cleanup_mod(void){
	int i;
	dev_t devno, devno_top;

	/* ---- キャラクタデバイスを削除 ---- */
	for (i = 0; i < cdev_index; i++) {
		cdev_del(&(cdev_array[i]));
	}

	/* ---- LED Device ---- */
	devno_top = MKDEV(_major_led, _minor_led);
	for (i = 0; i < NUM_DEV_LED; i++) {
		devno = MKDEV(_major_led, _minor_led + i);
		device_destroy(class_led, devno);
	}
	unregister_chrdev_region(devno_top, NUM_DEV_LED);

	/* ---- Buzzer Device ---- */
	devno_top = MKDEV(_major_switch, _minor_switch);
	for (i = 0; i < NUM_DEV_SWITCH; i++) {
		devno = MKDEV(_major_switch, _minor_switch + i);
		device_destroy(class_switch, devno);
	}
	unregister_chrdev_region(devno_top, NUM_DEV_SWITCH);

	/* ---- Switch Device ---- */
	devno = MKDEV(_major_buzzer, _minor_buzzer);
	device_destroy(class_buzzer, devno);
	unregister_chrdev_region(devno, NUM_DEV_BUZZER);

	/* ---- Sensor Device ---- */
	devno = MKDEV(_major_sensor, _minor_sensor);
	device_destroy(class_sensor, devno);
	unregister_chrdev_region(devno, NUM_DEV_SENSOR);

	/* ---- MotorRawR Device ---- */
	devno = MKDEV(_major_motorrawr, _minor_motorrawr);
	device_destroy(class_motorrawr, devno);
	unregister_chrdev_region(devno, NUM_DEV_MOTORRAWR);

	/* ---- MotorRawR Device ---- */
	devno = MKDEV(_major_motorrawl, _minor_motorrawl);
	device_destroy(class_motorrawl, devno);
	unregister_chrdev_region(devno, NUM_DEV_MOTORRAWL);

	/* ---- Motor Device ---- */
	devno = MKDEV(_major_motor, _minor_motor);
	device_destroy(class_motor, devno);
	unregister_chrdev_region(devno, NUM_DEV_MOTOR);

	/* ---- Motoren Device ---- */
	devno = MKDEV(_major_motoren, _minor_motoren);
	device_destroy(class_motoren, devno);
	unregister_chrdev_region(devno, NUM_DEV_MOTOREN);

	/* ---- CounterR Device ---- */
	devno = MKDEV(_major_cntr, _minor_cntr);
	device_destroy(class_cntr, devno);
	unregister_chrdev_region(devno, NUM_DEV_CNTR);

	/* ---- CounterL Device ---- */
	devno = MKDEV(_major_cntl, _minor_cntl);
	device_destroy(class_cntl, devno);
	unregister_chrdev_region(devno, NUM_DEV_CNTL);

	/* ---- Counter Device ---- */
	devno = MKDEV(_major_cnt, _minor_cnt);
	device_destroy(class_cnt, devno);
	unregister_chrdev_region(devno, NUM_DEV_CNT);

	/* ---- remove device node ---- */
	class_destroy(class_led);
	class_destroy(class_buzzer);
	class_destroy(class_switch);
	class_destroy(class_sensor);
	class_destroy(class_motorrawr);
	class_destroy(class_motorrawl);
	class_destroy(class_motor);
	class_destroy(class_motoren);
	class_destroy(class_cntr);
	class_destroy(class_cntl);
	class_destroy(class_cnt);

	printk("Unload Device Driver.\n");
}

module_init(init_mod);
module_exit(cleanup_mod);
