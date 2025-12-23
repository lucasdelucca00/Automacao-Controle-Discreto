/*
 * Arquivo:C:/Users/Lukinhas/Documents/Automa��o-Controle-Discreto/Jogo_memoria.c
 * C�digo Gerado para a familia MSP430Gxx com sa�da Moore
 * Foi utilizado listas encadeadas como jogador de aut�mato
 * Desenvolvido por C�sar Rafael Claure Torrico.
 */
#include "msp430.h"
// Dados do aut�mato (N�o pode ser declarado dentro da fun��o main por ser const)
#define NTRANS 157  // N�mero de Transi��es
#define NESTADOS 52 // N�mero de Estados
#define BUFFER 10   // M�ximo N�mero de Eventos no Buffer

const unsigned int event[NTRANS]={4,7,6,5,6,4,2,5,0,1,3,2,5,0,1,3,2,5,0,1,3,2,5,0,1,3,2,0,1,3,4,0,1,2,3,4,6,5,0,5,1,2,3,0,5,1,2,3,0,5,1,
        2,3,6,5,3,5,0,1,2,3,5,0,1,2,6,5,1,5,0,2,3,6,5,4,7,6,4,6,5,6,5,6,5,6,5,7,4,6,4,6,5,6,5,6,5,7,4,6,4,6,5,
        6,5,7,4,6,6,5,7,4,5,6,4,5,4,5,6,4,4,5,0,1,2,3,4,3,0,1,2,4,3,0,1,2,4,1,0,2,3,4,1,0,2,3,4,2,0,1,3,4,6,4,
        4,5,4,5};
const unsigned int in_state[NTRANS]={42,49,51,42,50,42,9,42,40,40,40,10,42,40,40,40,11,42,40,40,40,12,42,40,40,40,43,39,39,39,42,44,39,39,39,42,22,42,13,42,40,40,40,14,42,40,40,40,15,42,40,
        40,40,23,42,16,42,40,40,40,17,42,40,40,40,24,42,18,42,40,40,40,1,42,42,25,26,42,3,42,27,42,28,42,29,42,30,42,31,42,4,42,32,42,33,42,34,42,35,42,5,42,
        36,42,7,42,8,6,42,19,37,38,21,42,42,42,42,20,42,37,38,45,39,39,39,42,46,39,39,39,42,47,39,39,39,42,48,39,39,39,42,0,39,39,39,42,2,39,39,39,42,41,42,
        42,42,42,42};
const unsigned int rfirst[NESTADOS] = {2,4,6,11,16,21,26,31,36,38,43,48,53,55,60,65,67,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,107,109,111,113,115,117,119,121,126,131,136,141,146,151,153,155,
        157};
const unsigned int rnext[NTRANS] = {0,1,0,3,0,5,0,7,8,9,10,0,12,13,14,15,0,17,18,19,20,0,22,23,24,25,0,27,28,29,30,0,32,33,34,35,0,37,0,39,40,41,42,0,44,45,46,47,0,49,50,
        51,52,0,54,0,56,57,58,59,0,61,62,63,64,0,66,0,68,69,70,71,0,73,0,75,0,77,0,79,0,81,0,83,0,85,0,87,0,89,0,91,0,93,0,95,0,97,0,99,0,101,
        0,103,0,105,0,0,108,0,110,0,112,0,114,0,116,0,118,0,120,0,122,123,124,125,0,127,128,129,130,0,132,133,134,135,0,137,138,139,140,0,142,143,144,145,0,147,148,149,150,0,152,
        0,154,0,156};

// Mapeamento de eventos n�o control�veis como entradas
#define B1 0    // Entrada 0
#define B2 1    // Entrada 1
#define B3 2    // Entrada 2
#define B4 3    // Entrada 3
#define M1 4    // Entrada 4
#define M2 5    // Entrada 5

// Mapeamento de eventos control�veis
#define Temp300 6
#define Temp500 7

