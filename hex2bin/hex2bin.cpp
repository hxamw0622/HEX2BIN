// hex2bin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "hex2bin.h"

using namespace std;

int sigment_base_addr = 0x0;

int hex_to_int(char s)
{
	if(s>='0'&&s<='9')
		return s -'0';
	if(s>='A'&&s<='F')
		return s - 55;
	if(s>='a'&&s<='f')
		return s - 87;

}

bool check_data(char data[],int crc,int length)
{
	int i=0;
	int sum=0;
	int tmp_crc = 0;
	for(i=0;i<(length-2);)
	{
		sum += (hex_to_int(data[i])*16+hex_to_int(data[i+1]));
		i   += 2;
	}
	tmp_crc = 256 - sum%256;
	if(tmp_crc == crc)
		return true;
	else
		return false;

}

void print_bin_data(char s[],int length)
{
	int i;
	for(i=0;i<length;i++)
	{
		cout<<hex<<(int)s[i];
	}
	cout<<endl;
}

int calculate_char_array(char hex_data[],int length)
{
     int i=0;
	 int weight = 1;
	 int value=0;
	 for(i=length-1;i>=0;i--)
	 {
		 value += hex_data[i]*weight;
		 weight *= 16;
	 }
	 return value;
}

/********************************************************************
   return -1: CRC check error
   return 0 : pure data,store in array bin_data, in address data_addr
   return 1 : EOF
   return 2 : extend segment address record,only used to update segment_base_addr
   return 3 : start sigment address, boot from this address
   return 4 : linear addr offset ,absolute address = segment_base_addr*16 + linear_addr_offset
   return 5 : linear base address, boot from this address
*********************************************************************/
//without char ':'
int analysis_hex(char hex_data[], char bin_data[],int *absolute_addr,int *start_addr, int *bin_length)
{
    char *p;
    int data_length;
	int data_type;
	int i=0;
    int return_value=0;
	int tmp_addr;
	int check_sum;
    p = hex_data;

    data_length =  hex_to_int(*p++)<<4;
	data_length +=  hex_to_int(*p++);

    tmp_addr    =  hex_to_int(*p++)<<12 ;
	tmp_addr    += hex_to_int(*p++)<<8;
	tmp_addr    += hex_to_int(*p++)<<4;
	tmp_addr    += hex_to_int(*p++); 

    data_type   =  hex_to_int(*p++)<<4;
	data_type   +=  hex_to_int(*p++);


    for(i=0;i<data_length*2;i++)
    {
     	bin_data[i] = hex_to_int(*p++);
    } 
    bin_data[i] = '\0';

	*bin_length = i;

	check_sum = hex_to_int(*p++)*16+hex_to_int(*p);

	if(check_data(hex_data,check_sum,strlen(hex_data))==true)
		cout<<"verify ok"<<endl;
	else
	{
		cout<<"verify fail"<<endl;
		return -1;
	}

    switch(data_type)
    {
    	case DATA_RECORD:                   *absolute_addr =  tmp_addr;
			                                return_value   =  DATA_RECORD;
    	                                    break;

    	case END_OF_FILE_RECORD:            *absolute_addr =  -1;
			                                return_value   =  END_OF_FILE_RECORD; 
    	                                    break;

    	case EXTEND_SEGMENT_ADDRESS_RECORD: sigment_base_addr =  calculate_char_array(bin_data,*bin_length);
			                                *absolute_addr    =  -1;
    	                                    return_value      =  EXTEND_SEGMENT_ADDRESS_RECORD;
    	                                    break;
        case START_SEGMENT_ADDRESS_RECORD:  *start_addr       =  calculate_char_array(bin_data,*bin_length);
    	                                    return_value      =  START_SEGMENT_ADDRESS_RECORD;
    	                                    break;
		case EXTEND_LINEAR_ADDRESS_RECORD:  *absolute_addr    =  sigment_base_addr<<16 | tmp_addr;
    	                                    return_value      =  EXTEND_LINEAR_ADDRESS_RECORD;
    	                                    break;

		case START_LINEAR_ADDRESS_RECORD:   *start_addr       =  calculate_char_array(bin_data,*bin_length);
    	                                    return_value      =  START_LINEAR_ADDRESS_RECORD;
    	                                    break;

    }

    return return_value;


}

int _tmain(int argc, _TCHAR* argv[])
{
	fstream hexfile,binfile;
	char hex_buffer[1000]={0},bin_buffer[1000]={0};
	int absolute_addr = 0;
	int start_addr = 0;
	int bin_length = 0;
	
    hexfile.open("test.hex",ios::in);
    if(hexfile.fail())
    {
    	cout<<"open hex file error"<<endl;
    	system("pause");
    	return -1;
    }

    binfile.open("test.bin",ios::out | ios::binary);
    if(binfile.fail())
    {
    	cout<<"open bin file error"<<endl;
    	system("pause");
    	return -1;
    }
   hexfile.getline(hex_buffer,1000,'\n');
   while(!hexfile.eof())
   {
	   cout<<"hex_buffer:"<<hex_buffer<<endl;
	   analysis_hex(hex_buffer,bin_buffer,&absolute_addr,&start_addr,&bin_length);   
	   cout<<"bin_buffer : ";
	   print_bin_data(bin_buffer,bin_length);
	   cout<<" absolute_addr: 0x"<<absolute_addr<<" start_addr: 0x"<<start_addr<<endl;
	   system("pause");
	   memset(hex_buffer,0,1000);
       memset(bin_buffer,0,1000);
	   hexfile.getline(hex_buffer,1000,'\n'); 
	   
   }
   hexfile.close();
   system("pause");
   return 0;
}

