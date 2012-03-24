#define PROGRAM 	260
#define END			261
#define INTEGER		262
#define STRING		263
#define BOOLEAN		264
#define AND			265
#define OR			266
#define NOT			267
#define ID			268
#define TABLE		269

#define GT			270
#define GE			271
#define LT			272
#define LE			273
#define EQ			274
#define NEQ			275

#define INTCONST	280
#define STRCONST	281
#define BOOLCONST	282
#define IDNAME		283

#define IF			290
#define THEN		291
#define ELSE		292
#define WHILE		293
#define DO			294

#define JOIN		300
#define PROJECT		301
#define SELECT		302
#define EXISTS		303
#define ALL			304
#define EXTEND		305
#define UPDATE		306
#define RENAME		307

#define READ		310
#define WRITE		311

#define ERROR		666
#define FILEEND		667

typedef union {
	int ival; 
	char *sval;
} Lexval;

char *newstring(char *s);
int line;
