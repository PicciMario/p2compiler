#include <stdio.h>
#include <stdlib.h>
#include "p2symbols.h"
#include "p2lexer.h"

void next();
void match2(int symbol, const char* func);
void print_error();

void parse();
void parse_program();
void parse_stat_list();
void parse_stat();
void parse_def_stat();
void parse_id_list() ;
void parse_type() ;
void parse_atomic_type() ;
void parse_table_type() ;
void parse_attr_list() ;
void parse_attr_decl() ;
void parse_assign_stat() ;
void parse_expr() ;
void parse_bool_term();
void parse_comp_term();
void parse_low_term();
void parse_factor();
void parse_unary_op();
void parse_join_op();
void parse_project_op();
void parse_select_op();
void parse_exists_op();
void parse_all_op();
void parse_extend_op();
void parse_update_op();
void parse_rename_op();
void parse_constant();
void parse_atomic_const();
void parse_table_const();
void parse_tuple_const();
void parse_if_stat();
void parse_while_stat();
void parse_read_stat();
void parse_specifier();
void parse_write_stat();
