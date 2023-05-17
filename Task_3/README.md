Добавляем новое поле unsigned long scheduled в task_struct.

Создаем файл /proc/<xxx>/sheduled_counter, в который добавляем это поле.

Счетчик у нас увеличивается в планировщике во время добавления процесса очередь

linux-6.1.9/fs/proc/base.c:

```
static int proc_sheduled_counter(struct seq_file *m, struct pid_namespace *ns,
				struct pid *pid, struct task_struct *task)
{
    int err = lock_trace(task);
	if (!err) {
		seq_printf(m, "Processes scheduled: %ld\n", task->scheduled);
		unlock_trace(task);
	}
	return err;
}
```