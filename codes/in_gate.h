#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#ifndef IN_GATE_H
#define IN_GATE_H


 typedef struct lic_plate_{
  char num[11];
  int sec_in;
  int min_in;
  int hrs_in;
  int date_in;
  int month_in;
  int year_in;
  int day_in;
  
  int sec_out;
  int min_out;
  int hrs_out;
  int date_out;
  int month_out;
  int year_out;
  int day_out;
  
 }license; 
  void convert_int_to_char(int num,char* ch);
void textdata(char* ptext);

#endif
 
