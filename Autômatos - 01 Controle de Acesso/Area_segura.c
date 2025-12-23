/*
 * Arquivo:C:/Users/Lukinhas/Documents/Automa��o-Controle-Discreto/Area_segura.c
 * C�digo Gerado para a familia MSP430Gxx com sa�da Moore
 * Foi utilizado listas encadeadas como jogador de aut�mato
 * Desenvolvido por C�sar Rafael Claure Torrico.
 */
#include "msp430.h"
// Dados do aut�mato (N�o pode ser declarado dentro da fun��o main por ser const)
#define NTRANS 95   // N�mero de Transi��es
#define NESTADOS 18 // N�mero de Estados
#define BUFFER 10   // M�ximo N�mero de Eventos no Buffer

const unsigned int event[NTRANS]={0,2,3,4,5,1,1,2,3,4,0,5,3,5,2,4,0,1,3,2,4,5,0,1,5,2,3,4,0,1,4,2,3,5,0,1,3,5,2,4,0,1,5,2,3,4,0,1,3,2,4,
        5,0,1,4,2,3,5,0,1,0,2,3,4,5,1,0,1,0,1,0,0,2,3,4,5,1,1,2,3,4,5,0,2,3,4,5,1,0,1,2,3,4,5,0};
const unsigned int in_state[NTRANS]={11,14,14,14,14,14,0,14,14,14,14,14,4,3,13,13,16,14,5,13,13,13,16,14,5,13,13,13,16,14,17,13,13,13,16,14,7,8,12,12,11,16,9,12,12,12,11,16,9,12,12,
        12,11,16,15,12,12,12,11,16,11,6,12,12,12,16,11,16,11,14,16,1,14,14,14,14,14,10,12,12,12,12,11,2,13,13,13,14,16,10,13,13,13,13,16};
const unsigned int rfirst[NESTADOS] = {6,12,18,24,30,36,42,48,54,60,61,67,69,71,77,83,89,95};
const unsigned int rnext[NTRANS] = {0,1,2,3,4,5,0,7,8,9,10,11,0,13,14,15,16,17,0,19,20,21,22,23,0,25,26,27,28,29,0,31,32,33,34,35,0,37,38,39,40,41,0,43,44,45,46,47,0,49,50,
        51,52,53,0,55,56,57,58,59,0,0,62,63,64,65,66,0,68,0,70,0,72,73,74,75,76,0,78,79,80,81,82,0,84,85,86,87,88,0,90,91,92,93,94};

// Mapeamento de eventos n�o control�veis como entradas
#define CL 0    // Entrada 0
#define OK 1    // Entrada 1
#define a 2 // Entrada 2
#define b 3 // Entrada 3
#define c 4 // Entrada 4
#define d 5 // Entrada 5

// Mapeamento de eventos control�veis

// Defini��o dos eventos de sa�da
// Defini��o dos eventos de sa�da
#define SPF_ON  P2OUT |= BIT0   // Saida 0 ON  SinalPortaFechada
#define SPF_OFF P2OUT &=~BIT0   // Saida 0 OFF
#define SPA_ON  P2OUT |= BIT1   // Saida 1 ON  SinalPortaAberta
#define SPA_OFF P2OUT &=~BIT1   // Saida 1 OFF
#define Sirene_ON  P2OUT |= BIT2    // Saida 2 ON
#define Sirene_OFF P2OUT &=~BIT2    // Saida 2 OFF
#define Tentativa_ON  P2OUT |= BIT3 // Saida 3 ON
#define Tentativa_OFF P2OUT &=~BIT3 // Saida 3 OFF

//Defini��o de fun��es de inicializa��o
void config_clk(void);      //Configura Clock
void config_timer(void);        //Configura Timer
void config_io(void);       //Configura entradas e sa�das
void tempo_ms(unsigned int);    // Fun��o de temporiza��o do timer A em milisegundos
void delay_ms(unsigned int);    // Fun��o delay em milisegundos para debounce

//Declara��o de vari�veis globais
unsigned int buffer[BUFFER];        //Buffer para armazenar a fila de eventos externos
unsigned int n_buffer=0;        //N�mero de eventos no Buffer

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;       //Desabilita o WDT
    config_clk();               //configura o clk
    config_timer();         //configura o timer
    config_io();                //configura entradas e sa�das
    unsigned int k;
    int occur_event;            //Evento ocorrido
    unsigned int current_state = 11;    //Estado atual inicializado com estado inicial
    char g=0;           //Flag para gerador aleat�rio de eventos
    char gerar_evento=1;            //Flag para habilitar a temporiza��o de eventos control�veis
    char moore_output = 0;      //Inicializa sa�da perif�rica
    __enable_interrupt();       //Habilita interrup��o geral

    while(1)
    {

        if(n_buffer == 0)//se n�o existir evento no buffer ent�o gerar um evento interno(evento control�vel)
        {
            if(TACTL&TAIFG) //Se o timer estourar, habilita a gera��o de eventos
            {
                gerar_evento=1;
            }
            if(gerar_evento==1)
            {
                switch(g)   //Aqui � implementado um gerador autom�tico de eventos control�veis
                {
                }
            }
        }

        else    //se existir evento n�o control�vel pegar do buffer
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


        //Jogador de aut�mato
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

        if(moore_output) //Se o evento ocorrido for v�lido, ent�o imprimir sa�da f�sica
        {
            gerar_evento=1;
            switch(current_state)
            {
                case(0):    //Adicionar A��o para o Estado S0;

                    break;
                case(1):    //Adicionar A��o para o Estado S1;

                    break;
                case(2):    //Adicionar A��o para o Estado digito2_1;

                    break;
                case(3):    //Adicionar A��o para o Estado digito2_2_c1;

                    break;
                case(4):    //Adicionar A��o para o Estado digito2_2_c2;

                    break;
                case(5):    //Adicionar A��o para o Estado digito2_3;

                    break;
                case(6):    //Adicionar A��o para o Estado digito_1;

                    break;
                case(7):    //Adicionar A��o para o Estado digito_2_c1;

                    break;
                case(8):    //Adicionar A��o para o Estado digito_2_c2;

                    break;
                case(9):    //Adicionar A��o para o Estado digito_3;

                    break;
                case(10):   //Adicionar A��o para o Estado porta_abre;
                    SPA_ON;
                    SPF_OFF;
                    Sirene_OFF;
                    Tentativa_OFF;
                    break;
                case(11):   //Adicionar A��o para o Estado repouso;
                    SPA_OFF;
                    SPF_ON;
                    Sirene_OFF;
                    Tentativa_OFF;
                    break;
                case(12):   //Adicionar A��o para o Estado senha1_errada;

                    break;
                case(13):   //Adicionar A��o para o Estado senha2_errada;

                    break;
                case(14):   //Adicionar A��o para o Estado sirene;
                    Sirene_ON;
                    SPA_OFF;
                    SPF_ON;
                    Tentativa_OFF;
                    break;
                case(15):   //Adicionar A��o para o Estado tentativa1_correta;

                    break;
                case(16):   //Adicionar A��o para o Estado tentativa2;
                    Sirene_OFF;
                    Tentativa_ON;
                    SPF_ON;
                    SPA_OFF;
                    break;
                case(17):   //Adicionar A��o para o Estado tentativa2_correta;

                    break;
            }//fim switch
            moore_output = 0;
            occur_event = -1;
        }//fim if(moore_output)
    }//fim while(1)
}//fim main


