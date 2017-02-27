/*
 *
 * Elarix
 * src/kernel/dev/video/console.c
 *
 * Copyright (C) 2017 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#include <console.h>
#include <fs.h>
#include <ioctl.h>
#include <kernel.h>
#include <list.h>
#include <module.h>

static const char devname[] = "con";

static LIST_HEAD(consoles);

int con_reg(struct con_driver *driver)
{
	/* TODO Check if not already present */

	list_init(&driver->l);
	list_add(&consoles, &driver->l);

	return 0;
}

void con_unreg(struct con_driver *driver)
{
	/* TODO Check if present */

	list_rm(&driver->l);
}

int con_open(struct inode *ip, struct file *fp)
{

}

int con_close(struct inode *ip, struct file *fp)
{

}

int con_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	(void) off;

	/* TODO NO, don't write to every console, very bad, very very bad XXX */

	struct con_driver *driver;
	size_t i;

	for (i = 0; i < n; i++)
		list_for_each(driver, &consoles, l)
			driver->write(buf[i]);

#ifdef CONFIG_SERIAL /* TODO Write a serial console driver! */
	for (i = 0; i < n; i++) {
		if (buf[i] == '\n')
			serial_out(0x3F8, '\r');
		serial_out(0x3F8, buf[i]);
	}
	serial_out(0x3F8, 0x0D);
#endif
}

int con_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	/* TODO */
}

static struct file_ops con_file_ops = {
	.open		= &con_open,
	.close		= &con_close,
	.write		= &con_write,
	.ioctl		= &con_ioctl
};

/* XXX Register all console drivers FIRST */
int con_init(void)
{
	struct con_driver *driver;
	int res;

	if ((res = dev_reg(0, devname, &con_file_ops)) < 0)
		kprintf("%s: unable to register console (%d)", devname, res);

	list_for_each(driver, &consoles, l)
		driver->probe();

	return 0;
}

void con_exit(void)
{
	/* TODO */
}

MODULE(con, &con_init, &con_exit);