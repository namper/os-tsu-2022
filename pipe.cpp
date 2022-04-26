#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>


class MyPipe{

    public:
        int fd[2];

       MyPipe(){}

       void popen(){
           if (pipe(this->fd) < 0){
              printf("Could not create pipe");
              exit(EXIT_FAILURE);
           }
       }

       size_t fwrite(char mass[], size_t size){
            size_t writed_size = write(this->fd[1], mass, size);
            return writed_size;
       }

       size_t fread(char mass[], size_t size){
           // read and close read descriptor
          size_t read_size = read(this->fd[0], mass, size);
          return read_size;
       }

       void frclose(){
           close(fd[0]);
       }

       void fwclose(){
          close(fd[1]);
       }

};


void safe_ask_user(int * input, char * text){
    // ask user bool question
    while(*input > 2 || *input < 1){
        printf("%s", text);
        scanf("%d", input);

        if((*input > 2 || *input < 1)){
            printf("\nPlease provide number between 1, 2\n");
        }else{
            break;
        }
    }

}

void read_file(FILE *fptr, char * file_name, char * buff){
   // read data from file
   fptr = fopen(file_name, "r");

   if(fptr == NULL)
   {
      printf("Error!");
      exit(1);
   }

   fscanf(fptr, "%s", buff);
   fclose(fptr);
}


void write_file(FILE *ofptr, char * file_name, char * buff){
   // read data from file
   ofptr = fopen(file_name, "w+");

   if(ofptr == NULL)
   {
      printf("Error!");
      exit(1);
   }

   fputs(buff, ofptr);
   fclose(ofptr);
}


int main()
{

    int pid;
    size_t s1, s2;
    char filename[1024];
    char fileout[1024];
    char mass[1024];
    FILE *fptr;
    FILE *ofptr;
    int direction = 0, data_input_dest = 0, data_output_dest = 0;

    // ask for data direction
    safe_ask_user(&direction, "Enter direction of data [1 - forward | 2 - backward]: ");

    // ask data input destination
    safe_ask_user(&data_input_dest, "Choose input from [1 - file | 2 - console]: ");
    if (data_input_dest == 1){
        printf("Enter file name: ");
        scanf("%s", filename);
        read_file(fptr, filename, mass);

    }
    else{
        printf("Enter input data: ");
        scanf("%s", mass);
     }

    s2 = sizeof(mass);
    char re_mass[s2];



    safe_ask_user(&data_output_dest, "Choose output to [1 - file | 2 - console]: ");
    if (data_output_dest == 1){
        printf("Enter output file name: ");
        scanf("%s", fileout);
    }



    MyPipe pipeline = MyPipe();
    pipeline.popen();

    if ((pid = fork()) > 0){
        if(direction == 1){
            // parrent process
            s1 = pipeline.fwrite(mass, s2);
            pipeline.fwclose();
        }else{
            s1 = pipeline.fread(re_mass, s2);
            pipeline.frclose();

            if (data_output_dest == 1){
             write_file(ofptr, fileout, re_mass);
            }
            else{
                printf("parent: ");
                printf("%s\n", re_mass);
           }

        }

        if ( s1 != s2 ){
            printf("Failed to perform action on pipeline\n");
            exit(EXIT_FAILURE);
        }

        wait(NULL);

    }else{
        // child process
        if(direction == 1){
            s1 = pipeline.fread(re_mass, s2);
            pipeline.frclose();

            if (data_output_dest == 1){
                write_file(ofptr, fileout, re_mass);
            }
            else{
                printf("child: ");
                printf("%s\n", re_mass);
           }

        }
        else{
            s1 = pipeline.fwrite(mass, s2);
            pipeline.fwclose();
        }
        if (s1 != s2){
            printf("Failed to perform action on pipeline\n");
            exit(EXIT_FAILURE);
        }

     }

    return 0;
}

