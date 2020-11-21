/* ccat.c -- ccat utility.
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

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#include "colors.h"

#define VERSION "1.1"
#define MAX_MATCH_PER_LINE 255

#define fatal(...) { \
        fprintf(stderr, __VA_ARGS__); \
        _exit(EXIT_FAILURE); \
    }

typedef struct style_node {
    char *color;
    int decoration;
    char *pattern;
    struct style_node *next;
} style_node_t;

typedef struct match_node {
    style_node_t *style_node;
    regmatch_t match[MAX_MATCH_PER_LINE];
    struct match_node *next;
} match_node_t;

typedef struct {
    char *progname;
    char *filename;
    char *config_filename;
    bool group;
    int output;
    style_node_t *style_node_head;
} opt_t;

opt_t g_opt;

static void parse_options(int, char **);
static match_node_t *process_line(char *, size_t);
static void colorize_line(char *, ssize_t, match_node_t *);
static void free_match_list(match_node_t *);
static void usage(const char *, int);
static void style_node_add(style_node_t **, style_t *, char *);
static void show_version(void);
static void load_config(char *);
static void match_node_add(match_node_t **, style_node_t *, size_t, regmatch_t *);


static void show_version(void) {
    fprintf(stdout, "%s\n", VERSION);

    exit(EXIT_SUCCESS);
}

static void usage(const char *progname, const int exit_status) {
    fprintf(stderr,
            "usage: %s [options] [color1[:decoration1] pattern1] [[color2[:decoration2] pattern2] ...] [filename]\n\n"
            "  -c|--config <cfg>\tUse configuration file\n"
            "  -g|--group\t\tOnly colorize sub matches (use parentheses)\n"
            "  -h|--help\t\tDisplay this help\n"
            "  -t|--html\t\tSet output as HTML\n"
            "  -v|--version\t\tDisplay current version\n\n"
            "Possible colors are: black red green brown blue magenta cyan white\n"
            "Possible decorations are: 'b' (bold) 'i' (italic) or 'u' (underline)"
            "\n", progname);

    exit(exit_status);
}

static void style_node_add(style_node_t ** head, style_t *style,
                           char *pattern) {
    style_node_t *p = NULL;
    style_node_t *new = NULL;

    new = (style_node_t *) calloc(1, sizeof(style_node_t));

    if (!new)
        fatal("Error: style_node_add(): calloc() failed.\n");

    new->color = style->color;
    new->decoration = decoration_num(style->decoration);
    new->pattern = pattern;
    new->next = NULL;

    if (!*head)
        *head = new;
    else {
        p = *head;
        while (p->next)
            p = p->next;
        p->next = new;
    }
}

static void match_node_add(match_node_t ** head, style_node_t *style_node,
                           size_t nmatch, regmatch_t * match) {
    match_node_t *p = NULL;
    match_node_t *new = NULL;

    new = (match_node_t *) calloc(1, sizeof(match_node_t));

    if (!new)
        fatal("Error: match_node_add(): calloc() failed.\n");

    new->style_node = style_node;
    memcpy(new->match, match, nmatch * sizeof(regmatch_t));

    new->next = NULL;

    if (!*head)
        *head = new;
    else {
        p = *head;
        while (p->next)
            p = p->next;
        p->next = new;
    }
}

static void free_match_list(match_node_t * head) {
    match_node_t *p = head;
    match_node_t *old;

    while (p) {
        old = p;
        p = p->next;
        xfree(old);
    }
}

static void parse_options(int argc, char **argv) {
    int c, i;
    style_t style;

    const struct option options[] = {
        { "config", required_argument, NULL, 'c' },
        { "help", no_argument, NULL, 'h' },
        { "html", no_argument, NULL, 't' },
        { "version", no_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 },
    };

    /* initializes g_opt variable with default values. */
    g_opt.progname = argv[0];
    g_opt.filename = NULL;
    g_opt.config_filename = NULL;
    g_opt.style_node_head = NULL;
    g_opt.group = false;
    g_opt.output = OUT_ESQSEC;

    while ((c = getopt_long(argc, argv, "bc:ghtv", options, NULL)) != EOF) {

        switch (c) {
        case 'c':
            g_opt.config_filename = optarg;
            break;
        case 'g':
            g_opt.group = true;
            break;
        case 'h':
            usage(g_opt.progname, EXIT_SUCCESS);
            break;
        case 't':
            g_opt.output = OUT_HTML;
            break;
        case 'v':
            show_version();
            break;
        default:
            usage(g_opt.progname, EXIT_FAILURE);
            break;
        }
    }

    i = optind;

    while (i < argc && argv[i]) {
        if (argv[i + 1]) {
            style.color = NULL;
            style.decoration = NULL;
            if(is_style_valid(argv[i], &style))
                style_node_add(&g_opt.style_node_head, &style, argv[i + 1]);
            i += 2;
        } else
            break;
    }

    /* if configuration file is specified, we overwrites values. */
    if (g_opt.config_filename)
        load_config(g_opt.config_filename);

    /* if filename is set by the configuration file, it's overwritten by
     * the one specified in argument (if any). */
    if (argv[i])
        g_opt.filename = argv[i];
}

