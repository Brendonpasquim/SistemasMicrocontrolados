// main.c
// Desenvolvido para a placa EK-TM4C1294XL

// Prof. Guilherme Peron
#include <stdint.h>
#include <string.h>
#include "tm4c1294ncpdt.h"
//#define GPIO_PORTJ_AHB_ICR_R        (*((volatile uint32_t *)0x4006041C))

typedef enum estCofre
{
	CofreAbrindo,
	CofreAberto,
	CofreFechando,
	CofreFechado,
	CofreTravado,
}estadosCofre;

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);

uint32_t PortJ_Input(void);
uint32_t PortL_Input(void);
void PortA_Output(uint32_t valor);
void PortB_Output(uint32_t valor);
void PortQ_Output(uint32_t valor);
void PortC_Output(uint32_t valor);
void PortH_Output(uint32_t valor);
void PortK_Output(uint32_t valor);
void PortN_Output(uint32_t leds);
void Pulse_Enable(void);
void Ativa_Rs(void);
void Desativa_Rs(void);
void Config_Interrupt_J(void);
void GPIOPortJ_Handler(void);
void loop(void);
void DisplayLCD_Init(void);
void Limpa_LCD(void);
void LCD_Write(uint32_t valor);
void Escreve_2Linhas(char *frase1, char *frase2);
void Escreve_Linha2(void);
uint32_t Verifica_Teclado(void);
void Identifica_Tecla(uint32_t tecla);
void AbreCofre(uint32_t *password);
int FechaCofre(uint32_t *password, int tries);
void TravaCofre(void);
void Motor_Sentido_Anti_Horario(int passos);
void Motor_Sentido_Horario(int passos);
void Mestre_Correta(void);

int parou=0;

int main(void)
{
	//inicializacao
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	SysTick_Wait1ms(50);
	
	//config
	Config_Interrupt_J();
	DisplayLCD_Init();
	loop();
}

void loop(void)
{	
	uint32_t password[4];
	int countErro = 0; // variavel de contagem de erros (senha digitada errada)
	
	estadosCofre estados = CofreAberto;
	
	while(1) {
		// para cada estado
		switch (estados) {
			case CofreAberto:						// quando o cofre é aberto
				AbreCofre(password);				// função para abrir cofre (enviando senha)
				estados = CofreFechando;		// passa para o estado de cofre fechando
				break;
		
			case CofreFechado:						// estado do cofre quando é fechado
				if(FechaCofre(password,countErro)) {		// função para fechar o cofre
					countErro++;	
					estados = CofreFechado;						// passa para o estado de cofre fechado
				}
				else {
					countErro = 0; 									// reseta quantidade de erros
					estados = CofreAbrindo;				// passa a abrir o cofre
				}
				if(countErro > 2) {
					estados = CofreTravado;				// no terceiro erro, trava o cofre
				}
				break;
			
			case CofreAbrindo:								// estado de transição, abrindo o cofre
				SysTick_Wait1ms(1000);					// espera 1s
				Escreve_2Linhas("Abrindo Cofre","");	
				Motor_Sentido_Anti_Horario(1024);			// anti horário - passo completo
				estados = CofreAberto;					// ao finalizar, passa para o estado de cofre aberto
			break;
				
			case CofreFechando:
				Escreve_2Linhas("Fechando Cofre","");
				Motor_Sentido_Horario(2048);			// meio passo - dobro
				estados = CofreFechado;					// passa para o estado de cofre fechado
				break;
			
			case CofreTravado:
				TravaCofre();								// função que trava o cofre 
				break;
			
			default: break;
		}
	}
}

// Inicializa LCD
void DisplayLCD_Init(void)
{
	Desativa_Rs();
	PortK_Output(0x38);
	Pulse_Enable();
	PortK_Output(0x0C);
	Pulse_Enable();
	PortK_Output(0x06);
	Pulse_Enable();
	Limpa_LCD();
}

