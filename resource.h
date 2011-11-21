#include <stdio.h>
#include <sys/types.h>

typedef struct{
        long long user;
        long long kernel;
        long long idle;
        long long iowait;
        long long swap;
        long long nice;
        long long total;
        time_t systime;
}cpu_states_t;

typedef struct{
        float user;
        float kernel;
        float idle;
        float iowait;
        float swap;
        float nice;
	time_t time_taken;
}cpu_percent_t;

typedef struct{
	long long total;
	long long free;
	long long used;
	long long cache;
}mem_stat_t;

cpu_states_t *get_cpu_totals();
cpu_states_t *get_cpu_diff();
cpu_percent_t *cpu_percent_usage();

mem_stat_t *get_memory_stats();
char *f_read_line(FILE *f, const char *string);
