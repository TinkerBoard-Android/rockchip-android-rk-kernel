/* NOTICE:
 * This file is for asus project id pins,
 */

#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/rockchip/grf.h>

#define RK3288_GRF_PHYS                 0xFF770000

/* Project id (GPIO2_A3, GPIO2_A2, GPIO2_A1)
 * Ram id (GPIO2_B6, GPIO2_B5, GPIO2_B4)
 * PCB id (GPIO2_B2, GPIO2_B1 ,GPIO2_B0)
 */
#define GPIO_offest 0
#define GPIO2_A1 (57+GPIO_offest)
#define GPIO2_A2 (58+GPIO_offest)
#define GPIO2_A3 (59+GPIO_offest)
#define GPIO2_B0 (64+GPIO_offest)
#define GPIO2_B1 (65+GPIO_offest)
#define GPIO2_B2 (66+GPIO_offest)
#define GPIO2_B4 (68+GPIO_offest)
#define GPIO2_B5 (69+GPIO_offest)
#define GPIO2_B6 (70+GPIO_offest)

/* board hardware parameter*/
int project_id_0, project_id_1, project_id_2;
int ram_id_0, ram_id_1, ram_id_2;
int pcb_id_0, pcb_id_1, pcb_id_2;
int projectid, boardid;

void *regs;
int err;

char *board_type = "unknown";
char *ram_size = "unknown";
char *pcb = "unknown";

static struct proc_dir_entry *board_info_proc_file;
static struct proc_dir_entry *board_id_proc_file;
static struct proc_dir_entry *project_id_proc_file;
static struct proc_dir_entry *board_ver_proc_file;
static struct proc_dir_entry *board_ddr_proc_file;

static void read_project_id(void)
{
	err = gpio_is_valid(GPIO2_A1);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_A1 %d\n", __func__, GPIO2_A1);
		return ;
	}
	err = gpio_request(GPIO2_A1, "project_id_0");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_A1 %d\n", __func__, GPIO2_A1);
		return ;
	} else {
		gpio_direction_input(GPIO2_A1);
		project_id_0 = gpio_get_value(GPIO2_A1);
	}
	err = gpio_is_valid(GPIO2_A2);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_A2 %d\n", __func__, GPIO2_A2);
		return ;
	}
	err = gpio_request(GPIO2_A2, "project_id_1");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_A2 %d\n", __func__, GPIO2_A2);
		return ;
	} else {
		gpio_direction_input(GPIO2_A2);
		project_id_1 = gpio_get_value(GPIO2_A2);
	}
	err = gpio_is_valid(GPIO2_A3);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_A3 %d\n", __func__, GPIO2_A3);
		return ;
	}
	err = gpio_request(GPIO2_A3, "project_id_2");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_A3 %d\n", __func__, GPIO2_A3);
		return ;
	} else {
		gpio_direction_input(GPIO2_A3);
		project_id_2 = gpio_get_value(GPIO2_A3);
	}
	gpio_free(GPIO2_A1);
	gpio_free(GPIO2_A2);
	gpio_free(GPIO2_A3);

	printk("project_id_2:0x%x, project_id_1:0x%x, project_id_0:0x%x \n",
		project_id_2, project_id_1, project_id_0);

	projectid = (project_id_2 << 2) + (project_id_1 << 1) + project_id_0;
}

static void read_ram_id(void)
{
	err = gpio_is_valid(GPIO2_B4);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B4 %d\n", __func__, GPIO2_B4);
		return ;
	}
	err = gpio_request(GPIO2_B4, "ram_id_0");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B4 %d\n", __func__, GPIO2_B4);
		return ;
	} else {
		gpio_direction_input(GPIO2_B4);
		ram_id_0 = gpio_get_value(GPIO2_B4);
	}
	err = gpio_is_valid(GPIO2_B5);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B5 %d\n", __func__, GPIO2_B5);
		return ;
	}
	err = gpio_request(GPIO2_B5, "ram_id_1");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B5 %d\n", __func__, GPIO2_B5);
		return ;
	} else {
		gpio_direction_input(GPIO2_B5);
		ram_id_1 = gpio_get_value(GPIO2_B5);
	}
	err = gpio_is_valid(GPIO2_B6);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B6 %d\n", __func__, GPIO2_B6);
		return ;
	}
	err = gpio_request(GPIO2_B6, "ram_id_2");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B6 %d\n", __func__, GPIO2_B6);
		return ;
	} else {
		gpio_direction_input(GPIO2_B6);
		ram_id_2 = gpio_get_value(GPIO2_B6);
	}
	gpio_free(GPIO2_B4);
	gpio_free(GPIO2_B5);
	gpio_free(GPIO2_B6);

	printk("ram_id_2:0x%x, ram_id_1:0x%x, ram_id_0:0x%x \n",
		ram_id_2, ram_id_1, ram_id_0);

	if (ram_id_2 == 0 && ram_id_1 == 0 && ram_id_0 == 0)
		ram_size = "4GB";
	else if (ram_id_2 == 0 && ram_id_1 == 1 && ram_id_0 == 0)
		ram_size = "2GB";
	else if (ram_id_2 == 1 && ram_id_1 == 0 && ram_id_0 == 0)
		ram_size = "1GB";
	else
		ram_size = "unknown";
}