//Tratamento da Interrup��o da porta P1
//--------------------------------------------------------------
//--------------------------------------------------------------
#pragma vector=PORT1_VECTOR
__interrupt void RTI_PORT1(void)
{
    //delay_ms(10);  // Debounce: ajustar conforme necessidade. Se necess�rio descomentar
    if(P1IFG&BIT0)  //verifica se P1.0 foi pressionado
    {
        P1IFG&=~BIT0;       //apaga o flag
        buffer[n_buffer]=CL;    //Atribuir evento a P1.0
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT1)  //verifica se P1.1 foi pressionado
    {
        P1IFG&=~BIT1;       //apaga o flag
        buffer[n_buffer]=OK;    //Atribuir evento a P1.1
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT2)  //verifica se P1.2 foi pressionado
    {
        P1IFG&=~BIT2;       //apaga o flag
        buffer[n_buffer]=a; //Atribuir evento a P1.2
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT3)  //verifica se P1.3 foi pressionado
    {
        P1IFG&=~BIT3;       //apaga o flag
        buffer[n_buffer]=b; //Atribuir evento a P1.3
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT4)  //verifica se P1.4 foi pressionado
    {
        P1IFG&=~BIT4;       //apaga o flag
        buffer[n_buffer]=c; //Atribuir evento a P1.4
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT5)  //verifica se P1.5 foi pressionado
    {
        P1IFG&=~BIT5;       //apaga o flag
        buffer[n_buffer]=d; //Atribuir evento a P1.5
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    P1IFG=0;
}


//Configura��o de perif�ricos
//--------------------------------------------------------------
//--------------------------------------------------------------
void config_clk(void)   //configura o clock
{
    //O clock foi configurado a partir do DCO para SMCLK=16Mhz
    DCOCTL = 0x00;
    __delay_cycles(100000);
    BCSCTL1 = CALBC1_16MHZ;  // Carrega a calibra��o do DCO para 16 MHz
    DCOCTL = CALDCO_16MHZ;   // Define o DCO para 16 MHz
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void config_timer(void) //configura o timer
{
    //Configura o timer se necess�rio
    //para este caso ser� utilizado o Timer A com temporiza��o de eventos
    //f_ref aprox = (VLOCLK)12e3/4 = 3kHz
    //TASSEL_1 -- ACLK
    //ID_2 -- Divide - /4
    //MC_1 -- contagem at� TACCR0
    TACTL = TASSEL_1 + ID_2 + MC_1;
    BCSCTL3 |= LFXT1S1; //coloca como fonte de clk o VLOCLK ao inv�s do cristal oscilador externo
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void config_io(void)    //configura entradas e sa�das
{
    //Por default a porta 1 ser� considerada entrada e a porta 2 sa�da
    P1DIR = 0;      //toda a porta P1 como entrada
    P1REN |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;   //habilita resistores de pull(up/down)
    P1OUT |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;   //pull UP
    P1IE  |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;   //habilita interrup��o das entradas P1
    P1IES |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;   //a borda de descida que provoca interrup��o (descida = 1 - subida = 0)
    //P1IES &= ~(BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7);  //a borda de subida que provoca interrup��o (descida = 1 - subida = 0)
    P1IFG = 0;          //Apaga o Flag da interrup��o de todas as entradas
    P2SEL &= ~(BIT6 | BIT7);                     //Bit 6 e 7 do P2 como IO
    P2DIR |= BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;   //toda a porta P2 como sa�da
    P2OUT = 0;          //todas as sa�das em n�vel l�gico 0
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void tempo_ms(unsigned int tempo)   //Fun��o de temporiza��o do timer A
{
    TACTL|= TACLR;      //zera o timer A
    TACCR0 =3*tempo;    //especifica tempo de estouro
    TACTL &= ~TAIFG;    //zera o Flag do estouro do Timer
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void delay_ms(unsigned int ms)  //Fun��o delay em milisegundos
{
    while(ms--)
    {
        __delay_cycles(16000);  // 16.000 ciclos para 1ms com DCO a 16 MHz
    }
}
