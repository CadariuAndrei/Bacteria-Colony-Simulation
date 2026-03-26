#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<omp.h>

int suma_serial(int *v,int size)
{
    int sum=0;
    for(int i=0;i<size;i++)
    {
        sum+=v[i];
    }
    return sum;
}

int suma_paralel(int *v,int size)
{
    int sum=0;
    #pragma omp parallel for reduction(+ : sum)
    for(int i=0;i<size;i++)
    {
        sum+=v[i];
    }
    return sum;
}
int main(int argc,char **argv)
{
    int v[]={1,2,3,4,5,6,7,8,9,10};
    int sum=suma_serial(v,sizeof(v)/sizeof(v[0]));
    printf("%d\n",sum);
    sum=suma_paralel(v,sizeof(v)/sizeof(v[0]));
    printf("%d",sum);
    return 0;
}