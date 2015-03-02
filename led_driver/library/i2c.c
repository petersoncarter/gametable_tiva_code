#include "i2c.h"

static bool has_stopped = true;

bool i2c_enable(uint8_t cgc_mask)
{
	if((cgc_mask & INVALID_CGC) != 0x00)
		return false;

	SYSCTL->RCGCI2C |= cgc_mask;
	
	while((SYSCTL->PRI2C & cgc_mask) != cgc_mask)
		; // Wait
	
	return true;
}

void i2c_init_master(I2C0_Type* i2c)
{
	i2c->MCR |= I2C_MCR_MASTER_EN;
}

void i2c_init_slave(I2C0_Type* i2c)
{
	i2c->MCR |= I2C_MCR_SLAVE_EN;
}

void i2c_config_speed(I2C0_Type* i2c, uint8_t speed)
{
	i2c->MTPR &= ~(0x7F);
	i2c->MTPR |= speed;
}

void i2c_master_interrupt_enable(I2C0_Type* i2c, uint8_t priority)
{
	IRQn_Type interrupt_vector;
	
	switch((uint32_t) i2c) {
	case(I2C0_BASE):
		interrupt_vector = I2C0_IRQn;
		break;
	case(I2C1_BASE):
		interrupt_vector = I2C1_IRQn;
		break;
	case(I2C2_BASE):
		interrupt_vector = I2C2_IRQn;
		break;
	case(I2C3_BASE):
		interrupt_vector = I2C3_IRQn;
		break;
	default:
		return;
	}
	
	i2c->MIMR = 0x01;
	NVIC_SetPriority(interrupt_vector, priority);
	NVIC_EnableIRQ(interrupt_vector);
}

void i2c_slave_address_set(I2C0_Type* i2c, uint8_t addr)
{
	uint8_t data = i2c->MSA;
	data &= ~(I2C_MSA_ADDR_MASK);
	i2c->MSA = data | (addr << I2C_MSA_ADDR_SHAMT);
}

void i2c_slave_rw_set(I2C0_Type* i2c, uint8_t rw_mask)
{
	i2c->MSA &= ~(I2C_MSA_RW_MASK);
	i2c->MSA |= rw_mask;
}

bool i2c_send_byte(I2C0_Type* i2c, uint8_t data, bool stop, bool repeat_start)
{
	uint8_t command = I2C_MCS_RUN;
	
	i2c->MDR = data;
	
	if(has_stopped || repeat_start) {
		command |= I2C_MCS_START;
	}
	
	if(stop) {
		command |= I2C_MCS_STOP;
		has_stopped = true;
	} else {
		has_stopped = false;
	}
	
	i2c->MCS = command;
	
	return true;
}

bool i2c_read_byte(I2C0_Type* i2c, uint8_t *data, bool stop, bool repeat_start)
{
	uint8_t command = I2C_MCS_RUN;
	
	if(has_stopped || repeat_start) {
		command |= I2C_MCS_START;
	}
	
	if(stop) {
		command |= I2C_MCS_STOP;
		has_stopped = true;
	} else {
		has_stopped = false;
	}
	
	i2c->MCS = command;
	while(i2c->MCS & I2C_MCS_BUSY)
		;	// Do nothing
	
	if(i2c->MCS & (I2C_MCS_ERROR))
		return false;
	
	*data = i2c->MDR;
	return true;
}

void i2c_handle_request(I2C_channel *channel, I2C_request *req)
{
	printf("%x\n\r", req->device_addr);
	
	while(channel->busy)
		; // Wait for channel to open up
	
	channel->current_request = req;
	channel->busy = true;
	i2c_slave_address_set(channel->channel, req->device_addr);
	
	if(req->read_req) {
		i2c_slave_rw_set(channel->channel, I2C_MSA_SLAVE_READ);
	} else {
		i2c_slave_rw_set(channel->channel, I2C_MSA_SLAVE_WRITE);
		channel->channel->MDR = req->data;
	}
	
	channel->channel->MCS = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
}

void i2c_retry_request(I2C_channel *channel)
{
	printf("%x\n\r", channel->current_request->device_addr);
	
	/*channel->current_request = req;
	channel->busy = true;
	i2c_slave_address_set(channel->channel, req->device_addr);
	
	if(req->read_req) {
		i2c_slave_rw_set(channel->channel, I2C_MSA_SLAVE_READ);
	} else {
		i2c_slave_rw_set(channel->channel, I2C_MSA_SLAVE_WRITE);
		channel->channel->MDR = req->data;
	}
	
	channel->channel->MCS = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;*/
}
