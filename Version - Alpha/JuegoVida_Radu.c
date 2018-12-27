#include "mpi.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <string.h>

#define DDefaultOutputFilename "./Life_%04d.txt"
#define DNumIterForPartialResults 1


//void filename_inc ( char *filename );
void new_borders(int m, int size_of_recv, int up[], int down[]);
void divide_rows(int size, int m, int n);
int *life_init ( char *filename, double prob, int m, int n, int *seed );
void life_update ( int m, int n, int grid[] );
void life_write ( char *output_filename, int m, int n, int grid[] );
void life_read ( char *input_filename, int m, int n, int grid[] );
double r8_uniform_01 ( int *seed );
//int s_len_trim ( char *s );
void timestamp ( void );

//int rows=100;
int *balance, *displs, *rbuf;

int main(int argc, char *argv[]) {

  char *initial_file=NULL, output_filename[100];
  int it;
  int it_max=25;
  int m=10;
  int n=10;
  int *grid;
  int *up, *down;
  int i;
  double prob;
  int seed;
  int to_send = 0;

  int rank;
  int size;


  MPI_Status status;
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  timestamp ( );
  printf ( "\n" );
  /*printf ( "LIFE_SERIAL\n" );
  printf ( "  C version\n" );
  printf ( "  Carry out a few steps of John Conway's\n" );
  printf ( "  Game of Life.\n" );
  printf ( "  Parameters: life_game [FicheroEstadoInicial] [TamañoX] [TamañoY] [NumIteraciones].\n" );
  printf ( "\n" );*/

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

  /* Apartado MPI_Type_Vector*/
  //int vector[M][M];
  MPI_Datatype coltype;
  MPI_Type_vector(m, 1, 1, MPI_INT, &coltype);
  MPI_Type_commit(&coltype);

  prob = 0.20;
  seed = 123456789;

  grid = life_init( initial_file, prob, m, n, &seed );
  divide_rows(size, n, m);
  rbuf = ( int * ) malloc(balance[rank]  * sizeof(int));
  //balance
  MPI_Scatterv(grid, balance, displs, MPI_INT, rbuf, balance[rank], MPI_INT, 0, MPI_COMM_WORLD);
  /*int MPI_Scatterv(const void *sendbuf, const int *sendcounts, const int *displs,
                 MPI_Datatype sendtype, void *recvbuf, int recvcount,
                 MPI_Datatype recvtype,
                 int root, MPI_Comm comm)*/

 up = ( int * ) malloc (m  * sizeof(int));
 down = ( int * ) malloc (m  * sizeof(int));

  if (rank == 0)
  {
    printf("-----------Proceso %d----------------\n", rank );
    /*for (i = 0; i < balance[rank]; i++) {
      //balance[i] = balance[i] * n;
      //displs[i] = (i == 0) ? 0 : balance[i-1];
      printf("%d ", rbuf[i]);
    }
    printf("\n" );
    printf("---------------------------\n" );*/
    new_borders(m, balance[rank], up, down);
    for (i = 0; i < m; i++) {
      /* code */
      printf("%d ", up[i]);
    }
    printf("Se acabo el up\n");

    if(MPI_Send(up, 1, coltype, 3, 66, MPI_COMM_WORLD) != MPI_SUCCESS){
      exit(1);
    }
    printf("-----Proceso %d-----\n", rank);
    printf("MPI_Type_vector enviado. \n");

    /*char buf[1000];
    int position = 0;
    MPI_Pack(&up, m, MPI_INT, buf, 1000, &position, MPI_COMM_WORLD);
    //MPI_Pack(&j, 10, MPI_ FLOAT, buff, 1000, &position, MPI_COMM_WORLD);
    MPI_Send( buf, position, MPI_PACKED, 6, 0, MPI_COMM_WORLD);
    //grid = life_init( initial_file, prob, m, n, &seed );
    //divide_rows(size, n, m);
    //printf("Tamaño del gridaso %d. Tamaño dividido entre ints %d\n", sizeof(grid), sizeof(grid) / sizeof(int));
    //to_send = 0;

    /*for ( it = 0; it <= it_max; it++ )
    {
      if ( it == 0 )
      {
        grid = life_init ( initial_file, prob, m, n, &seed );
      }
      else
      {
        life_update( m, n, grid );
        //it = it_max;
      }
      if ((it%DNumIterForPartialResults)==0)
      {
        sprintf(output_filename,DDefaultOutputFilename,it);
        life_write( output_filename, m, n, grid );
        printf( "  %s\n", output_filename );
      }

    }*

    //sprintf(output_filename,DDefaultOutputFilename,it-1);
    //life_write ( output_filename, m, n, grid );
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
    //printf("Soy el proceso %d. Y no hago verga \n",rank);
    printf("-----------Proceso %d---------------\n", rank);
    new_borders(m, balance[rank], up, down);

    if (rank == 3)
    {
      //int position;
      /*char buff[1000];
      //MPI_Recv( buf, 1000, MPI_PACKED, 0, 0, &status);
      if(MPI_Recv(buff, 1000, MPI_PACKED, 0, 66, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
        exit(2);
      }
      int position = 0;
      int up_fronter[m];
      MPI_Unpack(buff, 1000, &position, &up_fronter, m, MPI_INT, MPI_COMM_WORLD);*/
      int up_fronter[m];
      if(MPI_Recv(&up_fronter, 1, coltype, 0, 66, MPI_COMM_WORLD, &status) != MPI_SUCCESS){
        exit(2);
      }
      printf("-----Proceso %d-----\n", rank);
      printf("MPI_Type_vector recibido. \n");
      //printf("Frontera de arriba:\n");
      for (i = 0; i < m; i++) {

        printf("%d", up_fronter[i]);
      }
      printf("\n");
    }

    /*for (i = 0; i < balance[rank]; i++) {
      //balance[i] = balance[i] * n;
      //displs[i] = (i == 0) ? 0 : balance[i-1];
      printf("%d ", rbuf[i]);
    }
    printf("-----------Besos---------------\n" );*/
  }
  MPI_Finalize();
  return 0;


}

