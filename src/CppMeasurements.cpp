#include <Windows.h>
#include <iostream>
#include <chrono>
#include <intrin.h> // Platform specific, gives __rdtsc()
#include <pthread.h>
#include <unistd.h>

//#define ADDITIONAL_OUTPUT_MACRO

#define SIZE_OF_ARRAY 100000
#ifdef ADDITIONAL_OUTPUT_MACRO
    #define TRIES 1
#else
    #define TRIES 1000
#endif
#define PROCESSOR_GHZ 3 // the CPU frequency on my machine (in GHz)
#define NB_THREADS 10
#define NB_CORES 2

using namespace std;
using namespace chrono;

steady_clock::time_point thread_conSwitch0;
steady_clock::time_point thread_conSwitch1;

long one_thread_migration_duration = 0;

long staticMemoryAllocationTime()
{
    unsigned long long sum = 0;
    for(int i = 0 ; i < TRIES ; i++)
    {
        // __rdtsc() return the value of Time Stamp Counter register, which counts the nb of CPU cycles
        unsigned long long start = __rdtsc();
        
        int p[SIZE_OF_ARRAY];
        
        unsigned long long end = __rdtsc();
        
        sum += end-start;
    }
    return (long)( ( ( (float) ((long)sum/TRIES) ) / PROCESSOR_GHZ) * 1000);
}

long dynamicMemoryAllocationTime()
{
    long sum = 0;   
    for(int i = 0 ; i < TRIES ; i++)
    {
        
        time_point t0 = steady_clock::now();
        
        int *p = (int*)malloc(sizeof(int) * SIZE_OF_ARRAY);

        
        time_point t1 = steady_clock::now();
        sum += duration_cast<nanoseconds>(t1 - t0).count();
        
        free(p);
    }
    return sum/TRIES;
}

long staticMemoryAccessTime()
{
    long sum = 0;
    int p[SIZE_OF_ARRAY];
    for(int i = 0 ; i < SIZE_OF_ARRAY ; i ++)
        p[i] = rand()%10000;
    
    int a;
    long aux_sum = 0;
    
    for(int i = 0 ; i < TRIES ; i++)
    {
        int random_nb = rand() % SIZE_OF_ARRAY;
        time_point t0 = steady_clock::now();
        
        for(int j = 0 ; j < SIZE_OF_ARRAY ; j++)
            a = p[random_nb];
        
        time_point t1 = steady_clock::now();
        sum += duration_cast<nanoseconds>(t1 - t0).count();
        aux_sum += a;
    }
    return sum/TRIES;
}

long dynamicMemoryAccessTime()
{
    long sum = 0;
    int *p = (int*)malloc(sizeof(int) * SIZE_OF_ARRAY);
    for(int i = 0 ; i < SIZE_OF_ARRAY ; i ++)
        p[i] = rand()%10000;
    
    int a;

    for(int i = 0 ; i < TRIES ; i++)
    {
        time_point t0 = steady_clock::now();
        
        for(int j = 0 ; j < SIZE_OF_ARRAY ; j++)
            a = p[j];
        
        time_point t1 = steady_clock::now();
        sum += duration_cast<nanoseconds>(t1 - t0).count();
    }
    return sum/TRIES;
}

void* tc_function(void* arg) 
{
    return NULL;
}

long threadCreationTime()
{
    pthread_t th;
    long sum = 0;
    int i;
    for(i = 0 ; i < TRIES ; i++)
    {
        time_point t0 = steady_clock::now();
        pthread_create(&th, NULL, tc_function, &i);
        time_point t1 = steady_clock::now();
        
        sum += duration_cast<nanoseconds>(t1 - t0).count();
    }

    return sum/TRIES;
}


void* tcsw_function(void* arg)
{
    #ifdef ADDITIONAL_OUTPUT_MACRO
        int thread_id = *(int*)arg;
        printf("Secondary thread runs on core %d\n", GetCurrentProcessorNumber());
    #endif

    thread_conSwitch1 = steady_clock::now();
    
    return NULL;
}

long threadContextSwitchTime()
{
    HANDLE process = GetCurrentProcess();
    /*
    Set the bit mask for the available logical processors for threads to run on.
    1 - means the logical processor is available for a thread to run on
    0 - means not

    So 1 means the bit mask is ...001, so only the logical processor with id 0 is available.
    */
    DWORD_PTR processAffinityMask = 1;

    BOOL success = SetProcessAffinityMask(process, processAffinityMask);
    
    #ifdef ADDITIONAL_OUTPUT_MACRO
        cout << "Success status of affinity mask (0 means fail): " << success << endl;
    #endif

    pthread_t thread;
    int arg;
    
    #ifdef ADDITIONAL_OUTPUT_MACRO
        printf("Thread main runs on core %d\n", GetCurrentProcessorNumber());
    #endif
    
    long sum = 0;
    for(int i = 0 ; i < TRIES ; i++)
    {
        thread_conSwitch0 = steady_clock::now();
        pthread_create(&thread, NULL, tcsw_function, &arg);
        pthread_join(thread, NULL);

        sum += duration_cast<nanoseconds>(thread_conSwitch1 - thread_conSwitch0).count();
    }

    return sum/TRIES;
}

void* tmgr_function(void* arg)
{
    HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 1;
    BOOL success = SetProcessAffinityMask(process, processAffinityMask);
    
    #ifdef ADDITIONAL_OUTPUT_MACRO
        printf("Success status of SetProcessAffinityMask() is %d\n", success);
        printf("Thread from thread migration runs on core %d\n", GetCurrentProcessorNumber());
    #endif
    processAffinityMask = 2;

    time_point t0 = steady_clock::now();
    success = SetProcessAffinityMask(process, processAffinityMask);
    time_point t1 = steady_clock::now();

    #ifdef ADDITIONAL_OUTPUT_MACRO
        printf("Success status of SetProcessAffinityMask() is %d\n", success);
        printf("Thread from thread migration runs on core %d\n", GetCurrentProcessorNumber());
    #endif

    one_thread_migration_duration = duration_cast<nanoseconds>(t1 - t0).count();
    return NULL;
}

long threadMigrationTime()
{
    pthread_t thread;
    int arg;
    
    long sum = 0;
    for(int i = 0 ; i < TRIES ; i++)
    {
        pthread_create(&thread, NULL, tmgr_function, &arg);
        pthread_join(thread, NULL);

        sum += one_thread_migration_duration;
    }
    
    return sum/TRIES;
}

int main()
{
    cout << "C++ MEASUREMENTS" << endl;
    cout <<"Static memory allocation: " << staticMemoryAllocationTime() << endl; // calculate the nanoseconds from CPU cycles
    cout <<"Dynamic memory allocation: " << dynamicMemoryAllocationTime() << endl;
    cout <<"Static memory access: " << staticMemoryAccessTime() << endl;
    cout <<"Dynamic memory access: " << dynamicMemoryAccessTime() << endl;
    
    // thread creation is more heavyweight
    cout << "Thread creation: " << threadCreationTime() << endl;

    cout << "Thread context switch: " << threadContextSwitchTime() << endl;
    cout << "Thread migration: " << threadMigrationTime() << endl;
    cout << "_______________________________________________________" << endl << endl;
    return 0;
}