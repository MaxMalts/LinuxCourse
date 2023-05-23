#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "phonebook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maltsev Maxim");
MODULE_DESCRIPTION("phonebook");


#define DEVICE "1c_phonebook"
#define OUTPUT_SIZE_MAX 1000

int init_module(void);
void exit_module(void);
static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

static char cur_output[OUTPUT_SIZE_MAX + 1]  = "output";

static int  major = 0;
static struct class* char_class = NULL;

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release
};


int init_module() {
    major = register_chrdev(0, DEVICE, &fops);
    if (major < 0) {
        return -1;
    }

    char_class = class_create(THIS_MODULE, "1c_phonebook");
    if (IS_ERR(char_class)) {
        unregister_chrdev(major, DEVICE);
        return -2;
    }

    struct device* char_device = device_create(char_class, NULL, MKDEV(major, 0), NULL, DEVICE);
    if (IS_ERR(char_device)) {
        class_destroy(char_class);
        unregister_chrdev(major, DEVICE);
        return -3;
    }
    
    printk(KERN_INFO "Phonebook device init\n");
    return 0;
}

void exit_module() {
    device_destroy(char_class, MKDEV(major, 0));
    class_unregister(char_class);
    unregister_chrdev(major, DEVICE);
    class_destroy(char_class);

    printk(KERN_INFO "Phonebook device exit\n");
}

static int device_open(struct inode* inode, struct file* filp) {
    return 0;
}

static int device_release(struct inode* inode, struct file* filp) {
    return 0;
}

static ssize_t device_read(struct file* filp, char* buffer, size_t length, loff_t* offset) {
	if (length == 0) {
		return 0;
    }
    if (length > OUTPUT_SIZE_MAX - *offset) {
		length = OUTPUT_SIZE_MAX - *offset;
    }

    size_t msg_len = strlen(cur_output);
    if (*offset >= msg_len) {
        return 0;
    } else if (msg_len < length) {
        length = msg_len;
    }

    if (copy_to_user(buffer, cur_output + *offset, length) != 0) {
        return -EFAULT;
    }

	*offset += length;
	return length;
}

void handle_input(void) {
    const char* get_command = "get";
    const char* create_command = "create";
    const char* delete_command = "delete";
    
    char command[10];
    sscanf(cur_output, "%s", command);
    
    if (strcmp(command, create_command) == 0) {
        user_data new_user;

        if (sscanf(cur_output + strlen(create_command), "%s %s %u %s", new_user.name, new_user.surname, &new_user.age, new_user.phone) != 4) {
            return;
        }
        
        add_phonebook_user(&new_user);

    } else if (strcmp(command, get_command) == 0) {
        char surname[sizeof(((user_data*) NULL)->surname)];
        if (sscanf(cur_output + strlen(get_command), "%s", surname) != 1) {
            return;
        }

        user_data cur_user;
        memset(&cur_user, 0, sizeof(user_data));
        if (get_phonebook_user(surname, 0, &cur_user) != 0) {
            strcpy(cur_output, "User not found\n");
            return;
        }

        sprintf(
            cur_output,
            "name: %s, surname: %s, age: %d, phone: %s\n", 
            cur_user.name,
            cur_user.surname,
            cur_user.age,
            cur_user.phone
        );

    } else if (strcmp(command, delete_command) == 0) {
        char surname[sizeof(((user_data*) NULL)->surname)];
        if (sscanf(cur_output + strlen(delete_command), "%s", surname) != 1) {
            return;
        }

        if (del_phonebook_user(surname, 0) != 0) {
            strcpy(cur_output, "User not found\n");
            return;
        }

    } else {
        printk(KERN_ALERT "bad command\n");
        strcpy(cur_output, "bad command\n");
    }
}

static ssize_t device_write(struct file* filp, const char* buffer, size_t length, loff_t* offset) {
    if (length == 0) {
		return 0;
    }
	if (length > OUTPUT_SIZE_MAX - *offset) {
		length = OUTPUT_SIZE_MAX - *offset;
    }

	if (copy_from_user(cur_output + *offset, buffer, length) != 0) {
		return -EFAULT;
    }
    memset(cur_output + *offset + length, '\0', OUTPUT_SIZE_MAX - *offset - length);

    handle_input();

	*offset += length;
	return length;
}

module_exit(exit_module);