// Limpa campos do LCD
void Limpa_LCD(void)
{
	Desativa_Rs();
	PortK_Output(0x01);
	Pulse_Enable();
	Ativa_Rs();
}

//Escreve no LCD (manda o comando e pulsa o enable)
void LCD_Write(uint32_t valor)
{
	PortK_Output(valor);
	Pulse_Enable();
}

// Escreve uma frase (linha 1 + linha 2)
void Escreve_2Linhas(char *frase1, char*frase2)
{
	int i;
	Limpa_LCD();
	for(i=0;i<strlen(frase1);i++)
	{
		LCD_Write(frase1[i]);
	}
	Escreve_Linha2();
	for(i=0;i<strlen(frase2);i++)
	{
		LCD_Write(frase2[i]);
	}
}

// Poe o cursor na linha 2
void Escreve_Linha2(void)
{
	Desativa_Rs();
	PortK_Output(0xC0);
	Pulse_Enable();
	Ativa_Rs();
}

// verifica entrada do teclado
uint32_t Verifica_Teclado(void)
{
		int count;
		uint32_t aux;
		uint32_t aux2;
		uint32_t aux3;
		// maximo de quatro digitos podem ser digitados
		for(count = 0; count <=4; count++) {
			aux2 = 0x0F;
			if(count == 0) {			// primeiro
				aux = 0xE0; 
				PortC_Output(aux);
			}
			if(count==1) {				// segundo
				aux = 0xD0; 
				PortC_Output(aux);
			}
			if(count == 2) {			// terceiro
				aux = 0xB0; 
				PortC_Output(aux);
			}
			if(count == 3) {			// quarto
				aux = 0x70; 
				PortC_Output(aux);
			}
			aux3 = PortL_Input();
			aux2 = aux2 & aux3;
			if(aux2 != 0x0F) {
				aux = aux | aux2;
				return aux;
			}
		}
		return 0xFF;
}

/*
TECLAS
0x77 = 1
0x7B = 2
0x7D = 3
0x7E = A
0xB7 = 4
0xBB = 5
0xBD = 6
0xBE = B
0xD7 = 7
0xDB = 8
0xDD = 9
0xDE = C
0xE7 = *
0xEB = 0
0xED = #
0xEE = D
*/



void Identifica_Tecla(uint32_t tecla)
{
		switch(tecla)
		{
			case 0x77:
				LCD_Write('1');
				break;
			case 0x7B:
				LCD_Write('2');
				break;
			case 0x7D:
				LCD_Write('3');
				break;
			case 0x7E:
				LCD_Write('A');
				break;
			case 0xB7:
				LCD_Write('4');
				break;
			case 0xBB:
				LCD_Write('5');
				break;
			case 0xBD:
				LCD_Write('6');
				break;
			case 0xBE:
				LCD_Write('B');
				break;
			case 0xD7:
				LCD_Write('7');
			  break;
			case 0xDB:
				LCD_Write('8');
				break;
			case 0xDD:
				LCD_Write('9');
				break;
			case 0xDE:
				LCD_Write('C');
			  break;
			case 0xEB:
				LCD_Write('0');
				break;
			case 0xEE:
				LCD_Write('D');
				break;
		}
}

void AbreCofre(uint32_t *password)
{
		uint32_t aux;
		int  i=0;
		Escreve_2Linhas("Cofre Aberto","Nova senha:");
		while(1)
		{
			// recebe entrada do teclado
			aux = Verifica_Teclado();
			if(aux != 0xFF && i!=4 && aux != 0xE7 && aux != 0xED)
			{
				password[i] = aux;
				Identifica_Tecla(aux);
				SysTick_Wait1ms(250);
				i++;
			}
			if(aux==0xED)
			{
				break;
			}
		}
}

