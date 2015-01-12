/*
    Copyright (c) 2015
    vurtun <polygone@gmx.net>
    MIT license
*/
#ifndef JSON_H_
#define JSON_H_

typedef int json_int;
typedef unsigned char json_char;
typedef unsigned long json_size;
typedef double json_number;

enum json_typ {
    JSON_NONE,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_NUMBER,
    JSON_STRING,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
};

struct json_token {
    const json_char *str;
    json_size len;
    json_size children;
};

struct json_pair {
    struct json_token name;
    struct json_token value;
};

struct json_iter {
    json_int depth;
    json_int err;
    const void **go;
    const json_char *src;
    json_size len;
};

struct json_iter json_begin(const json_char*, json_size);
struct json_iter json_read(struct json_token*, const struct json_iter*);
struct json_iter json_parse(struct json_pair*, const struct json_iter*);
json_char *json_dup(const struct json_token*, void*(*alloc)(json_size));
json_size json_cpy(json_char*, json_size, const struct json_token*);
json_int json_cmp(const struct json_token*, const json_char*);
json_int json_type(const struct json_token*);
json_int json_num(json_number *, const struct json_token*);
void json_deq(struct json_token*);

#endif
