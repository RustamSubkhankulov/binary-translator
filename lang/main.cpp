#include <stdlib.h>

//-------------------------------------------------------------------

#include "lang_common/lang_common.h"
#include "lexic/lexic.h"
#include "general/general.h"
#include "syntax/syntax.h"
#include "trans/trans.h"
#include "tree_simp/tree_simp.h"

//===================================================================

int main(int argc, char* argv[]) {

    FILE* logs_file = open_log_file(argv[1]);
    if (!logs_file)
        return -1;

    Buffer_struct buffer_struct = { 0 };

    int ret = 0;

    #ifdef CODE_FILE_INPUT

        code_read_file(&buffer_struct, argv[2]);

    #else

        code_read_console(&buffer_struct);

    #endif
    
    struct Tokens* tokens = lexic_analysis(&buffer_struct);
    if (!tokens)
        return -1;

    // ret = tokens_dump(tokens, logs_file); 
    // RETURN_CHECK(ret);

    struct Tree tree = { 0 };
    ret = tree_ctor(&tree);
    if (ret == -1) return -1;

    ret = build_a_tree(&tree, tokens);
    if (ret == -1) return -1;

    #ifdef AST_SIMLPIFY

        TREE_SIMPLIFY(&tree);
        tree_draw_graph(&tree);

    #endif

    ret = translation_execute(&tree, argv[3]);
    if (ret == -1) return -1;

    ret = tree_dtor(&tree);
    if (ret == -1) return -1;

    buffer_free(&buffer_struct);
    free_tokens(tokens);

    ret = assemble_and_execute(argv[3]);
    RETURN_CHECK(ret);

    close_log_file();

    return 0;
}