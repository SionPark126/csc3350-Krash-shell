#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

//Function declaration
void cd (char* dir, char* output);
void help(char* output);
void pwd(char* output);
void otherCommands(char **, int, char*);

int main(int args,char *argv[] ){
  //variable to store current directory and user input
  char cdir[250];
  char input[256];

  printf("------------------Welcome to krash----------------------\n");

  start:
  //Print current working directory
  printf("%s", getcwd(cdir,250));
  printf("$Krash$ ");

  while(1){
    //fgets read a line from the specified stream and stores into
    //the string pointed by the first argument. The second argument is
    //for specifying the size of the string, and the third argument
    // is to specify stream.
    //It reads user input from standard input and stores into the "input" variable.
    fgets(input, sizeof input, stdin);

    //strtok takes user input (string) and separtes into tokens based on
    //the delimeters given (" ","\t","\n")
    //The variable token stores first token
    char *token = strtok (input, " \t\n");

    //Variable to store output stream. The default is set to stdout
    char* output ="stdout";
    //Variable to store all the tokens.
    char *arguments[20];
    //memset(void *str,int c, size_t n). It copies the character c to the
    //first n characters of the string pointed by the argument str.
    //The above function sets all the elements in arguments to 0. The arguments
    //variable is reset everytime user inputs a new command.
    memset(arguments, 0, sizeof arguments);

    //Variable to keep track of token numbers before ">"
    int index = 0;

    while (token != NULL) {
        //strcmp takes two strings and compares them. If the strings are the
        //same, it returns 0. If the first string > second string,
        //the returned value is > 0, and if the first string < second string,
        //the returned value is <0.
        //Check if token is ">"
       if (strcmp(token,">")==0){
         //if there is a redirection sign, then get the next token and save it
         //to the output variable.
         output = strtok(NULL," \t\n");

         //If the variable output is null or if there is another argument after output
         //then output error as we expect to have only one argument after ">".
         if (output == NULL || (strtok(NULL," \t\n") != NULL)){
           fprintf(stderr,"Wrong number of arguments after \'>\'. Please enter again.\n");
           //Go back to start
           goto start;
         }
       }
       else{ // If there is no redirection sign, save each token to arguments
          arguments[index] = token;
          index ++;
       }
       //Move to the next token
       token = strtok(NULL," \t\n");
   }

    //If there is only one argument and it is "exit",
    //Then end the shell program
    if ((index ==1) && (strcmp(arguments[0], "exit")==0)){
      printf("**************GOOD BYE**************\n");
      exit(0);
    }
    //If user inputs pwd, then call pwd function
    else if((index ==1) && strcmp(arguments[0],"pwd")==0){
      pwd(output);
    }
    //If user inputs cd, call cd function
    else if ((index ==1) && strcmp(arguments[0],"cd")==0){
      cd(arguments[1],output);
    }
    //If user inputs help, call help function
    else if( (index ==1) && strcmp(arguments[0],"help")==0){
      help(output);
    }
    else{
      //For non-built in functions, call otherCommands function
      otherCommands(arguments, index, output);
    }
  }
  return 0;
}

//Function to Change directory
void cd (char* dir, char* output){
  char cdir[250];
  //If the destination is not specified
  if (dir ==NULL){
    //getenv seraches for the environment string, in this case
    //"HOME" directory, and returns the path to User's home directory
     dir = getenv("HOME");
  }
  //it changes directory to the path specified by the variable dir
  int ret = chdir(dir);
  //If chdir fails, it will return a negative number
  if (ret < 0){
    fprintf(stderr,"No such directory found\n");
  }
  //getcwd find path to current working directory and stores in to
  //the variable cdir.
  printf("%s", getcwd(cdir,250));
  printf("$krash$ ");
}

