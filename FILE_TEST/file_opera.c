/*
 * file_opera.c
 *
 *  Created on: Dec 13, 2019
 *      Author: Wang Weibo
 *      Function: Test FatFS's application interface function
 */

#include "usart.h"
#include "ff.h"
#include "fatfs.h"
#include "file_opera.h"

#include "rtc.h"
#include <stdio.h>


DWORD dt={};

void fatTest_WriteBinFile(TCHAR *filename, uint32_t pointCount, uint32_t sampFreq)
{
	FIL file;
	FRESULT res=f_open(&file,filename, FA_CREATE_ALWAYS | FA_WRITE);

	if(res == FR_OK)
	{
		TCHAR headStr[]="ADC1-IN5\n";
		f_puts(headStr, &file); 	//Write string data, ending with "\n" without "\0"

		UINT  bw=0;  				//Actual number of bytes written
		f_write(&file, &pointCount, sizeof(uint32_t), &bw);  //Number of data points
		f_write(&file, &sampFreq, sizeof(uint32_t), &bw);    //Sampling frequency

		uint32_t  value=1000;
		for(uint16_t i=0; i<pointCount; i++,value++)
			f_write(&file, &value, sizeof(uint32_t), &bw);

		printf("Write file OK: %s\r\n",filename);			//Put a formatted string to the file
	}

	f_close(&file);
}

void fatTest_ReadBinFile(TCHAR *filename)
{
	FIL file;
	FRESULT res=f_open(&file,filename, FA_READ);

	if(res == FR_OK)
	{
		TCHAR str[50];
		f_gets(str,50, &file);								//Read 1 string

		UINT  bw=0;											//The actual number of bytes read
		uint32_t pointCount, sampFreq;						//Save variables for reading data
		f_read(&file, &pointCount, sizeof(uint32_t), &bw);	//Number of data points
		f_read(&file, &sampFreq, sizeof(uint32_t), &bw);	//Sampling frequency

		uint32_t  value;
		for(uint16_t i=0; i< pointCount; i++)
			f_read(&file, &value, sizeof(uint32_t), &bw);

		//USART display
		printf("Reading BIN file: %s",str);
		printf("Sampling freq: %ld\r\n",sampFreq);
		printf("Point count: %ld\r\n",pointCount);
	}
	else if (res==FR_NO_FILE)
		printf("File does not exist.\r\n");
	else
		printf("f_open() error.\r\n");

	f_close(&file);
}


//Create a text file and write texts to the file.
void fatTest_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day)
{
	FIL file;
	FRESULT res=f_open(&file,filename, FA_CREATE_ALWAYS | FA_WRITE);

	if(res == FR_OK)
	{
		TCHAR str[]="Line1: Hello, FatFS\n";	//The string must have a newline character "\n"
		f_puts(str, &file);						//The ending character "\0" will not be written

		TCHAR str2[]="Line2: UPC, AnHui Hefei\n";
		f_puts(str2, &file);

		f_printf(&file, "Line3: Date: %d-%d-%d\n",year,month,day);

		printf("Write file OK: %s\r\n",filename);
	}
	else
		printf("Open file error.\r\n");
	f_close(&file);
}

//Read the texts of a text file.
void fatTest_ReadTXTFile(TCHAR *filename)
{
	FIL file;
	FRESULT res=f_open(&file,filename, FA_READ);   //Open the file read-only

	if(res == FR_OK)
	{
		TCHAR str[100];
		while (!f_eof(&file))
		{
			f_gets(str,100, &file);	//Read 1 string and automatically add the ending character "\0"
			printf("Reading TXT file: %s",str);
		}
	}
	else if (res==FR_NO_FILE)
		printf("File does not exist.\r\n");
	else
		printf("f_open() error.\r\n");

	f_close(&file);
}


