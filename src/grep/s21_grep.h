#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#define STRLEN 1024

typedef struct Flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int error;
  char regexfile[STRLEN];
  char patterns[STRLEN];
  char files[STRLEN];
  int num_patterns;
  int num_files;
} Flags;
void scan_files(char* argv[], Flags* fl);
void regex_files(Flags* fl, FILE* f_patterns);
void parcer(int argc, char* argv[], Flags* fl);
void check_file(char* file_name, Flags* fl, FILE* f_files);
int search_pattern(char* pattern, Flags* fl, char* line, int print);
void standart_output(Flags* fl, char* line);
void past_output(Flags* fl, char* file, int count);
void grep_files(Flags* fl);
void prev_output(Flags* fl, char* file, int printed, int line_num);

#endif