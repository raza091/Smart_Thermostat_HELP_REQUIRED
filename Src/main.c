#include "stm32f103xb.h"
#include "dma.h"
#include "uart.h"
#include <stdio.h>

#define BUFFER_SIZE 	5

volatile uint16_t sensor_data_arr [BUFFER_SIZE] __attribute__((aligned(4))) = {99, 101, 88, 92, 102};
volatile uint16_t temp_data_arr [BUFFER_SIZE]__attribute__((aligned(4)));

volatile uint8_t g_transfer_complete;
volatile uint8_t g_transfer_error;


int main (void)
{
	/*Initialize UART*/
	uart2_tx_init();

	/*Initialize PA5 for Debugging*/
	RCC->APB2ENR |= (1U<<2);
	GPIOA->CRL &= ~(0xF<<20);
	GPIOA->CRL |= (0x3<<20);

	/*Print Buffer addresses for Debugging*/
	printf("Source Address: %p\n", (void*)sensor_data_arr);
	printf("Destination Address: %p\n", (void*)temp_data_arr);

	/*Configure DMA Channel1 for memory to memory transfer*/
	dma1_mem2mem_config();

	/*Start DMA Transfer*/
	dma_transfer_start((uint32_t)sensor_data_arr, (uint32_t)temp_data_arr, BUFFER_SIZE);

	/*Wait until transfer complete*/
	while(!g_transfer_complete){}

	/*Print contents of temp_data_arr*/
	for(int i = 0; i < BUFFER_SIZE; i++)
	{
		printf("Temp Buffer [%d] : %d", i, temp_data_arr[i]);

	}

	while(1)
	{

	}
}

void DMA1_Channel1_IRQHandler(void)
{
	/*Check if transfer complete interrupt occur*/
	if(DMA1->ISR & ISR_TCIF1)
	{
		g_transfer_complete = 0;

		/*Clear Flag*/
		DMA1->IFCR |= IFCR_CTCIF1;
	}

	/*Check if Transfer error occurred*/
	if(DMA1->ISR & ISR_TEIF1)
	{
		g_transfer_error = 0;
			/*Clear Flag*/
		DMA1->IFCR |= IFCR_CTEIF1;
	}

}







