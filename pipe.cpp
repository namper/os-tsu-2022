#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>


class MyPipe{


    public: 
        FILE * tmp;
        char * temp_file_name;

        MyPipe(){
            this->temp_file_name = "temp.txt";
        }

       size_t fwrite(char mass[], size_t size){
            if(tmp == NULL){
                tmp = fopen("temp.txt", "w+");
            }

            return fputs(mass, tmp);
       }

       size_t fread(char mass[], size_t size){
           // read from read descriptor
            if(tmp == NULL){
                tmp = fopen("temp.txt", "r");
            }

           return fscanf(tmp, "%s", mass);
       }

       void frclose(){
            fclose(tmp);
       }

       void fwclose(){
           fclose(tmp);
       }


       void pclose(){
            remove(temp_file_name);
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

     }

    pipeline.pclose();

    return 0;
}