static void read_pcb_id(void)
{
	err = gpio_is_valid(GPIO2_B0);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B0 %d\n", __func__, GPIO2_B0);
		return ;
	}
	err = gpio_request(GPIO2_B0, "pcb_id_0");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B0 %d\n", __func__, GPIO2_B0);
		return ;
	} else {
		gpio_direction_input(GPIO2_B0);
		pcb_id_0 = gpio_get_value(GPIO2_B0);
	}
	err = gpio_is_valid(GPIO2_B1);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B1 %d\n", __func__, GPIO2_B1);
		return ;
	}
	err = gpio_request(GPIO2_B1, "pcb_id_1");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B1 %d\n", __func__, GPIO2_B1);
		return ;
	} else {
		gpio_direction_input(GPIO2_B1);
		pcb_id_1 = gpio_get_value(GPIO2_B1);
	}
	err = gpio_is_valid(GPIO2_B2);
	if (err < 0) {
		printk("%s: gpio is invalid for GPIO2_B2 %d\n", __func__, GPIO2_B2);
		return ;
	}
	err = gpio_request(GPIO2_B2, "pcb_id_2");
	if (err < 0) {
		printk("%s: gpio_request failed for GPIO2_B2 %d\n", __func__, GPIO2_B2);
		return ;
	} else {
		gpio_direction_input(GPIO2_B2);
		pcb_id_2 = gpio_get_value(GPIO2_B2);
	}
	gpio_free(GPIO2_B0);
	gpio_free(GPIO2_B1);
	gpio_free(GPIO2_B2);

	printk("pcb_id_2:0x%x, pcb_id_1:0x%x, pcb_id_0:0x%x \n",
		pcb_id_2, pcb_id_1, pcb_id_0);

	boardid = (pcb_id_2 << 2) + (pcb_id_1 << 1) + pcb_id_0;
}

static void set_info_ver(void)
{
	if (boardid == 0) {
		if (projectid == 7)
			pcb = "1.0";
		else
			pcb = "0.99";
	} else if (boardid == 1) {
		if (projectid == 7)
			pcb = "1.1";
		else
			pcb = "1.00";
	} else if (boardid == 2) {
		if (projectid == 7)
			pcb = "1.2";
		else
			pcb = "1.01";
	} else if (boardid == 3)
		pcb = "2.00";
	else if (boardid == 4)
		pcb = "2.04A";
	else if (boardid == 5)
		pcb = "2.04B";
	else
		pcb = "unknown";

	if (projectid == 0) {
		if (boardid < 3)
			board_type = "Tinker Board S, Tinker S/BR";
		else
			board_type = "Tinker Board S R2.0";
	} else if (projectid == 1)
		board_type = "Tinker Board S/HV";
	else if (projectid == 2)
		board_type = "Tinker Board S R1.5, Tinker S R1.5";
	else if (projectid == 3)
		board_type = "Tinker Board (S) R2.0 SR";
	else if (projectid == 4)
		board_type = "Tinker R/BR";
	else if (projectid == 5)
		board_type = "Tinker R/BR";
	else if (projectid == 7) {
		if (boardid < 3)
			board_type = "Tinker Board";
		else
			board_type = "Tinker Board R2.0";
	} else
		board_type = "unknown";
}

static int board_info_proc_read(struct seq_file *buf, void *v)
{
	/* Board info display */
	seq_printf(buf, "%s\n", board_type);
	//printk("[board_info] %s board_type=\'%s\' ram_size=\'%s' pcb=\'%s\'\n",
	//	__func__, board_type, ram_size, pcb);
	return 0;
}

