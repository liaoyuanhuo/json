# JSON
A one-pass JSON parser with no memory overhead, copying or
allocating. The implementation is very simple but should parse any valid
json input, but trades validation for efficency.
One pass of the parser only reads up to the first depth of value
and pair(key,value), but deeper levels can be reached with the parsed output.

## Features
- written in C
- small (~350 loc)
- no memory allocation
- UTF-8 support
- minimal validation

## Prerequisites
To build and use the JSON parser you only need to drop the two
source files into your project and a compiler that can compile
computed gotos (GCC, clang).

## Usage
First the json parser iterator needs to be created by using
the `json_begin` function which takes a UTF-8 string + length in bytes
and returns the newly created iterator.

```c
struct json_iter iter;
iter = json_begin(json, len);
```
To further parse the JSON file there are two functions.
First the `json_read` function which reads in the next token
and returns the next iterator. `json_read` main purpose is reading
array values and providing the base for the next function.

```c
struct json_token tok;
iter = json_read(&tok, &iter);
```
The second parse function `json_parse` is for name + value pairs
and uses the `json_read` function internally to read in two tokens.
`json_parse` takes an iterator and a `json_pair` as arguments and
returns the next iterator.

```c
struct json_pair pair;
iter = json_parse(&pair, &iter);
```
To access the string in the token or token pair there are two utility functions.
The first function `json_dup` takes a read token and a allocator callback and
returns a newly allocated string which contains the parsed string.
```c
json_char *str;
str = json_dup(&tok, malloc);
```
The second function `json_cpy` takes a read token and a buffer with a maximum
size to copy into and returns the number of bytes that have been copied.
```c
json_char buf[BUF_SIZ]
int size = json_cpy(buf, BUF_SIZ, &tok);
```
The last four utitlity function are `json_cmp`, `json_type`, `json_num` and `json_deq`
`json_cmp` compares a token with a given string pointer and returns if these two are equal.
```c
const json_char buf[] = "token";
int eq = !json_cmp(&tok, buf);
```
`json_type` returns the type of a given token, but as well as the main parsing
function does not perform extensive validation of content.
```c
const enum json_typ t = json_type(&tok);
```
`json_num` converts a read token into a number that
is provided as an argument and returns JSON_NUMBER on success.
```c
json_number num = 0;
int res = json_num(&num, &tok);
```
Last but not least `json_deq` removes the quotes in string tokens.
Please notice that after the removal you CAN NOT detect the type of the token
anymore.
```c
struct json_token tok;
json_deq(&tok);
```
## References
- [computed gotos](http://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables): Eli Bendersky's article describing the gcc extension computed gotos.
- [json](www.json.org): Homepage of the JSON standard describing the parser grammar.
- [js0n](https://github.com/quartzjer/js0n): The project where I stole parts of the parser from.
- [vec](https://github.com/rxi/vec): The project where I stole the nice unit-test macros from.

# License
    (The MIT License)
