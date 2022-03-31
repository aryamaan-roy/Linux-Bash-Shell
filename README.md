# Linux Bash Shell

## Working:

- Using priciples of a Linux Bash Shell in the Linux Operating system,
the User-defined interactive shell program can create and manage new processes.
The shell is able to create a process out of a system program like emacs, vi,
or any user-defined executable

## Features :

- cd : The cd command is used to change the current working directory, navigate to root directory or show current directory path
Usage :
<Name@UBUNTU:~>cd .. : Displays absolute path of current directory from root
<Name@UBUNTU:~>cd ../ : Navigates to the parent directory
<Name@UBUNTU:~>cd abc/xyz : Navigates to directory root/abc/xyz
- ls : ls command is used to view all files and directories. Flags -a and -l are also implemented.
Usage :
Any of the following versions of the command shall work :
● ls -a
● ls -l
● ls .
● ls ..
● ls ~
● ls -a -l
● ls -la / ls -al
● ls <Directory/File_name>
● ls -<flags> <Directory/File_name>
- System Command like emacs and vi can also be executed on the shell in the background and Foreground
- Foreground Processes :
Executing any system program like "vi" command in the foreground implies
that my shell will wait for this process to complete and regain control when this process
exits
- Background Processes :
Any command that begins with "&" is considered a background command. This means that my shell will create
that process instead of waiting for it to exit. It will continue to accept user commands. Whenever a new background is available,
PID of the newly formed background process is printed on my shell after it has started.
Usage:
<Name@UBUNTU:~> gedit &
456
<Name@UBUNTU:~> ls -l -a
.
.
Execute other commands
- pinfo Command :
This prints the process-related info of your shell program
Usage :
<Name@UBUNTU:~>pinfo
pid -- 231
Process Status -- {R/S/S+/Z}
memory -- 67854 {Virtual Memory}
Executable Path -- ~/a.out
- Input Output Redirection :
Using the symbols <, > and >>, the output of commands, usually written to stdout, can be redirected to another file, or the input taken from a file other than stdin. Both
input and output redirection can be used simultaneously.
Usage :
<tux@linux:~> sort < file1.txt > lines_sorted.txt : Takes input from file1.txt and output stored at lines_sorted.txt
- Command Pipelines :
A pipe, identified by |, redirects the output of the command on the left as input to the command on the right. One or more commands can be piped as the following
examples show.
Usage :
<tux@linux:~> cat sample2.txt | head -7 | tail -5
<tux@linux:~> cat < in.txt | wc -l > lines.txt
- Jobs :
This command prints a list of all currently running background processes spawned by the shell in alphabetical order of the command name, along with
their job number (a sequential number assigned by your shell), process ID and their state, which can either be running or stopped. There may be flags specified
as well. If the flag specified is -r, then only the running processes is printed else if the flag is -s then the stopped processes are printed.
    
    Usage :
    <tux@linux:~> jobs
    [1] Running emacs assign1.txt [221]
    [3] Stopped vim [3211]
    
    <tux@linux:~> jobs -r
    [1] Running emacs assign1.txt [221]
    
    <tux@linux:~> jobs -s
    [3] Stopped vim [3211]
    
- sig :
Takes the job number (assigned by your shell) of a running job and sends the signal corresponding to signal number to that process.
Usage:
<tux@linux:~> sig 2 9
sends SIGKILL (signal number 9) to the process causing it to terminate
- fg and bg commands :
fg Brings the running or stopped background job corresponding to job number to the foreground, and changes its state to running
bg Changes the state of a stopped background job to running (in the background).
    
    Usage :
    <tux@linux:~> fg 4
    brings [4] gedit to the foreground
    
    <tux@linux:~> bg 3
    Changes the state of [3] vim to running
    
- Replay Command :
executes a particular command in fixed time interval for a certain period
Usage :
<Name@UBUNTU:~> replay -command echo "hi" -interval 3 -period 6
executes echo "hi" command after every 3 seconds until 6 seconds are elapsed
- Signal Handling of Cntrl Z (pushes current foreground process to background), Cntrl-C (interrupts any currently running foreground process)
and Cntrl-D (logs out of shell) is implemented
- pwd : full pathname of the current working directory
Usage :
<Name@UBUNTU:~> pwd
- echo : echo is a command that outputs the strings that are passed to it as arguments
Usage :
<Name@UBUNTU:~>echo abcdef
abcdef
