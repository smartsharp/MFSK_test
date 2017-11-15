//============================================================================
// Name        : MFSK2.cpp
// Author      : zhanghongbiao@vargo.com.cn
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "modem.h"
#include "mfsk.h"
#include "olivia.h"
#include "globals.h"
#include "util.h"
#include "psk.h"

//using namespace std;

static modem* active_modem = NULL;

static pthread_t tx_thread;
static pthread_t rx_thread;
static int tx_thread_state = 0; // 0 wait: 1: run, 2: quit
static int rx_thread_state = 0;

static pthread_mutex_t mutex_txbuf = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_rxbuf = PTHREAD_MUTEX_INITIALIZER;

#define OUT_NAME "/home/vargo/work/tmp/mfsk16.enc"


static FILE	*fp_out = NULL;
static short fp_buf[512];
static double fp_data[512];
static void on_sound_write(double *buf, int len){
	printf("on_sound_write: %d\n", len);

	for(int i=0;i<len;i++){
		fp_buf[i] = (short)(buf[i]*32767/2);
	}
	fwrite(fp_buf, sizeof(short), len, fp_out);
	//fwrite(buf, sizeof(double), len, fp_out);
}
static int on_sound_read(double **buf, int *len){
	//int r = fread(fp_data, sizeof(double), *len, fp_out);
	//printf("on_sound_read: %d, %d\n", *len, r);

	int r = fread(fp_buf, sizeof(short), *len, fp_out);

	printf("on_sound_read: %d\n", r);

	for(int i=0;i<r;i++){
		fp_data[i] = (fp_buf[i]*1.0)/(32767/2);
	}
	//for(int i=0;i<r;i++){
	//	printf("on_sound_read: i=%d, %f\n", i, fp_float[i]);
	//}
	*buf = fp_data;
	*len = r;

	return r<=0?-1:r;
}
int put_tx_chars(uint8_t *buf, int length){
	pthread_mutex_lock(&mutex_txbuf);
	if(active_modem->tx_in_idx > 0){
		memmove(&active_modem->tx_in_buf[0], &active_modem->tx_in_buf[active_modem->tx_in_idx], sizeof(uint8_t)*(active_modem->tx_in_len-active_modem->tx_in_idx));
		active_modem->tx_in_idx = 0;
		active_modem->tx_in_len -= active_modem->tx_in_idx;
	}
	int remains = TX_IN_BUF_SIZE - active_modem->tx_in_len;
	int cplen = MIN(remains, length);
	memcpy(&active_modem->tx_in_buf[active_modem->tx_in_len], buf, cplen);
	active_modem->tx_in_len+=cplen;
	printf("put_tx_char: %d, %d, %d\n", length, cplen, active_modem->tx_in_len);
	pthread_mutex_unlock(&mutex_txbuf);
	return cplen;
}

int get_tx_char()
{
	int ch = GET_TX_CHAR_NODATA;
	pthread_mutex_lock(&mutex_txbuf);
	if(active_modem->tx_in_idx<active_modem->tx_in_len){
		ch = active_modem->tx_in_buf[active_modem->tx_in_idx++];
		printf("get_tx_char: %c, %d, %d\n", ch, active_modem->tx_in_idx, active_modem->tx_in_len);
	}else{
		active_modem->tx_in_idx = active_modem->tx_in_len = 0;
		printf("get_tx_char: empty\n");
	}
	pthread_mutex_unlock(&mutex_txbuf);
	return ch;
}

void put_rx_char(uint32_t ch)
{
	pthread_mutex_lock(&mutex_rxbuf);
	printf("put_rx_char: 0x%02x, %c\n", ch, ch);
	pthread_mutex_unlock(&mutex_rxbuf);
}




static void set_modem(modem* m)
{
	active_modem = m;
}

