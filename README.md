# Linux Bash Shell

## Working:

- Using priciples of a Linux Bash Shell in the Linux Operating system,
the User-defined interactive shell program can create and manage new processes.
The shell is able to create a process out of a system program like emacs, vi,
or any user-defined executable

## Features :

- cd :<br> The cd command is used to change the current working directory, navigate to root directory or show current directory path<br>
Usage :<br>
<Name@UBUNTU:\~>cd .. : Displays absolute path of current directory from root<br>
<Name@UBUNTU:\~>cd ../ : Navigates to the parent directory<br>
<Name@UBUNTU:\~>cd abc/xyz : Navigates to directory root/abc/xyz<br>
- ls :<br> ls command is used to view all files and directories. Flags -a and -l are also implemented.<br>
Usage :
Any of the following versions of the command shall work :<br>
● ls -a <br>
● ls -l<br>
● ls .<br>
● ls ..<br>
● ls ~<br>
● ls -a -l<br>
● ls -la / ls -al<br>
● ls <Directory/File_name><br>
● ls -<flags> <Directory/File_name><br>
- System Command like emacs and vi can also be executed on the shell in the background and Foreground<br>
- Foreground Processes :<br>
Executing any system program like "vi" command in the foreground implies
that my shell will wait for this process to complete and regain control when this process
exits<br>
- Background Processes :<br>
Any command that begins with "&" is considered a background command. This means that my shell will create
that process instead of waiting for it to exit. It will continue to accept user commands. Whenever a new background is available,
PID of the newly formed background process is printed on my shell after it has started.<br>
Usage:<br>
<Name@UBUNTU:~> gedit &<br>
456<br>
<Name@UBUNTU:~> ls -l -a<br>
.<br>
.<br>
Execute other commands<br>
- pinfo Command :<br>
This prints the process-related info of your shell program<br>
Usage :<br>
<Name@UBUNTU:~>pinfo<br>
pid -- 231<br>
Process Status -- {R/S/S+/Z}<br>
memory -- 67854 {Virtual Memory}<br>
Executable Path -- ~/a.out<br>
- Input Output Redirection :<br>
Using the symbols <, > and >>, the output of commands, usually written to stdout, can be redirected to another file, or the input taken from a file other than stdin. Both
input and output redirection can be used simultaneously.<br>
Usage :<br>
<tux@linux:~> sort < file1.txt > lines_sorted.txt : Takes input from file1.txt and output stored at lines_sorted.txt<br>
- Command Pipelines :<br>
A pipe, identified by |, redirects the output of the command on the left as input to the command on the right. One or more commands can be piped as the following
examples show.<br>
Usage :<br>
<tux@linux:\~> cat sample2.txt | head -7 | tail -5<br>
<tux@linux:\~> cat < in.txt | wc -l > lines.txt<br>
- Jobs :<br>
This command prints a list of all currently running background processes spawned by the shell in alphabetical order of the command name, along with
their job number (a sequential number assigned by your shell), process ID and their state, which can either be running or stopped. There may be flags specified
as well. If the flag specified is -r, then only the running processes is printed else if the flag is -s then the stopped processes are printed.<br>
    
    Usage :<br>
    <tux@linux:\~> jobs<br>
    [1] Running emacs assign1.txt [221]<br>
    [3] Stopped vim [3211]<br>
    <br>
    <tux@linux:\~> jobs -r<br>
    [1] Running emacs assign1.txt [221]<br>
    <br>
    <tux@linux:\~> jobs -s<br>
    [3] Stopped vim [3211]<br>
    
- sig :<br>
Takes the job number (assigned by your shell) of a running job and sends the signal corresponding to signal number to that process.<br>
Usage:<br>
<tux@linux:~> sig 2 9<br>
sends SIGKILL (signal number 9) to the process causing it to terminate<br>
- fg and bg commands :<br>
fg Brings the running or stopped background job corresponding to job number to the foreground, and changes its state to running
bg Changes the state of a stopped background job to running (in the background).<br>
    
    Usage :<br>
    <tux@linux:\~> fg 4<br>
    brings [4] gedit to the foreground<br>
    <br>
    <tux@linux:\~> bg 3<br>
    Changes the state of [3] vim to running<br>
    
- Replay Command :<br>
executes a particular command in fixed time interval for a certain period<br>
Usage :<br>
<Name@UBUNTU:\~> replay -command echo "hi" -interval 3 -period 6<br>
executes echo "hi" command after every 3 seconds until 6 seconds are elapsed<br>
- Signal Handling of Cntrl Z (pushes current foreground process to background), Cntrl-C (interrupts any currently running foreground process)
and Cntrl-D (logs out of shell) is implemented<br>
- pwd : full pathname of the current working directory<br>
Usage :<br>
<Name@UBUNTU:~> pwd<br>
- echo : echo is a command that outputs the strings that are passed to it as arguments<br>
Usage :<br>
<Name@UBUNTU:~>echo abcdef<br>
abcdef<br>
