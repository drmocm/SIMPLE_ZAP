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
    
    dvb_init(&dev, &fe, &lnb);
    if ((out=parse_args(argc, argv, &dev, &fe, &lnb, filename)) < 0)
	exit(2);
    printf("out %d\n",out);
    if (out == 6){
	int k = 0;
	int done = 0;
	do {
	    int fd = 0;
	    if ( (fd = open_fe(dev.adapter, k)) < 0){
		done =1;
	    } else {
		k++;
		close(fd);
	    }
	} while(!done);
	max = k;
	err ("Found %d frontends\n",max);
    }
    
    dvb_open(&dev, &fe, &lnb);
    if ((lock = dvb_tune(&dev, &fe, &lnb)) != 1) exit(lock);
    if (lock){
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
	    while(lock){
		int re = read(dev.fd_dvr,buf,BUFFSIZE);
		re = write(fd,buf,re);
	    }
	    break;
	}

	case 0:
	    close(dev.fd_dvr);
	    close(dev.fd_dmx);
	    
	    while (1) {
		fe_status_t stat;
		int64_t str, cnr;
                
		stat = dvb_get_stat(dev.fd_fe);
		str = dvb_get_strength(dev.fd_fe);
		cnr = dvb_get_cnr(dev.fd_fe);
                
		printf("stat=%02x, str=%" PRId64 ".%03udBm, "
		       "snr=%" PRId64 ".%03uddB \n",
		       stat, str/1000, abs(str%1000),
		       cnr/1000, abs(cnr%1000));
                sleep(1);
	    }

	
	default:
	    break;
	}
    }
}
