/*
 * file_opera.h
 *
 *  Created on: Dec 13, 2019
 *      Author: wwb
 */

#ifndef FILE_OPERA_H_
#define FILE_OPERA_H_

#include	"main.h"

// === 3 general function functions==============================
// Modify the time stamp of a file using the date and time in RTC format
void fat_SetTimeStamp(TCHAR *filename, const RTC_DateTypeDef *sDate, const RTC_TimeTypeDef *sTime);
// Convert file time stamp data to RTC format date and time
void fat_GetTimeStamp(const FILINFO *fno, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
DWORD fat_GetFatTimeFromRTC();						//Get time as FatFS time stamp data from RTC

// ==== FatFS file read and write test function=======================
void	fatTest_WriteBinFile(TCHAR *filename, uint32_t pointCount, uint32_t sampFreq);
void	fatTest_ReadBinFile(TCHAR *filename);		//Test to read a binary file

void	fatTest_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day);
void	fatTest_ReadTXTFile(TCHAR *filename);		//Test to read a text file

void	fatTest_GetDiskInfo( );									//Get the FAT information of the drive

void	fatTest_GetFileInfo(TCHAR *filename); 		//Get information about a file

void	fatTest_ScanDir(const TCHAR* PathName);  //Scan and display files and directories in directories

void	fatTest_RemoveAll();									//Delete all files and directories in the root directory

void	TestFindFiles(TCHAR *filter); 						//Find files

#endif /* FILE_OPERA_H_ */