int FechaCofre(uint32_t *password, int erros)
{		
		uint32_t aux;
		int cont = 0;
		int i = 0;
		Escreve_2Linhas("Cofre Fechado","Senha:");
		while(1) {
			aux = Verifica_Teclado();				// verifica valor digitado no teclado
			if(aux != 0xFF && cont!=4 && aux != 0xE7 && aux != 0xED) {		// elimina os valores inválidos
				if(password[cont] != aux){i=1;}
				cont++;
				LCD_Write('*');					// escreve no LCD como *, para não poder ser visualizada a senha
				SysTick_Wait1ms(250);
			}
			if(aux == 0xED && cont ==4 && i==0){return 0;}
			if(aux == 0xED && cont ==4 && i==1) {	
				Escreve_2Linhas("Senha Incorreta","Chances: ");
					if(erros==0)
					{
						LCD_Write('2');
					}
					if(erros==1)
					{
						LCD_Write('1');
					}
					if(erros==2)
					{
						LCD_Write('0');
					}
					SysTick_Wait1ms(250);
					return 1;
			}
		}
}


void TravaCofre(void)
{
	int i = 0;
	int aux = 0;
	int count = 0;
	uint32_t mestre_password [4] = {0x77, 0x7B, 0x7D, 0xB7};
	
	parou=0;
	Escreve_2Linhas("Cofre Travado","");
	while(parou==0)
	{
	}
	Escreve_2Linhas("Digite Mestre","");
	while(1) {
			aux = Verifica_Teclado();
			if(aux != 0xFF && count!=4 && aux != 0xE7 && aux != 0xED)
			{
				if(mestre_password[count] != aux)
				{
					i=1;
				}
				count++;
				LCD_Write('*');
				SysTick_Wait1ms(250);
			}
			if(aux == 0xED && count ==4 && i==0)
			{
				i=0;
				break;
			}
			if(aux == 0xED && count ==4 && i==1)
			{	
				Escreve_2Linhas("Mestre Incorreta","");
				SysTick_Wait1ms(250);
				i=1;
				break;
			}
		}
	if(!i) {
		Mestre_Correta();
	}
	else {
		TravaCofre();
	}
}

void Mestre_Correta()
{
	//mostra 5
	PortB_Output(0x00);
	PortA_Output(0x60);
	PortQ_Output(0x0D);
	PortB_Output(0x10);
	SysTick_Wait1ms(1000);
	//mostra 4
	PortB_Output(0x00);
	PortA_Output(0x60);
	PortQ_Output(0x06);
	PortB_Output(0x10);
	SysTick_Wait1ms(1000);
	//mostra 3
	PortB_Output(0x00);
	PortA_Output(0x40);
	PortQ_Output(0x0F);
	PortB_Output(0x10);
	SysTick_Wait1ms(1000);
	//mostra 2
	PortB_Output(0x00);
	PortA_Output(0x50);
	PortQ_Output(0x0B);
	PortB_Output(0x10);
	SysTick_Wait1ms(1000);
	//mostra 1
	PortB_Output(0x00);
	PortA_Output(0x00);
	PortQ_Output(0x06);
	PortB_Output(0x10);
	SysTick_Wait1ms(1000);
	PortB_Output(0x00);
	Escreve_2Linhas("Cofre Abrindo","");
	Motor_Sentido_Anti_Horario(1024);
	loop();
}

//meio passo
void Motor_Sentido_Horario (int passos)
{
	uint32_t sinal=0;
	int i=0;
	for (i=0; i<passos; i=i+8)
	{
		sinal=0x6;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0x7;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0x3;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xB;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0x9;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xD;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xC;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xE;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
	}
}


//passo completo
void Motor_Sentido_Anti_Horario (int passos)
{
	uint32_t sinal=0;
	int i=0;
	for (i=0; i<passos; i=i+4)
	{
		sinal=0xE;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xD;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0xB;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
		sinal=0x7;
		PortH_Output(sinal);
		SysTick_Wait1ms(2);
	}
}

// interrupção
void GPIOPortJ_Handler(void)
{
		GPIO_PORTJ_AHB_ICR_R = 0x01;
		parou=1;
}

