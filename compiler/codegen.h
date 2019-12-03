#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "ast.h"

int emit(FILE *output_file, ASTNode *ast);

#endif /* CODEGEN_H */
