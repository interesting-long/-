/*
 * Flash.c
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 */
#include "Flash.h"

/**
 * @brief �洢���� float ���鵽 Flash ��һҳ
 * @param array ��ά��������Num��
 * @param Page Ŀ�� Flash ҳ
 * @example Flash_Save_Float(Num, 0); // �洢 Num �������ݵ� Flash �� 0 ҳ
 */
void Flash_Save_Float(float (*array)[Page_Row_Number], unsigned char Page)
{
    flash_buffer_clear(); // ȷ���������Ǹɾ���

    // ���������к��У���仺����
    for (int row = 0; row < Num_Col; row++) // Num_Row ��������
    {
        for (int col = 0; col < Page_Row_Number; col++)
        {
            // ���㻺�����������������ȴ洢��
            int buffer_index = row * Page_Row_Number + col;
            flash_union_buffer[buffer_index].float_type = array[row][col];
        }
    }

    // ��� Flash ҳ�Ƿ��������ݣ��������
    if (flash_check(FLASH_SECTION_INDEX, Page))
    {
        flash_erase_page(FLASH_SECTION_INDEX, Page);
    }

    // д�� Flash
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, Page);
}
/**
 * @brief �� Flash ��һҳ��ȡ���� float ��ά����
 * @param array Ŀ�����飨Num��
 * @param Page Դ Flash ҳ
 * @example Flash_Load_Float(Num, 0); // �� Flash �� 0 ҳ��ȡ�������ݵ� Num
 */
void Flash_Load_Float(float (*array)[Page_Row_Number], unsigned char Page)
{
    if (!flash_check(FLASH_SECTION_INDEX, Page)) // ��� Flash ҳ�Ƿ�������
    {
        return; // �����ݣ�ֱ�ӷ��أ�����������
    }

    flash_buffer_clear(); // ȷ���������ɾ�
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, Page); // ��ȡ Flash ��������

    // ���������к��У����Ŀ������
    for (int row = 0; row < Num_Col; row++) // Num_Row ��������
    {
        for (int col = 0; col < Page_Row_Number; col++)
        {
            // ���㻺�����������������ȴ洢��
            int buffer_index = row * Page_Row_Number + col;
            array[row][col] = flash_union_buffer[buffer_index].float_type;
        }
    }
}
/**
 * @brief ��ȡ���� Flash ҳ���ݵ� Num ����
 * @note ���� Num ��������� = Flash ҳ����Num_Row = Num_Col��
 * @example Get_Num_All();
 */
void Get_Num_All(void)
{
        // �ӵ� page ҳ��ȡ���ݵ� Num ����
        Flash_Load_Float(Num, 0);
}
/**
 * @brief ��� Num �����������ݣ���������Ӧ Flash ҳ
 * @note ʹ��ȫ����ʱ���鸲�� Flash
 * @example Clear_Num_ALL();
 */
void Clear_Num_ALL(void)
{
    // ����ȫ����ʱ���飨��С = һҳ Flash ��������
    float temp_zero[Page_Row_Number] = {0};
    float (*temp_array)[Page_Row_Number] = &temp_zero;

    // �������� Flash ҳ��д��ȫ������
    for (unsigned char page = 0; page < 1; page++)
    {
        Flash_Save_Float(temp_array, page); // д��ȫ�㵽�� page ҳ
    }

    // ���¶�ȡ Flash��ȷ�� Num ����ͬ��Ϊ 0
    Get_Num_All();
}
///*
// * �������洢float���麯��
// * ����1������������
// * ����2���������������Num_Col*Page_Row_Number
// * ����3������������Flash��ҳ��
// * ���ӣ�Flash_Save_Float(Num,0,0);    ��Num�ĵ�һ�����ݴ洢��Flash�ĵ�һҳ����
// */
//void Flash_Save_Float(float (*array)[Page_Row_Number], uint8_t row_index,unsigned char Page)
//{
//    flash_buffer_clear();//ȷ���������Ǹɾ���
//    for(int i=0;i<Page_Row_Number;i++)
//    {
//        flash_union_buffer[i].float_type = array[row_index][i];
//    }
//    if(flash_check(FLASH_SECTION_INDEX, Page))                      // �ж��Ƿ�������
//    {
//        flash_erase_page(FLASH_SECTION_INDEX, Page);                // ������һҳ
//    }
//    flash_write_page_from_buffer(FLASH_SECTION_INDEX, Page);        // ��ָ�� Flash ������ҳ��д�뻺��������
//}
