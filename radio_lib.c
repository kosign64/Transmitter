#include <util/delay.h>
#include "radio_lib.h"

#include <avr/io.h>

void portInit(void)
{
    DDR_NRF |= (1 << CE);  
    spiInit();            
    CSN_HIGH();   
    CE_LOW();
}

uchar* sendToNrf(uchar readWrite, uchar reg, uchar *array, uchar arraySize)
{
    static uchar ret[32]; 
    uchar i;
    if(readWrite == W)
    {
        reg |= W_REGISTER;
    }                    
    
    _delay_us(30);
    CSN_LOW();
    _delay_us(30);
    spiSendByte(reg);
    _delay_us(30);
    
    for(i = 0; i < arraySize; ++i)
    {
        if((readWrite == R) && (reg != W_TX_PAYLOAD))
        {
            ret[i] = spiSendByte(NOP);
        }                
        else
        {
            spiSendByte(array[i]);
            
        }
        _delay_us(30);
    }
    CSN_HIGH();
    
    return ret;
}

void initAsTrans(uchar *address, uchar addressSize, uchar payloadSize)
{
    uchar val;        
    portInit();
    
    _delay_ms(1);
                     
    // EN_AA - (enable auto-acknowledgment)
    // Transmitter gets automatic response from receiver when successful transmission
    // Only if Transmitter has identical RF_Adress on it's channel ex: RX_ADDR_P0 = TX_ADDR
    val = 0x00;
    sendToNrf(W, EN_AA, &val, 1); 
    
    //val = 0x2F;
    //sendToNrf(W, SETUP_RETR, &val, 1);                                                          
    
    // Choose number of enabled data pipes (1-5)
    val = 0x01;
    sendToNrf(W, EN_RXADDR, &val, 1); // enable data pipe 0
    
    // RF_Adress width setup
    switch(addressSize)
    {
        case 3:
        {
            val = 1;
            break;
        }
        
        case 4:
        {
            val = 2;
            break;
        }
        
        case 5:
        {
            val = 3;
            break;
        }
    }
    sendToNrf(W, SETUP_AW, &val, 1);
    
    // RF channel setup - choose frequency 2,400 - 2,527GHz 1MHz/step
    val = 0x20; // 2,401GHz
    sendToNrf(W, RF_CH, &val, 1);
    
    // RF setup - choose power mode and data speed.
    val = 0x07; // 0b00000111 bit 3='0' 1Mbps = longer range, bit 2-1 power mode ("11" = - 0dB; "00" = -18dB)
    sendToNrf(W, RF_SETUP, &val, 1);
                                   
    // RX RF_Adress setup 5 bytes - Set Receiver address (set RX_ADDR_P0 = TX_ADDR if EN_AA is enabled!!!)
    sendToNrf(W, RX_ADDR_P0, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR
    
    //TX RF_Adress setup 5 bytes - Set transmitter address (not used in a receiver but can be set anyway)
    sendToNrf(W, TX_ADDR, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR   
    
    // Payload Width Setup 1-32byte (how many bytes to send per transmission)
    val = payloadSize; // Send 5 bytes per package (same on receiver and transmitter)
    sendToNrf(W, RX_PW_P0, &val, 1);
    
    // CONFIG reg setup - Now it's time to boot up the nrf and choose if it's suppose to be a transmitter or receiver
    //val[0] = 0x1E; // 0b0001 1110 - bit 0 = '0'/'1' - transmitter/receiver
                   // bit 1 = '1' - power up
                   // bit 4 = '1' - mask_Max_RT i.e. IRQ-interrupt is not triggered if transmission failed    
    val = (1 << PWR_UP) | (1 << EN_CRC) | (1 << MASK_MAX_RT); // Add interrupt for receiver
    sendToNrf(W, CONFIG, &val, 1);
    
    // device need 1.5 ms to reach standby mode (CE=low)
    _delay_ms(1);
}

void initAsReceiver(uchar *address, uchar addressSize, uchar payloadSize)
{
    uchar val;        
    portInit();
    
    _delay_ms(1);
                     
    // EN_AA - (enable auto-acknowledgment)
    // Transmitter gets automatic response from receiver when successful transmission
    // Only if Transmitter has identical RF_Adress on it's channel ex: RX_ADDR_P0 = TX_ADDR
    val = 0x00;
    sendToNrf(W, EN_AA, &val, 1); 
    
    //val = 0x2F;
    //sendToNrf(W, SETUP_RETR, &val, 1);                                                          
    
    // Choose number of enabled data pipes (1-5)
    val = 0x01;
    sendToNrf(W, EN_RXADDR, &val, 1); // enable data pipe 0
    
    // RF_Adress width setup
    switch(addressSize)
    {
        case 3:
        {
            val = 1;
            break;
        }
        
        case 4:
        {
            val = 2;
            break;
        }
        
        case 5:
        {
            val = 3;
            break;
        }
    }
    sendToNrf(W, SETUP_AW, &val, 1);
    
    // RF channel setup - choose frequency 2,400 - 2,527GHz 1MHz/step
    val = 0x20; // 2,401GHz
    sendToNrf(W, RF_CH, &val, 1);
    
    // RF setup - choose power mode and data speed.
    val = 0x07; // 0b00000111 bit 3='0' 1Mbps = longer range, bit 2-1 power mode ("11" = - 0dB; "00" = -18dB)
    sendToNrf(W, RF_SETUP, &val, 1);
                                   
    // RX RF_Adress setup 5 bytes - Set Receiver address (set RX_ADDR_P0 = TX_ADDR if EN_AA is enabled!!!)
    sendToNrf(W, RX_ADDR_P0, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR
    
    //TX RF_Adress setup 5 bytes - Set transmitter address (not used in a receiver but can be set anyway)
    sendToNrf(W, TX_ADDR, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR   
    
    // Payload Width Setup 1-32byte (how many bytes to send per transmission)
    val = payloadSize; // Send 5 bytes per package (same on receiver and transmitter)
    sendToNrf(W, RX_PW_P0, &val, 1);
    
    // CONFIG reg setup - Now it's time to boot up the nrf and choose if it's suppose to be a transmitter or receiver
    //val[0] = 0x1E; // 0b0001 1110 - bit 0 = '0'/'1' - transmitter/receiver
                   // bit 1 = '1' - power up
                   // bit 4 = '1' - mask_Max_RT i.e. IRQ-interrupt is not triggered if transmission failed    
    val = (1 << PWR_UP) | (1 << EN_CRC) | (1 << MASK_MAX_RT) |
    (1 << PRIM_RX); // Add interrupt for receiver
    sendToNrf(W, CONFIG, &val, 1);
    
    // device need 1.5 ms to reach standby mode (CE=low)
    _delay_ms(1);
}

void transmitPayload(uchar *buff, uchar payloadSize)
{
    sendToNrf(R, FLUSH_TX, buff, 0); // Sends 0xE1 to flush the regestry from old data
    sendToNrf(R, W_TX_PAYLOAD, buff, payloadSize);  
    
    _delay_ms(10);
    CE_HIGH();
    _delay_us(30);
    CE_LOW();
    _delay_ms(10);
}

void setNewAdressAndSize(uchar *address, uchar addressSize)
{
    uchar val;
    // RF_Adress width setup
    switch(addressSize)
    {
        case 3:
        {
            val = 1;
            break;
        }
        
        case 4:
        {
            val = 2;
            break;
        }
        
        case 5:
        {
            val = 3;
            break;
        }
    }
    sendToNrf(W, SETUP_AW, &val, 1);
    sendToNrf(W, RX_ADDR_P0, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR
    sendToNrf(W, TX_ADDR, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR    
}

void setNewAddress(uchar *address, uchar addressSize)
{
    uchar val = 0x00;
    sendToNrf(W, CONFIG, &val, 1);
    _delay_ms(7);
    sendToNrf(W, RX_ADDR_P0, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR
    sendToNrf(W, TX_ADDR, address, addressSize); // RX_ADDR_P0 because we chose pipe 0 on EN_RXADDR   
    val = (1 << PWR_UP) | (1 << EN_CRC) | (1 << MASK_MAX_RT);
    sendToNrf(W, CONFIG, &val, 1); 
    _delay_ms(7);
}

inline void startListengForPayload(void)
{
    CE_HIGH();
}
inline void stopListeningForPayload(void)
{
    CE_LOW();
}

void resetIRQ(void)
{
    _delay_us(30);
    CSN_LOW();
    _delay_us(30);
    spiSendByte(W_REGISTER + STATUS); // write to STATUS regisrty
    _delay_us(30);
    spiSendByte(0x70); // Reset all IRQ in STATUS registry
    _delay_us(30);
    CSN_HIGH();
}

uchar getReg(uchar reg)
{
    _delay_us(10);
    CSN_LOW(); 
    _delay_us(10);
    // R_REGISTER - set the nRF to reading mode,
    // reg - register to read
    spiSendByte(R_REGISTER + reg);
    _delay_us(10);
    reg = spiSendByte(NOP);
    _delay_us(10);
    CSN_HIGH();
    
    return reg;
}
