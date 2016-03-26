#include<iostream.h>   
#include<fstream.h>   
#include<math.h>   
#include<stdlib.h>   
#include<stdio.h>   
#include <time.h>   
//====================   
int openfile();   
void randomselect();   
   
   
void writefile();   
   
int training(int *,int);   
   
//=========================   
int vector[20480][16];   
   
int group[20480];   
int mark[256];   
   
   
   
   
void main()   
{   
    double start_t,end_t,time;   
    openfile();   
    randomselect();   
       
       
    //===========Âk¸s=============================   
    start_t=clock();   
    double distortion,threshold;   
    int i,j,Number,round;   
    double *sum;   
    sum = new double[16];   
    Number=0;   
    distortion=10000;   
    threshold=1500;   
    round=0;   
    while(distortion>threshold)   
    {          
        round++;   
        distortion=0;   
        for(i=0;i<20480;i++)   
        {   
                int temp=0;   
                for(j=0;j<256;j++)   
                    if(i==mark[j])   
                        temp++;   
                if(temp==0)   
                    group[i]=training(vector[i],i);    
        }   
        for(i=0;i<256;i++)   
        {   
            for(int Q=0;Q<16;Q++)   
                sum[Q]=0;   
            Number=0;   
            for(j=0;j<20480;j++)   
            {   
                if(group[j]==i)   
                {   
                    Number++;   
                    for(int Q=0;Q<16;Q++)   
                    {   
                        sum[Q]=sum[Q]+vector[j][Q];    
                    }   
                }   
            }   
            for(Q=0;Q<16;Q++)   
                sum[Q]=sum[Q]+vector[mark[i]][Q];   
            Number++;   
            double SUM=0;   
            for(Q=0;Q<16;Q++)   
            {   
                sum[Q]=sum[Q]/Number;   
                SUM = SUM+pow((vector[mark[i]][Q]-sum[Q]),2);   
                vector[mark[i]][Q]=(int)sum[Q];   
            }   
            distortion = distortion + sqrt(SUM);   
               
        }   
           
    }   
    free(sum);   
    end_t=clock();   
    time=end_t-start_t;   
    cout<<time<<"  "<<round<<endl;   
    writefile();   
    //==========================================   
}   
   
