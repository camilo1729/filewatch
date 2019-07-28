#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "stack.h"
#include <errno.h>
#include <sys/queue.h>

#ifdef DEBUG
#define DEBUG_PRINT(fmt,args...) fprintf(stderr,"DEBUG: %s:%d:%s(): "fmt, __FILE__,__LINE__,__func__,## args)
#else
#define DEBUG_PRINT(fmt,args...)
#endif

struct f_state {
  char full_name[4352];//path + file_name 4096+256
  time_t last_access;
  LIST_ENTRY(f_state) entries;
};
typedef struct f_state f_state;


//link list histoy of file system
LIST_HEAD(listhead_history, f_state) head_list_history = LIST_HEAD_INITIALIZER(head_list_history);

void add_to_history(char *top_dir,struct dirent *entry);


static int c_mem = 100;


enum Filetypes { File_type = 1,Dir_type = 4};


f_state* increase_size(f_state* list)
{
  f_state* tmp;
  c_mem+=100;
  tmp = (f_state*) realloc(list, sizeof(f_state)*c_mem);
  return tmp;
}

/* void copy_state() */
/* { */
/*   f_state *element,*h_element; */
  
/*   LIST_FOREACH(element, &head_list_fstate,entries){ */
/*     h_element = malloc(sizeof(f_state)); */
/*     strcpy(h_element->dir_path,element->dir_path); */
/*     strcpy(h_element->f_name,element->f_name); */
/*     h_element->last_access = h_element->last_access; */
/*     LIST_INSERT_HEAD(&head_list_history, h_element, entries); */
/*   } */
  
/* } */

void print_state()
{
  f_state *element;
  LIST_FOREACH(element, &head_list_history,entries){
    printf("%s : ",element->full_name);
    printf("%d \n",element->last_access);
  }

}


int search_entry(char *top_dir,struct dirent *entry)
{
  f_state *element;
  struct stat file_s;
  f_state *f_data;
  char f_full_path[4096];
  strcpy(f_full_path,top_dir);
  strcat(f_full_path,"/");
  strcat(f_full_path,entry->d_name);
  int f_found = 0;
  
  LIST_FOREACH(element, &head_list_history,entries){
    if(strcmp(element->full_name,f_full_path)==0)
      {
	f_found=1;
	stat(f_full_path,&file_s);
	if(element->last_access != file_s.st_mtime)
	  {
	    printf("CHANGED: %s \n",f_full_path);
	    DEBUG_PRINT("CHANGED: %s -> ht: %d, st: %d\n",f_full_path,element->last_access,file_s.st_mtime);
	    LIST_REMOVE(element,entries);
	    //update history	    
	    add_to_history(top_dir,entry);//probably we should just call LIST INSERT directly
	    return 0;  
	  }
	   
      }
    
  }
  
  if(!f_found)
    {  //new file
      printf("NEW: %s \n",f_full_path);
      add_to_history(top_dir,entry);
    }

}

void scan_history()
{
  f_state *element;
  struct stat file_s;
  int ret;
  LIST_FOREACH(element,&head_list_history,entries){
    ret = stat(element->full_name,&file_s);
    if(ret)
      {
	printf("DELETED: %s\n",element->full_name);
	LIST_REMOVE(element,entries);
      }
  }
}

void add_to_history(char *top_dir,struct dirent *entry)
{
  struct stat file_s;
  f_state *f_data;
  char f_full_path[4096];
  f_data = malloc(sizeof(f_state));
  strcpy(f_full_path,top_dir);
  strcat(f_full_path,"/");
  strcat(f_full_path,entry->d_name);
  
  strcpy(f_data->full_name,f_full_path);
  stat(f_full_path,&file_s); //not checking that the file does not exist
  f_data->last_access = file_s.st_mtime;
  LIST_INSERT_HEAD(&head_list_history, f_data, entries);
  f_data=NULL;
}


void process_entry(char *top_dir,struct dirent *entry,int init_h)
{
  if(init_h==1)
    {
      add_to_history(top_dir,entry);
      
    }else
    {
      search_entry(top_dir,entry);
      /* if(result >0) */
      /* 	{ */
      /* 	  printf("CHANGED: %s \n",f_full_path); */
      /* 	} */
      //0 no change, <0 deleted, >0 changed
    }
    
}


//action : 1=> add to history 0=> scan
int dir_state(int action, char *dir_to_scan)
{
  struct stat sb;
  struct dirent *entry;
  f_state *registry = malloc(sizeof(f_state)*c_mem);
  f_state *f_data; 
  char dir_path[4096]; //maximum length of path in linux 4096
  char top_dir[255];
  int n_files=0;
  DIR *dir;

  
  //create a stack to store the dir names
  node *dir_list = NULL;
  
  errno = 0;
  DEBUG_PRINT("Printing the contents of root directory %s\n",dir_to_scan);
  
  init_stack(dir_list);
  
  dir_list = push(dir_list,dir_to_scan);

  strcpy(top_dir,dir_to_scan);
  
  while(!empty(dir_list))
    {

      dir_list = pop(dir_list,top_dir);
      DEBUG_PRINT("Opening directory %s\n",top_dir);
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
	    
	    process_entry(top_dir,entry,action);

	    n_files++;
	     
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
  DEBUG_PRINT("Total number of files: %d \n",n_files);
  
}


void init_history(char *dir_name)
{
  dir_state(1,dir_name);
  //copy state to history
}

int main(int argc, char *argv[])
{

  node *dir_list;
  char dir_name[4096];
  int test;
  int dir_fd;
  init_stack(dir_list);
  LIST_INIT(&head_list_history);
  
  printf("Initialization of stack good %\n");


  if(argc>1)
    {
      dir_fd = open(argv[1],O_RDONLY);
      if(dir_fd == -1)
	{
	  perror("Directory not found");
	  exit(EXIT_FAILURE);
	}
      printf("Scanning directory %s: \n",argv[1]);
      strcpy(dir_name,argv[1]);
    }
  
  init_history(dir_name);
  while(1)
    {
      dir_state(0,dir_name);
      scan_history();
      sleep(10);
    }
  //print_state();
  /* printf("Number of bytes: %ld \n",sb.st_size); */
  /* printf("Last time acces: %ld \n",sb.st_mtime); */
  /* printf("Current directory: %s\n",dir_name); */  
  /* printf("Lenght of directory name: %d\n",strlen(dir_name)); */

  return 0;
}
