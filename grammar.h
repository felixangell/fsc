#ifndef GRAMMAR_H
#define GRAMMAR_H

// all of the grammar in this file is
// referenced here:
// https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm

bool is_reserved_keyword(const char* keyword);

/*
	<type-specifier> ::= void
                   | char
                   | short
                   | int
                   | long
                   | float
                   | double
                   | signed
                   | unsigned
*/
bool is_type_specifier(const char* str);

/*
	<type-qualifier> ::= const
	                   | volatile
*/
bool is_type_qualifier(const char* str);

/*
	<storage-class-specifier> ::= auto
                            | register
                            | static
                            | extern
                            | typedef
*/
bool is_storage_class_specifier(const char* str);

#endif