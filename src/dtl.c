#include <stdio.h>
#include <unistd.h>

#include "fs-ast.h"
#include "fs-ebpf.h"
#include "fs-parse.h"
#include "fs-lex.h"

struct fs_node *fs_load(FILE *fp)
{
	struct fs_node *script = NULL;
	yyscan_t scanner;
 
	if (yylex_init(&scanner))
		return NULL;

	yyset_in(fp, scanner);
	yyparse(&script, scanner);
 
	yylex_destroy(scanner); 
	return script;
}

int main(int argc, char **argv)
{
	struct fs_node *script;
	struct ebpf *e;

	script = fs_load(stdin);
	if (!script)
		return 1;

	e = malloc(sizeof(*e));
	
	if (fs_compile(script->script.probes, ebpf_init(e))) {
		fs_ast_dump(script);
	}

	fs_ast_dump(script);
	/* if (write(1, e->prog, (e->ip - e->prog) << 3) <= 0) */
	/* 	return 1; */
	
	free(e);
	fs_free(script);
	return 0;
}
