#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>
#include<iostream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
struct task {
        double t;
        char local_filename[256];
        char tmp_filename[256];
        int id;
        jmp_buf env;
        bool is_finished;
	char local_data[128];
};
static int current_task_i = 0;
static int task_count;
#define check_resched {/* For an example: just resched after each line. */	\
	int old_i = current_task_i;				\
	current_task_i = (current_task_i + 1) % task_count;	\
	if (setjmp(tasks[old_i].env) == 0)			\
		longjmp(tasks[current_task_i].env, 1);}

int merge_two_files(char *resfilelast,char *newfile,char *resfilenew);
static void
my_coroutine(task *tasks)
{
    check_resched;
    tasks[current_task_i].t=clock();
    check_resched;
    sprintf(tasks[current_task_i].tmp_filename, "tmp%d.tmp",current_task_i);
    check_resched;
    sprintf(tasks[current_task_i].local_data, "sort -g %s > %s",tasks[current_task_i].local_filename,tasks[current_task_i].tmp_filename);
    check_resched;
    system(tasks[current_task_i].local_data);
	check_resched;
    sprintf(tasks[current_task_i].local_data, "cat %s > %s",tasks[current_task_i].tmp_filename,tasks[current_task_i].local_filename);
    check_resched;
    system(tasks[current_task_i].local_data);
	check_resched;
    sprintf(tasks[current_task_i].local_data, "cat %s",tasks[current_task_i].local_filename);
    check_resched;
    system(tasks[current_task_i].local_data);
    check_resched;
    tasks[current_task_i].is_finished = true;
    check_resched;
    tasks[current_task_i].t=clock()-tasks[current_task_i].t;
    check_resched;
    printf("#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);
    check_resched;
    while (true) {
		bool is_all_finished = true;
		for (int i = 0; i < task_count; ++i) {
			if (! tasks[i].is_finished) {
				fprintf(stderr, "Task_id=%d still active, "\
					"re-scheduling\n", i);
				is_all_finished = false;
				break;
			}
		}
		if (is_all_finished) {
			fprintf(stderr, "No more active tasks to schedule.\n");
			return;
		}
		check_resched;
	}
}

int main(int argc, char **argv)
{
        double t;
        int i,flag=1;
        char tmp_buf[256],*char_tmp;
        char *resfilelast,*resfilenew;
        FILE *f0;
        if(!(resfilelast=(char*)malloc(500*sizeof(char))))  
        {
            printf("Not enough memory!\n");
            return -1009;
        }
        resfilenew=resfilelast+250;
        strcpy(resfilelast,"res1.tmp");
        strcpy(resfilenew,"res2.tmp");
        f0=fopen("1.txt","r");
        if(f0==NULL)
        {
            printf("Error\n");
            return -1;
        }
      
        if(!(fscanf(f0,"%d",&task_count))==1)
        {
            printf("Error3\n");
            return -2;
        }
        cout<<"task_count="<<task_count <<endl;
        struct task tasks[task_count];
        for (i = 0; i < task_count; ++i) {
                tasks[i].id = i;
                tasks[i].is_finished = false;
                if(!(fscanf(f0,"%s",&tasks[i].local_filename)==1))
                {
                    printf("Error2\n");
                    return -1;
                    
                }
                setjmp(tasks[i].env);
        }
        if(flag==1)
        {
            fclose(f0);
            flag=0;
        }
 
        t=clock();
        my_coroutine(tasks);
        t=clock()-t;
        printf("All time is %.2lf \n",t/CLOCKS_PER_SEC);
        system("rm -f *.tmp");
        sprintf(tmp_buf,"cat %s > %s",tasks[0].local_filename,resfilelast);
        system(tmp_buf);
        for(i=1;i<task_count;i++)
        {
            system(tmp_buf);
            merge_two_files(resfilelast,tasks[i].local_filename,resfilenew);
            char_tmp=resfilelast;
            resfilelast=resfilenew;
            resfilenew=char_tmp;
        }
        system(tmp_buf);
        system("rm -f *.tmp");
        printf("Finished\n");
        free(resfilelast);
        return 0;
}

int merge_two_files(char *resfilelast,char *newfile,char *resfilenew)
{     
    FILE *fr1=fopen(resfilelast,"r");
    if(fr1==NULL)
    {
        printf("Error5\n");
        return -1;
    }
    FILE *fr2=fopen(newfile,"r");
    if(fr2==NULL)
    {
        printf("Error6\n");
        fclose(fr1);
        return -1;
    }
    FILE *fw=fopen(resfilenew,"w");
    if(fw==NULL)
    {
        printf("Error7\n");
        fclose(fr1);
        fclose(fr2);
        return -1;
    }
    return 0;
}
