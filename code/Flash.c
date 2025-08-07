/*
 * Flash.c
 *
 *  Created on: 2025年7月15日
 *      Author: interesting
 */
#include "Flash.h"

/**
 * @brief 存储整个 float 数组到 Flash 的一页
 * @param array 二维数组名（Num）
 * @param Page 目标 Flash 页
 * @example Flash_Save_Float(Num, 0); // 存储 Num 所有数据到 Flash 第 0 页
 */
void Flash_Save_Float(float (*array)[Page_Row_Number], unsigned char Page)
{
    flash_buffer_clear(); // 确保缓冲区是干净的

    // 遍历所有行和列，填充缓冲区
    for (int row = 0; row < Num_Col; row++) // Num_Row 是总行数
    {
        for (int col = 0; col < Page_Row_Number; col++)
        {
            // 计算缓冲区索引（按行优先存储）
            int buffer_index = row * Page_Row_Number + col;
            flash_union_buffer[buffer_index].float_type = array[row][col];
        }
    }

    // 检查 Flash 页是否已有数据，有则擦除
    if (flash_check(FLASH_SECTION_INDEX, Page))
    {
        flash_erase_page(FLASH_SECTION_INDEX, Page);
    }

    // 写入 Flash
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, Page);
}
/**
 * @brief 从 Flash 的一页读取整个 float 二维数组
 * @param array 目标数组（Num）
 * @param Page 源 Flash 页
 * @example Flash_Load_Float(Num, 0); // 从 Flash 第 0 页读取所有数据到 Num
 */
void Flash_Load_Float(float (*array)[Page_Row_Number], unsigned char Page)
{
    if (!flash_check(FLASH_SECTION_INDEX, Page)) // 检查 Flash 页是否有数据
    {
        return; // 无数据，直接返回（或做错误处理）
    }

    flash_buffer_clear(); // 确保缓冲区干净
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, Page); // 读取 Flash 到缓冲区

    // 遍历所有行和列，填充目标数组
    for (int row = 0; row < Num_Col; row++) // Num_Row 是总行数
    {
        for (int col = 0; col < Page_Row_Number; col++)
        {
            // 计算缓冲区索引（按行优先存储）
            int buffer_index = row * Page_Row_Number + col;
            array[row][col] = flash_union_buffer[buffer_index].float_type;
        }
    }
}
/**
 * @brief 读取所有 Flash 页数据到 Num 数组
 * @note 假设 Num 数组的行数 = Flash 页数（Num_Row = Num_Col）
 * @example Get_Num_All();
 */
void Get_Num_All(void)
{
        // 从第 page 页读取数据到 Num 数组
        Flash_Load_Float(Num, 0);
}
/**
 * @brief 清空 Num 数组所有数据，并擦除对应 Flash 页
 * @note 使用全零临时数组覆盖 Flash
 * @example Clear_Num_ALL();
 */
void Clear_Num_ALL(void)
{
    // 创建全零临时数组（大小 = 一页 Flash 的容量）
    float temp_zero[Page_Row_Number] = {0};
    float (*temp_array)[Page_Row_Number] = &temp_zero;

    // 遍历所有 Flash 页，写入全零数据
    for (unsigned char page = 0; page < 1; page++)
    {
        Flash_Save_Float(temp_array, page); // 写入全零到第 page 页
    }

    // 重新读取 Flash，确保 Num 数组同步为 0
    Get_Num_All();
}
///*
// * 函数：存储float数组函数
// * 参数1：给出数组名
// * 参数2：给出保存的行数Num_Col*Page_Row_Number
// * 参数3：给出保存再Flash的页数
// * 例子：Flash_Save_Float(Num,0,0);    将Num的第一行数据存储到Flash的第一页当中
// */
//void Flash_Save_Float(float (*array)[Page_Row_Number], uint8_t row_index,unsigned char Page)
//{
//    flash_buffer_clear();//确保缓冲区是干净的
//    for(int i=0;i<Page_Row_Number;i++)
//    {
//        flash_union_buffer[i].float_type = array[row_index][i];
//    }
//    if(flash_check(FLASH_SECTION_INDEX, Page))                      // 判断是否有数据
//    {
//        flash_erase_page(FLASH_SECTION_INDEX, Page);                // 擦除这一页
//    }
//    flash_write_page_from_buffer(FLASH_SECTION_INDEX, Page);        // 向指定 Flash 扇区的页码写入缓冲区数据
//}
