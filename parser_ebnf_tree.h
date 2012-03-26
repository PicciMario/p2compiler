#include <stdio.h>
#include <stdlib.h>
#include "p2symbols.h"
#include "p2lexer.h"

// Node types
typedef enum {
	T_INTEGER,
	T_STRING,
	T_BOOLEAN,
	T_INTCONST,
	T_BOOLCONST,
	T_STRCONST,
	T_ID,
	T_NONTERMINAL
} Typenode;

// Non terminals
typedef enum {
	NPROGRAM,
	NSTAT_LIST,
	NSTAT,
	NDEF_STAT,
	NID_LIST,
	NTYPE,
	NATOMIC_TYPE,
	NTABLE_TYPE,
	NATTR_LIST,
	NATTR_DECL,
	NASSIGN_STAT,
	NEXPR,
	NBOOL_TERM,
	NCOMP_TERM,
	NLOW_TERM,
	NFACTOR,
	NUNARY_OP,
	NJOIN_OP,
	NPROJECT_OP,
	NSELECT_OP,
	NEXISTS_OP,
	NALL_OP,
	NEXTEND_OP,
	NUPDATE_OP,
	NRENAME_OP,
	NCONSTANT,
	NATOMIC_CONST,
	NTABLE_CONST,
	NTUPLE_CONST,
	NIF_STAT,
	NWHILE_STAT,
	NREAD_STAT,
	NSPECIFIER,
	NWRITE_STAT
} Nonterminal;

// Node structure
typedef struct snode{
	Typenode type;
	Lexval value;
	struct snode *child, *brother;
} Node;

typedef Node *Pnode;

// Node functions prototypes
Pnode newnode(Typenode);
Pnode nontermnode(Nonterminal);
Pnode keynode(Typenode);
Pnode idnode();
Pnode intconstnode();
Pnode strconstnode();
Pnode boolconstnode();

// General functions prototypes
void next();
void match2(int symbol, const char* func);
void print_error();

// Parsing rules prototypes
void parse();
Pnode parse_program();
Pnode parse_stat_list();
Pnode parse_stat();
Pnode parse_def_stat();
Pnode parse_id_list() ;
Pnode parse_type() ;
Pnode parse_atomic_type() ;
Pnode parse_table_type() ;
Pnode parse_attr_list() ;
Pnode parse_attr_decl() ;
Pnode parse_assign_stat() ;
Pnode parse_expr() ;
Pnode parse_bool_term();
Pnode parse_comp_term();
Pnode parse_low_term();
Pnode parse_factor();
Pnode parse_unary_op();
Pnode parse_join_op();
Pnode parse_project_op();
Pnode parse_select_op();
Pnode parse_exists_op();
Pnode parse_all_op();
Pnode parse_extend_op();
Pnode parse_update_op();
Pnode parse_rename_op();
Pnode parse_constant();
Pnode parse_atomic_const();
Pnode parse_table_const();
Pnode parse_tuple_const();
Pnode parse_if_stat();
Pnode parse_while_stat();
Pnode parse_read_stat();
Pnode parse_specifier();
Pnode parse_write_stat();
