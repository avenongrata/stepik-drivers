ssize_t get_void_size(void)
{
	ssize_t len;
	return len;
}

void submit_void_ptr(void *p);

ssize_t get_int_array_size(void)
{
	ssize_t len;
	return len;
}

void submit_int_array_ptr(int *p);

void submit_struct_ptr(struct device *p);

void checker_kfree(void *p)
{
	kfree(p);
}
