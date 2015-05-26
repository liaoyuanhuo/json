/*
    Copyright (c) 2015
    vurtun <polygone@gmx.net>
    MIT licence
*/
#include "json.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

static const struct json_iter ITER_NULL;
static const struct json_token TOKEN_NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"

void json_init(void){};
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
        [':']       = &&l_sep,
        ['=']       = &&l_sep,
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
    json_int utf8_remain = 0;
    for (cur = iter.src; len; cur++, len--) {
        goto *iter.go[*((unsigned char*)cur)];
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

l_sep:
    if (iter.depth == 2)
        obj->children--;
    goto l_loop;

l_up:
    if (iter.depth == 2)
        obj->children++;
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
    if (iter.depth == 2)
        obj->children++;
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
    if (iter.depth == 2)
        obj->children++;
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
    goto *iter.go[*(unsigned char*)cur];

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

static json_number
ipow(int base, unsigned exp)
{
    long res = 1;
    while (exp) {
        if (exp & 1)
            res *= base;
        exp >>= 1;
        base *= base;
    }
    return (json_number)res;
}

static json_number
stoi(struct json_token *tok)
{
    json_number n = 0;
    json_size i = 0;
    json_size off;
    json_size neg;
    if (!tok->str || !tok->len)
        return 0;

    off = (tok->str[0] == '-' || tok->str[0] == '+') ? 1 : 0;
    neg = (tok->str[0] == '-') ? 1 : 0;
    for (i = off; i < tok->len; i++) {
        if ((tok->str[i] >= '0') && (tok->str[i] <= '9'))
            n = (n * 10) + tok->str[i]  - '0';
    }
    return (neg) ? -n : n;
}

static json_number
stof(struct json_token *tok)
{
    json_number n = 0;
    json_number f = 0.1;
    json_size i = 0;
    if (!tok->str || !tok->len) return 0;
    for (i = 0; i < tok->len; i++) {
        if ((tok->str[i] >= '0') && (tok->str[i] <= '9')) {
            n = n + (tok->str[i] - '0') * f;
            f *= 0.1;
        }
    }
    return n;
}


int
json_num(json_number *num, const struct json_token *tok)
{
    static const void **go_num[] = {
        [0 ... 255] = &&l_fail,
        [48 ... 57] = &&l_loop,
        ['-'] = &&l_loop,
        ['+'] = &&l_loop,
        ['.'] = &&l_flt,
        ['e'] = &&l_exp,
        ['E'] = &&l_exp,
        [' '] = &&l_break,
        ['\n'] = &&l_break,
        ['\t'] = &&l_break,
        ['\r'] = &&l_break,
    };
    if (!num || !tok || !tok->str || !tok->len)
        return JSON_NONE;

    enum {INT, FLT, EXP, TOKS};
    struct json_token nums[TOKS] = {{0}};
    struct json_token *write = &nums[INT];
    write->str = tok->str;

    json_size len = tok->len;
    const json_char *cur;
    for (cur = tok->str; len; cur++, len--) {
        goto *go_num[*(unsigned char*)cur];
        l_loop:;
    }
    write->len = (json_size)(cur - write->str);

    const json_number i = stoi(&nums[INT]);
    const json_number f = stof(&nums[FLT]);
    const json_number e = stoi(&nums[EXP]);
    json_number p = ipow(10, (unsigned)((e < 0) ? -e : e));
    if (e < 0)
        p = (1 / p);
    *num = (i + ((i < 0) ? -f : f)) * p;
    return JSON_NUMBER;

l_flt:
    if (nums[FLT].str)
        return JSON_NONE;
    if (nums[EXP].str)
        return JSON_NONE;
    write->len = (json_size)(cur - write->str);
    write = &nums[FLT];
    write->str = cur + 1;
    goto l_loop;

l_exp:
    if (nums[EXP].str)
        return JSON_NONE;
    write->len = (json_size)(cur - write->str);
    write = &nums[EXP];
    write->str = cur + 1;
    goto l_loop;

l_break:
    len = 1;
    goto l_loop;
l_fail:
    return JSON_NONE;
}

#pragma GCC diagnostic pop

struct json_iter
json_begin(const json_char *str, json_size len)
{
    struct json_iter iter = ITER_NULL;
    iter.src = str;
    iter.len = len;
    return iter;
}

struct json_iter
json_parse(struct json_pair *p, const struct json_iter* it)
{
    struct json_iter next;
    next = json_read(&p->name, it);
    if (next.err)
        return next;
    return json_read(&p->value, &next);
}

json_size
json_cpy(json_char *dst, json_size max, const struct json_token* tok)
{
    unsigned i = 0;
    json_size ret;
    json_size siz;
    if (!dst || !max || !tok)
        return 0;

    ret = (max < (tok->len + 1)) ? max : tok->len;
    siz = (max < (tok->len + 1)) ? max-1 : tok->len;
    for (i = 0; i < siz; i++)
        dst[i] = tok->str[i];
    dst[siz] = '\0';
    return ret;
}

int
json_cmp(const struct json_token* tok, const json_char* str)
{
    json_size i;
    if (!tok || !str) return 1;
    for (i = 0; (i < tok->len && *str); i++, str++){
        if (tok->str[i] != *str)
            return 1;
    }
    return 0;
}

int
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
    if (tok->str[0] == 't')
        return JSON_TRUE;
    if (tok->str[0] == 'f')
        return JSON_FALSE;
    if (tok->str[0] == 'n')
        return JSON_NULL;
    return JSON_NUMBER;
}

