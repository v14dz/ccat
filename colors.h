/* colors.h -- This file is part of the ccat projects [1].
 *
 * MIT License
 *
 * Copyright (c) 2020 vladz@devzero.fr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef _COLORS_H
#define _COLORS_H

#include <stdbool.h>

/* output format. */
#define OUT_ESQSEC      1
#define OUT_HTML        2

/* From console_codes(4). */
#define black        30
#define red          31
#define green        32
#define brown        33
#define blue         34
#define magenta      35
#define cyan         36
#define white        37

#define cprint_escseq(color, bold, fmt, msg) { \
        if (bold) \
            printf("\e[1;%dm" fmt "\e[0m", color, msg); \
        else \
            printf("\e[%dm" fmt "\e[0m", color, msg); \
    }

#define cprint_html(color, bold, fmt, msg) { \
        if (bold) \
            printf("<font color=\"%s\"><b>" fmt "</b></font>", color, msg); \
        else \
            printf("<font color=\"%s\">" fmt "</font>", color, msg); \
    }

#define color_func(col) \
    void col##_pr_escseq(char *str, bool bold) { \
        cprint_escseq(col, bold, "%s", str); \
    } \
    void col##_pr_html(char *str, bool bold) { \
        cprint_html(#col, bold, "%s", str); \
    }

#define colfn_entry(col) { #col , col##_pr_escseq, col##_pr_html }

extern bool is_color_valid(char *);
extern void cprint(char *, int, bool, char *);

#endif
