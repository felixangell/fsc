struct array_list* 
tokenize(struct lexer* lex, struct compilation_unit* unit) {
	struct array_list* tokens = array_list_make(8);
	lex->unit = unit;

	while (!is_eof(lex)) {
		skip_layout(lex);
		char current = peek(lex);
		
		if (current == '#') {
			skip_line(lex);
		}
		else if (current == '_' || islower(current) || isupper(current)) {
			array_list_push(tokens, recognize_identifier(lex));
		}
		else if (isdigit(current)) {
			array_list_push(tokens, recognize_number(lex));
		}
		else {
			switch (current) {
				case '/': {
					if (peek_at(lex, 1) == '/')
						skip_line(lex);
					else
						array_list_push(tokens, recognize_symbol(lex));
					break;
				}
				case '"': {
					array_list_push(tokens, recognize_string(lex));
					break;
				}
				case '\'': {
					array_list_push(tokens, recognize_character(lex));
					break;
				}
				case '[': case ']': case '(': case ')': case '{': case '}':
				case '.': case '&': case '*': case '+': case '-': case '~':
				case '!': case '%': case '<': case '>': case '^':
				case '|': case '?': case ':': case ';': case '=':
				case ',': {
					array_list_push(tokens, recognize_symbol(lex));
					break;
				}
				default: {
					if (is_eof(lex)) 
						break;

					char sample[128];
					memcpy(&sample, &lex->unit->contents[lex->pos], 96);

					fprintf(stderr, "error: %s:%d/%d, illegal character '%c':\n-> %s\n", 
					lex->unit->path, (int) lex->pos, (int) lex->unit->length, current, sample);
					exit(1);  
					break;
				}
			}
		}
	}
	return tokens;
}