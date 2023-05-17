#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maltsev Maxim");
MODULE_DESCRIPTION("Keyboard statistics");


int init_module(void);
void exit_module(void);
static irqreturn_t keyboard_interrupt(int, void*);
static void printer_handler(struct work_struct*);

static DECLARE_DELAYED_WORK(PrinterHandler, printer_handler);

const int shedule_time_seconds = 10;

static int irq_id, n_clicks = 0;
static bool stop = false;

static int keyboard_irq = 1;
module_param(keyboard_irq, int, S_IRUGO);


int init_module() {
    if (request_irq(keyboard_irq, keyboard_interrupt, IRQF_SHARED, "keyboard_interrupt", &irq_id) != 0) {
        return -1;
    }
    
    schedule_delayed_work(&PrinterHandler, shedule_time_seconds * HZ);

    printk(KERN_INFO "Keyboard stats module init\n");
    return 0;
}


void exit_module() {
    stop = true;
    synchronize_irq(keyboard_irq);
    cancel_delayed_work_sync(&PrinterHandler);
    free_irq(keyboard_irq, &irq_id);
    printk(KERN_INFO "Keyboard stats module exit\n");
}


static irqreturn_t keyboard_interrupt(int irq, void* dev_id) {
    n_clicks++;
    return IRQ_NONE;
}


void printer_handler(struct work_struct* not_used) {
    if (!stop) {
        schedule_delayed_work(&PrinterHandler, shedule_time_seconds * HZ);
    }
    
    printk(KERN_INFO "number of clicks: %d\n", n_clicks);
}

module_exit(exit_module);