#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ast.h"
#include "hindley-milner.h"

int check_types(ASTNode *ast);

#endif /* TYPECHECK_H */
