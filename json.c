/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include "json.h"

/* remove gcc warning for static init*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"

static const struct json_iter ITER_NULL;
static const struct json_token TOKEN_NULL;

struct json_iter
json_begin(const json_char* str, json_size len)
{
    struct json_iter iter = {0};
    iter.src = str;
    iter.len = len;
    return iter;
}

struct json_iter
json_read(struct json_token *obj, const struct json_iter* prev)
{
    static const void *go_struct[] = {
        [0 ... 255] = &&l_fail,
        ['\t']      = &&l_loop,
        ['\r']      = &&l_loop,
        ['\n']      = &&l_loop,
        [' ']       = &&l_loop,
        ['"']       = &&l_qup,
        [':']       = &&l_loop,
        ['=']       = &&l_loop,
        [',']       = &&l_loop,
        ['[']       = &&l_up,
        [']']       = &&l_down,
        ['{']       = &&l_up,
        ['}']       = &&l_down,
        ['-']       = &&l_bare,
        [48 ... 57] = &&l_bare,
        ['t']       = &&l_bare,
        ['f']       = &&l_bare,
        ['n']       = &&l_bare,
    };
    static const void *go_bare[] = {
        [0 ... 255] = &&l_fail,
        [32 ... 126]= &&l_loop,
        ['\t']      = &&l_unbare,
        ['\r']      = &&l_unbare,
        ['\n']      = &&l_unbare,
        [',']       = &&l_unbare,
        [']']       = &&l_unbare,
        ['}']       = &&l_unbare,
    };
    static const void *go_string[] = {
        [0 ... 31]    = &&l_fail,
        [32 ... 126]  = &&l_loop,
        [127]         = &&l_fail,
        ['\\']        = &&l_esc,
        ['"']         = &&l_qdown,
        [128 ... 191] = &&l_fail,
        [192 ... 223] = &&l_utf8_2,
        [224 ... 239] = &&l_utf8_3,
        [240 ... 247] = &&l_utf8_4,
        [248 ... 255] = &&l_fail,
    };
    static const void *go_utf8[] = {
        [0 ... 127]   = &&l_fail,
        [128 ... 191] = &&l_utf_next,
        [192 ... 255] = &&l_fail
    };
    static const void *go_esc[] = {
        [0 ... 255] = &&l_fail,
        ['"']       = &&l_unesc,
        ['\\']      = &&l_unesc,
        ['/']       = &&l_unesc,
        ['b']       = &&l_unesc,
        ['f']       = &&l_unesc,
        ['n']       = &&l_unesc,
        ['r']       = &&l_unesc,
        ['t']       = &&l_unesc,
        ['u']       = &&l_unesc
    };

    if (!prev || !obj || !prev->src || !prev->len || prev->err) {
        struct json_iter it = {0};
        *obj = TOKEN_NULL;
        it.err = 1;
        return it;
    }

    struct json_iter iter = *prev;
    *obj = TOKEN_NULL;
    iter.err = 0;
    if (!iter.go)
        iter.go = go_struct;

    json_size len = iter.len;
    const json_char *cur;
    int utf8_remain = 0;
    for (cur = iter.src; len; cur++, len--) {
        goto *iter.go[*cur];
        l_loop:;
    }

    if (!iter.depth) {
        iter.src = 0;
        iter.len = 0;
        if (obj->str)
            obj->len = (json_size)((cur-1) - obj->str);
        return iter;
    }

l_fail:
    iter.err = 1;
    return iter;

l_up:
    if (iter.depth++ == 1)
        obj->str = cur;
    goto l_loop;

l_down:
    if (--iter.depth == 1) {
        obj->len = (json_size)(cur - obj->str) + 1;
        goto l_yield;
    }
    goto l_loop;

l_qup:
    iter.go = go_string;
    if (iter.depth == 1)
        obj->str = cur;
    goto l_loop;

l_qdown:
    iter.go = go_struct;
    if (iter.depth == 1) {
        obj->len = (json_size)(cur - obj->str) + 1;
        goto l_yield;
    }
    goto l_loop;

l_esc:
    iter.go = go_esc;
    goto l_loop;

l_unesc:
    iter.go = go_string;
    goto l_loop;

l_bare:
    if (iter.depth == 1)
        obj->str = cur;
    iter.go = go_bare;
    goto l_loop;

l_unbare:
    iter.go = go_struct;
    if (iter.depth == 1) {
        obj->len = (json_size)(cur - obj->str);
        iter.src = cur;
        iter.len = len;
        return iter;
    }
    goto *iter.go[*cur];

l_utf8_2:
    iter.go = go_utf8;
    utf8_remain = 1;
    goto l_loop;

l_utf8_3:
    iter.go = go_utf8;
    utf8_remain = 2;
    goto l_loop;

l_utf8_4:
    iter.go = go_utf8;
    utf8_remain = 3;
    goto l_loop;

l_utf_next:
    if (!--utf8_remain)
        iter.go = go_string;
    goto l_loop;

l_yield:
    if (iter.depth != 1 || !obj->str)
        goto l_loop;
    iter.src = cur + 1;
    iter.len = len - 1;
    return iter;
}

struct json_iter
json_parse(json_pair p, const struct json_iter* it)
{
    struct json_iter next;
    next = json_read(&p[JSON_NAME], it);
    if (next.err)
        return next;
    return json_read(&p[JSON_VALUE], &next);
}

json_char*
json_dup(const struct json_token *tok, void*(*alloc)(json_size))
{
    if (!tok || !alloc)
        return 0;

    json_char *str = alloc(tok->len + 1);
    if (!str)
        return 0;

    unsigned i = 0;
    for (i = 0; i < tok->len; i++)
        str[i] = tok->str[i];
    str[tok->len] = '\0';
    return str;
}

json_size
json_cpy(json_char *dst, json_size max, const struct json_token* tok)
{
    if (!dst || !max || !tok)
        return 0;

    unsigned i = 0;
    const json_size ret = (max < (tok->len + 1)) ? max : tok->len;
    const json_size siz = (max < (tok->len + 1)) ? max-1 : tok->len;
    for (i = 0; i < siz; i++)
        dst[i] = tok->str[i];
    dst[siz] = '\0';
    return ret;
}

int
json_cmp(const struct json_token* tok, const json_char* str)
{
    if (!tok || !str)
        return 1;

    json_size i;
    for (i = 0; (i < tok->len && *str); i++, str++){
        if (tok->str[i] != *str)
            return 1;
    }
    return 0;
}

enum json_typ
json_type(const struct json_token *tok)
{
    if (!tok || !tok->str || !tok->len)
        return JSON_NONE;
    if (tok->str[0] == '{')
        return JSON_OBJECT;
    if (tok->str[0] == '[')
        return JSON_ARRAY;
    if (tok->str[0] == '\"')
        return JSON_STRING;
    if (!json_cmp(tok, (const json_char*)"true"))
        return JSON_TRUE;
    if (!json_cmp(tok, (const json_char*)"false"))
        return JSON_FALSE;
    if (!json_cmp(tok, (const json_char*)"null"))
        return JSON_NULL;
    return JSON_NUMBER;
}

enum json_typ
json_num(json_number *num, const struct json_token *tok)
{
    if (!tok || !num || !tok->str || !tok->len)
        return JSON_NONE;

    int frac = 0;
    double fac = 0.1;
    double res = 0;
    const int sign = (tok->str[0] == '-') ? -1 : 1;
    json_size i = (tok->str[0] == '-') ? 1 : 0;
    while (i < tok->len) {
        if (tok->str[i] == '.') {
            frac = 1;
            i++;
        }
        if (tok->str[i] < '0' || tok->str[i] > '9')
            return JSON_NONE;
        if (frac == 0) {
            res = res * 10 + tok->str[i++] - '0';
        } else {
            res = res + (tok->str[i++] - '0') * fac;
            fac *= 0.1;
        }
    }
    *num = (sign < 0) ? -res : res;
    return JSON_NUMBER;
}

#pragma GCC diagnostic pop
