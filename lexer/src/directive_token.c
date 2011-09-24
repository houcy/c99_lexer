#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "character_constants.h"


static const char const directives[][8] = {
	"",
	"",
	"",
	"",
	"define",
	"elif",
	"else",
	"endif",
	"error",
	"if",
	"ifdef",
	"ifndef",
	"include",
	"line",
	"pragma",
	"undef"
};


#define NUM_DIRECTIVES     (sizeof(directives) / sizeof(directives[0]))
#define MAX_LEN_DIRECTIVES sizeof(directives[0])


struct directive {

	size_t               len;
    enum directive_token type;
    unsigned int         flag;
};


typedef int (*cmp_f)(const void*, const void*);


struct directive is_directive (char* token) {

	size_t aux;

	if (token++[0] != '#')
		return (struct directive){0, DIR_NONE, 0};

	char* end = token;
	for ( ; ; ) {
		aux = strspn(end, space_character);
		if (!end[aux] || end[aux] == '\n')
            break;
		end += aux;
		end += strcspn(end, delimiter_character);
	}

	size_t size = end - token + 1;

	token += strspn(token, space_character); // TODO: check if this is redundant

	token += aux = strspn(token, digit_character);

	if (aux) {

		struct directive dir = {size, DIR_OUTPUT, 0};

		if (!sscanf(token, "%u", &dir.directive.flags))
            return (struct directive){size, DIR_OTHER, 0};

		dir.directive.flags <<= 4;

		token += strspn(token, space_character);
		token += aux = is_string(token);

        size_t flag = 0;

        while (sscanf(token, " %zu", &flag)) {

            if (flag < 1 || flag > 4)
                break;

			dir.directive.flags |= 1 << (flag - 1);
            flag = 0;
        }

        if (flag < 1 || flag > 4)
	        return (struct directive){size, DIR_OTHER, 0};

        return dir;
	}

	aux = strspn(token, identifier_character);

	if (!aux) {
		if (!token[0] || token[0] == '\n')
			return (struct directive){size, DIR_EMPTY, 0};

		return (struct directive){size, DIR_OTHER, 0};
	}

	char c = token[aux];
	token[aux] = 0;

	const char (*pos)[ MAX_LEN_DIRECTIVES ] = bsearch(token, directives, NUM_DIRECTIVES,
                                                      MAX_LEN_DIRECTIVES, (cmp_f)strcmp);

	token[aux] = c;

	return (struct directive){size, pos ? pos - directives : DIR_OTHER, 0};
}