static void colorize_line(char *line, ssize_t len,
                          match_node_t * match_node_head) {
    match_node_t *cur_match = NULL;
    char *ptr = line;
    size_t offset = 0, match_len;
    bool already_matched;
    int i, start;
    char *tmpbuf = NULL;

    while (*ptr != '\0') {

        already_matched = false;
        cur_match = match_node_head;

        /* is this offset is in the matches list? */
        while (cur_match) {

            start = (g_opt.group) ? 1 : 0;

            for (i = start; i < MAX_MATCH_PER_LINE; i++) {
                if (cur_match->match[i].rm_so == offset) {

                    /* yes it matches! */
                    match_len =
                        cur_match->match[i].rm_eo -
                        cur_match->match[i].rm_so;

                    if (match_len == 0)
                        continue;

                    tmpbuf = calloc(1, match_len + 1);

                    if (!tmpbuf)
                        fatal("Error: calloc() failed.\n");

                    strncpy(tmpbuf, ptr, match_len);

                    /* print with color! */
                    cprint(cur_match->style_node->color, g_opt.output,
                           cur_match->style_node->decoration, tmpbuf);
                    xfree(tmpbuf);

                    offset += match_len;
                    ptr += match_len;
                    already_matched = true;
                    break;
                }
            }

            if (already_matched)
                break;

            cur_match = cur_match->next;
        }

        if (already_matched)
            continue;

        fwrite(ptr, 1, 1, stdout);

        offset += 1;
        ptr += 1;
    }
}

static match_node_t *process_line(char *line, size_t len) {
    style_node_t *p = g_opt.style_node_head;
    match_node_t *match_node_head = NULL;
    regmatch_t match[MAX_MATCH_PER_LINE], pm;
    regex_t preg;
    int eflags = 0, match_num;

    while (p) {

        if (regcomp(&preg, p->pattern, REG_NEWLINE | REG_EXTENDED) == 0) {

            if (g_opt.group) {

                if (regexec(&preg, line, MAX_MATCH_PER_LINE, match, eflags)
                    == 0)
                    match_node_add(&match_node_head, p, MAX_MATCH_PER_LINE,
                                   match);

            } else {

                match_num = 0;
                regoff_t last_match = 0;

                /* we use a regmatch_t struct to store each occurence of our pattern. */
                memset(match, -1, sizeof(regmatch_t) * MAX_MATCH_PER_LINE);

                while (regexec(&preg, line + last_match, 1, &pm, eflags) ==
                       0) {
                    match[match_num].rm_so = pm.rm_so + last_match;
                    match[match_num].rm_eo = pm.rm_eo + last_match;
                    match_num++;
                    last_match += pm.rm_so + 1;
                    eflags |= REG_NOTBOL;
                }

                if (match_num > 0)
                    match_node_add(&match_node_head, p, MAX_MATCH_PER_LINE,
                                   match);
            }

            regfree(&preg);
        }

        p = p->next;
    }

    return match_node_head;
}

static void load_config(char *config_file) {
    char line[1024];
    char *key = NULL;
    char *value = NULL;
    FILE *fp = NULL;
    char *k = NULL;
    char *v = NULL;
    style_t style;

    fp = fopen(config_file, "r");

    if (!fp)
        fatal("Error: can't fopen() configuration file.\n");

    while (fgets(line, sizeof(line) - 1, fp)) {

        /* ignore comment. */
        if (line[0] == '#')
            continue;

        sscanf(line, "%ms %m[^\n]\n", &key, &value);

        if (strcmp("filename", key) == 0) {

            g_opt.filename = strdup(value);

        } else if (strcmp("flags", key) == 0) {

            if (strchr(value, 'g'))
                g_opt.group = true;
            if (strchr(value, 'h'))
                g_opt.output = OUT_HTML;

        } else {
            k = strdup(key);
            v = strdup(value);
            style.color = NULL;
            style.decoration = NULL;
            if(is_style_valid(k, &style))
                style_node_add(&g_opt.style_node_head, &style, v);
        }

        xfree(key);
        xfree(value);
    }

    fclose(fp);
}

int main(int argc, char **argv) {
    FILE *input_file = NULL;
    char *line = NULL;
    size_t len = 0;
    match_node_t *matches = NULL;

    parse_options(argc, argv);

    if (!g_opt.filename)
        input_file = stdin;
    else {
        input_file = fopen(g_opt.filename, "r");

        if (!input_file)
            fatal("Error: failed to open %s\n", g_opt.filename);
    }

    if (g_opt.output == OUT_HTML)
        printf("<pre>\n");

    while (getline(&line, &len, input_file) != EOF) {
        matches = process_line(line, len);
        colorize_line(line, len, matches);
        free_match_list(matches);
        xfree(line);
        len = 0;
    }

    if (g_opt.output == OUT_HTML)
        printf("</pre>\n");

    fclose(input_file);

    return EXIT_SUCCESS;
}
