/* fxor_stream_xor.h */
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


#ifndef _FXOR_STREAM_XOR_H
#define _FXOR_STREAM_XOR_H


#include <stdio.h>


#define FXOR_XOR_BUFFSIZE 1024*32
#define HEX_ENCODED_MSG_MAX_BUFSIZE 250


/**
 * is_empty_fp()
 * 
 * Need fp name to show error messages
 * 
 * Return:  0 if NOT Empty; 1 if Empty; -1 if I/O Error
 */

int is_empty_fp(FILE *fp, const char *fp_name);

/**
 * fxor_stream_xor()
 * 
 * files/fp names are just to show them in errors messages
 * 
 * Return FXOR_EX_OK (0): Done Successfully
 * Return non-zero:  Errors (e.g., I/O errors), OR key_s is empty.
 */

int fxor_stream_xor(FILE *in_fp, FILE *key_fp, FILE *out_fp,
	const char *in_n, const char *key_n, const char *out_n, 
	size_t *consumed_key_l,long int keystart_i);


/**
 * check_key_material()
 * 
 * Check key file content for validity (eg. it's not zero's)
 * 
 * Return 0 when ok; 1 when key block contains zero's
 * 
 */

int check_key_block_validity(const char *in_n, const char *key_n,long int keystart_i);

#endif /* fxor_stream_xor.h */
