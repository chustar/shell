#include <time.h>
#include <stdio.h>
#include <string.h>
#include "resource.h"

#define CP_USER   0
#define CP_NICE   1
#define CP_SYS    2
#define CP_INTR   3
#define CP_IDLE   4
#define CPUSTATES 5

static cpu_states_t cpu_now;
static int cpu_now_uninit=1;

cpu_states_t *get_cpu_totals(){

	FILE *f;
	long cp_time[CPUSTATES];
	size_t size;

	cpu_now.user=0;
	/* Not stored in linux or freebsd */
	cpu_now.iowait=0;
	cpu_now.kernel=0;
	cpu_now.idle=0;
	/* Not stored in linux or freebsd */
	cpu_now.swap=0;
	cpu_now.total=0;
	/* Not stored in solaris */
	cpu_now.nice=0;

	if ((f=fopen("/proc/stat", "r" ))==NULL) {
		return NULL;
	}
	/* The very first line should be cpu */
	if((fscanf(f, "cpu %lld %lld %lld %lld", \
		&cpu_now.user, \
		&cpu_now.nice, \
		&cpu_now.kernel, \
		&cpu_now.idle)) != 4){
		fclose(f);
		return NULL;
	}

	fclose(f);

	cpu_now.total=cpu_now.user+cpu_now.nice+cpu_now.kernel+cpu_now.idle;
	cpu_now.user=cp_time[CP_USER];
	cpu_now.nice=cp_time[CP_NICE];
	cpu_now.kernel=cp_time[CP_SYS];
	cpu_now.idle=cp_time[CP_IDLE];

	cpu_now.total=cpu_now.user+cpu_now.nice+cpu_now.kernel+cpu_now.idle;
	cpu_now.systime=time(NULL);
	cpu_now_uninit=0;


	return &cpu_now;
}

cpu_states_t *get_cpu_diff(){
	static cpu_states_t cpu_diff;
	cpu_states_t cpu_then, *cpu_tmp;

	if (cpu_now_uninit){
		if((cpu_tmp=get_cpu_totals())==NULL){
		/* Should get_cpu_totals fail */
			return NULL;
		}
		return cpu_tmp;
	}

    cpu_then.user=cpu_now.user;
    cpu_then.kernel=cpu_now.kernel;
    cpu_then.idle=cpu_now.idle;
    cpu_then.iowait=cpu_now.iowait;
    cpu_then.swap=cpu_now.swap;
    cpu_then.nice=cpu_now.nice;
    cpu_then.total=cpu_now.total;
    cpu_then.systime=cpu_now.systime;

	if((cpu_tmp=get_cpu_totals())==NULL){
		return NULL;
	}

	cpu_diff.user = cpu_now.user - cpu_then.user;
	cpu_diff.kernel = cpu_now.kernel - cpu_then.kernel;
	cpu_diff.idle = cpu_now.idle - cpu_then.idle;
	cpu_diff.iowait = cpu_now.iowait - cpu_then.iowait;
	cpu_diff.swap = cpu_now.swap - cpu_then.swap;
	cpu_diff.nice = cpu_now.nice - cpu_then.nice;
	cpu_diff.total = cpu_now.total - cpu_then.total;
	cpu_diff.systime = cpu_now.systime - cpu_then.systime;

	return &cpu_diff;
}

cpu_percent_t *cpu_percent_usage(){
	static cpu_percent_t cpu_usage;
	cpu_states_t *cs_ptr;

	cs_ptr=get_cpu_diff();
	if(cs_ptr==NULL){
		return NULL;
	}

	cpu_usage.user =  ((float)cs_ptr->user / (float)cs_ptr->total)*100;
	cpu_usage.kernel =  ((float)cs_ptr->kernel / (float)cs_ptr->total)*100;
	cpu_usage.idle = ((float)cs_ptr->idle / (float)cs_ptr->total)*100;
	cpu_usage.iowait = ((float)cs_ptr->iowait / (float)cs_ptr->total)*100;
	cpu_usage.swap = ((float)cs_ptr->swap / (float)cs_ptr->total)*100;
	cpu_usage.nice = ((float)cs_ptr->nice / (float)cs_ptr->total)*100;
	cpu_usage.time_taken = cs_ptr->systime;

	return &cpu_usage;
}

mem_stat_t *get_memory_stats(){
	static mem_stat_t mem_stat;
	char *line_ptr;
	unsigned long long value;
	FILE *f;
	if ((f = fopen("/proc/meminfo", "r")) == NULL) {
		return NULL;
	}

	while ((line_ptr = f_read_line(f, "")) != NULL) {
		if (sscanf(line_ptr, "%*s %llu kB", &value) != 1) {
			continue;
		}
		value *= 1024;

		if (strncmp(line_ptr, "MemTotal:", 9) == 0) {
			mem_stat.total = value;
		} else if (strncmp(line_ptr, "MemFree:", 8) == 0) {
			mem_stat.free = value;
		} else if (strncmp(line_ptr, "Cached:", 7) == 0) {
			mem_stat.cache = value;
		}
	}

	fclose(f);
	mem_stat.used = mem_stat.total - mem_stat.free;
	mem_stat.free = mem_stat.free;
	mem_stat.used = mem_stat.total - mem_stat.free;

	return &mem_stat;
}

char *f_read_line(FILE *f, const char *string){
    /* Max line length. 8k should be more than enough */
    static char line[8192];

    while((fgets(line, sizeof(line), f))!=NULL){
        if(strncmp(string, line, strlen(string))==0){
            return line;
        }
    }
    return NULL;
}

