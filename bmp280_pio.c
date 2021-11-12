//This Code was written by LEARN EMBEDDED SYSTEMS
//https://learnembeddedsystems.co.uk/bmp280-and-pi-pico-over-spi
//and was edited by Alfonso Samuel Pungus with purpose of using PIO SPI

//Include necessary library
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pio_spi.h"



//We define our SPI Connecting Pin
#define MISO 20
#define CS   1
#define SCLK 18
#define MOSI 19

//** Code from Datasheet BMP280 **//
//https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf//

pio_spi_inst_t spi;
int32_t t_fine;
uint16_t dig_T1;
int16_t dig_T2, dig_T3;

// Returns temperature in DegC.
// t_fine carries fine temperature as global value
int32_t compTemp(int32_t adc_T){
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

//*********** END Of Datasheet Code ********//

//Function for read the Temperature Compensation
void read_temp_comp(const pio_spi_inst_t *spi){

    uint8_t buffer[6] ={0,0,0,0,0,0}; 
    uint8_t reg;
    reg = 0x88 | 0x80;
    gpio_put(spi->cs_pin, 0);
    pio_spi_write8_blocking(spi, &reg, 1);
    pio_spi_read8_blocking(spi, buffer, 6);
    gpio_put(spi->cs_pin, 1);

    dig_T1 = buffer[0] | (buffer[1] << 8);
    dig_T2 = buffer[2] | (buffer[3] << 8);
    dig_T3 = buffer[4] | (buffer[5] << 8);
}

void read_data(const pio_spi_inst_t *spi){
   int32_t temperature, rawtemp;
    uint8_t reg;
    uint8_t buffer[3] = {0,0,0};
  reg = 0xFA | 0X80; // Reg 0xFA is The “temp” register contains the raw temperature measurement output data 
        gpio_put(CS, 0);
        pio_spi_write8_blocking(spi, &reg, 1);  //EDITED
        pio_spi_read8_blocking(spi, buffer, 3); //EDITED
        gpio_put(CS, 1);

        rawtemp = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t) buffer[2] >> 4);

        temperature = compTemp(rawtemp);

        printf("Temp = %.2fC \n", temperature / 100.00);
}

 void event_callback(uint gpio, uint32_t events){
     read_temp_comp(&spi);
 }

int main() {
    stdio_init_all();

//*********This Code was written by Alfonso for PIO**********//
    //Declare spi instance
    //PIO instance 0, State mashine 0 and declare CS pin
    pio_spi_inst_t spi = {.pio = pio0, .sm = 0, .cs_pin = CS};

    //Initialize Chip Select pin, high, and as Output Pin 
    gpio_init(spi.cs_pin);
    gpio_put(spi.cs_pin, 1);
    gpio_set_dir(spi.cs_pin, GPIO_OUT);
    
    gpio_set_irq_enabled_with_callback(CS, 0x02, 1, event_callback);

    //define prog_offs  
    uint prog_offs = pio_add_program(spi.pio, &spi_cpha0_program);
    printf("Loaded program at %d\n", prog_offs);

    //Call the Function from the spi.pio
    pio_spi_init(spi.pio, spi.sm, prog_offs,
                 8,       // 8 bits per SPI frame
                 3.125f,  // 10 MHz @ 125 clk_sys
                 false,   // CPHA = 0
                 false,   // CPOL = 0
                 SCLK,
                 MOSI,
                 MISO
    );
//*********End Alfonso Code*************//  

    //read the temperature Compensation
    //read_temp_comp(&spi);

//This Code is from LEARN EMBEDDED SYSTEMS with some edited made by Alfonso for PIO//
//if this is deleted, we just get a constant number.
    uint8_t data[2]; // Array to store data to be sent
    data[0] = 0xF4 & 0x7F; // Remove first bit to indicate write operation
    data[1] = 0x27; // Data to be sent
    gpio_put(spi.cs_pin, 0);
    pio_spi_write8_blocking(&spi, data, 2); //EDITED
    gpio_put(spi.cs_pin, 1);
    
    // int32_t temperature, rawtemp;
    // uint8_t reg;
    // uint8_t buffer[3] = {0,0,0};
    while(1){
        //if no this, the temp will be minus
        // reg = 0xFA | 0X80; // Reg 0xFA is The “temp” register contains the raw temperature measurement output data 
        // gpio_put(spi.cs_pin, 0);
        // pio_spi_write8_blocking(&spi, &reg, 1);  //EDITED
        // pio_spi_read8_blocking(&spi, buffer, 3); //EDITED
        // gpio_put(spi.cs_pin, 1);

        // rawtemp = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t) buffer[2] >> 4);

        // temperature = compTemp(rawtemp);

        // printf("Temp = %.2fC \n", temperature / 100.00);
        tight_loop_contents();
    };
}
//*************END of LEARN EMBEDDED SYSTEMS Code****************//

