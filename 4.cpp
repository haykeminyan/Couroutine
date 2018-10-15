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

//#define task_count 4
//static struct task tasks[task_count];
static int current_task_i = 0;
static int task_count;

/**
 * Check not in a function, because setjmp result can not be used
 * after return.
 *
 * Another way how to do not put 'check_resched' after each line -
 * do "define ; check_resched;".
 */
#define check_resched {						\
	/* For an example: just resched after each line. */	\
	int old_i = current_task_i;				\
	current_task_i = (current_task_i + 1) % task_count;	\
	if (setjmp(tasks[old_i].env) == 0)			\
		longjmp(tasks[current_task_i].env, 1); }

int merge_two_files(char *resfilelast,char *newfile,char *resfilenew);//���������� ��������
		
static void
my_coroutine(task *tasks)
{
    //printf("%d hello! my filename=\'%s\'\n",current_task_i,tasks[current_task_i].local_filename);
        //sprintf(tasks[current_task_i].local_data, "Local data for task_id%d",
        	//current_task_i);
	//fprintf(stderr, "Before re-schedule: task_id = %d\n", current_task_i);
	//check_resched;
	//fprintf(stderr, "After first re-schedule, task_id = %d\n",current_task_i);
	check_resched;
    tasks[current_task_i].t=clock();
    sprintf(tasks[current_task_i].tmp_filename, "tmp%d.tmp",current_task_i);//�������� � ������ �������� ������ ����� �������� 
    sprintf(tasks[current_task_i].local_data, "sort -g %s > %s",tasks[current_task_i].local_filename,tasks[current_task_i].tmp_filename);
    system(tasks[current_task_i].local_data);
	sprintf(tasks[current_task_i].local_data, "cat %s > %s",tasks[current_task_i].tmp_filename,tasks[current_task_i].local_filename);
    system(tasks[current_task_i].local_data);
	sprintf(tasks[current_task_i].local_data, "cat %s",tasks[current_task_i].local_filename);
    system(tasks[current_task_i].local_data);
    //fprintf(stderr, "After second re-schedule, task_id = %d\n",current_task_i);
	tasks[current_task_i].is_finished = true;
    tasks[current_task_i].t=clock()-tasks[current_task_i].t;
    printf("#%d coroutine time is %.2lf\n",current_task_i,tasks[current_task_i].t/CLOCKS_PER_SEC);
	//fprintf(stderr, "This is local data: %s\n",tasks[current_task_i].local_data);
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

int
main(int argc, char **argv)
{
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
        //FILE fr[task_count];
        
       // task_count=4;
        struct task tasks[task_count];//courine massiv
        for (i = 0; i < task_count; ++i) {
                tasks[i].id = i;
                tasks[i].is_finished = false;
                if(!(fscanf(f0,"%s",&tasks[i].local_filename)==1))
                {
                    printf("Error2\n");
                    return -1;
                    
                }
                
                
		/* Entry point for new co-coutines. */
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
   //end of first part of task - all files are sorted
        
        sprintf(tmp_buf,"cat %s > %s",tasks[0].local_filename,resfilelast);
        system(tmp_buf);
   
        for(i=1;i<task_count;i++)
        {
            sprintf(tmp_buf,"cat %s",resfilelast);
            system(tmp_buf);
            printf("\nadding file \'%s\'\n",tasks[i].local_filename);
            merge_two_files(resfilelast,tasks[i].local_filename,resfilenew);
            char_tmp=resfilelast;
            resfilelast=resfilenew;
            resfilenew=char_tmp;
            printf("res1:\n");
            system("cat res1.tmp");
            printf("\n");
            printf("res2:\n");
            system("cat res2.tmp");
            printf("\n-----------\n");
        }
        sprintf(tmp_buf,"cat %s > result.txt",resfilelast);
        system(tmp_buf);

        system("rm -f *.tmp");
        printf("Finished\n");
        free(resfilelast);
        return 0;
}

int merge_two_files(char *resfilelast,char *newfile,char *resfilenew)
{     
    printf("\'%s\' \'%s\' \'%s\'\n",resfilelast,newfile,resfilenew);
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
    //merging
    double x,y;
    if(!(fscanf(fr1,"%lf",&x)==1))
    {
        printf("first x trouble!\n");
        if(!feof(fr1))
        {    
            printf("Error10\n");
            fclose(fr1);
            fclose(fr2);
            fclose(fw);
            return -1;
        }
        for(;;)
        {
            if(!(fscanf(fr2,"%lf",&y)==1))
            {
                if(!feof(fr2))
                {    
                    printf("Error110\n");
                   fclose(fr1);
                    fclose(fr2);
                    fclose(fw);
                    return -1;
                }
                printf("break1\n");
                break;
            }
            fprintf(fw,"%lf ",y);
        }
        fclose(fr1);
        fclose(fr2);
        fclose(fw);
        return 0;
    }
    if(!(fscanf(fr2,"%lf",&y)==1))
    {
        printf("first y trouble!\n");
        if(!feof(fr2))
        {    
            printf("Error111\n");
            fclose(fr1);
            fclose(fr2);
            fclose(fw);
            return -1;
        }
        for(;;)
        {
            if(!(fscanf(fr1,"%lf",&x)==1))
            {
                if(!feof(fr1))
                {    
                    printf("Error112\n");
                    fclose(fr1);
                    fclose(fr2);
                    fclose(fw);
                    return -1;
                }
                printf("break2\n");
                break;
            }
            fprintf(fw,"%lf ",x);
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
            printf("printing x: %lf\n",x);
            fprintf(fw,"%lf\n",x );
            if(!(fscanf(fr1,"%lf",&x)==1))
            {
                if(!feof(fr1))
                {    
                    printf("Error10\n");
                    fclose(fr1);
                    fclose(fr2);
                    fclose(fw);
                    return -1;
                }
                for(;;)
                {
                    if(!(fscanf(fr2,"%lf",&y)==1))
                    {
                        if(!feof(fr2))
                        {      
                            printf("Error110\n");
                            fclose(fr1);
                            fclose(fr2);
                            fclose(fw);
                            return -1;
                        }
                        printf("break3\n");
                        break;
                    }
                    fprintf(fw,"%lf ",y);
                }
                fclose(fr1);
                fclose(fr2);
                fclose(fw);
                return 0;
            }
            
        }
        else
        {
            
            printf("printing y: %lf\n",y);
            fprintf(fw,"%lf ",y);
            if(!(fscanf(fr2,"%lf",&y)==1))
            {
                if(!feof(fr2))
                {    
                    printf("Error111\n");
                    fclose(fr1);
                    fclose(fr2);
                    fclose(fw);
                    return -1;
                }
                for(;;)
                {
                    if(!(fscanf(fr1,"%lf",&x)==1))
                    {
                        if(!feof(fr1))
                        {    
                            printf("Error112\n");
                            fclose(fr1);
                            fclose(fr2);
                            fclose(fw);
                            return -1;
                        }
                        printf("break4\n");
                        break;
                    }
                    fprintf(fw,"%lf ",x);
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
        