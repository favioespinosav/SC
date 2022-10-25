#include <iostream>
#include <cmath>
#include <mpi.h>
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <typeinfo>
#define MASTER 0

using namespace std;
double fRand(double fMin, double fMax){
    return ((double) rand()/RAND_MAX);
}
long double func(long double x,long double y,long double z){
    if (x<0 || y<0 || z<0 || (x+y+z) >1){
        return 0.0;
    }
    return 1.0/pow(1.0+x+z+y,3);

}
int main(int argc,char *argv[]) {

    long double x,y,z;
    double eps = 0.00003;
    int my_rank;
    double real = 1.0/2*log(2)-5./16;
    double sum = 0;
    int total_points = 0;
    int size;
    clock_t start, end;
    MPI_Init ( &argc, &argv );

    MPI_Comm_size ( MPI_COMM_WORLD, &size );
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank == MASTER){
        start = clock();
    }
    int iter=1;
    bool continue_ = true;
//int iter = 1;
    srand(my_rank + 10);
    while(continue_){
        int cnt =0;
        double tmp_sum=0.0;
        int tmp_count = 0;
        for(int i=0; i<int(10000000/sqrt(size))*iter; i++) {

            x = fRand(0, 1);
            y = fRand(0, 1);
            z = fRand(0, 1);
            tmp_sum += func(x,y,z);
            tmp_count++;
        }

        iter+=1;

        double tmp_sum_total;
        int tmp_count_total;

        MPI_Reduce(&tmp_sum, &tmp_sum_total, 1, MPI_DOUBLE, MPI_SUM, 0,
                   MPI_COMM_WORLD);

        MPI_Reduce(&tmp_count, &tmp_count_total, 1, MPI_INT, MPI_SUM, 0,
                   MPI_COMM_WORLD);
        if (my_rank==MASTER){
            total_points+=tmp_count_total;
            sum+=tmp_sum_total;

            double val = sum/total_points;
         //   cout<<val<<endl;
            if (abs(val-real)<=eps){
                continue_=false;
            }
        }
        MPI_Bcast(&continue_,1,MPI_BYTE,0,MPI_COMM_WORLD);
    }
    if(my_rank==MASTER){
        cout<<"Error = "<<abs(sum/total_points-real)<<endl;
        cout<<"VAl = "<<sum/total_points<<endl;
        cout<<"EPS = "<<eps<<endl;
        cout<<"Size = "<<size<<endl;
        end = clock();
        long double time_;
        time_ = ((double) (end - start)) / CLOCKS_PER_SEC;
        cout<<"TIME ="<<time_<<endl;

    }
 //   cout<<"Finish"<<endl;
    MPI_Finalize();
}