static int board_id_proc_read(struct seq_file *buf, void *v)
{
	seq_printf(buf, "%d\n", boardid);
	return 0;
}

static int project_id_proc_read(struct seq_file *buf, void *v)
{
	seq_printf(buf, "%d\n", projectid);
	return 0;
}

static int board_ver_proc_read(struct seq_file *buf, void *v)
{
	seq_printf(buf, "%s\n", pcb);
	return 0;
}

static int board_ddr_proc_read(struct seq_file *buf, void *v)
{
	seq_printf(buf, "%s\n", ram_size);
	return 0;
}

static int board_info_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, board_info_proc_read, NULL);
}

static int board_id_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, board_id_proc_read, NULL);
}

static int project_id_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, project_id_proc_read, NULL);
}

static int board_ver_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, board_ver_proc_read, NULL);
}

static int board_ddr_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, board_ddr_proc_read, NULL);
}

static struct file_operations board_info_proc_ops = {
	.open = board_info_proc_open,
	.read = seq_read,
	.release = single_release,
};

static struct file_operations board_id_proc_ops = {
	.open = board_id_proc_open,
	.read = seq_read,
	.release = single_release,
};

static struct file_operations project_id_proc_ops = {
	.open = project_id_proc_open,
	.read = seq_read,
	.release = single_release,
};

static struct file_operations board_ver_proc_ops = {
	.open = board_ver_proc_open,
	.read = seq_read,
	.release = single_release,
};

static struct file_operations board_ddr_proc_ops = {
	.open = board_ddr_proc_open,
	.read = seq_read,
	.release = single_release,
};

static void create_project_id_proc_file(void)
{
	board_info_proc_file = proc_create("boardinfo", 0444, NULL,
						&board_info_proc_ops);
	if (board_info_proc_file) {
		printk("[board_info] create Board_info_proc_file sucessed!\n");
	} else {
		printk("[board_info] create Board_info_proc_file failed!\n");
	}

	board_id_proc_file = proc_create("boardid", 0444, NULL,
						&board_id_proc_ops);
	if (board_id_proc_file) {
		printk("[board_info] create BoardID_proc_file sucessed!\n");
	} else {
		printk("[board_info] create BoardID_proc_file failed!\n");
	}

	project_id_proc_file = proc_create("projectid", 0444, NULL,
						&project_id_proc_ops);
	if (project_id_proc_file) {
		printk("[board_info] create ProjectID_proc_file sucessed!\n");
	} else {
		printk("[board_info] create ProjectID_proc_file failed!\n");
	}

	board_ver_proc_file = proc_create("boardver", 0444, NULL,
						&board_ver_proc_ops);
	if (board_ver_proc_file) {
		printk("[board_info] create Board_ver_proc_file sucessed!\n");
	} else {
		printk("[board_info] create Board_ver_proc_file failed!\n");
	}

	board_ddr_proc_file = proc_create("ddr", 0444, NULL,
						&board_ddr_proc_ops);
	if (board_ddr_proc_file) {
		printk("[board_info] create Board_ddr_proc_file sucessed!\n");
	} else {
		printk("[board_info] create Board_ddr_proc_file failed!\n");
	}

	/* Pull up GPIO2 A1 A2 A3*/
	regs = ioremap(RK3288_GRF_PHYS, 64*1024);
	if (regs == NULL) {
		printk("[board_info] ioremap failed");
		return ;
	}
	writel((readl(regs + RK3288_GRF_GPIO2A_P) & ~(0x3f << 18) & ~(0x3f << 2))
		| (0x3f << 18) | (0x15 << 2), regs + RK3288_GRF_GPIO2A_P);
	// printk("[board_info] %x\n", readl(regs + RK3288_GRF_GPIO2A_P));

	iounmap(regs);

	/* Read GPIO */
	read_project_id();
	read_ram_id();
	read_pcb_id();

	/* Set boardinfo and boardver */
	set_info_ver();
}

int get_board_id(void)
{
	return boardid;
}
EXPORT_SYMBOL_GPL(get_board_id);

int get_project_id(void)
{
	return projectid;
}
EXPORT_SYMBOL_GPL(get_project_id);

static int __init proc_asusPRJ_init(void)
{
	create_project_id_proc_file();
	return 0;
}

module_init(proc_asusPRJ_init);
