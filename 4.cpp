#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <setjmp.h>
#include <stdio.h>
#include<string.h>
#include <stdbool.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <time.h>
using namespace std;
struct task {
        double t;//time
        double tmp;
        int temp;
        char local_filename[256];
        char tmp_filename[256];
        int id;
        FILE *fp;
        long int all_time; 
        jmp_buf env;
        double *arr;
        int j;
        int k;
        int count;
        bool is_finished;
	char local_data[128];
    char str[100];
};

static int current_task_i = 0;
static int task_count;

//int T;
#define check_resched {						\
	/* For an example: just resched after each line. */	\
	int old_i = current_task_i;				\
	current_task_i = (current_task_i + 1) % task_count;	\
	if (setjmp(tasks[old_i].env) == 0)			\
		longjmp(tasks[current_task_i].env, 1); }

int merge_two_files(char *resfilelast,char *newfile,char *resfilenew);
		
static void my_coroutine(task *tasks)
{
printf("1#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);check_resched;

    tasks[current_task_i].t=clock();check_resched;
    sprintf(tasks[current_task_i].tmp_filename, "tmp%d.tmp",current_task_i);check_resched;
   // sprintf(tasks[current_task_i].local_data, "echo | cat %s | xargs -n1 | sort -g | xargs > %s\n",tasks[current_task_i].local_filename,tasks[current_task_i].tmp_filename);check_resched;
   // system(tasks[current_task_i].local_data);check_resched;
    //schitaem skolko chisel v faile
    tasks[current_task_i].fp = fopen(tasks[current_task_i].local_filename, "r");
    check_resched;
    if(tasks[current_task_i].fp==NULL)
    {
//printf("CANT OPEN FILE#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);check_resched;        
    }
    tasks[current_task_i].count=0;check_resched;
    while (fscanf(tasks[current_task_i].fp, "%lf", &tasks[current_task_i].tmp) != EOF) 
    {
        tasks[current_task_i].count+=1;
        check_resched;
    }
//printf("2#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);check_resched;

    check_resched;
    fclose(tasks[current_task_i].fp);check_resched;
    tasks[current_task_i].arr=(double *)malloc(sizeof(double)*tasks[current_task_i].count);
    check_resched;
    tasks[current_task_i].fp = fopen(tasks[current_task_i].local_filename, "r");
    check_resched;
    tasks[current_task_i].j=0;check_resched;
    while (fscanf(tasks[current_task_i].fp, "%lf", &tasks[current_task_i].arr[tasks[current_task_i].j]) != EOF) 
    {
        check_resched;tasks[current_task_i].j++;check_resched;
    }
//printf("3#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);check_resched;
    
    //Try to realize Bubble sort
    for (tasks[current_task_i].j = 0;
         tasks[current_task_i].j < tasks[current_task_i].count; tasks[current_task_i].j++) {
        for (tasks[current_task_i].k = 0; tasks[current_task_i].k < tasks[current_task_i].count-tasks[current_task_i].j-1 
                                                                    ; tasks[current_task_i].k++) 
        {
            check_resched;
            if (tasks[current_task_i].arr[tasks[current_task_i].k] >
                tasks[current_task_i].arr[tasks[current_task_i].k+1]) {
                check_resched;
                tasks[current_task_i].tmp = tasks[current_task_i].arr[tasks[current_task_i].k];
                check_resched;
                tasks[current_task_i].arr[tasks[current_task_i].k] = tasks[current_task_i].arr[tasks[current_task_i].k+1];
                check_resched;
                tasks[current_task_i].arr[tasks[current_task_i].k+1] = tasks[current_task_i].tmp;
                check_resched;
            }
        }
    }


    check_resched;
    fclose(tasks[current_task_i].fp);
    check_resched;
    char str[12];

    tasks[current_task_i].fp = fopen(tasks[current_task_i].local_filename, "w"); // write mode;
    check_resched;
    for (tasks[current_task_i].j = 0;
         tasks[current_task_i].j < tasks[current_task_i].count; tasks[current_task_i].j++) {
        check_resched;
        fprintf(tasks[current_task_i].fp, "%lf ", tasks[current_task_i].arr[tasks[current_task_i].j]);
        check_resched;
    }


    fclose(tasks[current_task_i].fp);
    check_resched;
    printf("#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);check_resched;
    tasks[current_task_i].is_finished = true;

    while (true) {
        bool is_all_finished = true;
        for (int i = 0; i < task_count; ++i) {
            if (!tasks[i].is_finished) {
                is_all_finished = false;
                break;
            }
        }
        if (is_all_finished) {
            return;
        }
        check_resched;
    }
}
   
int main(int argc, char **argv)
{
	if (argc <= 1)
	{
	    printf("No args.\n");
	    return 0;
	}
        double t;
        int i,flag=1;
        char tmp_buf[256],*char_tmp;
        char *resfilelast,*resfilenew;
        FILE *f0;
        if(!(resfilelast=(char*)malloc(500*sizeof(char))))  
        {
            printf("Not enough memory!\n");
            return -100;
        }
        resfilenew=resfilelast+250;
        strcpy(resfilelast,"res1.tmp");//strcpy
        strcpy(resfilenew,"res2.tmp");
       	task_count = argc - 1;
        cout<<"task_count="<<task_count <<endl;
        struct task tasks[task_count];
        for (i = 0; i < task_count; ++i) {
                tasks[i].id = i;
                tasks[i].is_finished = false;
                strcpy(tasks[i].local_filename, argv[i + 1]);//strcpy
                setjmp(tasks[i].env);
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
            merge_two_files(resfilelast,tasks[i].local_filename,resfilenew);
            char_tmp=resfilelast;
            resfilelast=resfilenew;
            resfilenew=char_tmp;
        }
        sprintf(tmp_buf,"cat %s > result.txt",resfilelast);
        system(tmp_buf);
        system("rm -f *.tmp");
        printf("Finished. Result in \'result.txt\'\n");
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
    double x,y;
    if(!(fscanf(fr1,"%lf",&x)==1))
    {
        if(!feof(fr1))
        {    
            printf("Error10\n");

            return -1;
        }
        for(;;)
        {
            if(!(fscanf(fr2,"%lf",&y)==1))
            {
                if(!feof(fr2))
                {    
                    printf("Error110\n");
     
                    return -1;
                }
                break;
            }
            fprintf(fw,"%lf\n",y);
        }
        fclose(fr1);
        fclose(fr2);
        fclose(fw);
        return 0;
    }
    if(!(fscanf(fr2,"%lf",&y)==1))
    {
        if(!feof(fr2))
        {    
            printf("Error111\n");

            return -1;
        }
        for(;;)
        {
            if(!(fscanf(fr1,"%lf",&x)==1))
            {
                if(!feof(fr1))
                {    
                    printf("Error112\n");
                    return -1;
                }
                break;
            }
            fprintf(fw,"%lf\n",x);
        }
        fclose(fr1);
        fclose(fr2);
        fclose(fw);
        return 0;
    }
        for(;;)
    {
        if(x<y)
        {
            fprintf(fw,"%lf\n",x );
            if(!(fscanf(fr1,"%lf",&x)==1))
            {
                if(!feof(fr1))
                {    
                    printf("Error10\n");
                  
                    return -1;
                }
                for(;;)
                {
                    if(!(fscanf(fr2,"%lf",&y)==1))
                    {
                        if(!feof(fr2))
                        {      
                            printf("Error110\n");

                            return -1;
                        }
                        break;
                    }
                    fprintf(fw,"%lf\n",y);
                }
                fclose(fr1);
                fclose(fr2);
                fclose(fw);
                return 0;
            }
        }
        else
        {    
            fprintf(fw,"%lf\n",y);
            if(!(fscanf(fr2,"%lf",&y)==1))
            {
                if(!feof(fr2))
                {    
                    printf("Error111\n");
                    return -1;
                }
                for(;;)
                {
                    if(!(fscanf(fr1,"%lf",&x)==1))
                    {
                        if(!feof(fr1))
                        {    
                            printf("Error112\n");
                            return -1;
                        }
                        break;
                    }
                    fprintf(fw,"%lf\n",x);
                }
                fclose(fr1);
                fclose(fr2);
                fclose(fw);
                return 0;
            }
        }
    }
    fclose(fr1);
    fclose(fr2);
    fclose(fw);
    return 0;
}
