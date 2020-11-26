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

#define COLORS { \
    "black", "red", "green", "brown", "blue", \
    "magenta", "cyan", "white", NULL \
}

#define DECORATIONS { \
    "bold", "italic", "underline", NULL \
}

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
    void (*pr_escseq)(char *, int);
    void (*pr_html)(char *, int);
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

static int start_with(char *, char *);

/* Check a style string.  This has the form "color[:deco]" (for instance:
 * "blue:b", "blue:bold", "white:i", "white:it", etc.).  It splits color
 * and decoration to set the structure s.  Return true if the
 * style is valid.
*/
bool is_style_valid(char *style, style_t * s) {
    char *color_names[] = COLORS;
    char *deco_names[] = DECORATIONS;
    char *color = NULL, *deco = NULL, **ptr;
    int success = 0;
    bool ret = false;

    sscanf(style, "%m[^: ]:%ms", &color, &deco);

    if (deco) {
        for (ptr = deco_names; *ptr; ptr++)
            if (start_with(*ptr, deco) == 0) {
                s->decoration = *ptr;
                success = 1;
                break;
            }

        /* specified deco isn't the list. */
        if (success == 0) {
            goto out;
        }
    }

    for (ptr = color_names; *ptr; ptr++)
        if (strcmp(*ptr, color) == 0) {
            s->color = *ptr;
            ret = true;
            goto out;
        }

  out:
    xfree(deco);
    xfree(color);

    return ret;
}

static int start_with(char *s1, char *s2) {
    if (strlen(s2) > strlen(s1))
        return -1;

    return strncmp(s1, s2, strlen(s2));
}

int decoration_num(char *deco_str) {

    if (!deco_str)
        return 0;

    if (start_with("underline", deco_str) == 0)
        return DECO_UNDERLINE;
    if (start_with("bold", deco_str) == 0)
        return DECO_BOLD;
    if (start_with("italic", deco_str) == 0)
        return DECO_ITALIC;

    return -1;
}

void cprint(char *col, int output_type, int deco, char *msg) {
    struct colfn_st *p = NULL;

    for (p = &col_fn[0]; p->color != NULL; p++)
        if (strcmp(p->color, col) == 0) {
            switch (output_type) {
            case OUT_ESQSEC:
                p->pr_escseq(msg, deco);
                break;
            case OUT_HTML:
                p->pr_html(msg, deco);
                break;
            default:
                break;
            }
        }
}
