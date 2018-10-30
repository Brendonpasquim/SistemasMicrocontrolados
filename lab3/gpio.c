// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

#define GPIO_PORTAS (0x5F87) //bits 8 e 12 2_0101 1111 1000 0111 A B C K L M N Q

void SysTick_Wait1ms(uint32_t delay);
// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	int aux;
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R |= GPIO_PORTAS;
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTAS) ) != (GPIO_PORTAS) ){};
	
	// portas utilizadas
		// porta A: DISPLAY 7SEG A4-7
		// porta B: ENABLE DISPLAYS 7SEG
		// porta C: teclado C4-7
		// porta H: motor de passo unipolar ph0-3
		// porta J: 
		// porta K: LCD 0-7
		// porta L: teclado L0-3
		// porta M: LCD 0-2
		// porta N: 
		// porta Q: DISPLAY 7SEG Q0-3
		
	// 2. Limpar o AMSEL para desabilitar a analógica
	aux = GPIO_PORTC_AHB_AMSEL_R;
	aux = aux & 0xF0;
	aux = aux | 0x00;
	GPIO_PORTC_AHB_AMSEL_R = aux;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTB_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	aux = GPIO_PORTC_AHB_PCTL_R;
	aux = aux & 0x00001111;//////////// (AVISAR TERABE)
	GPIO_PORTC_AHB_PCTL_R = aux;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x00;
	GPIO_PORTB_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
		
	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTC_AHB_DIR_R = 0xF0;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTA_AHB_DIR_R = 0xF0; 			//2_11110000
	GPIO_PORTB_AHB_DIR_R = 0x30; 			//2_00110000
	GPIO_PORTH_AHB_DIR_R = 0x0F; 			//2_00001111
	GPIO_PORTK_DIR_R = 0xFF; 			//2_11111111
	GPIO_PORTL_DIR_R = 0x00; 			//2_00001111
	GPIO_PORTM_DIR_R = 0x47; 			//2_01000111
	GPIO_PORTN_DIR_R = 0x03; 			//2_00000011
	GPIO_PORTQ_DIR_R = 0x0F; 			//2_00001111
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	aux = GPIO_PORTC_AHB_AFSEL_R;
	aux = aux & 0x0F;
	aux = aux | 0x00;
	GPIO_PORTC_AHB_AFSEL_R = aux;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTA_AHB_AFSEL_R = 0x00;
	GPIO_PORTB_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTK_AFSEL_R = 0x00; 
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTC_AHB_DEN_R = 0xF0;   	//2_11110000
	GPIO_PORTJ_AHB_DEN_R = 0x03;   	//2_00000011
	GPIO_PORTA_AHB_DEN_R = 0xF0; 		   	//2_11110000
	GPIO_PORTB_AHB_DEN_R = 0x30; 		   	//2_00110000
	GPIO_PORTH_AHB_DEN_R = 0x0F; 		   	//2_00001111
	GPIO_PORTK_DEN_R = 0xFF; 		   	//2_11111111
	GPIO_PORTL_DEN_R = 0x0F; 		   	//2_00001111
	GPIO_PORTM_DEN_R = 0x47; 		   	//2_01000111
	GPIO_PORTN_DEN_R = 0x03; 		   	//2_00000011
	GPIO_PORTQ_DEN_R = 0x0F; 		   	//2_00001111
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTL_PUR_R = 0x0F;		//2_00001111
	GPIO_PORTJ_AHB_PUR_R = 0x03;   	//Bit0 e bit1	

}	

// -------------------------------------------------------------------------------
// Função PortL_Input
// Lê os valores de entrada do port L
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortL_Input(void)
{
	return GPIO_PORTL_DATA_R;
}
// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem



void PortA_Output(uint32_t valor)
{
    uint32_t aux;
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTA_AHB_DATA_R & 0x0F;	//2_00001111
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | valor;
    GPIO_PORTA_AHB_DATA_R = aux; 
}

void PortB_Output(uint32_t valor)
{
    uint32_t temp;
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTB_AHB_DATA_R & 0x0F;	//2_00001111
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTB_AHB_DATA_R = valor; 
}

void PortC_Output(uint32_t valor)
{
    GPIO_PORTC_AHB_DATA_R = valor; 
}

void PortH_Output(uint32_t valor)
{
		uint32_t aux;
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTH_AHB_DATA_R & 0x00;	//2_00001111
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | valor;
    GPIO_PORTH_AHB_DATA_R = aux;
}
void PortK_Output(uint32_t valor)
{
		uint32_t aux;
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTK_DATA_R & 0x00;	//2_00001111
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | valor;
    GPIO_PORTK_DATA_R = aux; 
}

void PortN_Output(uint32_t valor)
{
    uint32_t aux;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | valor;
    GPIO_PORTN_DATA_R = aux; 
}

void PortQ_Output(uint32_t valor)
{
    uint32_t aux;
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTQ_DATA_R & 0xF0;	//2_11110000
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | valor;
    GPIO_PORTQ_DATA_R = aux; 
}

void Pulse_Enable()
{
		uint32_t aux;
    aux = GPIO_PORTM_DATA_R & 0x07; 	//2_0000 0111
    aux = aux | 0x04;	//2_0000 0100
    SysTick_Wait1ms(5);
	  GPIO_PORTM_DATA_R = aux; 
		SysTick_Wait1ms(5);
		aux = aux & 0x03;	//2_0000 0011
		GPIO_PORTM_DATA_R = aux; 
}

void Ativa_Rs()
{
		uint32_t aux;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTM_DATA_R & 0x07; 	//2_0000 0111
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux | 0x01;	//2_0000 0001;
	  GPIO_PORTM_DATA_R = aux; ;
}

void Desativa_Rs()
{
		uint32_t aux;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    aux = GPIO_PORTM_DATA_R & 0x07; 	//2_0000 0111
    //agora vamos fazer o OR com o valor recebido na função
    aux = aux & 0x06;	//2_0000 0110
	  GPIO_PORTM_DATA_R = aux; 
}

void Config_Interrupt_J()
{
		int aux = 0x0;
	
		GPIO_PORTJ_AHB_IM_R = 0x0;
		
		aux = aux | GPIO_PORTJ_AHB_IS_R;
		GPIO_PORTJ_AHB_IS_R = aux;
		
		aux = 0x0;
		aux = aux | GPIO_PORTJ_AHB_IBE_R;
		GPIO_PORTJ_AHB_IBE_R = aux;
		
		aux = 0x00;
		aux = aux | GPIO_PORTJ_AHB_IEV_R;
		GPIO_PORTJ_AHB_IEV_R = aux;
	
		GPIO_PORTJ_AHB_RIS_R = 0x1;
	
		GPIO_PORTJ_AHB_IM_R = 0x0;
	
		aux = 0x1;
		aux = aux | GPIO_PORTJ_AHB_IM_R;
		GPIO_PORTJ_AHB_IM_R = aux;
		
		
		aux = 0x20000000;
		aux = aux | NVIC_PRI12_R;
		NVIC_PRI12_R = aux;
		
		aux = 0x00080000;
		aux = aux | NVIC_EN1_R;
		NVIC_EN1_R = aux;
		
		aux = 0x1;
		aux = aux | GPIO_PORTJ_AHB_ICR_R;
		GPIO_PORTJ_AHB_ICR_R = aux;
		
}