// Defini��o dos eventos de sa�da
#define L1_ON  P2OUT |= BIT0    // Saida 0 ON
#define L1_OFF P2OUT &=~BIT0    // Saida 0 OFF
#define L2_ON  P2OUT |= BIT1    // Saida 1 ON
#define L2_OFF P2OUT &=~BIT1    // Saida 1 OFF
#define L3_ON  P2OUT |= BIT2    // Saida 2 ON
#define L3_OFF P2OUT &=~BIT2    // Saida 2 OFF
#define L4_ON  P2OUT |= BIT3    // Saida 3 ON
#define L4_OFF P2OUT &=~BIT3    // Saida 3 OFF
#define LP_ON  P2OUT |= BIT4    // Saida 4 ON  LED Perdeu
#define LP_OFF P2OUT &=~BIT4    // Saida 4 OFF
#define LV_ON  P2OUT |= BIT5    // Saida 5 ON  LED Vit�ria
#define LV_OFF P2OUT &=~BIT5    // Saida 5 OFF

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
    unsigned int current_state = 42;    //Estado atual inicializado com estado inicial
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
                    case(0):
                        occur_event=Temp300;
                        g++;
                        break;
                    case(1):
                        occur_event=Temp500;
                        g=0;
                        break;
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
                case(0):    //Adicionar A��o para o Estado Confere1_m1;

                    break;
                case(1):    //Adicionar A��o para o Estado Confere1_m2;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(2):    //Adicionar A��o para o Estado Confere2_m1;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(3):    //Adicionar A��o para o Estado DesligaLed1_m2;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(4):    //Adicionar A��o para o Estado DesligaLed2_m2;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(5):    //Adicionar A��o para o Estado DesligaLed3_m2;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(6):    //Adicionar A��o para o Estado DesligaLed4_m2;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(7):    //Adicionar A��o para o Estado DesligaLed_m1;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(8):    //Adicionar A��o para o Estado DesligaLed_m11;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(9):    //Adicionar A��o para o Estado Dig1;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(10):   //Adicionar A��o para o Estado Dig11;

                    break;
                case(11):   //Adicionar A��o para o Estado Dig111;

                    break;
                case(12):   //Adicionar A��o para o Estado Dig1111;

                    break;
                case(13):   //Adicionar A��o para o Estado Dig2;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(14):   //Adicionar A��o para o Estado Dig22;

                    break;
                case(15):   //Adicionar A��o para o Estado Dig222;

                    break;
                case(16):   //Adicionar A��o para o Estado Dig3;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(17):   //Adicionar A��o para o Estado Dig33;

                    break;
                case(18):   //Adicionar A��o para o Estado Dig4;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(19):   //Adicionar A��o para o Estado Led1_m1;
                    L3_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(20):   //Adicionar A��o para o Estado Led1_m11;
                    L1_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(21):   //Adicionar A��o para o Estado Led1_m2;
                    L3_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(22):   //Adicionar A��o para o Estado Led1_m21;
                    L3_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(23):   //Adicionar A��o para o Estado Led1_m211;
                    L3_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(24):   //Adicionar A��o para o Estado Led1_m2111;
                    L3_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(25):   //Adicionar A��o para o Estado Led2_m1;
                    L3_OFF;
                    L1_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(26):   //Adicionar A��o para o Estado Led2_m11;
                    L1_OFF;
                    L4_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(27):   //Adicionar A��o para o Estado Led2_m2;
                    L3_OFF;
                    L1_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(28):   //Adicionar A��o para o Estado Led2_m22;
                    L3_OFF;
                    L1_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(29):   //Adicionar A��o para o Estado Led2_m222;
                    L3_OFF;
                    L1_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(30):   //Adicionar A��o para o Estado Led3_m1;
                    L1_OFF;
                    L4_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(31):   //Adicionar A��o para o Estado Led3_m11;
                    L4_OFF;
                    L2_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(32):   //Adicionar A��o para o Estado Led3_m2;
                    L1_OFF;
                    L4_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(33):   //Adicionar A��o para o Estado Led3_m23;
                    L1_OFF;
                    L4_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(34):   //Adicionar A��o para o Estado Led4_m1;
                    L4_OFF;
                    L2_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(35):   //Adicionar A��o para o Estado Led4_m11;
                    L2_OFF;
                    L3_ON;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(36):   //Adicionar A��o para o Estado Led4_m2;
                    L4_OFF;
                    L2_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(37):   //Adicionar A��o para o Estado Modo1;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(38):   //Adicionar A��o para o Estado Modo2;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(39):   //Adicionar A��o para o Estado Perdeu1;
                    LP_ON;
                    break;
                case(40):   //Adicionar A��o para o Estado Perdeu2;
                    LP_ON;
                    break;
                case(41):   //Adicionar A��o para o Estado ProximaRodada;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    LP_OFF;
                    LV_OFF;
                    delay_ms(300);
                    gerar_evento=1;
                    break;
                case(42):   //Adicionar A��o para o Estado Repouso;
                    L1_OFF;
                    L2_OFF;
                    L3_OFF;
                    L4_OFF;
                    LP_OFF;
                    LV_OFF;
                    break;
                case(43):   //Adicionar A��o para o Estado Seq1;

                    break;
                case(44):   //Adicionar A��o para o Estado Seq11;

                    break;
                case(45):   //Adicionar A��o para o Estado Seq2;

                    break;
                case(46):   //Adicionar A��o para o Estado Seq21;

                    break;
                case(47):   //Adicionar A��o para o Estado Seq3;

                    break;
                case(48):   //Adicionar A��o para o Estado Seq31;

                    break;
                case(49):   //Adicionar A��o para o Estado Venceu1Rodada;
                    L1_ON;
                    L2_ON;
                    L3_ON;
                    L4_ON;
                    delay_ms(500);
                    gerar_evento=1;
                    break;
                case(50):   //Adicionar A��o para o Estado VenceuM1;
                    L1_ON;
                    L2_ON;
                    L3_ON;
                    L4_ON;
                    break;
                case(51):   //Adicionar A��o para o Estado VenceuM2;
                    L1_ON;
                    L2_ON;
                    L3_ON;
                    L4_ON;
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
        buffer[n_buffer]=B1;    //Atribuir evento a P1.0
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT1)  //verifica se P1.1 foi pressionado
    {
        P1IFG&=~BIT1;       //apaga o flag
        buffer[n_buffer]=B2;    //Atribuir evento a P1.1
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT2)  //verifica se P1.2 foi pressionado
    {
        P1IFG&=~BIT2;       //apaga o flag
        buffer[n_buffer]=B3;    //Atribuir evento a P1.2
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT3)  //verifica se P1.3 foi pressionado
    {
        P1IFG&=~BIT3;       //apaga o flag
        buffer[n_buffer]=B4;    //Atribuir evento a P1.3
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT4)  //verifica se P1.4 foi pressionado
    {
        P1IFG&=~BIT4;       //apaga o flag
        buffer[n_buffer]=M1;    //Atribuir evento a P1.4
        if(n_buffer<BUFFER-1) n_buffer++;
    }
    if(P1IFG&BIT5)  //verifica se P1.5 foi pressionado
    {
        P1IFG&=~BIT5;       //apaga o flag
        buffer[n_buffer]=M2;    //Atribuir evento a P1.5
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
