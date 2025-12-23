/*
 * Arquivo:C:/Users/cinar/SUPREMICA_SUMO/pratica 3/pratica_3_sumo.c
 * Código Gerado para a familia MSP430FR com saída Moore
 * Foi utilizado listas encadeadas como jogador de autômato
 * Desenvolvido por César Rafael Claure Torrico.
 */
#include "msp430.h"
// Dados do autômato (Não pode ser declarado dentro da função main por ser const)
#define NTRANS 38	// Número de Transições
#define NESTADOS 12	// Número de Estados
#define BUFFER 10	// Máximo Número de Eventos no Buffer

const unsigned int event[NTRANS]={5,3,9,9,2,5,4,3,6,7,9,2,4,4,9,8,2,9,8,2,4,7,6,8,1,0,9,6,2,8,4,7,9,6,2,8,4,7};
const unsigned int in_state[NTRANS]={4,3,10,11,0,8,0,8,7,6,8,4,3,3,8,5,4,8,5,4,3,6,7,5,2,1,8,7,4,5,3,6,8,7,4,5,3,6};
const unsigned int rfirst[NESTADOS] = {2,3,4,6,8,13,16,19,24,26,32,38};
const unsigned int rnext[NTRANS] = {0,1,0,0,0,5,0,7,0,9,10,11,12,0,14,15,0,17,18,0,20,21,22,23,0,25,0,27,28,29,30,31,0,33,34,35,36,37};

// Mapeamento de eventos não controláveis como entradas
#define bd 0	// Entrada 0
#define be 1	// Entrada 1
#define p1_achou 2	// Entrada 2
#define p1_perdeu 3	// Entrada 3
#define p2_achou 4	// Entrada 4
#define p2_perdeu 5	// Entrada 5
#define p3_achou 6	// Entrada 6
#define p4_achou 7	// Entrada 7
#define p5_achou 8	// Entrada 8

// Mapeamento de eventos controláveis
#define temp 9

// Definição dos eventos de saída
#define be_ON  P2OUT |= BIT0	// Saida 0 ON
#define be_OFF P2OUT &=~BIT0	// Saida 0 OFF
#define bd_ON  P2OUT |= BIT1	// Saida 1 ON
#define bd_OFF P2OUT &=~BIT1	// Saida 1 OFF
#define procurar_ON  P2OUT |= BIT2	// Saida 2 ON
#define procurar_OFF P2OUT &=~BIT2	// Saida 2 OFF
#define gira_ON  P2OUT |= BIT4	// Saida 4 ON
#define gira_OFF P2OUT &=~BIT4	// Saida 4 OFF
#define gira_eixo_ON  P2OUT |= BIT5	// Saida 5 ON
#define gira_eixo_OFF P2OUT &=~BIT5	// Saida 5 OFF
//#define atacar_ON  P2OUT |= BIT6	// Saida 6 ON
//#define atacar_OFF P2OUT &=~BIT6	// Saida 6 OFF
#define atacar_ON  P2OUT |= BIT7	// Saida 7 ON
#define atacar_OFF P2OUT &=~BIT7	// Saida 7 OFF
#define gira_espera_ON  P3OUT |= BIT1	// Saida 3.1 ON
#define gira_espera_OFF P3OUT &=~BIT1	// Saida 3.1 OFF


//Definição de funções de inicialização
void config_clk(void);		//Configura Clock
void config_timer(void);		//Configura Timer
void config_io(void);		//Configura entradas e saídas
void tempo_ms(unsigned int);	// Função de temporização do timer A em milisegundos
void delay_ms(unsigned int);	// Função delay em milisegundos para debounce

