#include <lpc17xx.h>
#include <stdio.h>
#include <RTL.h>

#define RS_CTRL  0x02000000  //P3.25
#define RW_CTRL  0x04000000  //P3.26
#define EN_CTRL  0x10000000  //P4.28
#define DT_CTRL  0x00F00000  //P1.20 to P1.23
#define MOTOR_START 0x02
#define MOTOR_DONE 0x04
#define SERVO_PIN 0x04000000   // P0.26 (Pin for Servo PWM)
#define IR_SENSOR_PIN (1 << 10) // IR sensor connected to P0.10 (CN6-9)

OS_TID t_faucet_motor;
OS_TID t_faucet_sensor;
int j;

__task void init_task(void);
__task void faucet_motor(void);
__task void faucet_sensor(void);

unsigned long int var1 = 0, var2 = 0;
unsigned long int temp1 = 0, temp2 = 0;
unsigned char lcd_faucet[] = {"DISPENSING..."};
int lcd_controls[4] = {0x30, 0x30, 0x30, 0x20};
int lcd_commands[5] = {0x28, 0x0c, 0x06, 0x01, 0x80};

void Servo_Init(void);
void Servo_Control(int angle);
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void lcd_puts(unsigned char *str);
unsigned int delay(int r1);

__task void init_task(void)
{
	t_faucet_sensor = os_tsk_create(faucet_sensor, 1);
  t_faucet_motor = os_tsk_create(faucet_motor, 1);

  Servo_Control(0);
  os_tsk_delete_self();
}
__task void faucet_motor(void)
{
    while (1) {
        os_evt_wait_and(MOTOR_START, 0xFFFF);
        // Turn servo to 90 degrees
        Servo_Control(90);
        os_dly_wait(2);
        // Servo will return to 0 degrees in the sensor task when the object is removed
    }
}

__task void faucet_sensor(void)
{
    while (1) {
        // Read IR sensor state
        int current_state = (LPC_GPIO0->FIOPIN & IR_SENSOR_PIN) ? 1 : 0;

        if (current_state == 0) {
            lcd_puts(lcd_faucet);
            os_evt_set(MOTOR_START, t_faucet_motor);
        }
				else {
            clr_disp();
            Servo_Control(150);
        }
        os_dly_wait(2); 
    }
}

void lcd_init()
{
	int i=0;
  LPC_PINCON->PINSEL3 &= 0xFFFF00FF;  //P1.20 to P1.23
	LPC_PINCON->PINSEL7 &= 0XFFF3FFFF;  //P3.25
  LPC_PINCON->PINSEL7 &= 0xFFCFFFFF;  //P3.26
	LPC_PINCON->PINSEL9 &= 0xFCFFFFFF;  //P4.28

  LPC_GPIO1->FIODIR |= DT_CTRL;	// data lines - P1.20 to P1.23
	LPC_GPIO3->FIODIR |= RS_CTRL;	// RS - P3.25
  LPC_GPIO3->FIODIR |= RW_CTRL;	// RW - P3.26
	LPC_GPIO4->FIODIR |= EN_CTRL;	// P4.28 
       
  clear_ports();
	delay_lcd(3200);

	for (i=0; i<4; i++) {
		temp2 = lcd_controls[i];
		wr_cn();
		delay_lcd(10000);
	}
	
	for (i=0; i<5; i++) {
		temp1 = lcd_commands[i];
		lcd_com();
		delay_lcd(10000);
	}
  return;
}

void lcd_com(void)
{
	temp2= temp1 & 0xf0;
	temp2 = temp2 << 16;
	wr_cn();
	temp2 = temp1 & 0x0f;
	temp2 = temp2 << 20; 
	wr_cn();
	delay_lcd(1000);
  return;
}

void wr_cn(void)
{ 	 
	clear_ports();
	LPC_GPIO1->FIOPIN = temp2;   
	LPC_GPIO3->FIOCLR = RW_CTRL;
  LPC_GPIO3->FIOCLR = RS_CTRL;
	LPC_GPIO4->FIOSET = EN_CTRL;
	delay_lcd(25);
	LPC_GPIO4->FIOCLR  = EN_CTRL;
  return;  
}

void lcd_data(void)
{             
	temp2 = temp1 & 0xf0;
  temp2 = temp2 << 16;
  wr_dn();
  temp2= temp1 & 0x0f;	
  temp2= temp2 << 20;
  wr_dn();
  delay_lcd(1000);	
  return;
} 

void wr_dn(void)
{  	  
	clear_ports();
	LPC_GPIO1->FIOPIN = temp2;
	LPC_GPIO3->FIOSET = RS_CTRL;
	LPC_GPIO3->FIOCLR = RW_CTRL;
	LPC_GPIO4->FIOSET = EN_CTRL;
	delay_lcd(25);
	LPC_GPIO4->FIOCLR = EN_CTRL;
  return;
}

void delay_lcd(unsigned int r1)
{
	unsigned int r;
  for(r=0;r<r1;r++);
  return;
}

void clr_disp(void)
{
	temp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
  return;
}

void clear_ports(void)
{
	LPC_GPIO1->FIOCLR = DT_CTRL;
	LPC_GPIO3->FIOCLR = RS_CTRL;
  LPC_GPIO3->FIOCLR = RW_CTRL;
	LPC_GPIO4->FIOCLR = EN_CTRL;
  return;
}

void lcd_puts(unsigned char *str)
{
	while(*str) {
		temp1 = *str++;
		lcd_data(); 
  }
	temp1 = 0xc0;
	lcd_com();
  return;
}

void Servo_Init(void)
{
	LPC_PINCON->PINSEL1 &= ~0x00030000;
  LPC_PINCON->PINSEL1 |= 0x00010000;
  LPC_GPIO0->FIODIR |= SERVO_PIN;
}

void Servo_Control(int angle)
{
	int pulse_width = (angle * 10) + 500; // Calculate the pulse width (range: 500-2500)
  LPC_GPIO0->FIOPIN &= ~SERVO_PIN; // Clear the P0.26 pin (initial low)
  delay(pulse_width);              // Wait for the calculated pulse width
  LPC_GPIO0->FIOPIN |= SERVO_PIN;  // Set the P0.26 pin (high)
  delay(20000 - pulse_width);      // Wait for the remaining cycle time (20ms)
}

unsigned int delay(int r1)
{
	int r2;
  for (r2 = 0; r2 < r1; r2++);
  return 0;
}

int main(void)
{
	LPC_GPIO0->FIODIR &= ~IR_SENSOR_PIN;
  Servo_Init();
	lcd_init();
  SystemInit();
  SystemCoreClockUpdate();
  os_sys_init(init_task);
  while (1);
}
