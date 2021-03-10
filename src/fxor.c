/* fxor.c */
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
#include <unistd.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>

#include "fxor_stream_xor.h"
#include "fxor_exits.h"

#define KEY_INDEX_FILENAME_LEN	100

void safe_fclose(FILE *fp);
long int get_key_index(char *filename);
void set_key_index(char *filename, long int index);

/**
 * fxor()
 * 
 * If out_n is NULL, output to stdout
 * 
 * Return FXOR_EX_OK (0): Done Successfully
 * Return non-zero if:
 *   I/O errors
 *   key_fpile is empty
 *   (in_fpile OR key_fpile) NOT exist OR NOT readable
 *   out_fpile exist AND NOT writable
 */

int fxor(const char *in_n, const char *key_n, const char *out_n, bool write_from_beginning)
{
	FILE *in_fp, *key_fp, *out_fp;
	int r;
	long int key_start_index=0; // THIS FROM STORED VALUE and this is used to seek key file pointer
	size_t usedkey_len=0;
	size_t *usedkey;
	char overwrite = 0x00;
	size_t overwrite_loop;
	char keyindex_filename[KEY_INDEX_FILENAME_LEN];
	usedkey=&usedkey_len;
	
	
	
	/* debug of stored index */
	memset(keyindex_filename,0,KEY_INDEX_FILENAME_LEN); 
	sprintf(keyindex_filename,"%s.%s",key_n,"index");
	printf("Checking %s presense \n",keyindex_filename);
	
	if (! access(keyindex_filename, R_OK) ) {
		key_start_index = get_key_index(keyindex_filename);
		printf("Got key index from file: %ld \n", key_start_index);
	} else {
		/* Get key position from command line */
		key_start_index = 0;
	}
	
	if (access(in_n, R_OK) || access(key_n, R_OK) || (out_n && !access(out_n, F_OK) && access(out_n, W_OK))) {
		if (access(in_n, R_OK)) {
			/* in_n NOT exist OR NOT readable */
			warn("%s", in_n);
		}
		if (access(key_n, R_OK)) {
			/* key_n NOT exist OR NOT readable */
			warn("%s", key_n);
		}
		if (out_n && (!access(out_n, F_OK) && access(out_n, W_OK))) {
			/* out_n exist AND NOT writable */
			warn("%s", out_n);
		}		
		return FXOR_EX_NOINPUT;
	}
	
	in_fp  = fopen(in_n, "rb");
	key_fp = fopen(key_n, "rb");
	
	if (!in_fp || !key_fp) {
		if (!in_fp) {
			warn("%s", in_n);
		}
		if (!key_fp) {
			warn("%s", key_n);
		}
		
		r = FXOR_EX_IOERR;
	}
	else {
		if (out_n) {
			/* output to out_fp */			
			out_fp = write_from_beginning ? fopen(out_n, "rb+") : fopen(out_n, "wb");
			if (out_fp) {
				r = fxor_stream_xor(in_fp, key_fp, out_fp, in_n, key_n, out_n,usedkey,key_start_index);
			}
			else {
				warn("%s", out_n);
				r = FXOR_EX_IOERR;
			}
			
			safe_fclose(out_fp);
		}
		else {
			/* output to stdout */
			r = fxor_stream_xor(in_fp, key_fp, stdout, in_n, key_n, "(stdout)",usedkey,key_start_index);
		}
	}
	safe_fclose(in_fp);
	safe_fclose(key_fp);
	
	/* Key erase from 'key_start_index' to 'usedkey_len'
	 * write used key bytes (key_start_index + usedkey_len) to index file */
	
	if ( !access(key_n, R_OK) ) {
		memset(keyindex_filename,0,KEY_INDEX_FILENAME_LEN); 
		sprintf(keyindex_filename,"%s.%s",key_n,"index");
		set_key_index(keyindex_filename, key_start_index + (long int)usedkey_len ); 
		printf("Key index (start: %ld len: %ld) stored: %s\n",key_start_index,usedkey_len,keyindex_filename);
		
		key_fp = fopen(key_n, "rb+");
		if (fseek(key_fp, key_start_index, SEEK_SET)) {
			return FXOR_EX_IOERR;
		}	
		for ( overwrite_loop = 0; overwrite_loop < usedkey_len; overwrite_loop++)
		{
			fwrite(&overwrite, sizeof(char), 1, key_fp);	
		}
		safe_fclose(key_fp);
	}	
	
	return r;
}


void safe_fclose(FILE *fp)
{
	if (fp && fp != stdout && fp != stderr) {
		if (fclose(fp) == EOF) {
			perror("fclose()");
		}
		fp = NULL;
	}
}

long int get_key_index(char *filename) {
	long int index=0;
	FILE *keyindex_file;
	keyindex_file = fopen(filename, "rb");
	fread(&index, sizeof(long int),1,keyindex_file);
	safe_fclose(keyindex_file);
	return index;
}

void set_key_index(char *filename, long int index) {
	FILE *keyindex_file;
	keyindex_file = fopen(filename, "wb");
	fwrite(&index, sizeof(long int), 1, keyindex_file);
	safe_fclose(keyindex_file);
}