int init_modem(int mode)
{
	switch (mode) {
	case MODE_MFSK4:
	case MODE_MFSK11:
	case MODE_MFSK22:
	case MODE_MFSK31:
	case MODE_MFSK64:
	case MODE_MFSK8:
	case MODE_MFSK16:
	case MODE_MFSK32:
	case MODE_MFSK128:
	case MODE_MFSK64L:
	case MODE_MFSK128L:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new mfsk(mode));
		return 0;

	case MODE_PSK31: case MODE_PSK63: case MODE_PSK63F:
	case MODE_PSK125: case MODE_PSK250: case MODE_PSK500:
	case MODE_PSK1000:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;

	case MODE_QPSK31: case MODE_QPSK63: case MODE_QPSK125: case MODE_QPSK250: case MODE_QPSK500:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;


	case MODE_8PSK125:
	case MODE_8PSK250:
	case MODE_8PSK500:
	case MODE_8PSK1000:
	case MODE_8PSK125FL:
	case MODE_8PSK125F:
	case MODE_8PSK250FL:
	case MODE_8PSK250F:
	case MODE_8PSK500F:
	case MODE_8PSK1000F:
	case MODE_8PSK1200F:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;
	case MODE_PSK125R: case MODE_PSK250R: case MODE_PSK500R:
	case MODE_PSK1000R:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;

	case MODE_12X_PSK125 :
	case MODE_6X_PSK250 :
	case MODE_2X_PSK500 :
	case MODE_4X_PSK500 :
	case MODE_2X_PSK800 :
	case MODE_2X_PSK1000 :
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;

	case MODE_4X_PSK63R :
	case MODE_5X_PSK63R :
	case MODE_10X_PSK63R :
	case MODE_20X_PSK63R :
	case MODE_32X_PSK63R :

	case MODE_4X_PSK125R :
	case MODE_5X_PSK125R :
	case MODE_10X_PSK125R :
	case MODE_12X_PSK125R :
	case MODE_16X_PSK125R :

	case MODE_2X_PSK250R :
	case MODE_3X_PSK250R :
	case MODE_5X_PSK250R :
	case MODE_6X_PSK250R :
	case MODE_7X_PSK250R :

	case MODE_2X_PSK500R :
	case MODE_3X_PSK500R :
	case MODE_4X_PSK500R :

	case MODE_2X_PSK800R :
	case MODE_2X_PSK1000R :
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new psk(mode));
		return 0;

	case MODE_OLIVIA:
	case MODE_OLIVIA_4_250:
	case MODE_OLIVIA_8_250:
	case MODE_OLIVIA_4_500:
	case MODE_OLIVIA_8_500:
	case MODE_OLIVIA_16_500:
	case MODE_OLIVIA_8_1000:
	case MODE_OLIVIA_16_1000:
	case MODE_OLIVIA_32_1000:
	case MODE_OLIVIA_64_2000:
		set_modem(*mode_info[mode].modem ? *mode_info[mode].modem :
				  *mode_info[mode].modem = new olivia(mode));
		return 0;

	default:
		printf("init_modem failed: Unknown mode: %d\n", mode);
		return -1;
	}
}


static void *tx_loop(void *args)
{
	while(tx_thread_state==0){
		usleep(10*1000);
	}
	if(tx_thread_state == 2){
		return NULL;
	}
	active_modem->tx_init();
	active_modem->tx_sample_count = 0;
	active_modem->tx_sample_rate = active_modem->get_samplerate();

	while(!active_modem->stopflag && tx_thread_state == 1) {
		if (active_modem->tx_process() < 0)
			break;
	}
	tx_thread_state = 0;
	return NULL;
}
static void *rx_loop(void *args)
{
	double *buf;
	int len;
	while(rx_thread_state==0){
		usleep(10*1000);
	}
	if(rx_thread_state == 2){
		return NULL;
	}
	active_modem->rx_init();

	while(!active_modem->stopflag && rx_thread_state == 1) {
		len = active_modem->getbuflen();
		if (active_modem->on_rx_read(&buf, &len) < 0) {
			break;
		}
		active_modem->rx_process(buf, len);
	}
	rx_thread_state = 0;
	return NULL;
}


int start_modem(bool tx, bool rx)
{
	if (!active_modem) {
		printf("modem not init!\n");
		return -1;
	}
	tx_thread = 0;
	rx_thread = 0;
	tx_thread_state = 0;
	rx_thread_state = 0;
	active_modem->stopflag = false;
	if (tx && pthread_create(&tx_thread, NULL, tx_loop, NULL) < 0) {
		printf("pthread_create tx_loop failed.\n");
		goto fail;
	}
	if (rx && pthread_create(&rx_thread, NULL, rx_loop, NULL) < 0) {
		printf("pthread_create rx_loop failed.\n");
		goto fail;
	}
	if (tx) tx_thread_state = 1;
	if (rx) rx_thread_state = 1;

	return 0;
fail:
	tx_thread_state = 2;
	rx_thread_state = 2;
	return -1;
}
int stop_modem(){

	printf("stop_modem.\n");
	active_modem->stopflag = true;
	while(tx_thread_state == 1 || rx_thread_state==1){
		usleep(20*1000);
	}
	return 0;
}

int main() {
	char buf[64];
	strcpy(buf, "hello");
	if(init_modem(MODE_QPSK500)<0){
		return -1;
	}
	active_modem->on_tx_write = on_sound_write;
	active_modem->on_rx_read = on_sound_read;
#if 0
	put_tx_chars((uint8_t*)buf, strlen(buf));
	if ((fp_out = fopen(OUT_NAME,"wb")) == NULL){
		fprintf(stderr, "ERROR: cannot write file %s.\n", OUT_NAME);
		exit(1);
	}
	start_modem(true, false);
#else
	if ((fp_out = fopen(OUT_NAME,"rb")) == NULL){
		fprintf(stderr, "ERROR: cannot read file %s.\n", OUT_NAME);
		exit(1);
	}
	start_modem(false, true);
#endif
	usleep(500*1000);
	stop_modem();
	fclose(fp_out);
	printf("done.\n");

	return 0;
}
