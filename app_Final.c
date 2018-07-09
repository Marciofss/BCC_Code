
/*
*********************************************************************************************************
*
*                                          APPLICATION CODE - BCC722
*
*                                         TARGET = Microsoft Windows
*
* Arquivo			: app.c
* Versao			:
* Alunos			: Márcio Flávio Sousa Silva, Raphael Augusto dos Santos Ribeiro, Sávio Nazareno Júnior
* Data				: 09/07/2018
* Projeto			: Aplicação de Artigo - "RTOS Based Vehicle Tracking System" 
* Descricao			: Desenvolvimento de um Sistema de Rastreamento de Veículos Utilizando RTOS
*********************************************************************************************************
*/


#include  <app_cfg.h>
#include  <os.h>
#include  <os_cfg_app.h>

#define SIZE 2000 //tamanho da stack pré definida

//Declara os sensores e atuadores existentes no sistema
int input_sensor_sono; 
int input_sensor_embriaguez;
int input_sensor_temperatura;
int input_sensor_nivel;
int input_sensor_movimento;
int output_sirene;


#define  APP_TASK_STOP();                             { while (DEF_ON) { \
                                                            ;            \
                                                        }                \
                                                      }


#define  APP_TEST_FAULT(err_var, err_code)            { if ((err_var) != (err_code)) {   \
                                                            APP_TASK_STOP();             \
                                                        }                                \
                                                      }

static  OS_TCB   AppStartTaskTCB;
static  CPU_STK  AppStartTaskStk[SIZE];

static  OS_TCB   AppTeste_BafometroTCB;
static  CPU_STK  AppTeste_BafometroStk[SIZE];

static  OS_TCB   AppCheck_TemperaturaTCB;
static  CPU_STK  AppCheck_TemperaturaStk[SIZE];

static  OS_TCB   AppCheck_NivelTCB;
static  CPU_STK  AppCheck_NivelStk[SIZE];

static  OS_TCB   AppCaminhaoMovimentoTCB;
static  CPU_STK  AppCaminhaoMovimentoStk[SIZE];

static  OS_TCB   AppCaminhaoSimulacaoTCB;
static  CPU_STK  AppCaminhaoSimulacaoStk[SIZE];

static OS_TCB AppLeitura_InputTCB;
static OS_TCB AppLeitura_InputStk[SIZE];

OS_SEM Bloqueio;

//declaração das funções que serão usadas no código
static void App_TaskStart(void  *p_arg);
static void App_Teste_Bafometro(void  *p_arg);
static void App_Check_Temperatura(void  *p_arg);
static void App_Check_Nível(void  *p_arg);
static void App_CaminhaoMovimento(void  *p_arg);
static void App_CaminhaoSimulacao(void  *p_arg);
static void App_Leitura_Input(void *p_arg);

