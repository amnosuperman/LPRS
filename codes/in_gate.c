/* B.Saikrishna
 *  29.08.2011
 * program to store the license numbers of incoming and outgoing vehicles each day
 */
 
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <time.h>
 
 #include "in_gate.h"
 
 
 
 void convert_int_to_char(int num,char* ch)
 {
        
        if(num==0)
        {
                strcpy(ch,"january");
        }
        if(num==1)
        {
                strcpy(ch,"february");
        }
        if(num==2)
        {
                strcpy(ch,"march");
        }
        if(num==3)
        {
                strcpy(ch,"april");
        }
        if(num==4)
        {
                strcpy(ch,"may");
        }
        if(num==5)
        {
                strcpy(ch,"june");
        }
        if(num==6)
        {
                strcpy(ch,"july");
        }
        if(num==7)
        {
                strcpy(ch,"august");
        }
        if(num==8)
        {
                strcpy(ch,"september");
        }
        if(num==9)
        {
                strcpy(ch,"october");
        }
        if(num==10)
        {
                strcpy(ch,"november");
        }
        if(num==11)
        {
                strcpy(ch,"december");
        }
        
 } 
 
 void textdata(char* ptext)
 {
 
      char ch[100];
       int flag;
       time_t present_time;
       struct tm *pres_time,*day_time;
       int i=0;
       char temp[3],temp1[5];
       int j,k;

       time(&present_time);
       day_time = localtime(&present_time);
       pres_time = localtime(&present_time);
       convert_int_to_char(day_time->tm_mon,ch);
      
       day_time->tm_year = day_time->tm_year + 1900;
       
       temp[0] = 48 + ((day_time->tm_mday)/10);
       temp[1] = 48 + ((day_time->tm_mday)%10);
       temp[2] = '\0';       
       printf("%s \n",temp);
       strcat(ch,"_");
      // printf("%s ",ch);
       strcat(ch,temp);
       
       strcat(ch,"_");
      
       temp1[0] = 48 + (day_time->tm_year)/1000;
       day_time->tm_year = day_time->tm_year %1000;
       temp1[1] = 48 + (day_time->tm_year)/100;
       day_time->tm_year = day_time->tm_year %100;
       temp1[2] = 48 + (day_time->tm_year)/10;
       
       temp1[3] = 48 + (day_time->tm_year)%10;
       
       temp1[4] = '\0';
       
       strcat(ch,temp1);   
       strcat(ch,".bin");
       
       license number,pred_number;
       FILE *fin_gate;
       FILE *fin_gate1;
       FILE *fpre;                              //to read from predefined
       FILE *fpre1;                             //to write to ch
       
       fin_gate= fopen(ch,"wb");                               //change to "ab"
       fin_gate1= fopen(ch,"rb");
       
       fpre = fopen("predefined.bin","rb");
       fpre1 = fopen(ch,"ab");
       
       i= 1 ; 
      
       time(&present_time);
       pres_time = localtime(&present_time);
                
       j=0;
       k=0;
       //scanf("%s",number.num);
       time(&present_time);
       pres_time = localtime(&present_time);
       number.sec_in = pres_time->tm_sec;                      //0-59
       number.min_in = pres_time->tm_min;                      //0-59
       number.hrs_in = pres_time->tm_hour;                     //0-23
       number.date_in = pres_time->tm_mday;                    //1-31
       number.month_in = pres_time->tm_mon;                    // 0 is jan 11 is dec
       number.year_in = pres_time->tm_year;                    //years since 1900
       number.day_in = pres_time->tm_wday;                     //0 is sunday 6 is saturday
               
       number.sec_out=0;
       number.min_out=0;
       number.hrs_out=0;
       number.date_out=0;
       number.month_out=0;
       number.year_out = 0;
       number.day_out = 0;
       flag=1;
       
       strcpy(number.num,ptext);

       while((fread(&pred_number, sizeof(license),1,fpre)) && (flag==1))
       {
                        
       		if(strcmp(number.num,pred_number.num)==0)
            {
            	number.sec_out = 0;
                number.min_out = 0;
                number.hrs_out = 0;
                number.date_out = 0;
                number.month_out = 0;
                number.year_out = 0;
                number.day_out = 0;
                                
                number.sec_in = pres_time->tm_sec;                      //0-59
                number.min_in = pres_time->tm_min;                      //0-59
                number.hrs_in = pres_time->tm_hour;                     //0-23
                number.date_in = pres_time->tm_mday;                    //1-31
                number.month_in = pres_time->tm_mon;                    // 0 is jan 11 is dec
                number.year_in = pres_time->tm_year;                    //years since 1900
                number.day_in = pres_time->tm_wday;                     //0 is sunday 6 is saturday                                
                fseek(fpre1,sizeof(license)*(k),SEEK_SET);
                fwrite(&number, sizeof(license),1,fpre1);
                                               
                flag = 0;
                k++;
                
                        //}
                        
                j++;
                        
                fseek(fpre,sizeof(license)*(j),SEEK_SET);
            }
                
            if(flag==1)
            {
                fwrite(&number, sizeof(license),1,fin_gate);
                fseek(fin_gate,sizeof(license)*i,SEEK_SET);
            }

            i++;
       }
        
       fclose(fin_gate);
       fclose(fin_gate1);

 }

 
