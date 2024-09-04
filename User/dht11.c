/**
****************************(C) COPYRIGHT 2023 ͼ����****************************
* @file       dht11.c
* @brief      dht11��ʪ�ȴ���������
* @note       
* @history
*  Version    Date            Author
*  V1.0.0     06-17-2023      ͼ����    
*
****************************(C) COPYRIGHT 2023 ͼ����****************************
*/

#include "dht11.h"

/**
  * @brief          ΢����ʱ����
  * @param[in]      delay:��ʱ��ʱ��,��λ΢��
  * @retval         none
  */
static void delay_us(uint32_t delay)
{
	uint32_t i=0;  
	while(delay--)
	{
		i=10;
		while(i--) ;    
	}
}

/**
  * @brief          �˿ڱ�Ϊ���
  * @param[in]      none
  * @retval         none
  */
void dht11_io_out (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          �˿ڱ�Ϊ����
  * @param[in]      none
  * @retval         none
  */
void dht11_io_in (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          DHT11�˿ڸ�λ��������ʼ�źţ�IO���ͣ�
  * @param[in]      none
  * @retval         none
  */
void dht11_rst (void)
{
	dht11_io_out();
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_RESET);
	HAL_Delay(20); //��������18ms
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_SET);
	delay_us(30); //��������20~40us
}

/**
  * @brief          �ȴ�DHT11��Ӧ
  * @param[in]      none
  * @retval         ����1:δ��⵽DHT11������0:�ɹ���IO���գ�
  */
uint8_t dht11_check(void)
{ 
  uint8_t retry=0;
  dht11_io_in();//IO������״̬
  while (HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11������40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1; else retry=0;
  while (!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11���ͺ���ٴ�����40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1;
  return 0;
}

/**
  * @brief          ��DHT11��ȡһ��λ
  * @param[in]      delay:��ʱ��ʱ��,��λ΢��
  * @retval         ����ֵ��1/0
  */
uint8_t dht11_read_bit(void)
{ 
	uint8_t retry=0;
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//�ȴ���Ϊ�͵�ƽ
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//�ȴ���ߵ�ƽ
        retry++;
        delay_us(1);
    }
    delay_us(40);//�ȴ�40us	//�����жϸߵ͵�ƽ��������1��0
    if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin))return 1; else return 0;
}

/**
  * @brief          ��DHT11��ȡһ���ֽ�  
  * @param[in]      none
  * @retval         ����ֵ������������
  */
uint8_t dht11_read_byte(void)
{
	uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++){
        dat<<=1;
        dat|=dht11_read_bit();
    }
    return dat;
}

/**
  * @brief          DHT11��ʼ��
  * @param[in]      none
  * @retval         ����1:��ʼ��ʧ�� ����0:��ʼ���ɹ�
  */
uint8_t dht11_init (void)
{	
	dht11_rst();
  HAL_Delay(1000);
  dht11_rst();
	return dht11_check();
}

/**
  * @brief          ��ȡһ������//ʪ��ֵ(ʮ���ƣ���Χ:20%~90%) ���¶�ֵ(ʮ���ƣ���Χ:0~50��)������ֵ��0,����;1,ʧ��
  * @param[in]      delay:��ʱ��ʱ��,��λ΢��
  * @retval         none
  */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi)
{
  uint8_t buf[5];
  uint8_t i;
  dht11_rst();//DHT11�˿ڸ�λ��������ʼ�ź�
  if(dht11_check()==0)
  { //�ȴ�DHT11��Ӧ
    for(i=0;i<5;i++)
    {//��ȡ5λ����
      buf[i]=dht11_read_byte(); //��������
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
    {	//����У��
      *humi = buf[0]; //��ʪ��ֵ����ָ��1
      *temp = buf[2]; //���¶�ֵ����ָ��2
    }
  }
  else 
  {
    return 1;
  }
  return 0;
}
