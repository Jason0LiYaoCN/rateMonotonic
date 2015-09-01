//////////////////////////////////////////////////////////////////////
//// Task 1 STR (Real Time Systems)
//// Rate Monotonic Scheduler
//// 
//// Author: Lucas Mior
//// email: lucas.mior@acad.pucrs.br
//// Date: 09/01/2015   September
//// Modified: --
//////////////////////////////////////////////////////////////////////


#include <stdio.h>  
#include <stdlib.h> // malloc
#include <math.h>   // pow

struct task
{
    int c;
    int p;
    int d;
    int remainder;
    char id;
    int deadline;
    int wantToProcess;
}task;

void Line               ( int );
void Sort               ( struct task*, int );
void VerifyPeriod       ( struct task*, int, int );
void CalculateMetrics   ( char*, int, struct task*, int, int );

char getIdleProcess     ( void );
int wantToProcess       ( struct task*, int );
int searchForProcess    ( struct task*, int, char );
int haveDeadline        ( char, int, struct task*, int );
int tc                  ( char*, int, struct task*, int );
char chooseBetter       ( struct task*, int, char*, int , int* );

int main( int argc, char **argv )
{
    // Variables
    int numberOfTasks, time;
    char *timeline;
    struct task *tasks;
    
    int i;
    char id;
    int preemption = 0;
    //int scalabilityTest = 0;
    //int index;
    
    // Process
    do
    {
        id = 0x41;
        preemption = 0;

        // Read numberOfTasks of tasks and the time to execute
        scanf( "%d %d", &numberOfTasks, &time );

        if( ( numberOfTasks != 0 ) && ( time != 0 ) )
        {
            tasks = malloc( sizeof(struct task) * numberOfTasks );
            timeline = malloc( sizeof(char) * time );

            // Read the task's inputs
            for( i = 0; i < numberOfTasks; i++ )
            {
                scanf( "%d %d %d", &tasks[i].c, &tasks[i].p, &tasks[i].d );
                tasks[i].remainder = tasks[i].c;
                tasks[i].wantToProcess = 0;
                tasks[i].deadline = 0;
                tasks[i].id = id;
                id++;
                //printf( "result %d %d %d id: %c\n", tasks[i].c, tasks[i].p, tasks[i].d, tasks[i].id );
            }

            // Sort the tasks by computing time
            Sort( tasks, numberOfTasks );

            // Start the scheduler
            for( i = 0; i < time; i++ )
            {
                int rval;

                VerifyPeriod( tasks, numberOfTasks, i );

                rval = wantToProcess( tasks, numberOfTasks );
                if ( rval != 0 )
                {
                    timeline[i] = chooseBetter( tasks, numberOfTasks, timeline, i, &preemption );
                }
                else
                {
                    timeline[i] = getIdleProcess( ); //change by a function that verify the process that passed the deadline and return a process or a ilde
                }
            }

            CalculateMetrics( timeline, time, tasks, numberOfTasks, preemption );
        }
    }while( numberOfTasks != 0 );
    return 0;
}

char getIdleProcess( )
{
    return '.';
}

void VerifyPeriod( struct task *tasks, int numberOfTasks, int index )
{
    int p;
    // Verify end of period
    for( p = 0; p < numberOfTasks; p++ )
    {
        //printf( "DEBUG:: tasks[p].p: %d and ( time + 1 ): %d \n", tasks[p].p, time + 1 );
        // WARNING
        // Não estou verificando se processou tudo no tempo certo e nem deadline...
        //printf( "index: %d, id: %c result: %d, remainder: %d\n", index, tasks[p].id, index % tasks[p].p, tasks[p].remainder );
        //printf( "IF %d == 0\n", ( index % tasks[p].p ) );
        if( ( index % tasks[p].p ) == 0 && index != 0 )
        {

            if( tasks[p].remainder > 0 )
            {
                // TODO add to a list 
                tasks[p].deadline += 1;
                //printf( "TASK %c has deadline at %d. Remainder is %d\n", tasks[p].id, index, tasks[p].remainder );
            }
            //printf( "DEBUG:: if id: %c \n", tasks[p].id );
            tasks[p].remainder = tasks[p].c ;
            //printf( "id: %c receive more %d of computing \n", tasks[p].id, tasks[p].remainder );
        }
    }
}
int wantToProcess( struct task *tasks, int numberOfTasks )
{
    int i;
    int count = 0;
    for( i = 0; i < numberOfTasks; i++ )
    {
        tasks[i].wantToProcess = 0;
    }
    for( i = 0; i < numberOfTasks; i++ )
    {
        if ( tasks[i].remainder > 0 )
        {
            tasks[i].wantToProcess = 1;
            count++;
        }
    }
    return count;
}

