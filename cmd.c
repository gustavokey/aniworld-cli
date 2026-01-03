#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define STRING_COLOR(s, c) ANSI_COLOR_##c s ANSI_COLOR_RESET

#define cmd(...) cmd_run((char *[]){ __VA_ARGS__, NULL })

extern inline int cmd_run(char *cmd[])
{
#ifdef _WIN32
  printf(__FILE__"::betterC:%d "ANSI_COLOR_GREEN">>"ANSI_COLOR_RESET" Exec ", __LINE__);

  for (int i = 0; cmd[i] != NULL; ++i)
    printf("%s ", cmd[i]);

  printf("\n");

  STARTUPINFO siStartInfo;
  ZeroMemory(&siStartInfo, sizeof(siStartInfo));
  siStartInfo.cb = sizeof(STARTUPINFO);

  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION piProcInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // @Note string array to string.
  long int cmdsize = strslen(cmd) + 1;
  char cmdstr[cmdsize];
  memset(cmdstr, 0, cmdsize);
  for (long int i = 0; cmd[i] != NULL; ++i)
  {
    strcat(cmdstr, cmd[i]);
    strcat(cmdstr, " ");
  }

  // @Note spawn cmd process.
  BOOL bSuccess = CreateProcessA(NULL, cmdstr, NULL, NULL, FALSE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

  if (!bSuccess)
  { printf("[INFO] Failed to start process!");
    exit(1); }

  CloseHandle(piProcInfo.hThread);

  return 0;
#else
  int rc = 0;
  pid_t pid = 0;

  printf("[INFO] Run ");
  for (int i = 0; cmd[i] != NULL; ++i)
    printf("%s ", cmd[i]);

  printf("\n");

  pid = fork();

  if (pid == 0)
  {
    if (execv(*cmd, cmd + 1) < 0)
    { printf("[INFO] Failed to start process!");
      exit(1); }
  }
  else
  {
    waitpid(pid, &rc, 0);

    if (WIFEXITED(rc))
      return WEXITSTATUS(rc);
    else
      return 1;
  }

  return 1;
#endif // WIN32 | UNIX
}
