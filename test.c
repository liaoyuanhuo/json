/*
    Copyright (c) 2014
    vurtun <polygone@gmx.net>
    MIT license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

#define PASS "\e[32mPASS\e[39m"
#define FAIL "\e[31mFAIL\e[39m"

#define test_section(desc) \
    do { \
        printf("--------------- {%s} ---------------\n", desc);\
    } while (0);

#define test_assert(cond) \
    do { \
        int pass = cond; \
        printf("[%s] %s:%d: ", pass ? PASS : FAIL, __FILE__, __LINE__);\
        printf((strlen(#cond) > 60 ? "%.47s...\n" : "%s\n"), #cond);\
        if (pass) pass_count++; else fail_count++; \
    } while (0)

#define test_result()\
    do { \
        printf("======================================================\n"); \
        printf("== Result:  %3d Total   %3d Passed      %3d Failed  ==\n", \
                pass_count  + fail_count, pass_count, fail_count); \
        printf("======================================================\n"); \
    } while (0)

static int pass_count;
static int fail_count;

int main(void)
{
    test_section("str")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"value\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"\"value\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
        test_assert(pair[JSON_VALUE].sub == 0);

        json_char buffer[8];
        test_assert(json_cpy(buffer, sizeof buffer, &pair[JSON_VALUE]) == 7);
        test_assert(!strcmp((char*)&buffer[0], "\"value\""));

        json_char *str;
        str = json_dup(&pair[JSON_VALUE], malloc);
        test_assert(!strcmp((char*)str, "\"value\""));
    }

    test_section("num")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=1234}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"1234"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.0);
    }

    test_section("negnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=-1234}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"-1234"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -1234.0);
    }

    test_section("fracnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=1234.5678}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("negfracnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=-1234.5678}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"-1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -1234.5678);
    }

    test_section("exponent")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=2e2}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"2e2"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 200.0);
    }

    test_section("negexponent")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=-1234e-2}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"-1234e-2"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -12.34);
    }

    test_section("smallexp")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=2.567e-4}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"2.567e-4"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num >= 0.0002567 && num <= 0.0002568);
    }

    test_section("utf8")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"$¢€𤪤\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"\"$¢€𤪤\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
    }

    test_section("map")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\"=\"test\", \"age\"=42, \"utf8\"=\"äöü\", \"alive\"=true}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"\"test\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"age\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"42"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"utf8\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"\"äöü\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"alive\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"true"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_TRUE);
    }

    test_section("array")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"list\"=[1,2,3,4]}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"[1,2,3,4]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);

        int i = 0;
        struct json_token tok;
        const json_char check[] = "1234";
        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_read(&tok, &iter);
        while (iter.src) {
            test_assert(tok.str[0] == check[i++]);
            iter = json_read(&tok, &iter);
        }
    }

    test_section("sub")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=1234.5678}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"{\"a\"=1234.5678}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 2);

        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("subarray")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=[1,2,3,4]}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"{\"a\"=[1,2,3,4]}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 6);

        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"[1,2,3,4]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);

        int i = 0;
        struct json_token tok;
        const json_char check[] = "1234";
        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_read(&tok, &iter);
        while (iter.src) {
            test_assert(tok.str[0] == check[i++]);
            iter = json_read(&tok, &iter);
        }
    }

    test_section("list")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"=\"b\"}, \"list\"={\"c\"=\"d\"}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"{\"a\"=\"b\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 2);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"{\"c\"=\"d\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 2);
    }

    test_section("table")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\"={\"a\"= \"b\"}, \"list\"=[1,2,3,4], \"a\"=true, \"b\"= \"0a1b2\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"{\"a\"= \"b\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 2);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"[1,2,3,4]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"true"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_TRUE);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], (const json_char*)"\"b\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], (const json_char*)"\"0a1b2\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
    }
    test_result();
    exit(EXIT_SUCCESS);
}

