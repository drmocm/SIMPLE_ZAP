/*
Copyright (C) 2022  Marcus Metzler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include "dvb.h"

#define BUFFSIZE (1024*188)

void print_help()
{
    dvb_print_tuning_options();
    err(
	    "\n OTHERS:\n"
	    " -O           : write full TS to stdout or filename\n"
	    " -o filename  : output filename\n"
	    " -h           : this help message\n\n"
	);
}

int parse_args(int argc, char **argv, dvb_devices *dev,
	       dvb_fe *fe, dvb_lnb *lnb, char *fname)
{
    int out = 0;
    int max =0;
    if (dvb_parse_args(argc, argv, dev, fe, lnb)< 0) return -1;
    
    optind = 1;

    while (1) {
	int option_index = 0;
	int c;
	static struct option long_options[] = {
	    {"open dvr", required_argument , 0, 'O'},
	    {"fileout", required_argument , 0, 'o'},
	    {"help", no_argument , 0, 'h'},
	    {0, 0, 0, 0}
	};
	
	c = getopt_long(argc, argv, "ho:O", long_options, &option_index);
	if (c==-1)
	    break;
	
	switch (c) {
	case 'O':
	    out = 1;
	    break;
	    
	case 'o':
	    out = 1;
	    fname = strdup(optarg);
	    break;
	    
	case 'h':
	    print_help();
	    return -1;
	    break;
	default:
	    break;
	}
    }

    return out;
}


int main(int argc, char **argv){
    dvb_devices dev;
    dvb_lnb lnb;
    dvb_fe fe;
    int lock = 0;
    int t=0;
    int out = 0;
    int fd = 0;
    char *filename = NULL;
    uint8_t sec_buf[4096];
    int re=0;
    int max = 0;
    pthread_mutex_t *tlock;
    pthread_t tMux;
    tune_data *tdat;
    
    dvb_init(&dev, &fe, &lnb);
    if ((out=parse_args(argc, argv, &dev, &fe, &lnb, filename)) < 0)
	exit(2);

    dvb_open(&dev, &fe, &lnb);
    tlock = dvb_add_lock(&dev);

    tdat = create_tune_data(&dev, &fe, &lnb);
    start_tune_thread(&tMux, tdat);
    switch (out) {
	
    case 1:
    {
	uint8_t *buf=(uint8_t *)malloc(BUFFSIZE);
	
	if (filename){
	    err("writing to %s\n", filename);
	    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
		      00644);
	} else {
	    err("writing to stdout\n");
	    fd = fileno(stdout); 
	}

	struct timespec tp,otp;
	clock_gettime(CLOCK_REALTIME, &otp);


	while(1){
	    int re = read(dev.fd_dvr,buf,BUFFSIZE);
	    re = write(fd,buf,re);
	    clock_gettime(CLOCK_REALTIME, &tp);
	    if (tp.tv_sec > otp.tv_sec+1){
		fprint_stat(stderr, tdat);
		clock_gettime(CLOCK_REALTIME, &otp);
	    }
	    
	}
	break;
    }
    
    case 0:
	close(dev.fd_dvr);
	close(dev.fd_dmx);
	int c=0;
	while (1) {
	    fprint_stat(stdout, tdat);

	    sleep(1);
	}
	
	
    default:
	break;
    }
}

