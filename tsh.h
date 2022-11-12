#ifndef _TSH_H_
#define _TSH_H_

#include <stdlib.h>
#include <unistd.h>

#define TSH_ERROR(x)                                                           \
  {                                                                            \
    fprintf(stderr, x);                                                        \
    exit(EXIT_FAILURE);                                                        \
  }

#define TSH_ARG_BUFSIZE 10

char** args = NULL;
int bufsize = TSH_ARG_BUFSIZE;
int cmdBufsize = TSH_ARG_BUFSIZE;

void add_arg(char* arg);

int tsh_cd(char **args);
int tsh_help(char **args);
int tsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {&tsh_cd, &tsh_help, &tsh_exit};

int tsh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/*
  Builtin function implementations.
*/
int tsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "tsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("tsh");
    }
  }
  return 1;
}

int tsh_help(char **args)
{
  int i;
  printf("Soumil's tsh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < tsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int tsh_exit(char **args) { return 0; }

struct SimpleCommand
{
    size_t nargs;
    char** args;
};

struct SimpleCommand* currSimpleCommand = NULL;

struct Command
{
    size_t ncommands;
    struct SimpleCommand** commands;
    char* outFile;
    char* inFile;
    char* errFile;
    int background;
};

struct Command currCommand;

int execute(void)
{
  for (int i = 0; i < tsh_num_builtins(); i++) {
    if (strcmp(currCommand.commands[0]->args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(currCommand.commands[0]->args);
    }
  }

  pid_t pid, wpid;
  int status;

  int tmpin = dup(0), tmpout = dup(1);
  int fdin;
  if (currCommand.inFile) {
    fdin = open(currCommand.inFile, O_RDONLY);
  } else {
    fdin = dup(tmpin);
  }
  int fdout;
  
  for (int i = 0; i < currCommand.ncommands; i++)
  {
    struct SimpleCommand* simpleCommand = currCommand.commands[i];
    dup2(fdin, 0);
    close(fdin);

    if (i == currCommand.ncommands - 1) {
      if (currCommand.outFile) {
        fdout = open(currCommand.outFile, O_CREAT | O_TRUNC | O_WRONLY, 777);
      } else {
        fdout = dup(tmpout);
      }
    } else {
      int fdpipe[2];
      pipe(fdpipe);
      fdout = fdpipe[1];
      fdin = fdpipe[0];
    }

    dup2(fdout, 1);
    close(fdout);

    pid = fork();
    if (pid == 0) {
      // execvp instructs the Operating System to find the executable in $PATH. A handy trick to avoid writing full paths such as
      // /bin/ls -la
      if (execvp(simpleCommand->args[0], simpleCommand->args) == -1) {
        perror("tsh");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) { // fork failure
      perror("fork");
      return 1;
    } else {
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }

  dup2(tmpin, 0);
  dup2(tmpout, 1);
  close(tmpin);
  close(tmpout);

  return 1;
}

void add_curr_simple_cmd_name(char* cmdname)
{
    if (currCommand.commands == NULL) {
        currCommand.commands = malloc(sizeof(struct SimpleCommand*) * cmdBufsize);
    }
    if (currSimpleCommand == NULL) {
        currSimpleCommand = malloc(sizeof(struct SimpleCommand*));
    }

    if (currSimpleCommand->args == NULL) {
        currSimpleCommand->args = malloc(sizeof(char*) * bufsize);
    }

    currSimpleCommand->args[0] = cmdname;

    currCommand.commands[currCommand.ncommands++] = currSimpleCommand;
    if (currCommand.ncommands >= cmdBufsize) {
        cmdBufsize += TSH_ARG_BUFSIZE;
        currCommand.commands = realloc(args, cmdBufsize);
    }

    currSimpleCommand = NULL;
}

void add_arg(char* arg)
{
    if (currSimpleCommand == NULL) {
        currSimpleCommand = malloc(sizeof(struct SimpleCommand*));
    }

    if (currSimpleCommand->args == NULL) {
        currSimpleCommand->args = malloc(sizeof(char*) * bufsize);
    }

    currSimpleCommand->args[1 + currSimpleCommand->nargs++] = arg;
    if (currSimpleCommand->nargs + 1 >= bufsize) {
        bufsize += TSH_ARG_BUFSIZE;
        currSimpleCommand->args = realloc(args, bufsize);
    }
}

void set_infile(char* in)
{
  currCommand.inFile = in;
}

void set_outfile(char* out)
{
  currCommand.outFile = out;
}

void reset()
{
    if (currSimpleCommand != NULL) {
      free(currSimpleCommand->args);
      free(currSimpleCommand);
    }

    free(currCommand.commands);

    currCommand.commands = NULL;
    currCommand.ncommands = 0;
    currSimpleCommand = NULL;
    currCommand.outFile = NULL;
}

#endif // _TSH_H_
