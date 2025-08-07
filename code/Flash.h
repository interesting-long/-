/*
 * Flash.h
 *
 *  Created on: 2025年7月15日
 *      Author: interesting
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (8)                                // 存储数据用的页码 倒数第一个页码

#include "zf_common_headfile.h"

void Flash_Save_Float(float (*array)[Page_Row_Number], unsigned char Page);
void Flash_Load_Float(float (*array)[Page_Row_Number], unsigned char Page);
void Get_Num_All(void);
void Clear_Num_ALL(void);
#endif /* CODE_FLASH_H_ */
