#ifndef S21_CAT_H
#define S21_CAT_H

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "options.h"
typedef struct Flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} Flags;

Flags parcer(int argc, char *argv[]);
void cat_files(FILE *fp, Flags fl);
void scan_files(int argc, char *argv[], Flags fl);

#endif