//Declaração de variáveis globais
unsigned int buffer[BUFFER];		//Buffer para armazenar a fila de eventos externos
unsigned int n_buffer=0;		//Número de eventos no Buffer

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		//Desabilita o WDT
	PM5CTL0 &= ~LOCKLPM5;		//Desabilita o GPIO - Alta impedância
	config_clk();				//configura o clk
	config_timer();			//configura o timer
	config_io();				//configura entradas e saídas

	unsigned int k;
	int occur_event;			//Evento ocorrido
	unsigned int current_state = 9;	//Estado atual inicializado com estado inicial
	char g=0; 			//Flag para gerador aleatório de eventos
	char gerar_evento=1;			//Flag para habilitar a temporização de eventos controláveis
	char moore_output = 0;		//Inicializa saída periférica
	__enable_interrupt();		//Habilita interrupção geral

	while(1)
	{

		if(n_buffer == 0)//se não existir evento no buffer então gerar um evento interno(evento controlável)
		{
			if(TA0CTL&TAIFG)	//Se o timer estourar, habilita a geração de eventos 
			{
				gerar_evento=1;
			}
			if(gerar_evento==1)
			{
				switch(g)	//Aqui é implementado um gerador automático de eventos controláveis
				{
					case(0):
						occur_event=temp;
						g=0;
						break;
				}
			}
		}

		else 	//se existir evento não controlável pegar do buffer
		{
			occur_event = buffer[0];
			n_buffer--;
			k = 0;
			while(k<n_buffer)
			{
				buffer[k] = buffer[k+1];
				k+=1;
			}
		}


		//Jogador de autômato
		k = rfirst[current_state];
		if(k==0)
		{
			return;     //Dead Lock!!!
		}
		else
		{
			while(k>0)
			{
				k-=1;
				if(event[k] == occur_event)
				{
					current_state = in_state[k];
					moore_output = 1;
					break;
				}
				k = rnext[k];
			}
		}

		if(moore_output) //Se o evento ocorrido for válido, então imprimir saída física
		{
			gerar_evento=1;
			switch(current_state)
			{
				case(0):	//Adicionar Ação para o Estado atacar;
                         atacar_ON;
                         gira_OFF;
                         gira_espera_OFF;

					break;
				case(1):	//Adicionar Ação para o Estado espera_d;
				        bd_ON;
				        tempo_ms(5000);
				        gerar_evento=0;
				        gira_espera_OFF;

					break;
				case(2):	//Adicionar Ação para o Estado espera_e;
				        be_ON;
				        tempo_ms(5000);
				        gerar_evento=0;
				        gira_espera_OFF;

					break;
				case(3):	//Adicionar Ação para o Estado gira_d;
				        gira_eixo_OFF;
				        gira_ON;
                        procurar_OFF;
                        atacar_OFF;
                        gira_espera_OFF;

					break;
				case(4):	//Adicionar Ação para o Estado gira_e;
				        gira_eixo_OFF;
				        gira_ON;
                        procurar_OFF;
                        atacar_OFF;
                        gira_espera_OFF;

					break;
				case(5):	//Adicionar Ação para o Estado gira_eixo_0;
				        procurar_OFF;
				        gira_eixo_ON;
				        tempo_ms(4000);
				        gerar_evento=0;
				        gira_espera_OFF;

					break;
				case(6):	//Adicionar Ação para o Estado gira_eixo_d;
				        procurar_OFF;
				        gira_eixo_ON;
				        tempo_ms(2000);
				        gerar_evento=0;
				        gira_espera_OFF;

					break;
				case(7):	//Adicionar Ação para o Estado gira_eixo_e;
				        procurar_OFF;
				        gira_eixo_ON;
				        tempo_ms(2000);
				        gerar_evento=0;
				        gira_espera_OFF;

					break;
				case(8):	//Adicionar Ação para o Estado procurar;
				        procurar_ON;
                        gira_eixo_OFF;
                        gira_OFF;
                        gira_espera_OFF;

					break;
				case(9):	//Adicionar Ação para o Estado repouso;
				        be_OFF;
				        bd_OFF;
				        procurar_OFF;
				        atacar_OFF;
				        gira_eixo_OFF;
				        gira_OFF;
				        gira_espera_OFF;

					break;
				case(10):	//Adicionar Ação para o Estado start_gira_d;
				        bd_OFF;
				        gira_espera_ON;
				        tempo_ms(2000);
				        gerar_evento=0;

					break;
				case(11):	//Adicionar Ação para o Estado start_gira_e;
				        be_OFF;
				        gira_espera_ON;
				        tempo_ms(2000);
				        gerar_evento=0;

					break;
			}//fim switch
			moore_output = 0;
			occur_event = -1;
		}//fim if(moore_output)
	}//fim while(1)
}//fim main


