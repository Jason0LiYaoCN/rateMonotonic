//////////////////////////////////////////////////////////////////////
//// Task 1 STR (Real Time Systems)
//// Rate Monotonic Scheduler
//// 
//// Author: Lucas Mior
//// email: lucas.mior@acad.pucrs.br
//// Date: 09/01/2015   September
//// Modified: 09/20/2015   September
//////////////////////////////////////////////////////////////////////

#include <stdio.h>  
#include <stdlib.h> // malloc
#include <math.h>   // pow, ceil

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

void Sort               ( struct task*, int );
void VerifyPeriod       ( struct task*, int, int );
void CalculateMetrics   ( char*, int, struct task*, int, int );

char getIdleProcess     ( void );
int wantToProcess       ( struct task*, int );
int searchForProcess    ( struct task*, int, char );
int haveDeadline        ( char, int, struct task*, int );
int tc                  ( char*, int, struct task*, int );
char chooseBetter       ( struct task*, int, char*, int , int* );

int dontHave( int, int*, int );
int dontHaveCoef( char, struct task*, int );
void SortNumbers( int*, int );
float functionWU( struct task*, int, int*, int );

int main( int argc, char **argv )
{
    // Variables
    int numberOfTasks, time, preemption = 0, i;
    char id, *timeline;
    struct task *tasks;

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

            if( timeline[time-1] == '.' )
            {
                preemption++;
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
        if( ( index % tasks[p].p ) == 0 && index != 0 )
        {

            if( tasks[p].remainder > 0 )
            {
                tasks[p].deadline += 1;
            }
            tasks[p].remainder = tasks[p].c ;
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
                if ( better != prev && ( searchForProcess( tasks, numberOfTasks, prev ) || prev == getIdleProcess( ) ) )
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

// context switching
int TC( char *timeline, int line, struct task *tasks, int numberOfTasks )
{
    int i, tc = 0;

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
    // TODO Verify it
    tc++;
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
    int deadline = 0;
    float u = 0;
    float limit = 0;
    
    limit = numberOfTasks * ( pow( 2, 1.0 / numberOfTasks ) - 1 );

    for( i = 0; i < numberOfTasks; i++ )
    {   
        u += (float)tasks[i].c / (float)tasks[i].p;        
    }
    
    // Show the scheduler
    for( i = 0; i < time; i++ )
    {
        printf( "%c", timeline[i] );
    }
    printf( "\n" );
    
    printf( "%d %d\n", TC( timeline, time, tasks, numberOfTasks ), preemption );
    printf( "%.4f %.4f\n", u, limit );

    //printf( "Start here..\n" );
    for( i = 0; i < numberOfTasks; i++ )
    {
        // Discovery the deadlines..
        int j;
        int amount = 1;
        int coefficients = 1;
        int earlyDeadline[100000];  //Hard-Coded, change it.
        struct task coef[numberOfTasks];
    
        deadline = tasks[i].d;  // Dont need, can use eD[0]
        coef[0] = tasks[i];
        earlyDeadline[0] = deadline;
    
        for( j = 0; j < numberOfTasks; j++ )
        {
            int auxDeadline = tasks[j].d;        
            //printf( "%d < %d\n", auxDeadline, deadline );
            if( auxDeadline < deadline )
            {
                if( dontHaveCoef( tasks[j].id, coef, coefficients) )
                {
                    coef[coefficients] = tasks[j];
                    coefficients++;
                }
            }
            while( auxDeadline < deadline )
            {

                if( dontHave( auxDeadline, earlyDeadline, amount ) )
                {        
                    //printf( " Add: %d \n",auxDeadline );        
                    earlyDeadline[amount] = auxDeadline;                
                    amount++;
                    SortNumbers( earlyDeadline, amount );
                    //printf( " Added: %d \n",auxDeadline ); 
                }
                auxDeadline += tasks[j].d;      
            }
            //printf( "Task %d has: %d \n", i, amount );
        }
        
        functionWU( coef, coefficients, earlyDeadline, amount );
    }
    printf( "\n" );    
}

float functionWU( struct task* coef, int coefficients, int* earlyDeadline, int amount )
{
    int s;

    for( s = 0; s < amount; s++ )
    {
        float result = 0;
        int d;
        
        // W operation
        for( d = 0; d < coefficients; d++ )
        {
            float toCeil = (float)earlyDeadline[s] / coef[d].p;
            int fromCeil = ceil( toCeil );
            result += fromCeil * coef[d].c;
        }

        // U operation
        result /= earlyDeadline[s];

        if( s != amount-1 )         //to keep diff format
        printf( "%.4f ", result );
        else                        //to keep diff format 
        printf( "%.4f", result );   //to keep diff format

    }
    printf( "\n" );
}

int dontHave( int d, int* deadline, int size )
{
    if( size > 0 )
    {   
        int i;
    
        for( i = 0; i < size; i++ )
        {
            if( d == deadline[i] )
            {
                return 0;
            }
        }
        return 1;
    }
    else
    {
        return 1;
    }
}

int dontHaveCoef( char d, struct task* deadline, int size )
{
    if( size > 0 )
    {   
        int i;
        for( i = 0; i < size; i++ )
        {
            if( d == deadline[i].id )
            {
                return 0;
            }
        }
        return 1;
    }
    else
    {
        return 1;
    }
}

void SortNumbers( int* tasks, int numberOfTasks )
{
    int i, j;
    int aux;
    
    for( i = 0; i < numberOfTasks - 1; i++ )
    {    
        for( j = 0; j < numberOfTasks - 1 - i; j++ )
        {
            if( tasks[j] > tasks[j+1] )
            {
                aux        = tasks[j];
                tasks[j]   = tasks[j+1];
                tasks[j+1] = aux;
            }
        }
    }
}