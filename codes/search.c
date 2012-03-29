/* B.Saikrishna
 *  29.08.2011
 * program to store the license numbers of incoming and outgoing vehicles each day
 */
 
 #include<stdio.h>
 #include<string.h>
 #include<stdlib.h>
 #include<time.h>
 
 char ch[100];
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
 
 int main()
 {
 
 time_t present_time;
       struct tm *pres_time,*day_time;
 printf("enter the date on which you want to check the records \nenter it in 'month_date_year' format\nexample august_29_2011\n");
 char ch[100];
 scanf("%s",ch);
 strcat(ch,".bin");
 FILE *fp;
 fp = fopen(ch,"rb");
  time(&present_time);
       day_time = localtime(&present_time);
       pres_time = localtime(&present_time);
 license number;
 number.day_in = day_time->tm_mday;
  number.day_out = day_time->tm_mday;
 int i=1;
         printf("\n\nlicense number           ENTER                      EXIT\n");  
         printf("license number  hours  minutes  seconds   hours  minutes  seconds\n");  
        // for(i=1;i<=3;i++)  
        int x=0;  
        int size;
        fseek(fp,-sizeof(license),SEEK_END);
        size = ftell(fp);     
        fseek(fp,0,SEEK_SET);                               //change it to while loop while(day_time->tm_mday == pres_time->tm_mday) and increment i inside
       while(x<size)
       {        
                //printf("hi \n");
                //time(&present_time);
               // pres_time = localtime(&present_time);
                fseek(fp,sizeof(license)*(i-1),SEEK_SET);
                x = ftell(fp);
                fread(&number, sizeof(license),1,fp);
                              
                printf("%s           ",number.num);
                printf("%d     %d       %d         %d      %d        %d\n",number.hrs_in, number.min_in, number.sec_in,number.hrs_out, number.min_out,             number.sec_out);
     
                i++;
       } 
        printf("\n");        
        fclose(fp);     
 
 
 
 return 0;
 }
