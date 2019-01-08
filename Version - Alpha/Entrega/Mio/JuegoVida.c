#include "mpi.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <string.h>

#define DDefaultOutputFilename "./Life_%04d.txt"
#define DNumIterForPartialResults 25
#define PARALLEL 1
#define SEQUENTIAL 0


int *life_update_parallel(int m, int n, int grid[], int up_fronter[], int down_fronter[]);
int *states_workout(int m, int column_size, int new_grid[], int type);
int *join_borders_with_grid(int m, int n, int up_border[], int down_border[], int grid[]);
int *substract_grid(int n, int m, int new_grid[]);
void life_update_sub_array(int m, int n, int new_grid[], int s[], int type);
int up_dest(int rank, int size);
int down_dest(int rank, int size);
void new_borders(int m, int size_of_recv, int up[], int down[]);
void divide_rows(int size, int m, int n);
int *life_init ( char *filename, double prob, int m, int n, int *seed );
void life_write( char *output_filename, int m, int n, int tasks, int scounts[], int recv[], int rank, int type);
void life_read ( char *input_filename, int m, int n, int grid[] );
double r8_uniform_01 ( int *seed );
void timestamp ( void );

int *balance, *displs, *rbuf;

int main(int argc, char *argv[]) {

  char *initial_file=NULL, output_filename[100];
  double begin_time;
  int it;
  int it_max=25;
  int m=10;
  int n=10;
  int *grid;
  int *up, *down;
  int i, j;
  double prob;
  int seed;
  int to_send = 0;

  int rank;
  int size, initial_size;


  MPI_Status status;
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  initial_size = size;

  if(rank == 0)
  {
    begin_time = MPI_Wtime();
  }

  if (argc>4)
    it_max = atoi(argv[4]);
  if (argc>3)
    n = atoi(argv[3]); //filas
  if (argc>2)
    m = atoi(argv[2]);
  if (argc>1)
    initial_file = argv[1];
  else
    initial_file = NULL;

  if(size > n)
  {
    if (rank == 0)
      printf("No se recomienda ejecutar el programa con estos parametros.\n Hay %d procesos ociosos %d. \n", (size - n));
    exit(0);
  }

  /* Apartado tipos derivados: MPI_Type_Vector*/
  MPI_Datatype coltype;
  MPI_Type_vector(m, 1, 1, MPI_INT, &coltype);
  MPI_Type_commit(&coltype);

  prob = 0.20;
  seed = 123456789;

  grid = life_init( initial_file, prob, m, n, &seed );

  if(size == 1)
  {
    int *s = ( int * ) malloc ( n * m * sizeof ( int ) );
    for ( it = 0; it <= it_max; it++ )
    {
      if(it != 0)
      {
        s = states_workout( m, n, grid, SEQUENTIAL );
        life_update_sub_array( m, n, grid, s, SEQUENTIAL );
      }
      if ((it%DNumIterForPartialResults)==0)
      {
        sprintf( output_filename, DDefaultOutputFilename, it );
        life_write( output_filename, m, n, size, balance, grid, rank, SEQUENTIAL);

      }
    }
    free(s);

  } else {

    divide_rows( size, n, m );
    rbuf = ( int * ) malloc(balance[rank]  * sizeof(int));
    MPI_Scatterv( grid, balance, displs, MPI_INT, rbuf, balance[rank], MPI_INT, 0, MPI_COMM_WORLD );

    /* Ranks to send and receive up and down borders */
    int down_border_dest;
    int up_border_dest;
    up_border_dest =  up_dest( rank, size );
    down_border_dest = down_dest( rank, size );

   for ( it = 0; it <= it_max; it++ )
   {
     up = ( int * ) malloc (m  * sizeof(int));
     down = ( int * ) malloc (m  * sizeof(int));

     if(it != 0)
     {
       if (rank%2 == 0)
       {

         MPI_Request Request;
         new_borders( m, balance[rank], up, down );

         if (MPI_Ssend(down, 1, coltype, down_border_dest, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
           printf("Fallo en enviar el up. Proceso %d\n", rank);
           exit(1);
         }

         int *up_fronter = ( int * ) malloc (m  * sizeof(int));
         int *down_fronter = ( int * ) malloc (m  * sizeof(int));

         if(MPI_Recv(up_fronter, 1, coltype, up_border_dest, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
           exit(2);
         }

         if (MPI_Ssend(up, 1, coltype, up_border_dest, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
           printf("Fallo en enviar el up. Proceso %d\n", rank);
           exit(1);
         }

         if(MPI_Recv(down_fronter, 1, coltype, down_border_dest, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
           exit(2);
         }


         rbuf = life_update_parallel( m, balance[rank], rbuf, up_fronter, down_fronter );

       } else {

         MPI_Request Request;
         MPI_Status status;

         new_borders(m, balance[rank], up, down);

         int *up_fronter = ( int * ) malloc (m  * sizeof(int));
         int *down_fronter = ( int * ) malloc (m  * sizeof(int));

         if(MPI_Recv(up_fronter, 1, coltype, up_border_dest, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
           exit(2);
         }


         if (MPI_Ssend(down, 1, coltype, down_border_dest, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
           printf("Fallo en enviar el down. Proceso %d\n", rank);
           exit(1);
         }

         if(MPI_Recv(down_fronter, 1, coltype, down_border_dest, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
           exit(2);
         }

         if (MPI_Ssend(up, 1, coltype, up_border_dest, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
           printf("Fallo en enviar el down. Proceso %d\n", rank);
           exit(1);
         }

         rbuf = life_update_parallel( m, balance[rank], rbuf, up_fronter, down_fronter );

       }

     }

     if ((it%DNumIterForPartialResults)==0)
     {
       sprintf( output_filename, DDefaultOutputFilename, it );
       life_write( output_filename, m, n, size, balance,  rbuf, rank, PARALLEL );
     }
     free(down);
     free(up);

   }
  }

  free(grid);

  if(rank == 0)
  {
    double time_spent = MPI_Wtime() - begin_time;
    printf("Array: %dx%d. CPU's: %d. Iteraciones %d. Total time spent: %f s.\n", n, m, initial_size, it_max, time_spent);
  }

  MPI_Finalize();
  return 0;


}

//Returns a new grid with UP&DOWN borders
int *join_borders_with_grid(int m, int n, int up_border[], int down_border[], int grid[])
{
  int i, j;
  int aux = n + (m * 2);
  int *new_grid = ( int * ) malloc ( aux * sizeof ( int ) );

  for (i = 0; i < m; i++) {
    new_grid[i] = up_border[i];
  }
  for(i = m; i < n + m; i++)
  {
    new_grid[i] = grid[i -(m)];
  }
  for (i = 0; i < m; i++) {
    new_grid[i + n + m] = down_border[i];
  }

  return new_grid;
}

int *states_workout(int m, int column_size, int new_grid[], int type)
{
  int i, j;
  int i_prev, i_next, j_prev, j_next;
  int *s = ( int * ) malloc ( column_size * m * sizeof ( int ) );

  int initial_row = (type == PARALLEL) ? 1 : 0;
  int final_row = (type == PARALLEL) ? (column_size - 1) : column_size;

  for ( j = initial_row; j < final_row; j++) {
    for ( i = 0; i < m; i++) {

      i_prev = (1 <= i) ? i - 1 : (m - 1);
      i_next = (i < (m - 1)) ? i + 1 : 0;

      j_prev = (1 <= j) ? (j - 1) : (column_size - 1);
      j_next = (j < (column_size - 1)) ? j + 1 : 0;

      s[i+(j)*(m)] =
          new_grid[i_prev+(j_prev)*(m)] + new_grid[i_prev+j*(m)] + new_grid[i_prev+(j_next)*(m)]
        + new_grid[i  +(j_prev)*(m)]                     + new_grid[i  +(j_next)*(m)]
        + new_grid[i_next+(j_prev)*(m)] + new_grid[i_next+j*(m)] + new_grid[i_next+(j_next)*(m)];

    }
  }
  return s;
}

int *life_update_parallel(int m, int n, int grid[], int up_fronter[], int down_fronter[])
{
  int *s;
  int i, j;
  int aux = n + (m * 2);
  int *new_grid = ( int * ) malloc ( aux * sizeof ( int ) );

  new_grid = join_borders_with_grid(m, n, up_fronter, down_fronter, grid);


  s = ( int * ) malloc ( aux * sizeof ( int ) );
  s = states_workout(m, aux / m, new_grid, PARALLEL);

  life_update_sub_array(m, aux / m, new_grid, s, PARALLEL);

  int *luis = ( int * ) malloc(n * sizeof(int));
  luis = substract_grid(n, m, new_grid);

  return luis;

}

//LIFE_UPDATE updates a Life grid of the sub-array.
void life_update_sub_array(int m, int n, int new_grid[], int s[], int type)
{
  int i, j;
  int initial_row = (type == PARALLEL) ? 1 : 0;
  int final_row = (type == PARALLEL) ? (n - 1) : n;

  for ( j = initial_row; j < final_row; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      if ( new_grid[i+j*(m)] == 0 )
      {
        if ( s[i+(j)*m] == 3 )
        {
          new_grid[i+j*(m)] = 1;
        }
      }
      else if ( new_grid[i+j*(m)] == 1 )
      {
        if ( s[i+(j)*m] < 2 || 3 < s[i+(j)*m] )
        {
          new_grid[i+j*(m)] = 0;
        }
      }
    }
  }

}

//Returns the initial grid without the UP&DOWN borders
int *substract_grid(int n, int m, int new_grid[])
{
  int j;
  int *final_grid = ( int * ) malloc(n * sizeof(int));
  for ( j = 0; j < n; j++) {
    final_grid[j] = new_grid[j + m];
  }
  return final_grid;
}

//Returns the id of the process that contains the UP border
int up_dest(int rank, int size)
{
  return (rank == 0) ? (size - 1) : (rank - 1);
}

//Returns the id of the process that contains the DOWN border
int down_dest(int rank, int size)
{
  return (rank == size - 1) ? 0 : (rank + 1);
}

//Creates UP and DOWN border for each process
void new_borders(int m, int size_of_recv, int up[], int down[])
{
  int i;
  for(i = 0; i < m; i++)
  {
    up[i] = rbuf[i];
    down[m - 1 - i] = rbuf[size_of_recv - 1 -i];
  }
}

//Divdes the initial array in sub arrays for each process
void divide_rows(int size, int m, int n)
{
  int i, aux;

  balance = ( int * ) malloc ( m  * sizeof ( int ) );
  displs = ( int * ) malloc ( m  * sizeof ( int ) );

  for (i = 0; i < size; i++) {
    balance[i] = 0;
  }
  i = 0;
  while(m != 0)
  {
    balance[i] += 1;
    m--;
    i++;
    if(i == size)
      i=0;
  }
  aux = 0;
  for (i = 0; i < size; i++) {
    displs[i] = aux;
    balance[i] = balance[i] * n;
    aux += balance[i];
  }

}

//TIMESTAMP prints the current YMDHMS date as a time stamp. Example:  31 May 2001 09:45:54 AM
void timestamp ( void )
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  fprintf ( stdout, "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}

//LIFE_INIT initializes the life grid.
int *life_init ( char *filename, double prob, int m, int n, int *seed )
{
  int *grid, *gridamen;
  int i;
  int j;
  double r;

  grid = ( int * ) malloc ( m  * n  * sizeof ( int ) );
  if (grid==NULL)
    perror("Error malloc grid:");

  if (filename!=NULL)
  {
      /* Read input file */
      life_read(filename, m, n, grid);
  } else {
    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < m; i++ )
      {
        grid[grid[i + j * (m)]] = 0;
      }
    }

    gridamen = ( int * ) malloc ( m  * n  * sizeof ( int ) );
    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < m; i++ )
      {
        r = r8_uniform_01 ( seed );
        if ( r <= prob )
        {
          gridamen[i + j * (m)] = 1;
        } else {
          gridamen[i + j * (m)] =   grid[i + j * (m)];
        }
      }
    }

    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < m; i++ )
      {
        grid[i + j * (m)] =   gridamen[i + j * (m)];
      }
    }
  }

  return grid;
}

//LIFE_WRITE writes a grid to a file using MPI_FILE_WRITE.
void life_write( char *output_filename, int m, int n, int tasks, int scounts[], int recv[], int rank, int type)
{
  int i, j, k;
  if (type == PARALLEL)
  {

    MPI_File myfile;
    //Array with quantity of '\n' that contain every process
    int lines[tasks];

    for (i=0; i<tasks;++i)
    lines[i] =  scounts[i]/n;

    char str[scounts[rank]+lines[rank]];
    int flag = n;
    int cont = 1;

    //create char array with files assigned to one process
    for (i = 0, k = 0; i < scounts[rank] + lines[rank] ; ++i, ++k)//hasta 30, 30, 20, 20
    {
      if(i == flag){//10, 21, 32
        str[i] = '\n';
        flag = flag + (n+1);
        k-=1;
      }else{
        str[i] = recv[k] + '0'; //10, 19, 29
      }


    }

    //add white spaces to char array
    char final_str[(scounts[rank] + lines[rank]) * 2];
    for(i = 0 , k = 0 ;i < (scounts[rank] + lines[rank] ) * 2; i++)
    {
      if(i % 2 == 0){
        final_str[i] = ' ';
      }else{
        final_str[i] = str[k];
        k++;
      }
    }

    //Calculate displacements
    int aux = 0;
    displs[0] = 0;

    for(i = 1 ;i<tasks; i++)
    {
      aux += (scounts[i-1] + lines[i-1]) * 2;
      displs[i] = aux;
    }

    MPI_File_open (MPI_COMM_WORLD, output_filename, MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL, &myfile);
    MPI_File_set_view(myfile, displs[rank],MPI_CHAR, MPI_CHAR, "native",MPI_INFO_NULL);
    MPI_File_write(myfile, final_str, (scounts[rank] + lines[rank]) * 2 * sizeof(char), MPI_CHAR,MPI_STATUS_IGNORE);
    MPI_File_close(&myfile);

  } else {
    FILE *output_unit;
    /*Open the file.*/
    output_unit = fopen ( output_filename, "wt" );

    /*Write the data.*/
    for ( j = 0; j < n ; j++ )
    {
      for ( i = 0; i < m ; i++ )
      {
        fprintf ( output_unit, " %d", recv[i+j*(m)] );
      }
      fprintf ( output_unit, "\n" );
    }

    /*Close the file.*/
    fclose ( output_unit );

  }
  return;

}

//LIFE_READ reads a file to a grid.
void life_read ( char *filename, int m, int n, int grid[] )
{
  int i;
  int j;
  FILE *input_unit;
/*
  input the file.
*/
  input_unit = fopen ( filename, "rt" );
  if (input_unit==NULL)
    perror("Reading input file:");

  /*Read the data.*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      fscanf ( input_unit, "%d", &(grid[i + j * (m)]));
    }
  }
  fclose(input_unit);

  return;
}

//R8_UNIFORM_01 returns a pseudorandom R8 scaled to [0,1].
double r8_uniform_01 ( int *seed )
{
  int i4_huge = 2147483647;
  int k;
  double r;

  k = *seed / 127773;

  *seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

  if ( *seed < 0 )
  {
    *seed = *seed + i4_huge;
  }

  r = ( ( double ) ( *seed ) ) * 4.656612875E-10;

  return r;
}
