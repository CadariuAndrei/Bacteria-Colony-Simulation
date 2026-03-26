/*Implementati un algoritm paralel pentru procesarea unui vector de real pozitive 1000000 elemente.Pentru fiecare element aplicati etapele
Verificati daca valoarea este strict pozitiva si se afla in intervalul 0.01,1000,ignolred daca nu respecta aceasta conditie
Transformarea matematica dependenta de pozitie: Pentru fiecare element valid de pe pozitia i aplicati functia f(X)=sqrt(x)*log(i+1)
Adunati toate valorile transformate intro suma globala aceasta suma  trebuie sincronizat */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<omp.h>
#include<time.h>

#define MAXRAND 100

void generate(double *v,int size)
{
    for(int i=0;i<size;i++)
    {
        v[i]=rand()%100;
    }
}

double compute(double *v,int size)
{
    double global_sum=0;
    #pragma omp parallel for num_threads(8)
         for(int i=0;i<size;i++)
            {
                if(v[i]>0 && v[i]>=0.01 && v[i]<=1000)
                 {
                     int aux=v[i];
                     v[i]=sqrt(aux)+log(i+1);
                 }
            }


    #pragma omp parallel for reduction(+ : global_sum)
      for(int i=0;i<size;i++)
         {
            global_sum+=v[i];
         }
        return global_sum;
}

int main(int argc,char **argv)
{
    srand(time(NULL));
    double v[1000000]={0};
    generate(v,1000000);
    printf("%lf",compute(v,1000000));



    return 0;
}