void new_borders(int m, int size_of_recv, int up[], int down[])
{
  //int *up, *down;
  int i;

  //up = ( int * ) malloc (m  * sizeof(int));
  //down = ( int * ) malloc (m  * sizeof(int));

  printf("M: %d. Size: %d\n", m, size_of_recv);
  for(i = 0; i < m; i++)
  {
    //printf("%d\n", rbuf[i]);
    up[i] = rbuf[i];
    down[m - 1 - i] = rbuf[size_of_recv - 1 -i];
  }
  /*printf("Ahi va el up:\n");
  for(i = 0; i < m; i++)
  {
    printf("%d ", up[i]);
  }
  printf("\n");
  printf("Ahi va el down:\n");
  for(i = 0; i < m; i++)
  {
    printf("%d ", down[i]);
  }*/
}

void divide_rows(int size, int m, int n)
{
  balance = ( int * ) malloc ( m  * sizeof ( int ) );
  displs = ( int * ) malloc ( m  * sizeof ( int ) );
  printf("Tamaño %d. La m %d\n", size, m);
  int i, aux;
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
    //printf("%d ", balance[i]);
  }
  //printf("\n");
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

  //Cambialrlo por mpi_int saes
  grid = ( int * ) malloc ( m  * n  * sizeof ( int ) );
  if (grid==NULL)
    perror("Error malloc grid:");

  if (filename!=NULL)
  {
      /* Read input file */
      printf("Reading Input filename %s\n",filename);
      life_read(filename, m, n, grid);
  }
  else
  {
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
          //printf("Entro en la gaver\n");
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

//LIFE_UPDATE updates a Life grid.
void life_update ( int m, int n, int grid[] )
{
  int i;
  int j;
  int *s;
  int i_prev, i_next, j_prev, j_next;

  s = ( int * ) malloc ( m * n * sizeof ( int ) );

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      /* n-1 / m-1*/
      i_prev = (1 <= i) ? i - 1 : m - 1;
      i_next = (i < m - 1) ? i + 1 : 0;
      j_prev = (1 <= j) ? j - 1 : n - 1;
      j_next = (j < n - 1) ? j + 1 : 0;
      printf("I_: %d. J_: %d. i_prev: %d. i_next: %d. j_prev: %d. j_next: %d\n", i, j, i_prev, i_next, j_prev, j_next);
      s[i+(j)*(m)] =
          grid[i_prev+(j_prev)*(m)] + grid[i_prev+j*(m)] + grid[i_prev+(j_next)*(m)]
        + grid[i  +(j_prev)*(m)]                     + grid[i  +(j_next)*(m)]
        + grid[i_next+(j_prev)*(m)] + grid[i_next+j*(m)] + grid[i_next+(j_next)*(m)];
    }
  }

  /*for (j = 0; j < n; j++) {
    for ( i = 0; i < m; i++) {
      printf("%d ", grid[i + j * (m)]);
    }
    printf("\n");
  }*/

  /*for (j = 0; j < n; j++) {
    for ( i = 0; i < m; i++) {

      printf("%d ", s[i + j * (m)]);
    }
    printf("\n");
  }*/
/*
  Any dead cell with 3 live neighbors becomes alive.
  Any living cell with less than 2 or more than 3 neighbors dies.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      if ( grid[i+j*(m)] == 0 )
      {
        if ( s[i+(j)*m] == 3 )
        {
          grid[i+j*(m)] = 1;
        }
      }
      else if ( grid[i+j*(m)] == 1 )
      {
        if ( s[i+(j)*m] < 2 || 3 < s[i+(j)*m] )
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
      fscanf ( input_unit, "%d", &(grid[i + j * (m)]));
    }
  }
  /* Set the grid borderline to 0's */
  /*for ( j = 0; j <= n +1; j++ )
=======
      fscanf ( input_unit, "%d", &(grid[i+j*(m)]) );
    }
  }
  /* Set the grid borderline to 0's */
  /*
  for ( j = 0; j <= n +1; j++ )
>>>>>>> f8bc16f276d0b2b5503de7404c0ac14b916d0304
  {
    grid[0+j*(m+2)] = 0;
    grid[(m+1)+j*(m+2)] = 0;

  }
  for ( i = 1; i <= m; i++ )
  {
    grid[i+0*(m+2)] = 0;
    grid[i+(n+1)*(m+2)] = 0;
  }*/
/*
  Close the file.
*/
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
