/*
 * Flash.h
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

#define FLASH_SECTION_INDEX       (0)                                 // �洢�����õ�����
#define FLASH_PAGE_INDEX          (8)                                // �洢�����õ�ҳ�� ������һ��ҳ��

#include "zf_common_headfile.h"

void Flash_Save_Float(float (*array)[Page_Row_Number], unsigned char Page);
void Flash_Load_Float(float (*array)[Page_Row_Number], unsigned char Page);
void Get_Num_All(void);
void Clear_Num_ALL(void);
#endif /* CODE_FLASH_H_ */
