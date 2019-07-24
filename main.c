#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "stack.h"
#include <errno.h>

struct f_state {
  char dir_path[4096];
  char f_name[255];
  time_t t_mtime;
};

typedef struct f_state f_state;

static int c_mem = 100;


enum Filetypes { File_type = 1,Dir_type = 4};


f_state* increase_size(f_state* list)
{
  f_state* tmp;
  c_mem+=100;
  //printf("Increasing buffer %d \n",c_mem);
  tmp = (f_state*) realloc(list, sizeof(f_state)*c_mem);
  return tmp;
}

int
exp_curdir()
{
  struct stat sb;
  struct dirent *entry;
  f_state *registry = malloc(sizeof(f_state)*c_mem);
  char dir_name[255],dir_path[4096]; //maximum length of path in linux 4096
  char top_dir[255];
  int n_files=0;
  DIR *dir;

  
  //create a stack to store the dir names
  node *dir_list = NULL;
  
  if(!getcwd(dir_name,255))
    {
      perror("getcwd");
      exit(EXIT_FAILURE);
    }

  errno = 0;
  printf("Printing the contents of the current directory\n");
  
  init_stack(dir_list);
  
  dir_list = push(dir_list,dir_name);
  
  while(!empty(dir_list))
    {

      dir_list = pop(dir_list,top_dir);
      printf("Opening directory %s\n",top_dir);
      errno = 0;
      dir = opendir(top_dir);
      
      if(errno)
	{
	  perror(top_dir);
	  exit(EXIT_FAILURE);
	}
      
      while((entry=readdir(dir)) != NULL){
	if((strcmp(entry->d_name,"..")!=0) && (strcmp(entry->d_name,".")!=0) )
	  {
	    printf("%s - %d\n",entry->d_name,entry->d_type);
	    strcpy(registry[n_files].f_name,entry->d_name);
	    n_files++;

	    if(n_files % 100 == 0) registry = increase_size(registry);
	     
	    if(entry->d_type == Dir_type)
	      {
		strcpy(dir_path,top_dir);
		strcat(dir_path,"/");
		strcat(dir_path,entry->d_name);
		dir_list = push(dir_list,dir_path);
	      }
	  }
      }
	
      
      
    }
  printf("Total number of files: %d \n",n_files);
  
}


int
main(int argc, char *argv[])
{

  node *dir_list;
  char dir_name[255];
  int test;


  /* strcpy(registry[0].f_name,"hola"); */
  /* strcpy(registry[1].f_name,"hola"); */
  
  init_stack(dir_list);
  
  dir_list = push(dir_list,"hello");
  dir_list = push(dir_list,"Cristian");
  dir_list = pop(dir_list,dir_name);
  dir_list = pop(dir_list,dir_name);


  /* ret = stat("hello.txt", &sb); */
  /* if(ret){ */
  /*   perror("stat"); */
  /*   printf("Problem reading the file \n"); */
  /*   return 1; */
  /* } */

  printf("Initialization good %\n");
  printf("First dirname %s\n",dir_name);
  exp_curdir();
  /* printf("Number of bytes: %ld \n",sb.st_size); */
  /* printf("Last time acces: %ld \n",sb.st_mtime); */


  
  /* printf("Current directory: %s\n",dir_name); */
  
  /* printf("Lenght of directory name: %d\n",strlen(dir_name)); */

  return 0;
}