void Teste_Bafometro(int i){
	OS_ERR err_os;
	if (i == 0){
		printf ("Motorista Embriagado - Caminhao Bloqueado\n\n");
	} else {
		printf ("Motorista Sem Sinais de Embriaguez - Caminhao Desbloqueado\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		printf ("Caminhao Ligado\n\n");
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
	}
	OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

void Alarme_Temperatura (int i)
{
	OS_ERR err_os;
	if (i == 0)
	{
		printf ("Temperatura do Motor Elevada\n\n");
	}
	else {
		printf ("Temperatura do Motor Normal\n\n");
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
	}
	OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

void Alarme_Nivel (int i)
{
	OS_ERR err_os;
	if (i == 0)
	{
		printf ("Nivel de Combustivel Baixo\n\n");
	}
	else {
		printf ("Nivel de Coonbustivel Normal\n\n");
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
	}
	OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

void EmMovimento (int i)
{
	OS_ERR err_os;
		if (i!= 0)
		{
			OS_ERR err_os;
			printf ("Caminhao em viagem a seu destino\n\n");
			OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
		} else {
		printf ("Caminhao Parado\n\n");
		}
	OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os);
}

void Sonolento (int i)
{
	OS_ERR err_os;
	if (i == 0)
	{
		printf ("Motorista Sonolento - Aciona sirene de alerta\n\n");
		output_sirene = 1;
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
	}
	else {
		printf ("Motorista Sem Sono\n\n");
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);

	}
	OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

static void App_Leitura_Input(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro

	input_sensor_embriaguez = 1;
	input_sensor_temperatura = 1;
	input_sensor_nivel = 1;
	input_sensor_movimento = 1;
	input_sensor_sono = 0;
}

static void App_Teste_Bafometro(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		printf("Caminhao Carregando\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		Teste_Bafometro(input_sensor_embriaguez);  
}

static void App_Check_Temperatura(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		printf("Check Motor - Temperatura\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		Alarme_Temperatura(input_sensor_temperatura); 
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

static void App_Check_Nivel(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		printf("Check Motor - Nivel de Combustivel\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		Alarme_Nivel(input_sensor_nivel); 
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
}

static void App_CaminhaoMovimento(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		printf("Caminhao pode Partir\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		EmMovimento(input_sensor_movimento); 
}

static void App_CheckSono(void *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os); 
		Sonolento(input_sensor_sono); 
}

static void App_CaminhaoSimulacao(void  *p_arg)
{
	OS_ERR err_os; //reter algum possível erro
		
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		Sonolento(input_sensor_sono);
		input_sensor_sono = 1;
		Sonolento(input_sensor_sono);
		EmMovimento(input_sensor_movimento);
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os);
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
		input_sensor_temperatura = 0;
		Alarme_Temperatura(input_sensor_temperatura);
		input_sensor_temperatura = 1;
		Alarme_Temperatura(input_sensor_temperatura);
		EmMovimento(input_sensor_movimento);
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os);
		OSSemPend(&Bloqueio, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);
		printf ("Caminhao Chegou ao seu Destino\n\n");
		OSTimeDlyHMSM(0, 0, 4, 0, OS_OPT_TIME_DLY, &err_os);
		OSSemPost(&Bloqueio, OS_OPT_POST_1, &err_os);
	}
	
static  void  App_TaskStart(void  *p_arg)
{
	OS_ERR  err_os; //detecta algum possível erro

		OSSemCreate (&Bloqueio, "Bloqueio", 1, &err_os);                 
		
		OSTaskCreate((OS_TCB     *)&AppLeitura_InputTCB,            
			(CPU_CHAR   *)"Leitura_Input",
			(OS_TASK_PTR)App_Leitura_Input,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppLeitura_InputStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);
		
		OSTaskCreate((OS_TCB     *)&AppTeste_BafometroTCB,               
			(CPU_CHAR   *)"Teste_Bafometro",
			(OS_TASK_PTR)App_Teste_Bafometro,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppTeste_BafometroStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);

		OSTaskCreate((OS_TCB     *)&AppCheck_TemperaturaTCB,             
			(CPU_CHAR   *)"Check_Temperatura",
			(OS_TASK_PTR)App_Check_Temperatura,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppCheck_TemperaturaStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);
			
		OSTaskCreate((OS_TCB     *)&AppCheck_NivelTCB,              
			(CPU_CHAR   *)"Check_Nivel",
			(OS_TASK_PTR)App_Check_Nivel,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppCheck_NivelStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);

	OSTaskCreate((OS_TCB     *)&AppCaminhaoMovimentoTCB,              
			(CPU_CHAR   *)"CaminhaoMovimento",
			(OS_TASK_PTR)App_CaminhaoMovimento,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppCaminhaoMovimentoStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);

		OSTaskCreate((OS_TCB     *)&AppCaminhaoSimulacaoTCB,               
			(CPU_CHAR   *)"CaminhaoSimulacao",
			(OS_TASK_PTR)App_CaminhaoSimulacao,
			(void       *)0,
			(OS_PRIO) APP_TASK_START_PRIO,
			(CPU_STK    *)&AppCaminhaoSimulacaoStk[0],
			(CPU_STK_SIZE)SIZE / 10u,
			(CPU_STK_SIZE)SIZE,
			(OS_MSG_QTY)0u,
			(OS_TICK)0u,
			(void       *)0,
			(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			(OS_ERR     *)&err_os);

}

int  main(void) //criação da tarefa principal
{
	OS_ERR  err_os;

	OSInit(&err_os);                                            /* inicializa uC/OS-III.                                */
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
	
	OSTaskCreate((OS_TCB     *)&AppStartTaskTCB,                /* Cria a tarefa inicial.                             */
		(CPU_CHAR   *)"App Start Task",
		(OS_TASK_PTR)App_TaskStart,
		(void       *)0,
		(OS_PRIO)APP_TASK_START_PRIO,
		(CPU_STK    *)&AppStartTaskStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY)0u,
		(OS_TICK)0u,
		(void       *)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSStart(&err_os);                                           /* Inicia o funcionamento do escalonador. */
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

}