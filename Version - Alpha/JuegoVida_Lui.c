#include "mpi.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <string.h>

#define DDefaultOutputFilename "./Life_%04d.txt"
#define DNumIterForPartialResults 10

//void filename_inc ( char *filename );
int *life_init ( char *filename, double prob, int m, int n, int *seed );
void life_update ( int m, int n, int grid[] );
void life_write ( char *output_filename, int m, int n, int grid[] );
void life_read ( char *input_filename, int m, int n, int grid[] );
double r8_uniform_01 ( int *seed );
//int s_len_trim ( char *s );
void timestamp ( void );

int main(int argc, char *argv[]) {

  char *initial_file=NULL, output_filename[100];
  int it;
  int it_max=25;
  int m=100;
  int n=100;
  int *grid;
  double prob;
  int i,j;
  int seed;
  int to_send = 0;

  int rank;
  int tasks;
  int *sendbuf, *scounts, *recv;


  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks);

  MPI_Status status;

  timestamp ( );
  /*printf ( "\n" );
  printf ( "LIFE_SERIAL\n" );
  printf ( "  C version\n" );
  printf ( "  Carry out a few steps of John Conway's\n" );
  printf ( "  Game of Life.\n" );
  printf ( "  Parameters: life_game [FicheroEstadoInicial] [TamañoX] [TamañoY] [NumIteraciones].\n" );
  printf ( "\n" );*/

  if (argc>4)
    it_max = atoi(argv[4]);
  if (argc>3)
    n = atoi(argv[3]);
  if (argc>2)
    m = atoi(argv[2]);
  if (argc>1)
    initial_file = argv[1];
  else
    initial_file = NULL;

  prob = 0.20;
  seed = 123456789;

  if (rank == 0)
  {

    grid = life_init( initial_file, prob, m, n, &seed );
  //  printf("GRID: %s\n", &grid);
    //    printf("GRID2: %s\n", grid);
    printf("Tamaño del gridaso %d. Tamaño dividido entre ints %d\n", sizeof(grid), sizeof(grid) / sizeof(int));


    sendbuf = (int *)malloc(m * sizeof(int));
    scounts = (int *)malloc(tasks * sizeof(int));


    to_send = (m / tasks) * tasks;// 8

    for(i = 0 ;i<tasks; i++){// 10/4
      scounts[i] = m * ( m / tasks );

      if (to_send<m){
        to_send += 1;
        scounts[i] += m;
      }

    }

    recv = (int *)malloc(scounts[rank] * sizeof(int));

    for(i = 0 ;i<tasks; i++){// 10/4
      printf("SCOUNTS: %d \n", scounts[i]);
    }


   MPI_Scatterv(grid, scounts, MPI_INT, recv,
                scounts[i], MPI_INT, 0, MPI_COMM_WORLD);

  /*MPI_Scatterv( sendbuf, scounts, displs, MPI_INT, rbuf, 100, MPI_INT, root, comm);
  int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root,
MPI_Comm comm)*/
    /*
    int MPI_Scatter(void* sendbuf, int sendcount,
    MPI_Datatype sendtype, void* recvbuf, int
    recvcount, MPI_Datatype recvtype, int root,
    MPI_Comm comm);

    - sendbuf dirección datos envío (solo para root)
    – sendcount número elementos enviados a cada proceso (solo para root)
    – sendtype tipo MPI de los datos enviados (solo para root)
    – recvbuf dirección recepción datos
    – recvcount número elementos recibidos por cada proceso
    – recvtype tipo MPI de los datos recibidos
    – root rango (identificador) del proceso emisor (root)
    – comm comunicador MPI de los procesos implicados
    */



    /*for ( it = 0; it <= it_max; it++ )
    {
      if ( it == 0 )
      {
        grid = life_init ( initial_file, prob, m, n, &seed );
      }
      else
      {
        life_update ( m, n, grid );
      }
      if ((it%DNumIterForPartialResults)==0)
      {
        sprintf(output_filename,DDefaultOutputFilename,it);
        life_write ( output_filename, m, n, grid );
        printf ( "  %s\n", output_filename );
      }
    }

    sprintf(output_filename,DDefaultOutputFilename,it-1);
    life_write ( output_filename, m, n, grid );
    /*
    Free memory.
    */
    //free ( grid );
    /*
    Terminate.
    */
    /*printf ( "\n" );
    printf ( "LIFE_SERIAL\n" );
    printf ( "  Normal end of execution.\n" );
    printf ( "\n" );
    timestamp ( );

    return 0;*/

  } else {
    printf("Soy el proceso %d. Y no hago verga \n",rank);
  }
  MPI_Finalize();
  return 0;


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
  int *grid;
  int i;
  int j;
  double r;


  grid = ( int * ) malloc ( ( m ) * ( n ) * sizeof ( int ) );
  if (grid==NULL)
    perror("Error malloc grid:");

  if (filename!=NULL)
  {

      printf("Reading Input filename %s\n",filename);
      life_read (filename, m, n, grid);
  }
  else
  {
    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < m; i++ )
      {
        grid[i+j*(m)] = 0;
      }
    }

    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < m; i++ )
      {

        r = r8_uniform_01 ( seed );

              //  printf("R: %lu\n", r);
        if ( r <= prob )
        {
          grid[i+j*(m)] = 1;
        }
      }
    }
  }

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      printf("%d ",  grid[i+j*(m)]);
    }
    printf("\n");
  }

  return grid;
}

