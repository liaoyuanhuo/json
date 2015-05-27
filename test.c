/*
    Copyright (c) 2015
    vurtun <polygone@gmx.net>
    MIT license
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

#define PASS "PASS"
#define FAIL "FAIL"

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


int main(void)
{
    int pass_count = 0;
    int fail_count = 0;
    json_init();
    test_section("str")
    {
        struct json_iter iter;
        struct json_pair pair;
        json_char buffer[8];

        const json_char buf[] = "{\"name\":\"value\"}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "\"value\""));
        test_assert(json_type(&pair.value) == JSON_STRING);
        test_assert(pair.value.children == 0);

        json_deq(&pair.value);
        test_assert(json_cpy(buffer, sizeof buffer, &pair.value) == 5);
        test_assert(!strcmp(&buffer[0], "value"));
    }

    test_section("num")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "\n{\n\"test\":13\n}\n";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"test\""));
        test_assert(!json_cmp(&pair.value, "13"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == 13.0);
    }

    test_section("negnum")
    {
        json_number num;
        struct json_pair pair;
        struct json_iter iter;
        const json_char buf[] = "{\"name\":-1234}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "-1234"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == -1234.0);
    }

    test_section("fracnum")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":1234.5678}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "1234.5678"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("negfracnum")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":-1234.5678}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "-1234.5678"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == -1234.5678);
    }

    test_section("exponent")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":2e+2}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "2e+2"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == 200.0);
    }

    test_section("negexponent")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":-1234e-2}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "-1234e-2"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == -12.34);
    }

    test_section("smallexp")
    {
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":2.567e-4}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "2.567e-4"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num >= 0.0002567 && num <= 0.0002568);
    }

    test_section("utf8")
    {
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":\"$¢€𤪤\"}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "\"$¢€𤪤\""));
        test_assert(json_type(&pair.value) == JSON_STRING);
    }

    test_section("map")
    {
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"name\":\"test\", \"age\":42, \"utf8\":\"äöü\", \"alive\":true}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"name\""));
        test_assert(!json_cmp(&pair.value, "\"test\""));
        test_assert(json_type(&pair.value) == JSON_STRING);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"age\""));
        test_assert(!json_cmp(&pair.value, "42"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"utf8\""));
        test_assert(!json_cmp(&pair.value, "\"äöü\""));
        test_assert(json_type(&pair.value) == JSON_STRING);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"alive\""));
        test_assert(!json_cmp(&pair.value, "true"));
        test_assert(json_type(&pair.value) == JSON_TRUE);
    }

    test_section("array")
    {
        int i = 1;
        json_number num;
        struct json_token tok;
        struct json_iter iter;
        struct json_pair pair;

        const json_char buf[] = "{\"list\":[ 1.0, 2.0, 3.0, 4.0 ]}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"list\""));
        test_assert(!json_cmp(&pair.value, "[ 1.0, 2.0, 3.0, 4.0 ]"));
        test_assert(json_type(&pair.value) == JSON_ARRAY);
        test_assert(pair.value.children == 4);

        iter = json_begin(pair.value.str, pair.value.len);
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
        json_number num;
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"sub\":{\"a\":1234.5678}}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"sub\""));
        test_assert(!json_cmp(&pair.value, "{\"a\":1234.5678}"));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 1);

        iter = json_begin(pair.value.str, pair.value.len);
        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"a\""));
        test_assert(!json_cmp(&pair.value, "1234.5678"));
        test_assert(json_type(&pair.value) == JSON_NUMBER);

        test_assert(json_num(&num, &pair.value) == JSON_NUMBER);
        test_assert(num == 1234.5678);
    }

    test_section("subarray")
    {
        int i = 0;
        struct json_token tok;
        struct json_iter iter;
        struct json_pair pair;
        const json_char check[] = "1234";
        const json_char buf[] = "{\"sub\":{\"a\":[1,2,3,4]}}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"sub\""));
        test_assert(!json_cmp(&pair.value, "{\"a\":[1,2,3,4]}"));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 1);

        iter = json_begin(pair.value.str, pair.value.len);
        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"a\""));
        test_assert(!json_cmp(&pair.value, "[1,2,3,4]"));
        test_assert(json_type(&pair.value) == JSON_ARRAY);
        test_assert(pair.value.children == 4);

        iter = json_begin(pair.value.str, pair.value.len);
        iter = json_read(&tok, &iter);
        while (iter.src) {
            test_assert(tok.str[0] == check[i++]);
            iter = json_read(&tok, &iter);
        }
    }

    test_section("list")
    {
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"sub\":{\"a\":\"b\"}, \"list\":{\"c\":\"d\"}}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"sub\""));
        test_assert(!json_cmp(&pair.value, "{\"a\":\"b\"}"));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 1);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"list\""));
        test_assert(!json_cmp(&pair.value, "{\"c\":\"d\"}"));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 1);
    }

    test_section("table")
    {
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] =
            "{\"sub\":{\"a\": \"b\"}, \"list\":[1,2,3,4], \"a\":true, \"b\": \"0a1b2\"}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"sub\""));
        test_assert(!json_cmp(&pair.value, "{\"a\": \"b\"}"));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 1);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"list\""));
        test_assert(!json_cmp(&pair.value, "[1,2,3,4]"));
        test_assert(json_type(&pair.value) == JSON_ARRAY);
        test_assert(pair.value.children == 4);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"a\""));
        test_assert(!json_cmp(&pair.value, "true"));
        test_assert(json_type(&pair.value) == JSON_TRUE);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"b\""));
        test_assert(!json_cmp(&pair.value, "\"0a1b2\""));
        test_assert(json_type(&pair.value) == JSON_STRING);
    }

    test_section("children")
    {
        struct json_iter iter;
        struct json_pair pair;
        const json_char buf[] = "{\"b\": {\"a\": {\"b\":5}, \"b\":[1,2,3,4],"
            "\"c\":\"test\", \"d\":true, \"e\":false, \"f\":null, \"g\":10},"
            "\"a\": [{\"b\":5}, [1,2,3,4], \"test\", true, false, null, 10]}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"b\""));
        test_assert(json_type(&pair.value) == JSON_OBJECT);
        test_assert(pair.value.children == 7);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"a\""));
        test_assert(json_type(&pair.value) == JSON_ARRAY);
        test_assert(pair.value.children == 7);
    }

    test_section("arrayofarray")
    {
        int i = 1;
        struct json_iter iter;
        struct json_pair pair;
        struct json_token tok;
        const json_char buf[] = "{\"coord\":[[[1,2], [3,4], [5,6]]]}";
        iter = json_begin(buf, sizeof buf);

        iter = json_parse(&pair, &iter);
        test_assert(!iter.err);
        test_assert(!json_cmp(&pair.name, "\"coord\""));
        test_assert(json_type(&pair.value) == JSON_ARRAY);
        test_assert(pair.value.children == 1);

        iter = json_begin(pair.value.str, pair.value.len);
        iter = json_read(&tok, &iter);
        test_assert(json_type(&tok) == JSON_ARRAY);
        test_assert(tok.children == 3);

        iter = json_begin(tok.str, tok.len);
        iter = json_read(&tok, &iter);
        while (!iter.err && iter.src) {
            struct json_iter it;
            test_assert(json_type(&tok) == JSON_ARRAY);
            test_assert(tok.children == 2);
            it = json_begin(tok.str, tok.len);
            it = json_read(&tok, &it);
            while (!it.err && it.src) {
                json_number n;
                test_assert(json_type(&tok) == JSON_NUMBER);
                json_num(&n, &tok);
                test_assert(n == i++);
                it = json_read(&tok, &it);
            }
            iter = json_read(&tok, &iter);
        }
    }

    test_result();
    exit(EXIT_SUCCESS);
}

