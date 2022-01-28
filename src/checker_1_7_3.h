#define CHECKER_MACRO

/*----------------------------------------------------------------------------*/

static int get_count(int digit)
{
	int len = 1;

	while (digit /= 10)
		len++;

	return len;

}

/*----------------------------------------------------------------------------*/

int array_sum(short * arr, size_t n)
{
	int i;
	short sum = 0;

	for (i = 0; i < (int)n; i++) 
		sum += arr[i];
	
	return (int)sum;
}

/*----------------------------------------------------------------------------*/

ssize_t generate_output(int sum, short * arr, size_t size, char * buf)
{
	int i;
	int last_len;

	last_len = get_count(sum);
	sprintf(buf, "<%d>", sum);
	last_len += 2;

    for (i = 0; i < size; i++)
    {
		sprintf(buf+last_len, " <%d>", arr[i]);

		last_len += get_count(arr[i]);
		last_len += 3;
	}

	buf[last_len] = '\0';

	return strlen(buf);
}
