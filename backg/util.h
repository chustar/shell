#ifndef _UTIL_H
#define _UTIL_H
#include "dstruct.h"


void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground); 


void launch_job(job *j, int foreground); 

job *find_job (pid_t pgid); 


int job_is_stopped(job *j);


int job_is_completed(job *j); 
 
void init_shell(); 

void set_first_job(job *job);

int mark_process_status(pid_t pid, int status);

void update_status(void);

void format_job_info(job *j, const char *stauts);

void do_job_notification(void);


void put_job_in_foreground(job *j, int cont);

void put_job_in_background(job *j, int cont);

void mark_job_as_running(job *j);

void continue_job(job *j, int foreground);


#endif
