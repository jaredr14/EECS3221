/***********************************************************************/
/**      Author: Jared Rego                                           **/
/**        Date: Jul. 2019                                            **/
/** Description: Assgn. III                                           **/
/***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "array.h"
#include "functions.h"

/**********************************************************************/
/********************       O P E N  A R R A Y     ********************/
/**********************************************************************/
void open_array(char *filename, array_t *arrayp, int *sizep)
{	/* Opens array from file filename and */
  /* returns its pointer and size */
  /* size is the size of the file divided by the size of the array struct */
  /* pointer array is obtained with mmap */

  int size, size_struct, fd, length;
  struct stat sb;

  size_struct = (int)(sizeof(**arrayp));

  if((fd = open(filename, O_RDWR, 0777)) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to create file \n");
  }
  if((ftruncate(fd, (int)index*size_struct)) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to truncate file \n");
  }
  if((fstat(fd, &sb)) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to get file status\n");
  }

  length = sb.st_size;
  size = length/size_struct;

  *arrayp = (array_t)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(arrayp == MAP_FAILED)
  {
    fatalerr("mmaparray", 0, "Failed mmap process\n");
  }
  close(fd);
}

/**********************************************************************/
/********************     C L O S E  A R R A Y     ********************/
/**********************************************************************/
void close_array(array_t *arrayp, int size)
{				/* Unmaps array and sets it to NULL */
  munmap(*arrayp, size);
  arrayp = NULL;
}

/**********************************************************************/
/********************    C R E A T E  A R R A Y    ********************/
/**********************************************************************/
void create_array(char *filename, int index, array_t *arrayp)
{	/* Creates a file with an array with index */
  /* elements, all with the valid member false. */
  /* It is fatal error if file exists */
  /* size is the size of the file divided by the size of the array struct */
  /* pointer array is obtained with mmap */

  int size, size_struct, fd, length;
  int i = 0;
  struct stat sb;

  size_struct = (int) (sizeof(**arrayp));

  if((fd = open(filename, O_RDWR |  O_CREAT | O_EXCL, 0777)) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to create file \n");
  }
  if(ftruncate(fd, (int)index*size_struct) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to truncate file \n");
  }
  if(fstat(fd, &sb) < 0)
  {
    fatalerr("mmaparray", 0, "Failed to get file status\n");
  }

  length = sb.st_size;
  size = length/size_struct;

  *arrayp = (array_t)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(arrayp == MAP_FAILED)
  {
    fatalerr("mmaparray", 0, "Failed mmap process\n");
  }

  while(i < index)
  {
    (*arrayp)[i].valid = 0;
    i++;
  }
  close(fd);
}


/**********************************************************************/
/********************       S E T  E N T R Y       ********************/
/**********************************************************************/
void set_entry(array_t array, char *name, int index, float age)
{				/* Sets the elements of the index'ed */
        /* entry to name and age and the valid */
        /* member to true */
  array[index].index = index;
  strncpy(array[index].name, *name, (int)sizeof(name));
  array[index].age = age;
  array[index].valid = 1;
}

/**********************************************************************/
/********************       G E T  E N T R Y       ********************/
/**********************************************************************/
void get_entry(array_t array, char **name, int index, float *age)
{				/* Returns the index'ed entry's name and age */
                                /* if valid, fatal error o/w */
  array[index].index = index;
  if(array[index].valid == 1)
  {
    **name = array[index].name;
    *age = array[index].age;
  }
  else
  {
    fatalerr("No such entry exists for ", index, "\n");
  }
}

/**********************************************************************/
/********************   D E L E T E   E N T R Y    ********************/
/**********************************************************************/
void delete_entry(array_t array, int index)
{				/* Sets the valid element of the index'ed */
                                /* entry to false */
  array[index].valid = 0;
}

/**********************************************************************/
/********************     P R I N T   A R R A Y    ********************/
/**********************************************************************/
void print_array(array_t array, int size)
{				/* Prints all entries with valid member true */
                                /* using the same format as in the main */
  int i;
  for(i = 1; i < size; ++i)
  {
    if(array[i].valid == 1)
    {
      printf("%d " , array[i]);
    }
  }
  printf("\n");
}
