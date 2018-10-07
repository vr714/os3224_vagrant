#include "types.h"
#include "stat.h"
#include "user.h"


#define EOF (-1)

/* print one character to stdout */
void
putc(char c)
{
  write(1, &c, 1);
}

/* print n characters to stdout */
void
putn(char *s, int n)
{
  write(1, s, n);
}

/* 
 * get one character at a time 
*/ 
int
getc(int fd)
{
  static int n = 0;
  static int i = 0;
  static char buf[512];

  if (i == n) {
    n = read(fd, buf, sizeof(buf));
    i = 0;
    
    if (n < 0) {
      printf(2, "sed: read error\n");
      exit();
    }
    else if (n == 0) {
      return EOF;
    }
  }

  return buf[i++];
}

/* open a file */
int
fopen(char *filename)
{
  int fd = open(filename, 0);
  if (fd < 0) {
    printf(2, "sed: cannot open %s\n", filename);
    exit();
  }
  return fd;
}

/* print out the usage in case of errors */
void
usage()
{
  printf(2, "sed: sed [-pattern] [-replacement] [file]\n");
}

int
main(int argc, char *argv[])
{
  // defaults
  int fd = 0;
  char *pat = "the"; // the pattern to search for
  char *rep = "xyz"; // pattern to replace 

  // figure out all the arguments
  switch (argc) {
  case 4:
    fd = fopen(argv[3]);
    // fall through
  case 3:
    pat = argv[1];
    rep = argv[2];

    if (pat[0] != '-' || rep[0] != '-') {
      usage();
      exit();
    }

    // start the strings after the '-'
    pat++;
    rep++;

    // check if we still have a string
    if (!*pat || !*rep) {
      usage();
      exit();
    }
    break;
  case 2:
    fd = fopen(argv[1]);
  case 1:
    // use defaults
    break;
  default:
    usage();
    exit();
  }

  // Now that we know what we are doing
  // do the actual work
  int c;
  uint count = 0;
  uint m = 0; // number of chars matched so far
  const uint n = strlen(pat); 

  while ((c = getc(fd)) != EOF) {
    // keep matching
    if (c == pat[m]) {
      m++;
    }
    else {
      // we don't have a match anymore
      // print the pattern we are searching for
      putn(pat, m);
      if (c == pat[0]) {
        m = 1;
      }
      else {
        putc(c);
        m = 0;
      }
    }

    if (m == n) { // match found!
      printf(1, "%s", rep);
      m = 0;
      count++;
    }
  }

  printf(1, "sed: found and replaced %d occurences of '%s' with '%s'\n",
         count, pat, rep);

  if (fd)
    close(fd);

  exit();
}
