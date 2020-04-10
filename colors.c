/* colors.c -- This file is part of the ccat utility.
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

#include <stdio.h>
#include <string.h>

#include "colors.h"

color_func(black);
color_func(red);
color_func(green);
color_func(brown);
color_func(blue);
color_func(magenta);
color_func(cyan);
color_func(white);

struct colfn_st {
    char *color;
    void (*pr_escseq)(char *, bool bold);
    void (*pr_html)(char *, bool bold);
} col_fn[] = {
    colfn_entry(black),
    colfn_entry(red),
    colfn_entry(green),
    colfn_entry(brown),
    colfn_entry(blue),
    colfn_entry(magenta),
    colfn_entry(cyan),
    colfn_entry(white),
    { NULL, NULL },
};

bool is_color_valid(char *col) {
    char *cur_color[] = { "black", "red", "green", "brown", "blue",
                          "magenta", "cyan", "white", NULL};
    char **ptr;

    for(ptr = cur_color; *ptr; ptr++)
        if (strcmp(*ptr, col) == 0)
            return true;

    return false;
}

void cprint(char *col, int style, bool bold, char *msg) {
    struct colfn_st *p = NULL;

    for (p = &col_fn[0]; p->color != NULL; p++)
        if (strcmp(p->color, col) == 0) {
            switch (style) {
            case OUT_ESQSEC:
                p->pr_escseq(msg, bold);
                break;
            case OUT_HTML:
                p->pr_html(msg, bold);
                break;
            default:
                break;
            }
        }
}