/* Get disk information and display it on USART Assistant */
void fatTest_GetDiskInfo()
{
	FATFS *fs;
	DWORD fre_clust;									//The number of remaining clusters
	FRESULT res = f_getfree("0:", &fre_clust, &fs); 	//Need to call refresh, fre_clust

	if (res != FR_OK)
	{
		printf("f_getfree() error.\r\n");
		return;
	}

	printf("\r\n*** FAT disk info ***.\r\n\r\n");

	DWORD  tot_sect = (fs->n_fatent - 2) * fs->csize;	//Total number of sectors
	DWORD  fre_sect = fre_clust * fs->csize;			//The number of sectors remaining = the number of clusters remaining * the number of sectors per cluster
#if _MAX_SS == _MIN_SS									//For SD cards and USB drives, _MIN_SS=512 bytes
	//	The SD card's _MIN_SS is fixed to 512, and the right shift of 11 bits is equivalent to dividing by 2048
	DWORD  freespace= (fre_sect>>11);					//Remaining space size, unit: MB, used for SD card, USB drive
	DWORD  totalSpace= (tot_sect>>11);					//Total space size, unit: MB, used for SD card, USB drive
#else													//Flash memory, small capacity
	DWORD  freespace= (fre_sect*fs->ssize)>>10;			//Remaining space size, unit: KB
	DWORD  totalSpace= (tot_sect*fs->ssize)>>10;		//Total space size, unit: KB
#endif

	printf("FAT type=  %d\r\n", fs->fs_type);
	printf("[1=FAT12,2=FAT16,3=FAT32,4=exFAT] \r\n");

#if  _MAX_SS == _MIN_SS									//SD card, USB drive
	printf("Sector size(bytes)=  %d\r\n",_MIN_SS);
#else
	printf("Sector size(bytes)=  %d\r\n", fs->ssize);
#endif

	printf("Cluster size(sectors)=  %d\r\n", fs->csize);
	printf("Total cluster count=  %ld\r\n", fs->n_fatent-2);
	printf("Total sector count=  %ld\r\n", tot_sect);

#if  _MAX_SS == _MIN_SS  								//SD card, USB drive
	printf("Total space(MB)=  %ld\r\n", totalSpace);
#else
	printf("Total space(KB)=  %ld\r\n", totalSpace);
#endif

	printf("Free cluster count=  %ld\r\n", fre_clust);
	printf("Free sector count=  %ld\r\n", fre_sect);

#if  _MAX_SS == _MIN_SS  								//SD card, USB drive
	printf("Free space(MB)=  %ld\r\n", freespace);
#else
	printf("Free space(KB)=  %ld\r\n", freespace);
#endif

	printf("Get FAT disk info OK.\r\n");
}

//Get file information for a file
void fatTest_GetFileInfo(TCHAR *filename)
{
	printf("File info of:  %s\r\n", filename);

	FILINFO fno;
	FRESULT fr=f_stat(filename, &fno);

	if (fr==FR_OK)
	{
		printf("File size(DWORD)= %ld\r\n",fno.fsize);
		printf("File fdate(WORD)= %d\r\n",fno.fdate);
		printf("File ftime(WORD)= %d\r\n",fno.ftime);
		printf("File attribute=  %d\r\n",fno.fattrib);
		printf("File Name=  %s\r\n",fno.fname);

		RTC_DateTypeDef sDate;
		RTC_TimeTypeDef sTime;
		char str[40];

		//test
		HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
		HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);

		//Test RTC DATE
		sprintf(str,"RTC Date= %d-%2d-%2d",2000+sDate.Year,sDate.Month,sDate.Date);
		printf("%s\r\n",str);

		//Test RTC TIME
		sprintf(str,"RTC Time= %2d:%2d:%2d",sTime.Hours, sTime.Minutes,sTime.Seconds);
		printf("%s\r\n",str);

		//Set the FILE timestamp by RTC.
		fat_GetFatTimeFromRTC();
		fno.ftime = (dt & 0xFFFF);
		dt = dt >>16;
		fno.fdate = (dt & 0xFFFF);

		fat_GetTimeStamp(&fno, &sDate, &sTime);  //Convert time stamps to easy-to-display format


		//sprintf(str,"File Date= %d",fno.fdate);
		sprintf(str,"File Date= %d-%2d-%2d",2000+sDate.Year,sDate.Month,sDate.Date);
		printf("%s\r\n",str);

		//sprintf(str,"File Time= %d",fno.ftime);
		sprintf(str,"File Time= %2d:%2d:%2d",sTime.Hours, sTime.Minutes,sTime.Seconds);
		printf("%s\r\n",str);
	}
	else if (fr==FR_NO_FILE)
		printf("File does not exist. \r\n");
	else
		printf("f_stat() error. \r\n");
}

//Set time stamp
//Set a time stamp for a file or directory
//need configure Enable _USE_CHMOD && !_FS_READONLY
void fat_SetTimeStamp(TCHAR *filename, const RTC_DateTypeDef *sDate, const RTC_TimeTypeDef *sTime)
{
	FILINFO fno;
	WORD date=(2000+sDate->Year-1980)<<9;
	WORD month=(sDate->Month)<<5;
	fno.fdate = date | month | (sDate->Date);

	WORD time=(sTime->Hours)<<11;
	WORD minute=(sTime->Minutes)<<5;
	fno.ftime =time | minute | ((sTime->Seconds) >>1);

	f_utime(filename, &fno);

// date2019-12-13
//	WORD date=(2019-1980)<<9;
//	WORD month=12<<5;
//	fno.fdate = date | month |13;
//
// time 14:32:15
//	WORD time=14<<11;
//	WORD minute=32<<5;
//	WORD sec=15>>1;		//Divided by 2
//	fno.ftime =time | minute | sec;
//	f_utime("readme.txt", &fno);
}

