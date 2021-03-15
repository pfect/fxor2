/* fxor_stream_xor.c */
/* 
 * Copyright (c) 2014-2015, Abderraouf Adjal
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include "fxor.h"
#include "fxor_exits.h"
#include "fxor_stream_xor.h"

/**
 * fxor_stream_xor()
 * 
 * files/fp names are just to show them in errors messages
 * 
 * Return FXOR_EX_OK (0): Done Successfully
 * Return non-zero:  Errors (e.g., I/O errors), OR key_fp is empty.
 */

int fxor_stream_xor(FILE *in_fp, FILE *key_fp, FILE *out_fp,
	const char *in_n, const char *key_n, const char *out_n, 
	size_t *consumed_key_l,long int keystart_i)
{
	static unsigned char data[FXOR_XOR_BUFFSIZE], key[FXOR_XOR_BUFFSIZE];
	size_t in_l, key_l = 0, key_i = 0, data_i, key_byte_counter=0;
	int is_empty_key_fp_r;
	char hexbuf[HEX_ENCODED_MSG_MAX_BUFSIZE];
	memset(hexbuf,0,HEX_ENCODED_MSG_MAX_BUFSIZE);
	
	
	if (!in_fp || !key_fp || !out_fp) {
		if (!in_fp) {
			warn("%s: %s", __func__, in_n);
		}
		if (!key_fp) {
			warn("%s: %s", __func__, key_n);
		}
		if (!out_fp) {
			warn("%s: %s", __func__, out_n);
		}
		
		return FXOR_EX_IOERR;
	}
	
	/* Check if key_fp is empty */
	if ((is_empty_key_fp_r = is_empty_fp(key_fp, key_n))) {
		if (is_empty_key_fp_r == 1) {
			warnx("Key file '%s' is empty.", key_n);
			return FXOR_EX_NOKEY;
		}
		
		return FXOR_EX_IOERR;
	}
	
	/* Seek to given index in key file */
	if (fseek(key_fp, keystart_i, SEEK_SET)) {
		return FXOR_EX_IOERR;
	}
	
	while (1)
	{
		data_i = 0;
		
		in_l = fread(data, sizeof(unsigned char), sizeof(data), in_fp);
		if (ferror(in_fp)) {
			warn("%s: %s", __func__, in_n);
			return FXOR_EX_IOERR;
		}
		
		if (in_l) {
			while (data_i < in_l)
			{
				if (key_i >= key_l) {
					/* Get Data From key_fp */
					
					key_i = 0; 
					key_l = fread(key, sizeof(unsigned char), sizeof(key), key_fp);
					
					if (ferror(key_fp)) {
						warn("%s: %s", __func__, key_n);
						return FXOR_EX_IOERR;
					}
					
					if (!key_l && feof(key_fp)) {
						/* End Of key_fp; Back to key_fp beginning */
						if (fseek(key_fp, 0L, SEEK_SET)) {
							return FXOR_EX_IOERR;
						}
						continue;
					}
				}
				
				data[data_i] ^= key[key_i];
				
				data_i++;
				key_i++;
			}
			key_byte_counter = key_byte_counter + key_i;
			
			/* decrypt_hex */
			/* stdout out_fp => hex encoding [::] HEX_ENCODED_MSG_MAX_BUFSIZE 
			 * we should always use file
			if ( data_i < HEX_ENCODED_MSG_MAX_BUFSIZE ) {
				for(size_t i = 0; i < data_i; i++)
					sprintf(hexbuf+2*i, "%.2x", data[i]);
				
				printf("%s \n",hexbuf);
				*consumed_key_l = key_byte_counter; 
				return FXOR_EX_OK;
			} */
			
			fwrite(data, sizeof(unsigned char), in_l, out_fp);
			if (ferror(out_fp)) {
				warn("%s: %s", __func__, out_n);
				/* TODO: Handle error & key erase */
				return FXOR_EX_IOERR;
			}
		}
		else if (!in_l && feof(in_fp)) {
			/* End Of in_fp */
			*consumed_key_l = key_byte_counter;
			return FXOR_EX_OK;
		}
	}
}


/**
 * is_empty_fp()
 * 
 * Need fp name to show error messages
 * Return:  (0): NOT empty;  (1): Empty;  (-1): I/O Error
 */

int is_empty_fp(FILE *fp, const char *fp_name)
{
	if (fgetc(fp) == EOF && !ferror(fp)) {
		if (fseek(fp, 0L, SEEK_SET)) {
			goto err;
		}
		
		if (fgetc(fp) == EOF && !ferror(fp)) {
			return 1; /* Empty */
		}
		else if (!ferror(fp)) {
			if (fseek(fp, 0L, SEEK_END)) {
				goto err;
			}
		}
	}
	else if (!ferror(fp)) {
		if (fseek(fp, -1L, SEEK_CUR)) {
			goto err;
		}
	}
	
	if (!ferror(fp)) {
		return 0; /* NOT empty */
	}
	
	err:
	warn("%s", fp_name);
	return -1;
}

long int fsize(FILE *fp){
    
    long int sz, prev;
    
    prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); 
    return sz;
}

/**
 * check_key_material()
 * 
 * Check key file content for validity (eg. it's not zero's)
 * 
 * Return 0 when ok; 1 when key block contains zero's
 * 
 */
 
int check_key_block_validity(const char *in_n, const char *key_n,long int keystart_i) 
{
	/* 1. Read source file lenght (in_n)
	 * 2. Check key block starting from keystart_i for '00' '00'
	 * 3. Return value
	 * */
	FILE *in_fp, *key_fp;
	long int file_len=0;
	long int byte_check_loop=0;
	int keybyte_0=0;
	int keybyte_1=0;
	int result=0;
	
	/* 1. Source file len */
	in_fp  = fopen(in_n, "rb");
	file_len = fsize(in_fp);
	// printf("Source file len: %ld \n",file_len);
	safe_fclose(in_fp);
	
	/* 2. Open key file */
	key_fp = fopen(key_n, "rb");
	if (!key_fp) {
		warn("%s", key_n);
		return  FXOR_EX_IOERR;
	}
	else {
		if ( !access(key_n, R_OK) ) {
			key_fp = fopen(key_n, "rb+");
			
			if (fseek(key_fp, keystart_i, SEEK_SET)) {
				return FXOR_EX_IOERR; // TODO: CHECK RETURN VALUES!
			}	
			// printf("Seek to: %ld \n",keystart_i);
			for ( byte_check_loop = 0; byte_check_loop < file_len; byte_check_loop++)
			{
				/* read key file & compare for sequential zeros */
				fread(&keybyte_0, 1,1,key_fp);
				if (keybyte_1 == 0 && keybyte_0 == 0) {
					/* We've read two sequential zero's => invalid key (is this safe assumption?) */
					// printf("XXX Index: %ld Keybyte: %x \n",byte_check_loop,keybyte_0);
					result = 1;
				} else {
					// printf("Index: %ld Keybyte: %x \n",byte_check_loop,keybyte_0);
				}
				keybyte_1=keybyte_0;
			}
			safe_fclose(key_fp);
		}	
	}
	
	return result;
}
