#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <omp.h>

int count_words_serial(char *text)
{
    int count=0;
    char *p=strtok(text," ");
    while(p)
    {
        count++;
        p=strtok(NULL," ");
    }
    return count;
}

int count_words_parallel(char *text)
{
    int count=0;
    char *p=NULL;
    #pragma omp parallel shared(count)
    {
        p=strtok(text," ");
        while(p)
        {
            #pragma omp critical
            {
                count++;
            }

            p=strtok(NULL," ");
        }
    }
      
    return count;
}
int main(int argc,char **argv)
{
    int count;
    char text[2048];
    fgets(text,2048,stdin);
    char parallel_text[2048];
    strcpy(parallel_text,text);
    count=count_words_serial(text);
    printf("%d\n",count);
    count=0;
    count=count_words_parallel(parallel_text);
    printf("%d",count);

    return 0;
}