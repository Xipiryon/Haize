
#include <iostream>
#include <sstream>
#include <vector>
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/Lexical.hpp"

namespace
{
	void displayToken(muon::system::Log&, const hz::parser::Token&);
	bool isLetter(char);
	bool isNumber(char);
	bool isLogicSymbol(char);
	bool isMathSymbol(char);
	bool isWhitespace(char);
	bool isLitteral(char c);
	bool isNewLine(char c);
	bool isSeparator(char);
	bool isBracket(char c);
	bool isBrace(char c);
	bool isParenthesis(char c);
	bool matchCombinedChar(char, char, char, char);

	char get(hz::parser::Info&);
	char peek(hz::parser::Info&);
	void pushToken(hz::parser::Info&, muon::String&);
	void pushSeparatorToken(hz::parser::Info&, muon::String&);
}

namespace hz
{
	namespace parser
	{
		/*
		* Custom parser::InfoImpl
		* Specific for Lexical parsing
		*/
		struct InfoImplLexical : InfoImpl
		{
			Token token;
			bool lineComment;
			bool multiLineComment;
			std::istream* stream;

			f32 fvalue;
			u32 fdenOffset;
			bool fdenUsed;

			u32 line;
			u32 column;
		};
#define INFO_IMPL ((hz::parser::InfoImplLexical*)info.impl)

		namespace lexical
		{
			bool parse(parser::Info& info, const char* str)
			{
				std::stringstream stream;
				stream << str;
				return parse(info, stream);
			}

