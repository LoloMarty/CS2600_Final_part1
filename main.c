
//Section : Basic lifetime of a shell
int main (int argc, char **argv)
{
	//run command loop
	lsh_loop();

	//performing shutdown/cleanup
	return EXIT_SUCCESS;
}


//Section : Basic loop of a shell
void lsh_loop(void)
{
	char *line;
	char **args;
	int status; 

	do {
		printf("> ");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	} while (status);
}

//Section : Reading a line 
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1){
		//Read a character	
		c = getchar();

		// if we hit the end of file, replace it with a null character and return 
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;

		//if we've exceeded the buffer, reallocate
		if (position >= bufsize) {
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char *lsh_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us 

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin)) {
			exit(EXIT_SUCCESS);
		}else{
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return line;
}

//Section : Parsing the lines
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "		\t\r\n\a"
char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) 
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufsize)
		{
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens; 
}

//Section : How shells start processes
int lsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		//child process
		if (execvp(args[0], args) == -1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		//error forking 
		perror("lsh");
	}else{
		//parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

//Shell Builtins

/*
	Function declarations for builtin shell commands:
*/
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
	List of builtin commands, followed by their corresponding functions
*/
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};

int lsh_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

/*
	Builtin function implementations
*/

int lsh_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "lsh: expected arguement to \"cd\"\n");
	}else{
		if (chdir(args[1]) != 0
		{
			perror("lsh");
		}
	}
	return 1;
}

int lsh_help(char **args)
{
	int i;
	printf("Marvin Sevilla's LSH\n"); //originally Stephen Brennan's instead of Marvin Sevilla's
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are buult in:\n");

	for (i=0; i<lsh_num_builtins(); i++)
	{
		printf("	%s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int lsh_exit(char **args)
{
	return 0;
}