// Convert date time from a FILINFO to RTC date time
void fat_GetTimeStamp(const FILINFO *fno, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
//	WORD fdate;			/* Modified date */
//	bit[15:9], date from 1980  ; bit[8:5], month; bit[4:0], day

//	WORD ftime;			/* Modified time */
//	bit[15:11], hour; bit[10:5], minute; bit[4:0], second/2

	WORD date=fno->fdate;				//Temporary variables
	sDate->Date= (date & 0x001F);		//Day
	date = date>>5;
	sDate->Month= (date & 0x000F);		//Month
	date= date>>4;						//year from 1980
	sDate->Year= (1980+date-2000);		//Year, from 2000

	WORD time=fno->ftime;				//Temporary variables
	sTime->Seconds= (time & 0x001F)<<1;	//seconds
	time =time >>5;
	sTime->Minutes = time & 0x003F;		//minute
	sTime->Hours = (time >>6);			//hour
}

//Get time from RTC as the file system time stamp data
//dt is modified global variable.
DWORD fat_GetFatTimeFromRTC()
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	if (HAL_RTC_GetTime(&hrtc, &sTime,  RTC_FORMAT_BIN) == HAL_OK)
	{
		HAL_RTC_GetDate(&hrtc, &sDate,  RTC_FORMAT_BIN);
		WORD date=(2000+sDate.Year-1980)<<9;
		date = date |(sDate.Month<<5) |sDate.Date;

		WORD time=sTime.Hours<<11;
		time = time | (sTime.Minutes<<5) | (sTime.Seconds>1);
		dt=(date<<16) | time;
		return dt;
	}
	else
		return 0;
}


// Scan and display files and directories in the specified directory
void fatTest_ScanDir(const TCHAR* PathName)
{
	DIR dir;			//Directory Objects
	FILINFO fno;		//File information

	//FRESULT res = f_opendir(&dir, "0:"); 	//open dir
	FRESULT res = f_opendir(&dir, PathName);

	if (res != FR_OK)
	{
		f_closedir(&dir);
		return;
	}

	printf("All entries in dir %s.\r\n",PathName);

	while(1)
	{
		res = f_readdir(&dir, &fno);    //Read an item in the directory
		if (res != FR_OK || fno.fname[0] == 0)
			break;  // The file name is empty means that there are not many items to read.

		if (fno.fattrib & AM_DIR)  		//It's a directory
			printf("DIR   %s.\r\n",fno.fname);
		else  							//It's a file
			printf("FILE  %s.\r\n",fno.fname);
	}

	printf("Scan dir OK.\r\n");

	f_closedir(&dir);
}

void fatTest_RemoveAll()				//Delete all files and directories in the root directory
{
	DIR dir;							//Directory Objects
	FILINFO fno;						//File information
	FRESULT res = f_opendir(&dir, "0:");//Open the root directory

	if (res != FR_OK)
	{
		f_closedir(&dir);
		return;
	}

	printf("Remove all entries...\r\n");
	uint16_t  cnt=0;
	while(1)
	{
		res = f_readdir(&dir, &fno);			//Read an item in the directory
		if (res != FR_OK || fno.fname[0] == 0)
			break;  							//The file name is empty means that there are not many items to read.
		f_unlink(fno.fname);					//Delete 1 file
		cnt++;
	}

	f_closedir(&dir);

	printf("Removed entries count= %d\r\n",cnt);
}

// List matching files in the directory, such as *.txt
void TestFindFiles(TCHAR *filter)
{
	DIR dir;         										/* Search directory object */
	FILINFO fno;     										/* File information */
	//FRESULT fr = f_findfirst(&dir, &fno, "", "*.txt");	/* Find the first matching file */
	FRESULT fr = f_findfirst(&dir, &fno, "", filter);  		/* Find the first matching file */

	while (fr == FR_OK && fno.fname[0])
	{
		printf(" %s\r\n",fno.fname);
		fr = f_findnext(&dir, &fno);    					/* Find the next match */
	}
	f_closedir(&dir);										/* Close the directory */
}