//This function prints out instructions
void help(char* output){
  char cdir[100];
  char *text=
  "------------ Krash Manual -----------------\n\
  Here are the list of built-in commands\n\
  exit: exits the krash shell program\n\
  pwd: prints the current working directory\n\
  cd [dir]: changes directory to specified [dir]\n\
  \t if path is not specified, it changes to user\'s home directory\n\
  help: prints manual for krash shell program\n\
  ----------------------------------------------\n";

  //If output is "stdout", then print the help manual to standard out
  if (strcmp(output,"stdout")==0){
    printf("%s", text);
  }
  else{
    //Strcat concatenates strings. It takes what is in the second argument
    //and concatenates to the first argument provided and the first argument
    //is updated.
    //Take the file name and append .out
    strcat(output,".out");
    //If output is different than "stdout"
    //fopen opens the file pointed by the first argument (output)
    //and uses the given mode(w+). W+ creates an empty file for both
    //reading and writing.
    FILE *fp = fopen(output, "w+");
    //write the help manual to the file.
    fprintf(fp,"%s", text);
    //Close the file after writing and print out a message.
    fclose(fp);
    printf("File Successfully Created!\n");
  }
  //Print current working directory
  printf("%s", getcwd(cdir,250));
  printf("$krash$ ");
}

//function to print current working directory
void pwd(char* output){
  char dir[250];
  //If no file is specified, get current working directory
  //and print it to standard out.
  if (strcmp(output,"stdout")==0){
    printf("%s\n",getcwd(dir,250));

  }
  else{
    //Strcat concatenates strings. It takes what is in the second argument
    //and concatenates to the first argument provided and the first argument
    //is updated.
    //Take the file name and append .out
    strcat(output,".out");
    //If file name is specified, open the file and using "W+"
    FILE *fp = fopen(output, "w+");
    //Get current working directory and write to the file
    fprintf(fp,"%s", getcwd(dir,250));
    //Close the file when done writing and print out the message
    fclose(fp);
    printf("File Successfully Created!\n");
  }
  //Print current working directory
  printf("%s", getcwd(dir,250));
  printf("$krash$ ");
}


//functions to execute non built in functions
 void otherCommands(char **input, int index, char *output){
  //Variable to store current directory
  char cdir[250];
  //Variable to save file name
  char filenameErr[250];
  //The first argument of strcpy is a destination and the second one is source.
  //It copys what is the source string to the destination.
  strcpy(filenameErr, output);
  //Fork() creates a new child process.
  int child = fork();
  if (child < 0){ //Createioin of a child process was unsuccessful.
    printf("Error executing commands\n");
    exit(0);
  }
  else if( child ==0){ //If child process is successfully forked
    //If user entered > and a file name to redirect output
    if ( strcmp(output,"stdout")!= 0){
      //Close stdout and stderr to redirect output. The output is not printed to stdout
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

      //Strcat concatenates strings. It takes what is in the second argument
      //and concatenates to the first argument provided and the first argument
      //is updated.
      //Take the file name and append .out and .err
      strcat(output,".out");
      strcat(filenameErr,".err");


      //Open the file. O_CREAT tells if the file doesn't exist, create it
      //as a regular file, O_WRONLY tells that the file is in read and write only mode.
      //O_TRUNC tells if the file already exists, then it is truncated to length 0.
      //S_IRWXU gives the user to read, write and execute permission.
      open(output, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
      open(filenameErr, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);


    }

    //Variable to store arguments to pass to execvp()
    char *myargs[index];
    //Assign values to myargs
    for (int i=0; i < index; i ++){
      myargs[i]=strdup(input[i]);
    }
    //NUll tells the end of array.
    myargs[index] = NULL;

    //Pass in the command and other arguments to execute in chile process
    execvp(myargs[0],myargs);

    //If exec() fails,
    //output error, and finish the process.
    if (execvp(myargs[0],myargs) == -1){
      fprintf(stderr,"Command not found\n");
      exit(0);
    }

  }
  else{ //Child process is done and returned to the parent process.
    //Wait until it child process is done
    int parent = wait (NULL);
    //Print current working directory.
    printf("%s", getcwd(cdir,250));
    printf("$krash$ ");
  }
}
