Добавляем новое поле `unsigned long scheduled` в task_struct.

Создаем файл `/proc/<xxx>/sheduled_counter`, в который добавляем это поле.

Счетчик у нас увеличивается в планировщике во время добавления процесса очередь

Имеем следующие изменения:

```
diff -r linux-6.1.9/fs/proc/base.c linux-6.1.9_patch/fs/proc/base.c
3151a3152,3346
> static int proc_sheduled_counter(struct seq_file *m, struct pid_namespace *ns,
> 				struct pid *pid, struct task_struct *task)
> {
>     int err = lock_trace(task);
> 	if (!err) {
> 		seq_printf(m, "Processes scheduled: %ld\n", task->scheduled);
> 		unlock_trace(task);
> 	}
> 	return err;
> }
> 
3208a3220
> 	ONE("sheduled_counter",		S_IRUSR, proc_sheduled_counter),
3549a3562
> 	ONE("sheduled_counter",		S_IRUSR, proc_sheduled_counter),

diff -r linux-6.1.9/include/linux/sched.h linux-6.1.9_patch/include/linux/sched.h
1014a1015,1037
> 	unsigned long 			scheduled;

diff -r linux-6.1.9/kernel/fork.c linux-6.1.9_patch/kernel/fork.c
1491a1492
> 	tsk->scheduled = 0;

diff -r linux-6.1.9/kernel/sched/core.c linux-6.1.9_patch/kernel/sched/core.c
6261a6262
> 	prev->scheduled++;

```