void writefile()   
{   
    FILE *writefile;   
    writefile=fopen("NEW_256CW16V","wb");   
    if( !writefile )   
    {   
           printf("The file: NEW_256CW16V is not found, or no available space!\n");   
    }   
    for(int i=0;i<256;i++)   
    {   
        for(int j=0;j<16;j++)   
        {   
            char c;   
            c=vector[mark[i]][j];   
            fputc(c, writefile);   
        }   
    }   
    fclose(writefile);   
    char *buffer;   
    buffer = new char[5];   
    ofstream writefile2;   
    writefile2.open("256CB.dat");   
    for(i=0;i<256;i++)   
    {   
        for(int j=0;j<16;j++)   
        {   
            for(int Q=0;Q<5;Q++)   
                buffer[Q]=0;   
            itoa(vector[mark[i]][j],buffer,10);   
            writefile2<<buffer<<" ";   
        }   
        writefile2<<endl;   
    }   
    writefile2.close();   
    delete buffer;   
}   
int openfile()   
{   
    int image1[256][256];   
    FILE *file1;   
    int i,j,temp;   
    int width,height=0;   
//==============================================================================       
    if ( (file1=fopen("Boat256.raw","rb"))==NULL )   
    {   
        cout << "ÀÉ®×¤£¦s¦b..." << endl;   
            return 0;   
    }   
    for(i=0;i<256;i++)   
        for(j=0;j<256;j++)   
            image1[i][j]=fgetc(file1);   
    fclose(file1);   
    temp=-1;   
    while(height<256)   
    {   
        for(i=0;i<256;i++)   
        {   
            if(i%4==0)   
                temp++;   
            width=i%4;   
            vector[temp][width]=image1[height][i];   
            vector[temp][width+4]=image1[height+1][i];   
            vector[temp][width+8]=image1[height+2][i];   
            vector[temp][width+12]=image1[height+3][i];   
        }   
        height+=4;   
    }   
//=======================================================================================   
    FILE *file2;   
    if (  (file2=fopen("Lena256.raw","rb"))==NULL )   
    {   
        cout << "ÀÉ®×¤£¦s¦b..." << endl;   
            return 0;   
    }   
    for(i=0;i<256;i++)   
        for(j=0;j<256;j++)   
        {   
            image1[i][j]=fgetc(file2);   
        //  cout<<image1[i][j]<<"  ";   
        }   
    fclose(file2);   
    height=0;   
    while(height<256)   
    {   
        for(i=0;i<256;i++)   
        {   
            if(i%4==0)   
                temp++;   
            width=i%4;   
            vector[temp][width]=image1[height][i];   
            vector[temp][width+4]=image1[height+1][i];   
            vector[temp][width+8]=image1[height+2][i];   
            vector[temp][width+12]=image1[height+3][i];   
        }   
        height+=4;   
    }   
//===============================================================================   
    FILE *file3;   
    if ( (file3=fopen("Jet(F16)256.raw","rb"))==NULL  )   
    {   
        cout << "ÀÉ®×¤£¦s¦b..." << endl;   
            return 0;   
    }   
    for(i=0;i<256;i++)   
        for(j=0;j<256;j++)   
            image1[i][j]=fgetc(file3);   
    fclose(file3);   
    height=0;   
    while(height<256)   
    {   
        for(i=0;i<256;i++)   
        {   
            if(i%4==0)   
                temp++;   
            width=i%4;   
            vector[temp][width]=image1[height][i];   
            vector[temp][width+4]=image1[height+1][i];   
            vector[temp][width+8]=image1[height+2][i];   
            vector[temp][width+12]=image1[height+3][i];   
        }   
        height+=4;   
    }   
//=====================================================================================   
    FILE *file4;   
    if ( (file4=fopen("Toys256.raw","rb"))==NULL  )   
    {   
        cout << "ÀÉ®×¤£¦s¦b..." << endl;   
            return 0;   
    }   
    for(i=0;i<256;i++)   
        for(j=0;j<256;j++)   
            image1[i][j]=fgetc(file4);   
    fclose(file4);   
    height=0;   
    while(height<256)   
    {   
        for(i=0;i<256;i++)   
        {   
            if(i%4==0)   
                temp++;   
            width=i%4;   
            vector[temp][width]=image1[height][i];   
            vector[temp][width+4]=image1[height+1][i];   
            vector[temp][width+8]=image1[height+2][i];   
            vector[temp][width+12]=image1[height+3][i];   
        }   
        height+=4;   
    }   
//========================================================================================   
    FILE *file5;   
    if ( (file5=fopen("SailBoat256.raw","rb"))==NULL  )   
    {   
        cout << "ÀÉ®×¤£¦s¦b..." << endl;   
            return 0;   
    }   
    for(i=0;i<256;i++)   
        for(j=0;j<256;j++)   
            image1[i][j]=fgetc(file5);   
    fclose(file5);   
    height=0;   
    while(height<256)   
    {   
        for(i=0;i<256;i++)   
        {   
            if(i%4==0)   
                temp++;   
            width=i%4;   
            vector[temp][width]=image1[height][i];   
            vector[temp][width+4]=image1[height+1][i];   
            vector[temp][width+8]=image1[height+2][i];   
            vector[temp][width+12]=image1[height+3][i];   
        }   
        height+=4;   
    }   
//===========================================================================================//   
    return 0;   
}   
void randomselect()   
{   
    srand(time(NULL));   
    mark[0]=rand()%20480;   
    for(int i=1;i<256;i++)   
    {   
        int j;   
        while(1)   
        {   
            int temp=0;   
            mark[i]=rand()%20480;   
            for(j=0;j<i;j++)   
            {   
                if(mark[i]==mark[j])   
                    temp=1;   
            }   
            if(temp==0)   
                break;   
        }   
    }   
}   
   
   
int training(int *Vector,int i)   
{   
    int k,j;   
        //cout<<x<<"  "<<y<<endl;   
    int min=0;   
    double distance,min_distance;   
       
    min_distance=9999;   
    for(k=0;k<256;k++)   
    {   
        distance=0;   
        for(j=0;j<16;j++)   
        {   
            distance=distance+pow((Vector[j]-vector[mark[k]][j]),2);   
        }   
        distance=sqrt(distance);   
        if(distance<min_distance)   
        {   
            min_distance=distance;   
            min=k;   
        }   
    }   
    return min;   
}   