//LIFE_UPDATE updates a Life grid.
void life_update ( int m, int n, int grid[] )
{
  int i;
  int j;
  int *s;
  int i_prev, i_next, j_prev, j_next;

  s = ( int * ) malloc ( m * n * sizeof ( int ) );

  for ( j = 1; j <= n; j++ )
  {
    for ( i = 1; i <= m; i++ )
    {
      i_prev = (1 < i) ? i - 1 : m;
      i_next = (i < m ) ? i + 1 : 1;
      j_prev = (1 < j) ? j - 1 : n;
      j_next = (j < n) ? j + 1 : 1;
      s[i-1+(j-1)*m] =
          grid[i_prev+(j_prev)*(m)] + grid[i_prev+j*(m)] + grid[i_prev+(j_next)*(m)]
        + grid[i  +(j_prev)*(m)]                     + grid[i  +(j_next)*(m)]
        + grid[i_next+(j_prev)*(m)] + grid[i_next+j*(m)] + grid[i_next+(j_next)*(m)];
    }
  }
/*
  Any dead cell with 3 live neighbors becomes alive.
  Any living cell with less than 2 or more than 3 neighbors dies.
*/
  for ( j = 1; j <= n; j++ )
  {
    for ( i = 1; i <= m; i++ )
    {
      if ( grid[i+j*(m)] == 0 )
      {
        if ( s[i-1+(j-1)*m] == 3 )
        {
          grid[i+j*(m)] = 1;
        }
      }
      else if ( grid[i+j*(m)] == 1 )
      {
        if ( s[i-1+(j-1)*m] < 2 || 3 < s[i-1+(j-1)*m] )
        {
          grid[i+j*(m)] = 0;
        }
      }
    }
  }

  free ( s );

  return;
}

//LIFE_WRITE writes a grid to a file.
void life_write( char *output_filename, int m, int n, int grid[] )
{
  int i;
  int j;
  FILE *output_unit;
/*
  Open the file.
*/
  output_unit = fopen ( output_filename, "wt" );
/*
  Write the data.
*/
  for ( j = 0; j < n ; j++ )
  {
    for ( i = 0; i < m ; i++ )
    {
      fprintf ( output_unit, " %d", grid[i+j*(m)] );
    }
    fprintf ( output_unit, "\n" );
  }
/*
  Close the file.
*/
  fclose ( output_unit );

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
/*
  Read the data.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      fscanf ( input_unit, "%d", &(grid[i+j*(m)]));
    }
  }
/*
  Close the file.
*/
  fclose ( input_unit );

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
