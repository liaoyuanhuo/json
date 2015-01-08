/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#ifndef JSON_H_
#define JSON_H_

#define JSON_NAME 0
#define JSON_VALUE 1

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

typedef struct json_token {
    const json_char *str;
    json_size len;
    json_size sub;
    json_size children;
} json_pair[2];

struct json_iter {
    json_int depth;
    json_int err;
    const void **go;
    const json_char *src;
    json_size len;
};

struct json_iter json_begin(const json_char*, json_size);
struct json_iter json_read(struct json_token*, const struct json_iter*);
struct json_iter json_parse(json_pair, const struct json_iter*);
json_char *json_dup(const struct json_token*, void*(*alloc)(json_size));
json_size json_cpy(json_char*, json_size, const struct json_token*);
int json_cmp(const struct json_token*, const json_char*);
enum json_typ json_type(const struct json_token*);
enum json_typ json_num(json_number *, const struct json_token*);
void json_deq(struct json_token*);

#endif
