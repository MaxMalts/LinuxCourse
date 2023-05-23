#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <uapi/linux/string.h>
#include "phonebook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maltsev Maxim");
MODULE_DESCRIPTION("Phonebook");


struct list_head phonebook_head;

struct phonebook_node {
    struct list_head phonebook_head;
    user_data user;
};

static int __init phonebook_module_init(void) {
    INIT_LIST_HEAD(&phonebook_head);
    
    printk(KERN_INFO "Phonebook module init\n");
    return 0;
}

static void __exit phonebook_module_exit(void) {
    printk(KERN_INFO "Phonebook module exit\n");
}


long get_phonebook_user(const char* surname, unsigned int len, user_data* output_data) {
    struct list_head *head;
    struct phonebook_node *node;

    if (list_empty(&phonebook_head)) {
        return -1;
    }

    list_for_each(head, &phonebook_head) {
        node = list_entry(head, struct phonebook_node, phonebook_head);
        printk(node->user.name);
        printk(node->user.surname);
        printk(node->user.phone);
        
        if (strcmp(node->user.surname, surname) == 0) {
            memcpy(output_data, &node->user, sizeof(user_data));
            return 0;
        }
    }

    return -1;
}
EXPORT_SYMBOL(get_phonebook_user);


long add_phonebook_user(user_data* input_data) {
    struct phonebook_node *new_node = kmalloc(sizeof(struct phonebook_node), GFP_KERNEL);
    memcpy(&new_node->user, input_data, sizeof(user_data));
    list_add(&new_node->phonebook_head, &phonebook_head);

    return 0;
}
EXPORT_SYMBOL(add_phonebook_user);


long del_phonebook_user(const char* surname, unsigned int len) {
    struct list_head *head;
    struct phonebook_node *node;

    list_for_each(head, &phonebook_head) {
        node = list_entry(head, struct phonebook_node, phonebook_head);
        
        if (strcmp(node->user.surname, surname) == 0) {
            list_del(&node->phonebook_head);
            return 0;
        }
    }

    return -1;
}
EXPORT_SYMBOL(del_phonebook_user);


module_init(phonebook_module_init);
module_exit(phonebook_module_exit);
