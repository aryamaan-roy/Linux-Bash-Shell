#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <grp.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
extern int errno;

pid_t fore_pid = 0;
//structure to store background processes
typedef struct
{
    int job_no;
    char name[100];
    int is_running;
    int pid_no;
} job;
int bg_jobs = 0;
job table[1000];

void sig_Z(int signo)
{
    pid_t pid = getpid();
    if (fore_pid == pid)
    {
        kill(pid, 20);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTSTP, sig_Z);
    }
    else
    {
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTSTP, sig_Z);
    }
}

//compare function for qsort for jobs command
int job_compare(const void *a, const void *b)
{

    job *jobA = (job *)a;
    job *jobB = (job *)b;

    return strcmp(jobA->name, jobB->name);
}

//function to handle closing background processes.
void finished_bg_process(int sig_no)
{
    int i, status, finish_pid;
    while ((finish_pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (i = 0; i < bg_jobs; i++) //check for dead processes from all bancground processes
        {
            if (table[i].is_running != 1)
                continue;
            else if (table[i].pid_no == finish_pid)
                break;
        }
        if (i != bg_jobs)
        {
            if (WIFEXITED(status)) //checks for normal completion
                fprintf(stdout, "\n%s with pid %d exited normally\n", table[i].name, table[i].pid_no);
            else if (WIFSIGNALED(status)) //checks for abnormal completion
                fprintf(stdout, "\n%s with pid %d has exited with signal\n", table[i].name, table[i].pid_no);
            table[i].is_running = -1;
        }
    }
}

//fucntion to append current directory path_name based on requirements
void get_path(char *curr_dir, char *shell_dir)
{
    if (strcmp(shell_dir, curr_dir) == 0) //replace path with ~ if same dir in which shell.c is present
    {
        strcpy(curr_dir, "~");
    }
    int shell_dir_len = strlen(shell_dir);
    int curr_dir_len = strlen(curr_dir);

    if (curr_dir_len > shell_dir_len) //if pathname is longer
    {
        int i = 0;
        while (i <= shell_dir_len) //check whether curr dir is in shell dir by parsing pathname
        {
            if (shell_dir[i] == curr_dir[i])
                i++;
            else
                break;
        }
        if (i == shell_dir_len) //curr dir is in shell dir
        {
            char part[1000];
            int k = 1;
            for (int j = shell_dir_len; j < curr_dir_len; j++)
            {
                part[k++] = curr_dir[j];
            }
            part[0] = '~';
            part[k] = '\0';
            strcpy(curr_dir, part);
        }
    }
}

int execute_command(char *input_command, char *shell_dir, char *curr_dir, int history_no, char history_command[20][10000], int is_pipe)
{
    fore_pid = 0;
    char input[1000][50];
    int input_no, command_no;
    char delimeter[3];
    delimeter[0] = ' ';
    delimeter[1] = '\t';
    delimeter[2] = '\0';
    char *token2 = strtok(input_command, delimeter);

    // Keep printing tokens while one of the
    // delimiters present in str[].
    input_no = 0;
    while (token2 != NULL)
    {
        strcpy(input[input_no], token2);
        input_no++;
        token2 = strtok(NULL, delimeter);
    }
    if (input[input_no - 1][strlen(input[input_no - 1]) - 1] == '\n')
        input[input_no - 1][strlen(input[input_no - 1]) - 1] = '\0';
    //printf("%s, %s, %d", input[0], input[1], input_no);

    //update is_running for stopped processes:
    FILE *procfile = NULL;
    char file[200];
    for (int i = 0; i < bg_jobs; ++i)
    {
        if (table[i].is_running == -1)
            continue;
        sprintf(file, "/proc/%d/stat", table[i].pid_no);
        procfile = fopen(file, "r");
        unsigned long long val;
        char temp1[100];
        char temp2[100];
        fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
        if (strcmp(temp2, "R") == 0 || strcmp(temp2, "S") == 0)
            continue;
        table[i].is_running = 0;
    }

    //check for I/0 redirection
    int file_output = -1, file_input = -1;
    for (int i = 0; i < input_no; ++i)
    {
        if (strcmp(input[i], ">") == 0 || strcmp(input[i], ">>") == 0)
            file_output = i;
        if (strcmp(input[i], "<") == 0)
            file_input = i;
    }
    long long int fd_write = 0;
    int saved_stdout;
    if (file_output >= 0)
    {
        saved_stdout = dup(1);
        if (strcmp(input[file_output], ">") == 0)
        {
            fd_write = open(input[file_output + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        }
        else
            fd_write = open(input[file_output + 1], O_CREAT | O_WRONLY | O_APPEND);

        if (fd_write < 0)
        {
            perror("failed to open file");
        }

        if (dup2(fd_write, STDOUT_FILENO) < 0)
        {
            perror("Unable to duplicate file descriptor.");
            exit(1);
        }
        input_no -= 2;
    }
    long long int fd_read;
    int saved_stdin;
    if (file_input >= 0)
    {
        saved_stdin = dup(0);
        fd_read = open(input[file_input + 1], O_RDONLY);
        if (fd_read < 0)
        {
            perror("failed to open file");
        }

        if (dup2(fd_read, STDIN_FILENO) < 0)
        {
            perror("Unable to duplicate file descriptor.");
            exit(1);
        }
        input_no -= 2;
    }
    int repeat = 1;
    if (strcmp(input[0], "repeat") == 0)
    {
        repeat = atoi(input[1]);
        for (int i = 2; i < input_no; ++i)
            strcpy(input[i - 2], input[i]);
        input_no -= 2;
    }
    int s=0;
    if (strcmp(input[0], "replay") == 0)
    {
        if (input_no < 7)
        {
            perror("Very few arguments");
            return 10;
        }
        int interval = atoi(input[input_no - 3]);
        int period = atoi(input[input_no - 1]);
        for (int i = 2; i < input_no; i++)
        {
            strcpy(input[i-2],input[i]);
        }
        
        repeat=period/interval;
        s=interval;
        input_no-=6;
    }
    if (strcmp(input[0], "cd") == 0)
    {
        while (repeat-- > 0)
        {
            if (input_no == 1 || (input_no == 2 && strcmp("~", input[1]) == 0))
            {
                chdir(shell_dir);
            }
            else
            {
                chdir(input[1]);
            }
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "pwd") == 0)
    {
        while (repeat-- > 0)
        {
            curr_dir = getcwd(curr_dir, 1000);
            printf("%s\n", curr_dir);
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "echo") == 0)
    {
        while (repeat-- > 0)
        {
            if (file_input >= 0 || is_pipe == 1)
            {
                char c;
                while (scanf("%c", &c) != EOF)
                {
                    if (c == ' ' || c == '\t')
                        continue;
                    printf("%c", c);
                }
            }
            for (int word = 1; word < input_no; ++word)
                printf("%s\n", input[word]);
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "exit") == 0)
    {
        free(curr_dir);
        return -1; //goto exit
    }
    else if (strcmp(input[0], "ls") == 0)
    {
        while (repeat-- > 0)
        {
            DIR *folder;
            char folder_path[1000];
            int all = 0, hidden = 1, folder_flag = 0;
            //corner case
            if (input_no == 1 || (input_no == 2 && input[1][0] == '\0'))
            {
                strcpy(folder_path, ".");
            }
            else
            {
                for (int i = 1; i < input_no; ++i)
                {
                    if (strcmp(input[i], "-l") == 0)
                        all++;
                    else if (strcmp(input[i], "-a") == 0)
                        hidden = 0;
                    else if (strcmp(input[i], "-la") == 0 || strcmp(input[i], "-al") == 0)
                    {
                        hidden = 0;
                        all++;
                    }
                    else
                    {
                        folder_flag++;
                        strcpy(folder_path, input[i]);
                    };
                }
                if (strcmp(folder_path, "~") == 0)
                    strcpy(folder_path, shell_dir);
                if (!folder_flag)
                    strcpy(folder_path, ".");
            }

            struct dirent *entry;
            struct dirent *entry2;
            folder = opendir(folder_path);
            if (folder == NULL)
            {
                perror("Unable to read directory");
                break;
            }
            if (!all)
            {
                while ((entry = readdir(folder)))
                {
                    if (hidden && entry->d_name[0] == '.')
                        continue;
                    printf("%s\n", entry->d_name);
                }
            }
            if (all)
            {
                struct stat s;
                char fp[PATH_MAX];

                long int total_size = 0;

                while ((entry2 = readdir(folder)))
                {
                    if (hidden && entry2->d_name[0] == '.')
                        continue;
                    sprintf(fp, "%s/%s", folder_path, entry2->d_name);
                    if (stat(fp, &s) == -1)
                    {
                        perror("stat : path error folder");
                        break;
                    }
                    total_size += s.st_blocks;
                }
                printf("total %ld\n", total_size / 2);
                closedir(folder);
                folder = opendir(folder_path);
                while ((entry = readdir(folder)))
                {
                    if (hidden && entry->d_name[0] == '.')
                        continue;
                    struct stat s2;
                    sprintf(fp, "%s/%s", folder_path, entry->d_name);
                    if (stat(fp, &s2) == -1)
                    {
                        perror("stat : path error folder");
                        break;
                    }

                    //permission data/nlink
                    printf((S_ISDIR(s2.st_mode)) ? "d" : "-");
                    printf((s2.st_mode & S_IRUSR) ? "r" : "-");
                    printf((s2.st_mode & S_IWUSR) ? "w" : "-");
                    printf((s2.st_mode & S_IXUSR) ? "x" : "-");
                    printf((s2.st_mode & S_IRGRP) ? "r" : "-");
                    printf((s2.st_mode & S_IWGRP) ? "w" : "-");
                    printf((s2.st_mode & S_IXGRP) ? "x" : "-");
                    printf((s2.st_mode & S_IROTH) ? "r" : "-");
                    printf((s2.st_mode & S_IWOTH) ? "w" : "-");
                    printf((s2.st_mode & S_IXOTH) ? "x " : "- ");
                    printf("%li ", s2.st_nlink);

                    //group and user data
                    struct passwd *pw;
                    struct group *gid;
                    pw = getpwuid(s2.st_uid);
                    if (pw == NULL)
                    {
                        perror("error in fetching user data");
                        printf("%d ", s2.st_uid);
                    }
                    else
                    {
                        printf("%s ", pw->pw_name);
                    }
                    gid = getgrgid(s2.st_gid);
                    if (gid == NULL)
                    {
                        perror("error in fetching group data");
                        printf("%d ", s2.st_gid);
                    }
                    else
                    {
                        printf("%s ", gid->gr_name);
                    }

                    //file size
                    printf("%5ld ", s2.st_size);

                    //timestamp
                    struct tm *time_stamp;
                    char str[200];
                    time_t t = s2.st_mtime;
                    time_stamp = localtime(&t);
                    if (time_stamp == NULL)
                    {
                        perror("localtime");
                        exit(EXIT_FAILURE);
                    }
                    strftime(str, sizeof(str), "%b %d %R", time_stamp);
                    printf("%s ", str);

                    //file name
                    printf("%s\n", entry->d_name);
                }
            }

            closedir(folder);
            printf("\n");
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "history") == 0)
    {
        while (repeat-- > 0)
        {
            if (input_no == 1 && history_no <= 10)
            {
                for (int i = 0; i < history_no; ++i)
                    printf("%s", history_command[i]);
            }
            if (input_no == 1 && history_no > 10)
            {
                for (int i = history_no - 10; i < history_no; ++i)
                    printf("%s", history_command[i]);
            }
            if (input_no == 2)
            {
                for (int i = (history_no - atoi(input[1])); i < history_no; ++i)
                    printf("%s", history_command[i]);
            }
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "pinfo") == 0)
    {
        while (repeat-- > 0)
        {
            int c, len = 0;
            pid_t current_pid;

            if (input_no < 2)
                current_pid = getpid();
            else
                current_pid = atoi(input[1]);

            char path[1000];
            char buffer[1000];

            printf("pid -- %d\n", current_pid);

            sprintf(path, "/proc/%d/status", current_pid);
            FILE *file = fopen(path, "r");
            if (!file)
            {
                perror("file cannot be opened\n");
            }
            else
            {
                char state;
                fgets(buffer, 256, file);
                fgets(buffer, 256, file);
                sscanf(buffer, "State:\t%c", &state);
                printf("Process Status -- %c\n", state);
                fclose(file);
            }

            sprintf(path, "/proc/%d/statm", current_pid);
            file = fopen(path, "r");
            if (!file)
            {
                perror("file cannot be opened\n");
            }
            else
            {
                while ((c = fgetc(file)) != ' ')
                    buffer[len++] = c;
                buffer[len] = '\0';
                printf("Memory -- %s\n", buffer);
                fclose(file);
            }

            sprintf(path, "/proc/%d/exe", current_pid);
            len = -1;
            len = readlink(path, buffer, 999);
            if (len != -1)
            {
                buffer[len] = '\0';
                get_path(buffer, shell_dir);
                printf("Executable Path -- %s\n", buffer);
            }
            else
            {
                perror("file cannot be opened\n");
            }
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "jobs") == 0)
    {
        while (repeat-- > 0)
        {
            qsort(table, bg_jobs, sizeof(job), job_compare);

            if (input_no == 1)
            {
                for (int i = 0; i < bg_jobs; ++i)
                {
                    if (table[i].is_running == -1)
                        continue;
                    printf("[%d] ", table[i].job_no);
                    sprintf(file, "/proc/%d/stat", table[i].pid_no);
                    procfile = fopen(file, "r");
                    unsigned long long val;
                    char temp1[100];
                    char temp2[100];
                    fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
                    if (strcmp(temp2, "R") == 0 || strcmp(temp2, "S") == 0)
                        printf("Running ");
                    else
                        printf("Stopped ");
                    printf("%s [%d]\n", table[i].name, table[i].pid_no);
                }
            }
            else if (strcmp(input[1], "-r") == 0)
            {
                for (int i = 0; i < bg_jobs; ++i)
                {
                    if (table[i].is_running == -1)
                        continue;
                    sprintf(file, "/proc/%d/stat", table[i].pid_no);
                    procfile = fopen(file, "r");
                    unsigned long long val;
                    char temp1[100];
                    char temp2[100];
                    fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
                    if (strcmp(temp2, "R") == 0 || strcmp(temp2, "S") == 0)
                    {
                        printf("[%d] ", table[i].job_no);
                        printf("Running ");
                        printf("%s [%d]\n", table[i].name, table[i].pid_no);
                    }
                }
            }

            else if (strcmp(input[1], "-s") == 0)
            {
                for (int i = 0; i < bg_jobs; ++i)
                {
                    if (table[i].is_running == -1)
                        continue;
                    sprintf(file, "/proc/%d/stat", table[i].pid_no);
                    procfile = fopen(file, "r");
                    unsigned long long val;
                    char temp1[100];
                    char temp2[100];
                    fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
                    if (strcmp(temp2, "R") == 0 || strcmp(temp2, "S") == 0)
                        continue;
                    printf("[%d] ", table[i].job_no);
                    printf("Stopped ");
                    printf("%s [%d]\n", table[i].name, table[i].pid_no);
                }
            }
            else if (strcmp(input[1], "-a") == 0)
            {
                for (int i = 0; i < bg_jobs; ++i)
                {
                    printf("[%d] ", table[i].job_no);
                    if (table[i].is_running == -1)
                        printf("Terminated/killed ");
                    else
                    {
                        sprintf(file, "/proc/%d/stat", table[i].pid_no);
                        procfile = fopen(file, "r");
                        unsigned long long val;
                        char temp1[100];
                        char temp2[100];
                        fscanf(procfile, "%lld %s %s ", &val, temp1, temp2);
                        if (strcmp(temp2, "R") == 0 || strcmp(temp2, "S") == 0)
                            printf("Running ");
                        else
                            printf("Stopped ");
                    }
                    printf("%s [%d]\n", table[i].name, table[i].pid_no);
                }
            }
            else
                return 10;
            if(s!=0)sleep(s);
        }
    }
    else if (strcmp(input[0], "sig") == 0)
    {
        if (input_no < 3)
        {
            perror("Usage : sig job_no signal_no\n");
            return 4;
        }
        int job_num = atoi(input[1]);
        int i;
        for (i = 0; i < bg_jobs; i++)
        {
            if (table[i].job_no == job_num)
                break;
        }
        if (i == bg_jobs)
        {
            perror("No bg job with given job no exists");
            return 6;
        }

        pid_t pidnumber;
        pidnumber = table[i].pid_no;
        if (kill(pidnumber, atoi(input[2])) == -1)
        {
            perror("unable to change state of bg process");
            return 4;
        }
    }
    else if (strcmp(input[0], "bg") == 0)
    {
        if (input_no < 2)
        {
            perror("Usage : bg job_no\n");
            return 5;
        }
        int job_num = atoi(input[1]);
        int i;
        for (i = 0; i < bg_jobs; i++)
        {
            if (table[i].job_no == job_num)
                break;
        }
        if (i == bg_jobs)
        {
            perror("No bg job with given job no exists");
            return 6;
        }

        pid_t pidnumber;
        pidnumber = table[i].pid_no;
        if (table[i].is_running == 0)
            printf("%d", pidnumber);
        if (kill(pidnumber, SIGCONT) == -1)
        {
            perror("unable to change state of bg process");
            return 4;
        }
    }
    else if (strcmp(input[0], "fg") == 0)
    {
        if (input_no < 2)
        {
            perror("Usage : fd job_no\n");
            return 7;
        }
        int job_num = atoi(input[1]);
        int i;
        for (i = 0; i < bg_jobs; i++)
        {
            if (table[i].job_no == job_num)
                break;
        }
        if (i == bg_jobs)
        {
            perror("No bg job with given job no exists");
            return 8;
        }
        if (table[i].is_running == 0)
        {
            kill(table[i].pid_no, SIGCONT);
            table[i].is_running = 1;
        }
        if (table[i].is_running == -1)
        {
            perror("Cannot bring terminated program to fg");
            return 11;
        }

        int status, shell = STDERR_FILENO;
        int my_pid = getpid();
        int pid = table[i].pid_no, pgid;
        pgid = getpgid(pid);    /* get pgid of mentioned job */
        tcsetpgrp(shell, pgid); /* Give control of shell's terminal to this process */

        //fgpid = pgid;                            /* Set this pgid as fg pgid */
        if (killpg(pgid, SIGCONT) < 0) /* Send signal to thid pgid to continue if stopped */
            perror("Can't get in foreground!\n");
        waitpid(pid, &status, WUNTRACED); /* Wait for this process, return even if it has stopped without trace */
        if (!WIFSTOPPED(status))
        { /* returns true if the child process was stopped by delivery of a signal */
            fore_pid = pid;
            //terminating bg record
            table[i].is_running = -1;
        }
        tcsetpgrp(shell, my_pid);
    }
    else
    {
        char *args[input_no + 1];
        int i = 0;
        int background = 0;

        i = input_no;
        for (int j = 0; j < input_no; ++j)
        {
            args[j] = (char *)malloc(50);
            strcpy(args[j], input[j]);
            //printf("%s", args[j]);
        }
        args[input_no] = NULL;
        if (strcmp(args[i - 1], "&") == 0)
        {
            args[i - 1] = NULL;
            background = 1;
        }

        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("fork was unsuccessful in creating bg process");
            return -2;
        }

        if (pid == 0)
        {
            execvp(args[0], args); /* execute the command  */
            if (execvp(args[0], args) < 0)
            {
                perror("error: Invalid command\n");
                return -3;
            }
        }
        else if (!background)
        {
            fore_pid = pid;
            wait(NULL);
        }
        if (background)
        {
            printf("%d", pid);
            table[bg_jobs].pid_no = pid;
            table[bg_jobs].is_running = 1;
            table[bg_jobs].job_no = bg_jobs;
            strcpy(table[bg_jobs].name, args[0]);
            for (int i = 1; i < input_no - 1; ++i)
            {
                strcat(table[bg_jobs].name, " ");
                strcat(table[bg_jobs].name, input[i]);
            }
            bg_jobs++;
        }
        for (int k = 0; k < input_no; ++k)
            free(args[k]);
    }
    if (file_output >= 0)
    {
        close(fd_write);
        dup2(saved_stdout, 1);
        close(saved_stdout);
    }
    if (file_input >= 0)
    {
        close(fd_read);
        dup2(saved_stdin, 0);
        close(saved_stdin);
    }
}

int is_foreground()
{
    int devtty;
    if ((devtty = open("/dev/tty", O_RDWR)) < 0)
    {
        return 0;
    }
    return 1;
}

void sig_handler(int signo)
{
    if (signo == SIGINT && fore_pid != 0)
    {
        kill(fore_pid, SIGINT);
    }
}

int main()
{
    struct utsname buf;
    uname(&buf); //acess username
    char *shell_dir = (char *)malloc(1000);
    int shell_dir_filled = 0;
    if (!shell_dir_filled) //check whether shell_dir is filled
    {
        shell_dir = getcwd(shell_dir, 1000);
        shell_dir_filled++;
    }

    //all initializations
    char raw_input[10000];
    char command[20][500];
    int command_no;
    char pipe_command[100][100];
    int pipe_no;
    char history_command[20][10000];
    int history_no = 0;
    signal(SIGCHLD, finished_bg_process);
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_Z);
    while (1)
    {

        char *curr_dir = (char *)malloc(1000);
        curr_dir = getcwd(curr_dir, 1000);
        if (curr_dir == NULL)
        {
            perror("cannot access cur dir path");
            break;
        }
        get_path(curr_dir, shell_dir);

        printf("\n<%s@%s:%s>", getenv("USER"), buf.sysname, curr_dir); //can we use getenv??

        if (fgets(raw_input, 10000, stdin) == NULL)
            exit(0);
        //history record
        if (history_no > 19)
        {
            for (int i = 0; i < 19; ++i)
                strcpy(history_command[i], history_command[i + 1]);
            history_no = 19;
        }
        strcpy(history_command[history_no], raw_input);
        history_no++;

        //segregating = ;
        char *token1 = strtok(raw_input, ";");

        // Keep printing tokens while one of the
        // delimiters present in str[].
        command_no = 0;
        while (token1 != NULL)
        {
            strcpy(command[command_no], token1);
            command_no++;
            token1 = strtok(NULL, ";");
        }
        int return_value;
        for (int current_command_no = 0; current_command_no < command_no; ++current_command_no)
        {
            //segregating = |
            char *pipeline_token = strtok(command[current_command_no], "|");
            pipe_no = 0;
            while (pipeline_token != NULL)
            {
                strcpy(pipe_command[pipe_no], pipeline_token);
                pipe_no++;
                pipeline_token = strtok(NULL, "|");
            }
            if (pipe_no < 2)
            {
                return_value = execute_command(pipe_command[0], shell_dir, curr_dir, history_no, history_command, 0);
                if (return_value == -1)
                {
                    goto exit;
                }
                continue;
            }
            char filename[2][10];
            strcpy(filename[0], "1.txt");
            strcpy(filename[1], "2.txt");
            int r = 0, w = 1;
            for (int current_pipe_no = 0; current_pipe_no < pipe_no; ++current_pipe_no)
            {
                long long int file_read = 0;
                int save_stdout = dup(1);
                long long int file_output = 0;
                int save_stdin = dup(0);
                //output
                if (current_pipe_no != pipe_no - 1)
                {

                    file_output = open(filename[w], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (file_output < 0)
                    {
                        perror("Failed to open file.");
                        exit(1);
                    }
                    if (dup2(file_output, STDOUT_FILENO) < 0)
                    {
                        perror("Unable to duplicate file descriptor.");
                        exit(1);
                    }
                }
                //input
                if (current_pipe_no != 0)
                {
                    file_read = open(filename[r], O_RDONLY);
                    if (file_read < 0)
                    {
                        perror("Failed to open file.");
                        exit(1);
                    }
                    if (dup2(file_read, STDIN_FILENO) < 0)
                    {
                        perror("Unable to duplicate file descriptor.");
                        exit(1);
                    }
                }
                if (current_command_no == 0)
                    return_value = execute_command(pipe_command[current_pipe_no], shell_dir, curr_dir, history_no, history_command, 0);
                else
                    return_value = execute_command(pipe_command[current_pipe_no], shell_dir, curr_dir, history_no, history_command, 1);
                if (current_pipe_no != pipe_no - 1)
                {
                    close(file_output);
                    dup2(save_stdout, 1);
                    close(save_stdout);
                }
                if (current_pipe_no != 0)
                {
                    close(file_read);
                    dup2(save_stdin, 0);
                    close(save_stdin);
                }
                if (return_value == -1)
                {
                    goto exit;
                }
                if (r == 0)
                    r = 1;
                else
                    r = 0;
                if (w == 0)
                    w = 1;
                else
                    w = 0;
            }
            remove(filename[0]);
            remove(filename[1]);
        }
        if (curr_dir != NULL)
            free(curr_dir);
    }
exit:
    free(shell_dir);
    return (0);
}
