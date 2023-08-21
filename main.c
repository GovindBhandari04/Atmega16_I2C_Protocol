/*
 * I2C.c
 *
 * Created: 8/21/2023 11:19:32 AM
 * Author : Govind
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void lcd_data(unsigned char data)
{
	PORTA = data;
	PORTB |= (1<<PB0);
	PORTB &= ~(1<<PB1);
	PORTB |= (1<<PB2);
	_delay_ms(2);
	PORTB &= ~(1<<PB2);
}

void lcd_command(unsigned char cmd)
{
	PORTA = cmd;
	PORTB &= ~(1<<PB0);
	PORTB &= ~(1<<PB1);
	PORTB |= (1<<PB2);
	_delay_ms(2);
	PORTB &= ~(1<<PB2);
}

void lcd_string(const char *string,char length)
{
	for(int i=0;i<length;i++)
	{
		lcd_data(string[i]);
	}
}

void lcd_intialize()
{
	lcd_command(0x38);
	lcd_command(0x06);
	lcd_command(0x0C);
	lcd_command(0x01);
}

void hex_to_ascii(unsigned char value)
{
	unsigned char bcd;
	bcd = value;   //82
	bcd = bcd & (0xf0);
	bcd = bcd | 0x30;
	lcd_data(bcd);
	bcd = value;
	bcd = bcd & (0x0f);
	bcd = bcd | (0x30);
	lcd_data(bcd);
}

char decimal_to_bcd(unsigned char value)
{
	unsigned char msb,lsb,hex;
	msb = value/10;
	lsb = value%10;
	hex = (msb<<4)+lsb;
	return hex;
}

void I2C_init()
{
	TWSR = 0;  
	TWBR = ((F_CPU/1000000)-16)/(2*pow(4,(TWSR & ((1<<TWPS0)|(1<<TWPS1)))));
}

void I2C_addwrite(char write_address)
{
	TWCR |= (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = write_address;
	TWCR |= (1<<TWEN)|(1<<TWINT);
	while(!(TWCR & TWINT));
}

void I2C_read(char read_address)
{
	TWCR |= (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = read_address;
	TWCR |= (1<<TWEN)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR |= (1<<TWEN)|(1<<TWINT)|(1<<TWEA); 
	while(!(TWCR & (1<<TWINT)));
}

void I2C_STOP()
{
	TWCR |= (1<<TWEN)|(1<<TWSTO)|(1<<TWINT);
	while(TWEN & (1<<TWSTO));
}

void I2C_write(char data)
{
	TWDR = decimal_to_bcd(data);
	TWCR |= (1<<TWEN)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
}
 
unsigned char I2C_read_ack()
 {
	 TWCR |= (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	 while(!(TWCR & (1<<TWINT)));
	 return TWDR;
 }
 
 unsigned char I2C_read_Nack()
  {
	  TWCR |= (1<<TWEN)|(1<<TWINT);
	  while(!(TWCR & (1<<TWINT)));
	  return TWDR;
  }
  
  
int main(void)
{  
	uint8_t data = 0;
	sei();
    DDRA |= 0xff;
    DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2);
    DDRC = 0X00;
	PORTC = 0XFF;
	
    lcd_intialize();
    
    
	    lcd_command(0x85);
	    lcd_string("GOVIND",6);
	    
		I2C_init();
		
		I2C_addwrite(0xA0);
		I2C_write(32);
		I2C_write(45);
		I2C_write(98);
		I2C_STOP();
		_delay_ms(1000);
		
		I2C_addwrite(0xA0);
		I2C_write(32);
		I2C_write(45);
		I2C_read(0xA1);
		data = I2C_read_Nack();
		I2C_STOP();
		
		lcd_command(0xc0);
		hex_to_ascii(data);
		
		while (1)
		{
			
		}
}
