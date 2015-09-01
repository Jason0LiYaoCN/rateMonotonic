#include <stdio.h>
#include <stdlib.h>
#include <math.h>   //pow

struct task
{
    int c;
    int p;
    int d;
    int remainder;
    char id;
    int wantToProcess;
}task;

void Sort( struct task *tasks, int numberOfTasks );
void VerifyPeriod( struct task *tasks, int numberOfTasks, int index );
int WantToProcess( struct task *tasks, int numberOfTasks );
char ChooseBetter( struct task *tasks, int numberOfTasks , char* timeline, int time, int *preemption );
void CalculateMetrics( char *timeline, int time, struct task *tasks, int numberOfTasks, int preemption );
int haveDeadline( char task, int index, struct task *tasks, int numberOfTasks );
int searchForProcess( struct task *tasks, int numberOfTasks, char prev );
int TC( char *timeline, int time, struct task *, int numberOfTasks );
void Line( int time );
char getIdleProcess( );

int main( )
{
    // Variables
    int numberOfTasks, time;
    char *timeline;
    int i;
    char id;
    int preemption = 0;
    int scalabilityTest = 0;
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
                tasks[i].id = id;
                id++;
                //printf( "result %d %d %d id: %c\n", tasks[i].c, tasks[i].p, tasks[i].d, tasks[i].id );
            }

            // Sort the tasks by computing time
            Sort( tasks, numberOfTasks );
            /*for( i = 0; i < numberOfTasks; i++ )
            {
                printf( "Task: c: %d and p: %d \n", tasks[i].c, tasks[i].p );
            }*/
            int index;
            // Start the scheduler
            for( i = 0; i < time; i++ )
            {
                int rval;

                VerifyPeriod( tasks, numberOfTasks, i );

                rval = WantToProcess( tasks, numberOfTasks );
                if ( rval != 0 )
                {
                    timeline[i] = ChooseBetter( tasks, numberOfTasks, timeline, i, &preemption );
                }
                else
                {
                    timeline[i] = getIdleProcess( ); //change by a function that verify the process that passed the deadline and return a process or a ilde
                }

                //DEADLINE
                /*for( index = 0; index < numberOfTasks; index++ )
                {
                    if( ( ( i % tasks[index].d ) == 0 ) && ( tasks[index].remainder > 0 ) )
                    {
                        printf( " task %c has deadline at %d. Remainder is %d\n", tasks[index].id, i, tasks[index].remainder );
                    }
                }*/

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
        if( ( index % tasks[p].p ) == 0 )
        {
            if( tasks[index].remainder > 0 )
            {
                printf( " task %c has deadline at %d. Remainder is %d\n", tasks[p].id, index, tasks[p].remainder );
            }
            //printf( "DEBUG:: if id: %c \n", tasks[p].id );
            tasks[p].remainder = tasks[p].c ;
        }
    }
}
int WantToProcess( struct task *tasks, int numberOfTasks )
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

char ChooseBetter( struct task *tasks, int numberOfTasks, char* timeline, int time, int *preemption )
{
    int i;
    int havePreemp = 0;
    char better = 'x';
    for( i = 0; i < numberOfTasks; i++ )
    {
        if ( tasks[i].wantToProcess )
        {
            //printf( "Remainder is %d\n", tasks[i].remainder );
            tasks[i].remainder--;
            better = tasks[i].id;
            if( time != 0 )    //if 0 dont have preemption
            {
                //printf( "Iam here\n" );
                char prev = timeline[time-1];
                //printf( "prev: %c, time %d\n", timeline[time-1], time );
                if ( better != prev && ( searchForProcess( tasks, numberOfTasks, prev ) || prev == '.' ) )
                {
                    //printf( "preemption: %d, time %d\n", *preemption, time );
                    *preemption = *preemption + 1;
                    //printf( "preemption: %d\n", *preemption );
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
            //printf( "DEBUG:: %c %c %d\n", tasks[i].id, prev, tasks[i].wantToProcess );
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

int TC( char *timeline, int line, struct task *tasks, int numberOfTasks )
{

    // CHANGE numberOfTasksM
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
    //int scalabilityTest = 0;
    float u = 0;
    float limit = 0;
    
    limit = numberOfTasks * ( pow( 2, 1.0 / numberOfTasks ) - 1 );

    for( i = 0; i < numberOfTasks; i++ )
    {   
        //scalabilityTest = 1 || 0;
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
    //printf( "Troca de Contexto: %d\n", TC( timeline, time, tasks, numberOfTasks ) );
    //printf( "Preempção: %d\n", preemption );
    //printf( "Limit: %.4f\n", limit );
    printf( "%d %d\n", TC( timeline, time, tasks, numberOfTasks ), preemption );
    printf( "%.4f %.4f \n", u, limit );

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