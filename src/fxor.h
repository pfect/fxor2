/* fxor.h */
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


#ifndef _FXOR_H
#define _FXOR_H


#include <stdbool.h>


#define FXOR_VERSION "0.2.2"


#define FXOR_COPYING "\
Copyright (c) 2014-2015 Abderraouf Adjal.  All rights reserved.\n\
License: BSD 2-Clause License (Simplified BSD License).\n\
There is NO WARRANTY.\n\
See the file 'COPYING' for details.\n"


#define FXOR_USAGE "\
fxor - a tool to encrypt/decrypt a file using XOR operation to do one-time pad\n\
\n\
Usage:\n\
  fxor KEY_INDEX IN_FILE KEY_FILE\n\
  fxor KEY_INDEX IN_FILE KEY_FILE OUT_FILE [OPTION]\n\
\n\
  Display this usage information:\n\
    fxor --help\n\
\n\
  Display version and copyright information:\n\
    fxor --version\n\
\n\
Description:\n\
  fxor is a tool that you can use to encrypt/decrypt IN_FILE content\n\
  with KEY_FILE content using XOR operation, and output to:\n\
    * The file OUT_FILE.\n\
    * STDOUT if OUT_FILE not defined.\n\
\n\
  fxor can be used as OTP (One-Time Pad) tool.\n\
\n\
  KEY_INDEX Offset for key usage and deletion (decimal number)\n\
  IN_FILE:  Input file name, Witch will processed.\n\
  KEY_FILE: Key file name, Usually random bytes file.\n\
  OUT_FILE: Output file name.\n\
\n\
Options:\n\
  -r    Overwrite (destroy contents) OUT_FILE then output\n\
\n\
  -s    Start output from OUT_FILE beginning and replace bytes,\n\
        Perfect to encrypt/decrypt IN_FILE and output to IN_FILE!\n\
\n\
Report fxor bugs to: <abderraouf.adjal@gmail.com>\n\
Or create an issue on GitHub: <https://github.com/abderraouf-adjal/fxor/issues>\n"


# define INVALID_CMD_USAGE_STR "\
Invalid command line usage.\n\
Try 'fxor --help' to show usage information.\n"


/**
 * fxor()
 * 
 * If out_n == NULL, output to stdout
 * 
 * Return FXOR_EX_OK (0): Done Successfully
 * Return non-zero if:
 *   I/O errors
 *   key_file is empty
 *   (in_file Or key_file) not exist Or not readable
 *   out_file exist And not writable
 */

int fxor(const char *in_n, const char *key_n, const char *out_n, bool write_from_beginning);


#endif /* fxor.h */