			bool parse(parser::Info& info, std::istream& stream)
			{
				info.impl = MUON_CNEW(InfoImplLexical);
				info.TokenList->clear();
				INFO_IMPL->token.value = NULL;
				INFO_IMPL->token.type = S_INVALID;
				INFO_IMPL->lineComment = false;
				INFO_IMPL->multiLineComment = false;
				INFO_IMPL->stream = &stream;
				INFO_IMPL->fvalue = 0.0;
				INFO_IMPL->fdenOffset = 1;
				INFO_IMPL->fdenUsed = false;
				INFO_IMPL->line = 1;
				INFO_IMPL->column = 0;

				String word;

				while (!stream.eof())
				{
					++INFO_IMPL->column;
					char c = get(info);

					// *** PARSE INFO ***
					// **************
					if (isNewLine(c))
					{
						// If String, update word, line & column count
						if (INFO_IMPL->token.type == V_STRING)
						{
							word += c;
							++INFO_IMPL->line;
							INFO_IMPL->column = 0;
						}
						else
						{
							// Push stored token before increasing line count;
							pushToken(info, word);
							// Update line and column
							++INFO_IMPL->line;
							INFO_IMPL->column = 0;
							// Add Separator Token
							if (!INFO_IMPL->multiLineComment)
							{
								pushSeparatorToken(info, word);
							}
						}
					}

					// *** COMMENTS ***
					// **************
					else if (INFO_IMPL->lineComment)
					{
						char nc = peek(info);
						if (isNewLine(nc))
						{
							INFO_IMPL->lineComment = false;
						}
					}
					else if (INFO_IMPL->multiLineComment)
					{
						char nc = peek(info);
						if (c == '*' && nc == '/')
						{
							INFO_IMPL->multiLineComment = false;
							get(info);
						}
					}

					// *** STRING ***
					// **************
					else if (INFO_IMPL->token.type == V_STRING)
					{
						word += c;
						if (isLitteral(c) && word[0] == c)
						{
							pushToken(info, word);
						}
					}
					// *** LITTERAL ***
					// **************
					else if (isLitteral(c))
					{
						pushToken(info, word);
						INFO_IMPL->token.type = V_STRING;
						word += c;
					}

					// *** NUMBER ***
					// **************
					else if (isNumber(c))
					{
						if (INFO_IMPL->token.type != V_NUMBER && INFO_IMPL->token.type != V_IDENTIFIER)
						{
							pushToken(info, word);
							INFO_IMPL->token.type = V_NUMBER;
						}

						if (INFO_IMPL->token.type == V_IDENTIFIER)
						{
							word += c;
						}
						else
						{
							if (INFO_IMPL->fdenUsed)
							{
								INFO_IMPL->fdenOffset *= 10;
								f32 n = f32(c - '0') / INFO_IMPL->fdenOffset;
								INFO_IMPL->fvalue += n;
							}
							else
							{
								INFO_IMPL->fvalue *= 10;
								INFO_IMPL->fvalue += (c - '0');
							}
						}
					}

					// *** LETTER ***
					// **************
					else if (isLetter(c))
					{
						if (INFO_IMPL->token.type != V_NUMBER && INFO_IMPL->token.type != V_IDENTIFIER)
						{
							pushToken(info, word);
							INFO_IMPL->token.type = V_IDENTIFIER;
						}
						if (INFO_IMPL->token.type == V_IDENTIFIER && (isNumber(c) || isLetter(c)))
						{
							word += c;
						}
					}

					// *** LOGIC ***
					// **************
					else if (isLogicSymbol(c))
					{
						pushToken(info, word);
						word += c;
						switch (c)
						{
							case '&':
								INFO_IMPL->token.type = BITWISE_AND;
								break;
							case '|':
								INFO_IMPL->token.type = BITWISE_OR;
								break;
							case '!':
								INFO_IMPL->token.type = LOGIC_NOT;
								break;
							case '~':
								INFO_IMPL->token.type = BITWISE_NOT;
								break;
							case '^':
								INFO_IMPL->token.type = BITWISE_XOR;
								break;
							case '<':
								INFO_IMPL->token.type = LOGIC_LT;
								break;
							case '>':
								INFO_IMPL->token.type = LOGIC_GT;
								break;
						}
						char nc = peek(info);

						//(c == '&' || c == '|' || c == '!' || c == '~' || c == '^' || c == '<' || c == '>')
						if (isMathSymbol(nc) || isLogicSymbol(nc))
						{
							if (matchCombinedChar(c, nc, '&', '&'))
							{
								word += get(info);
								INFO_IMPL->token.type = LOGIC_AND;
							}
							else if (matchCombinedChar(c, nc, '|', '|'))
							{
								word += get(info);
								INFO_IMPL->token.type = LOGIC_OR;
							}
							else if (matchCombinedChar(c, nc, '!', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = LOGIC_NEQ;
							}
							else if (matchCombinedChar(c, nc, '~', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_NOT;
							}
							else if (matchCombinedChar(c, nc, '&', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_AND;
							}
							else if (matchCombinedChar(c, nc, '|', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_OR;
							}
							else if (matchCombinedChar(c, nc, '^', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_XOR;
							}
							else if (matchCombinedChar(c, nc, '<', '<'))
							{
							word += get(info);
							INFO_IMPL->token.type = BITWISE_LSH;
							}
							else if (matchCombinedChar(c, nc, '>', '>'))
							{
							word += get(info);
							INFO_IMPL->token.type = BITWISE_RSH;
							}
						}
						pushToken(info, word);
					}

					// *** MATH ***
					// **************
					else if (isMathSymbol(c))
					{
						pushToken(info, word);
						word += c;
						char nc = peek(info);
						//(c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '=')
						switch (c)
						{
							case '+':
								INFO_IMPL->token.type = MATH_ADD;
								break;
							case '-':
								INFO_IMPL->token.type = MATH_SUB;
								break;
							case '/':
								INFO_IMPL->token.type = MATH_DIV;
								break;
							case '*':
								INFO_IMPL->token.type = MATH_MUL;
								break;
							case '%':
								INFO_IMPL->token.type = MATH_MOD;
								break;
							case '=':
								INFO_IMPL->token.type = MATH_ASN;
								break;
						}

						//(c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '=')
						//(c == '&' || c == '|' || c == '!' || c == '~' || c == '^' || c == '<' || c == '>')
						if (isMathSymbol(nc) || isLogicSymbol(nc))
						{
							if (matchCombinedChar(c, nc, '+', '+'))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_INC;
							}
							else if (matchCombinedChar(c, nc, '-', '-'))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_DEC;
							}
							else if (matchCombinedChar(c, nc, '+', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_ADD;
							}
							else if (matchCombinedChar(c, nc, '-', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_SUB;
							}
							else if (matchCombinedChar(c, nc, '*', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_MUL;
							}
							else if (matchCombinedChar(c, nc, '/', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_DIV;
							}
							else if (matchCombinedChar(c, nc, '%', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = MATH_ASN_MOD;
							}
							else if (matchCombinedChar(c, nc, '=', '='))
							{
								word += get(info);
								INFO_IMPL->token.type = LOGIC_EQ;
							}
							else if (matchCombinedChar(c, nc, '/', '/'))
							{
								//Reset current Token
								INFO_IMPL->token.type = S_INVALID;
								word = "";
								INFO_IMPL->lineComment = true;
								continue;
							}
							else if (matchCombinedChar(c, nc, '/', '*'))
							{
								//Reset current Token
								INFO_IMPL->token.type = S_INVALID;
								word = "";
								get(info);
								INFO_IMPL->multiLineComment = true;
								continue;
							}
						}
						pushToken(info, word);
					}

					// *** BRACKET ***
					// **************
					else if (isBracket(c))
					{
						pushToken(info, word);
						word += c;
						switch (c)
						{
							case '[':
								INFO_IMPL->token.type = S_LBRACKET;
								break;
							case ']':
								INFO_IMPL->token.type = S_RBRACKET;
								break;
						}
						pushToken(info, word);
					}

					// *** CURLY BRACE ***
					// **************
					else if (isBrace(c))
					{
						pushToken(info, word);
						word += c;
						switch (c)
						{
							case '{':
								INFO_IMPL->token.type = S_LBRACE;
								break;
							case '}':
								INFO_IMPL->token.type = S_RBRACE;
								break;
						}
						pushToken(info, word);
					}

					// *** PARENTHESIS ***
					// **************
					else if (isParenthesis(c))
					{
						pushToken(info, word);
						word += c;
						switch (c)
						{
							case '(':
								INFO_IMPL->token.type = S_LPARENT;
								break;
							case ')':
								INFO_IMPL->token.type = S_RPARENT;
								break;
						}
						pushToken(info, word);
					}

					// *** WHITESPACE ***
					// **************
					else if (isWhitespace(c)
							 || stream.eof())
					{
						pushToken(info, word);
					}

					// *** SERPARATOR ***
					// **************
					else if (isSeparator(c))
					{
						pushToken(info, word);
						pushSeparatorToken(info, word);
					}

					// *** DOT ***
					// **************
					else if (c == '.')
					{
						if (INFO_IMPL->token.type == V_NUMBER)
						{
							if (!INFO_IMPL->fdenUsed)
							{
								INFO_IMPL->fdenUsed = true;
							}
							else
							{
								info.error.line = INFO_IMPL->line;
								info.error.column = INFO_IMPL->column;
								info.error.message = "Ill-formed Number: '.' should be used as decimal separator.";
								return false;
							}
						}
						else
						{
							pushToken(info, word);
							INFO_IMPL->token.type = S_ACCESSOR;
							word.clear();
							word += c;
							pushToken(info, word);
						}
					}

					// *** COMMA ***
					// **************
					else if (c == ',')
					{
						pushToken(info, word);
						word += c;
						INFO_IMPL->token.type = S_COMMA;
						pushToken(info, word);
					}
				}

				// Push the EOF token
				muon::String eof = "#EOF#";
				INFO_IMPL->token.type = S_EOF;
				pushToken(info, eof);

				MUON_CDELETE(INFO_IMPL);
				return true;
			}

			void display(parser::Info& info)
			{
#ifdef MUON_DEBUG
				muon::system::Log log("LEXICAL", muon::LOG_INFO);
				log() << "** Displaying Tokens **" << muon::endl;
				log() << "( " << info.TokenList->size() << " Tokens stored)" << muon::endl;
				if (info.TokenList->size() > 0)
				{
					for (u32 i = 0; i < (info.TokenList->size() - 1); ++i)
					{
						displayToken(log, (*info.TokenList)[i]);
					}
					displayToken(log, info.TokenList->back());
				}
#endif
			}
		}
	}
}

namespace
{
	void displayToken(muon::system::Log& log, const hz::parser::Token& token)
	{
		log() << hz::parser::TokenTypeStr[token.type] << muon::endl;
		//log() << "\t(" << token.value << ") " << muon::endl;
		log() << "\t#" << token.line << ":" << token.column << muon::endl;
	}

	bool isLetter(char c)
	{
		return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
	}

	bool isNumber(char c)
	{
		return (c >= '0' && c <= '9');
	}

	bool isLogicSymbol(char c)
	{
		return (c == '&' || c == '|' || c == '!' || c == '~' || c == '^' || c == '<' || c == '>');
	}

	bool isMathSymbol(char c)
	{
		return (c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '=');
	}

	bool isWhitespace(char c)
	{
		return (c == ' ' || c == '\t');
	}

	bool isLitteral(char c)
	{
		return (c == '\'' || c == '\"');
	}

	bool isNewLine(char c)
	{
		return (c == '\r' || c == '\n');
	}

	bool isSeparator(char c)
	{
		return (c == ';') || isNewLine(c);
	}

	bool isParenthesis(char c)
	{
		return (c == '(' || c == ')');
	}

	bool isBracket(char c)
	{
		return (c == '[' || c == ']');
	}

	bool isBrace(char c)
	{
		return ( c == '{' || c == '}');
	}

	bool matchCombinedChar(char c, char nc, char refc, char refnc)
	{
		return (c == refc) && (nc == refnc);
	}

	char get(hz::parser::Info& info)
	{
		char c;
		if (!INFO_IMPL->stream->get(c))
		{
			return 0;
		}
		return c;
	}

	char peek(hz::parser::Info& info)
	{
		if (INFO_IMPL->stream->eof())
		{
			return 0;
		}
		return INFO_IMPL->stream->peek();
	}

	void pushSeparatorToken(hz::parser::Info& info, muon::String& word)
	{
		//Push only a new separator token if not already the last in the list
		if (info.TokenList->size() > 0 && info.TokenList->back().type != hz::parser::S_SEPARATOR)
		{
			INFO_IMPL->token.type = hz::parser::S_SEPARATOR;
			pushToken(info, word);
		}
		else
		{
			//Manually revert current token
			INFO_IMPL->token.type = hz::parser::S_INVALID;
			INFO_IMPL->token.value = NULL;
			word = "";
		}
	}

	void pushToken(hz::parser::Info& info, muon::String& word)
	{
		if (INFO_IMPL->token.type != hz::parser::S_INVALID)
		{
			if (INFO_IMPL->token.type == hz::parser::V_IDENTIFIER)
			{
				if (word == "nil")
				{
					INFO_IMPL->token.type = hz::parser::V_NIL;
					INFO_IMPL->token.value = NULL;
				}
				else if (word == "true")
				{
					INFO_IMPL->token.type = hz::parser::V_TRUE;
					INFO_IMPL->token.value = true;
				}
				else if (word == "false")
				{
					INFO_IMPL->token.type = hz::parser::V_FALSE;
					INFO_IMPL->token.value = false;
				}
				else if (word == "namespace"
						 // Class and functions
						 || word == "class"
						 || word == "attr"
						 || word == "function"
						 // Logic keyword
						 || word == "if"
						 || word == "else"
						 || word == "for"
						 || word == "in"
						 || word == "while"
						 || word == "switch"
						 || word == "case"
						 || word == "break"
						 // other keyword
						 || word == "global"
						 || word == "return"
						 || word == "end"
						 )
				{
					INFO_IMPL->token.type = hz::parser::S_KEYWORD;
					INFO_IMPL->token.value = MUON_CNEW(muon::String, word);
				}
				else
				{
					INFO_IMPL->token.value = MUON_CNEW(muon::String, word);
				}
			}
			else if (INFO_IMPL->token.type == hz::parser::V_STRING)
			{
				INFO_IMPL->token.value = MUON_CNEW(muon::String, word);
			}
			else if (INFO_IMPL->token.type == hz::parser::V_NUMBER)
			{
				INFO_IMPL->token.value = INFO_IMPL->fvalue;
				INFO_IMPL->fvalue = 0.0;
				INFO_IMPL->fdenOffset = 1;
				INFO_IMPL->fdenUsed = false;
			}
			INFO_IMPL->token.line = INFO_IMPL->line;
			INFO_IMPL->token.column = INFO_IMPL->column;
			info.TokenList->push_back(INFO_IMPL->token);
		}
		else
		{
			//TODO:
			//Should warn there is an invalid Token
		}
		INFO_IMPL->token.type = hz::parser::S_INVALID;
		INFO_IMPL->token.value = NULL;
		word = "";
	}
}