//Tratamento da Interrupção da porta P1
//--------------------------------------------------------------
//--------------------------------------------------------------
#pragma vector=PORT1_VECTOR
__interrupt void RTI_PORT1(void)
{
	//delay_ms(10);  // Debounce: ajustar conforme necessidade. Se necessário descomentar
	if(P1IFG&BIT0)	//verifica se P1.0 foi pressionado
	{
		P1IFG&=~BIT0;		//apaga o flag
		buffer[n_buffer]=be;	//Atribuir evento a P1.0
		if(n_buffer<BUFFER-1) n_buffer++;
	}

	if(P1IFG&BIT1)	//verifica se P1.1 foi pressionado
	{
		P1IFG&=~BIT1;		//apaga o flag
		buffer[n_buffer]=bd;	//Atribuir evento a P1.1
		if(n_buffer<BUFFER-1) n_buffer++;
	}

	if(P1IFG&BIT2)	//verifica se P1.2 foi pressionado
	{
		P1IFG&=~BIT2;		//apaga o flag
		buffer[n_buffer]=p5_achou;	//Atribuir evento a P1.2
		if(n_buffer<BUFFER-1) n_buffer++;
	}

	if(P1IFG&BIT3)	//verifica se P1.3 foi pressionado
	{
		P1IFG&=~BIT3;		//apaga o flag
		buffer[n_buffer]=p4_achou;	//Atribuir evento a P1.3
		if(n_buffer<BUFFER-1) n_buffer++;
	}

	if(P1IFG&BIT4)	//verifica se P1.4 foi pressionado
	{
		P1IFG&=~BIT4;		//apaga o flag
		buffer[n_buffer]=p3_achou;	//Atribuir evento a P1.4
		if(n_buffer<BUFFER-1) n_buffer++;
	}

	// MONITORAMENTO BORDA DE SUBIDA E DESCIDA DO BOTÃO
	if((P1IFG&BIT5)&&!(P1IN&BIT5))	          // Borda de subida
	{
		P1IFG&=~BIT5;		                  //apaga o flag
		buffer[n_buffer]=p2_achou;	          //Atribuir evento a P1.6
		if(n_buffer<BUFFER-1) n_buffer++;
		P1IES &= ~BIT5;                       //configura bit5 borda de subida
	}
    if((P1IFG&BIT5)&&(P1IN&BIT5))            // Borda de descida
    {
        P1IFG&=~BIT5;                         //apaga o flag
        buffer[n_buffer]=p2_perdeu;           //Atribuir evento a P1.6
        if(n_buffer<BUFFER-1) n_buffer++;
        P1IES |= (BIT5);                      //configura bit5 borda de subida
    }

    // MONITORAMENTO BORDA DE SUBIDA E DESCIDA DO BOTÃO
    if((P1IFG&BIT6)&&!(P1IN&BIT6))            // Borda de Subida
    {
        P1IFG&=~BIT6;                         //apaga o flag
        buffer[n_buffer]=p1_achou;            //Atribuir evento a P1.6
        if(n_buffer<BUFFER-1) n_buffer++;
        P1IES &= ~BIT6;                       //configura bit6 borda de subida
    }
    if((P1IFG&BIT6)&&(P1IN&BIT6))            // Borda de descida
    {
        P1IFG&=~BIT6;                         //apaga o flag
        buffer[n_buffer]=p1_perdeu;           //Atribuir evento a P1.6
        if(n_buffer<BUFFER-1) n_buffer++;
        P1IES |= (BIT6);                      //configura bit6 borda de subida
    }

	P1IFG=0;
}


//Configuração de periféricos
//--------------------------------------------------------------
//--------------------------------------------------------------
void config_clk(void)	//configura o clock
{
	// Desbloquear registradores de clock
	CSCTL0_H = 0xA5;

	// Configurar o DCO para 16 MHz
	// DCORSEL = 1 => seleciona a faixa 1MHz–16MHz
	// DCOFSEL = 100b => 16 MHz
	CSCTL1 = (1 << 7) | (0b100 << 1); // DCORSEL | DCOFSEL=100

	// Selecionar DCO como fonte de MCLK e SMCLK
	CSCTL2 = (3 << 0) | (3 << 4); // SELM = DCO (3), SELS = DCO (3)

	// Divisores de clock = 1 (sem divisão)
	CSCTL3 = 0; // DIVM=0 (÷1), DIVS=0 (÷1)

	// Travar os registradores
	CSCTL0_H = 0x00;

	FRCTL0 = FRCTLPW | NWAITS_1; // 1 waitstate para FRAM acesso seguro acima de 8MHz
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void config_timer(void)	//configura o timer
{
	// Desbloquear registradores de clock
	CSCTL0_H = 0xA5;

	// Configurar Timer_A0:
	// TASSEL_1 = ACLK -> O ACLK está conectado ao REFO CLK 32.768KHz
	// ID_3 = divisor /8
	// MC_1 = modo up até TACCR0
	TA0CTL = TASSEL_1 | ID_3 | MC_1;

	// Trava registradores de clock
	CSCTL0_H = 0xA0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void config_io(void)	//configura entradas e saídas
{
	//Por default a porta 1 será considerada entrada e a porta 2 saída
	P1DIR = 0;		//toda a porta P1 como entrada
	P1REN |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;	//habilita resistores de pull(up/down)
	P1OUT |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;	//pull UP
	P1IE  |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;	//habilita interrupção das entradas P1
	P1IES |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;	//a borda de descida que provoca interrupção (descida = 1 - subida = 0)
	//P1IES &= ~(BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7);	//a borda de subida que provoca interrupção (descida = 1 - subida = 0)
	P1IFG = 0;			//Apaga o Flag da interrupção de todas as entradas
	P2SEL0 &= ~(BIT6 | BIT7); 					 //Bit 6 e 7 do P2 como IO 
	P2SEL1 &= ~(BIT6 | BIT7); 					 //Bit 6 e 7 do P2 como IO 
	P2DIR |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;	//toda a porta P2 como saída
	P2OUT = 0;			//todas as saídas em nível lógico 0
	P3DIR |= BIT1;	//P3.1 como saída
	P3OUT &=~BIT1;	//P3.1 em nível lógico 0
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void tempo_ms(unsigned int tempo)	//Função de temporização do timer A
{
	TA0CTL|= TACLR;		//zera o timer A
	TA0CCR0 =4*tempo;	//especifica tempo de estouro (está pegando do oscilador interno de 32.768kHz REFOCLK/8)
	TA0CTL &= ~TAIFG;	//zera o Flag do estouro do Timer
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void delay_ms(unsigned int ms)	//Função delay em milisegundos
{
	while(ms--)
	{
		__delay_cycles(16000);  // 16.000 ciclos para 1ms com DCO a 16 MHz
	}
}
