/*
 * This file is part of DAMC-TCK7 Board Support Package
 *
 * Copyright (c) 2018 Deutsches Elektronen-Synchrotron DESY
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <xiic.h>
#include <xintc.h>

static XIic iic;
static XIntc intc;

static volatile u8 TransmitComplete;
static volatile u8 ReceiveComplete;

static void SendHandler(XIic *InstancePtr)
{
	TransmitComplete = 0;
}

static void ReceiveHandler(XIic *InstancePtr)
{
	ReceiveComplete = 0;
}

static void StatusHandler(XIic *InstancePtr, int Event)
{

}

int iic_init(void) {
	int rc;

	XIic_Config *cfg_ptr = XIic_LookupConfig(XPAR_MB_AXI_IIC_0_DEVICE_ID);
	if (!cfg_ptr) {
		xil_printf("[iic] XIic_LookupConfig() failed\r\n");
		return XST_FAILURE;
	}

	rc = XIic_CfgInitialize(&iic, cfg_ptr, cfg_ptr->BaseAddress);
	if (rc != XST_SUCCESS) {
		xil_printf("[iic] XIic_CfgInitialize() failed\r\n");
		return rc;
	}

	rc = XIntc_Initialize(&intc, XPAR_MB_AXI_IIC_0_DEVICE_ID);
	if (rc != XST_SUCCESS) {
		xil_printf("[iic] XIntc_Initialize() failed\r\n");
		return rc;
	}

	rc = XIntc_Connect(&intc, XPAR_INTC_0_IIC_0_VEC_ID,
				   (XInterruptHandler) XIic_InterruptHandler,
				   &iic);
	if (rc != XST_SUCCESS) {
		xil_printf("[iic] XIntc_Connect() failed\r\n");
		return rc;
	}

	rc = XIntc_Start(&intc, XIN_REAL_MODE);
	if (rc != XST_SUCCESS) {
		xil_printf("[iic] XIntc_Connect() failed\r\n");
		return rc;
	}

	XIntc_Enable(&intc, XPAR_INTC_0_IIC_0_VEC_ID);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();


	XIic_SetSendHandler(&iic, &iic, (XIic_Handler) SendHandler);
	XIic_SetRecvHandler(&iic, &iic, (XIic_Handler) ReceiveHandler);
	XIic_SetStatusHandler(&iic, &iic, (XIic_StatusHandler) StatusHandler);

	return XST_SUCCESS;
}

int iic_set_slave_addr(u8 addr) {
	int rc;
	rc = XIic_SetAddress(&iic, XII_ADDR_TO_SEND_TYPE, addr);
	if (rc != XST_SUCCESS) {
		xil_printf("XIic_SetAddress FAILURE\n");
		return rc;
	}
	return XST_SUCCESS;
}

int iic_write(u8 WriteBuffer[8], int len)
{
	int Status;

	TransmitComplete = 1;
	iic.Stats.TxErrors = 0;

	Status = XIic_Start(&iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIic_MasterSend(&iic, WriteBuffer, len);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while ((TransmitComplete) || (XIic_IsIicBusy(&iic) == TRUE)) {
		//usleep(1);
		//xil_printf(".");
	}

	Status = XIic_Stop(&iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int iic_read_data(u8 *BufferPtr)
{
	int Status;

	ReceiveComplete = 1;

	Status = XIic_Start(&iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIic_MasterRecv(&iic, BufferPtr, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while ((ReceiveComplete) || (XIic_IsIicBusy(&iic) == TRUE)) {
		//usleep(1);
		//xil_printf(".");
	}

	Status = XIic_Stop(&iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