char chooseBetter( struct task *tasks, int numberOfTasks, char* timeline, int time, int *preemption )
{
    int i;
    char better = 'x';  // error code
    for( i = 0; i < numberOfTasks; i++ )
    {
        if ( tasks[i].wantToProcess )
        {

            if ( tasks[i].deadline > 0 )
            {
                tasks[i].deadline--;
                better = tasks[i].id + 0x20;
            }
            else
            {
                tasks[i].remainder--;
                better = tasks[i].id;
            }

            //TODO Test this part of code when we have BbB tc? preemp?
            if( time != 0 )    //if 0 dont have preemption
            {
                char prev = timeline[time-1];
                if ( better != prev && ( searchForProcess( tasks, numberOfTasks, prev ) || prev == '.' ) )
                {
                    *preemption +=  1;
                }
            }
            return better;
        }
    }
}

int searchForProcess( struct task *tasks, int numberOfTasks, char prev )
{
    int i;
    for( i = 0; i < numberOfTasks; i++ )
    {
        if( tasks[i].id == prev )
        {
            return tasks[i].wantToProcess;
        }
    }

    return 0;   //default value
}


void Sort( struct task *tasks, int numberOfTasks )
{
    int i, j;
    struct task aux;
    
    for( i = 0; i < numberOfTasks - 1; i++ )
    {    
        for( j = 0; j < numberOfTasks - 1 - i; j++ )
        {
            if( tasks[j].p > tasks[j+1].p )
            {
                aux        = tasks[j];
                tasks[j]   = tasks[j+1];
                tasks[j+1] = aux;
            }
            else if( ( tasks[j].p == tasks[j+1].p ) && ( tasks[j].c > tasks[j+1].c ) )
            {
                aux        = tasks[j];
                tasks[j]   = tasks[j+1];
                tasks[j+1] = aux;
            }
        }
    }
}
// /context switching
int TC( char *timeline, int line, struct task *tasks, int numberOfTasks )
{
    int i;
    // TODO Verify it
    int tc = 1;
    for( i = 0; i < line - 1; i++ )
    {
        if( timeline[i] != timeline[i + 1] )
        {
            tc++;
        }
        else if( timeline[i] != '.' )
        {
            if( haveDeadline( timeline[i], i, tasks, numberOfTasks ) )
            {
                printf( "DEADLINE %d\n", i );
                tc++;
            }
        }
    }
    return tc;
}

int haveDeadline( char task, int index, struct task *tasks, int numberOfTasks )
{
    int i;
    for( i = 0; i < numberOfTasks; i++ )
    {
        if( ( tasks[i].id == task ) && ( tasks[i].d == index + 1 ) )
        {
            return 1;
        }
    }
    return 0;
}

void CalculateMetrics( char *timeline, int time, struct task *tasks, int numberOfTasks, int preemption )
{
    int i;
    float u = 0;
    float limit = 0;
    
    limit = numberOfTasks * ( pow( 2, 1.0 / numberOfTasks ) - 1 );

    for( i = 0; i < numberOfTasks; i++ )
    {   
        u += (float)tasks[i].c / (float)tasks[i].p;        
    }
    // Show the scheduler
    //Line( time );
    //printf( "|" );
    for( i = 0; i < time; i++ )
    {
        printf( "%c", timeline[i] );
    }
    printf( "\n" );
    //Line( time );

    printf( "%d %d\n", TC( timeline, time, tasks, numberOfTasks ), preemption );
    printf( "%.4f %.4f\n", u, limit );

    printf( "\n" );

}

void Line( int time )
{
    int i;
    printf( "+" );
    for( i = 0; i < time; i++ )
    {
        printf( "-" );
    }
    printf( "+\n" );
}