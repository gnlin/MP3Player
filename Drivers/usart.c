/****************************************************************************
*
* �ļ���: usart.c
* ���ݼ���: ��ģ��ʵ��printf��scanf�����ض��򵽴���1
*	ʵ���ض���ֻ��Ҫ���2������
		int fputc(int ch, FILE *f);
		int fgetc(FILE *f);
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.1    2009-12-27 armfly  �������ļ�
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "usart.h"

/*******************************************************************************
	��������PrintfLogo
	��  ��: �������ƺ���������������
	��  ��:
	����˵����
*/
void PrintfLogo(char *strName, char *strDate)
{
	printf("*************************************************************\n\r");
	printf("* Application Name : %s\r\n", strName);
	printf("* Update Date  : %s\r\n", strDate);
	printf("* StdPeriph_Lib Version : V3.4.0\n\r");
	printf("* \n\r");
	printf("* Copyright Lin Guannan \r\n");
	printf("* Email : gnuemacs@126.com \r\n");
	printf("*************************************************************\n\r");
}

/*******************************************************************************
	��������USART_Configuration
	��  ��:
	��  ��:
	����˵����
	��ʼ������Ӳ���豸��δ�����жϡ�
	���ò��裺
	(1)��GPIO��USART��ʱ��
	(2)����USART�����ܽ�GPIOģʽ
	(3)����USART���ݸ�ʽ�������ʵȲ���
	(4)���ʹ��USART����
*/
void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* ��4��������USART����
	    - BaudRate = 115200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* ��5����ʹ�� USART�� ������� */
	USART_Cmd(USART1, ENABLE);

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
}

/*******************************************************************************
	��������fputc
	��  ��:
	��  ��:
	����˵����
	�ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*/
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/*******************************************************************************
	��������fputc
	��  ��:
	��  ��:
	����˵����
	�ض���getc��������������ʹ��scanff�����Ӵ���1��������
*/
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART1);
}
