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
#define utf (const json_char*)

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
        const json_char buf[] = "{\"name\":\"value\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"\"value\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
        test_assert(pair[JSON_VALUE].sub == 0);
        test_assert(pair[JSON_VALUE].children == 0);

        json_char buffer[8];
        json_deq(&pair[JSON_VALUE]);
        test_assert(json_cpy(buffer, sizeof buffer, &pair[JSON_VALUE]) == 5);
        test_assert(!strcmp((char*)&buffer[0], "value"));

        json_char *str;
        json_deq(&pair[JSON_VALUE]);
        str = json_dup(&pair[JSON_VALUE], malloc);
        test_assert(!strcmp((char*)str, "value"));
    }

    test_section("num")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":1234}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"1234"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.0);
    }

    test_section("negnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":-1234}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"-1234"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -1234.0);
    }

    test_section("fracnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":1234.5678}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("negfracnum")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":-1234.5678}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"-1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -1234.5678);
    }

    test_section("exponent")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":2e+2}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"2e+2"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 200.0);
    }

    test_section("negexponent")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":-1234e-2}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"-1234e-2"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == -12.34);
    }

    test_section("smallexp")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":2.567e-4}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"2.567e-4"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num >= 0.0002567 && num <= 0.0002568);
    }

    test_section("utf8")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":\"$¢€𤪤\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"\"$¢€𤪤\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
    }

    test_section("map")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"name\":\"test\", \"age\":42, \"utf8\":\"äöü\", \"alive\":true}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"name\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"\"test\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"age\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"42"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"utf8\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"\"äöü\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"alive\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"true"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_TRUE);
    }

    test_section("array")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"list\":[ 1.0, 2.0, 3.0, 4.0 ]}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"[ 1.0, 2.0, 3.0, 4.0 ]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);
        test_assert(pair[JSON_VALUE].children == 4);

        int i = 1;
        json_number num;
        struct json_token tok;
        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_read(&tok, &iter);
        while (iter.src) {
            test_assert(json_num(&num, &tok) == JSON_NUMBER);
            test_assert((json_number)i == num);
            iter = json_read(&tok, &iter);
            i++;
        }
    }

    test_section("sub")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\":{\"a\":1234.5678}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"{\"a\":1234.5678}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 1);
        test_assert(pair[JSON_VALUE].children == 1);

        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"1234.5678"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_NUMBER);

        json_number num;
        test_assert(json_num(&num, &pair[JSON_VALUE]) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("subarray")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\":{\"a\":[1,2,3,4]}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"{\"a\":[1,2,3,4]}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 5);
        test_assert(pair[JSON_VALUE].children == 1);

        iter = json_begin(pair[JSON_VALUE].str, pair[JSON_VALUE].len);
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"[1,2,3,4]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);
        test_assert(pair[JSON_VALUE].children == 4);

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
        const json_char buf[] = "{\"sub\":{\"a\":\"b\"}, \"list\":{\"c\":\"d\"}}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"{\"a\":\"b\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 1);
        test_assert(pair[JSON_VALUE].children == 1);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"{\"c\":\"d\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 1);
        test_assert(pair[JSON_VALUE].children == 1);
    }

    test_section("table")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"sub\":{\"a\": \"b\"}, \"list\":[1,2,3,4], \"a\":true, \"b\": \"0a1b2\"}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"sub\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"{\"a\": \"b\"}"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 1);
        test_assert(pair[JSON_VALUE].children == 1);


        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"list\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"[1,2,3,4]"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 4);
        test_assert(pair[JSON_VALUE].children == 4);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"a\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"true"));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_TRUE);

        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"b\""));
        test_assert(!json_cmp(&pair[JSON_VALUE], utf"\"0a1b2\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_STRING);
    }

    test_section("children")
    {
        struct json_iter iter;
        const json_char buf[] = "{\"b\": {\"a\": {\"b\":5}, \"b\":[1,2,3,4],"
            "\"c\":\"test\", \"d\":true, \"e\":false, \"f\":null, \"g\":10},"
            "\"a\": [{\"b\":5}, [1,2,3,4], \"test\", true, false, null, 10]}";
        iter = json_begin(buf, sizeof buf);

        json_pair pair;
        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"b\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_OBJECT);
        test_assert(pair[JSON_VALUE].sub == 12);
        test_assert(pair[JSON_VALUE].children == 7);


        iter = json_parse(pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair[JSON_NAME], utf"\"a\""));
        test_assert(json_type(&pair[JSON_VALUE]) == JSON_ARRAY);
        test_assert(pair[JSON_VALUE].sub == 12);
        test_assert(pair[JSON_VALUE].children == 7);
        printf("children: %lu", pair[1].children);
    }

    test_result();
    exit(EXIT_SUCCESS);
}

