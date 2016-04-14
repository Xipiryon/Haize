#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace utils
{
	int hex2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0x
		int len = strlen(str);
		for (; i < len; ++i)
		{
			val <<= 4;
			if (str[i] >= '0' && str[i] <= '9')
			{
				val |= str[i] - '0';
			}
			else if (str[i] >= 'a' && str[i] <= 'f')
			{
				val |= str[i] - 'a' + 10;
			}
			else
			{
				val |= str[i] - 'A' + 10;
			}
		}
		return val;
	}

	int oct2int(char* str)
	{
		int val = 0;
		int i = 1; // Skip 0
		int len = strlen(str);
		for (; i < len; ++i)
		{
			val <<= 3;
			val |= str[i] - '0';
		}
		return val;
	}

	int bin2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0b
		int len = strlen(str);
		for (; i < len; ++i)
		{
			val <<= 1;
			val |= (str[i] == '1' ? 1 : 0);
		}
		return val;
	}
}

namespace
{
	/*
	* Custom SharedData
	* Specific for Lexical parsing
	*/
	struct InternalDataLexical : public hz::parser::InternalCompilerData
	{
		virtual ~InternalDataLexical()
		{
		}

		std::vector<hz::parser::Token>* tokenList;
		hz::parser::Token token;
		bool lineComment;
		bool multiLineComment;
		std::stringstream stream;

		m::f32 fvalue;
		m::u32 fdenOffset;
		bool fdenUsed;

		m::u32 line;
		m::u32 column;		// Token start column
		m::u32 columnCount; // Absolute column count
		// Section
		std::vector<hz::parser::Compiler::Section>* sections;
		m::u64 charCount;
		m::u64 nextEndSection;
		m::u32 currSection;
	};

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
		return (c == ';');
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
		return (c == '{' || c == '}');
	}

	bool matchCombinedChar(char c, char nc, char refc, char refnc)
	{
		return (c == refc) && (nc == refnc);
	}

	char get(InternalDataLexical& impl)
	{
		char c;
		if (!impl.stream.get(c))
		{
			return 0;
		}
		if (++impl.charCount >= impl.nextEndSection)
		{
			if (impl.currSection + 1 < impl.sections->size())
			{
				++impl.currSection;
				impl.nextEndSection = impl.sections->at(impl.currSection)._end;
			}
		}
		return c;
	}

	char peek(InternalDataLexical& impl)
	{
		if (impl.stream.eof())
		{
			return 0;
		}
		return impl.stream.peek();
	}

	void pushToken(InternalDataLexical* impl, m::String& word)
	{
		if (impl->token.type != hz::parser::S_INVALID)
		{
			impl->token.value = word;
			if (impl->token.type == hz::parser::V_IDENTIFIER)
			{
				if (word == "nil")
				{
					impl->token.type = hz::parser::V_NIL;
				}
				else if (word == "true")
				{
					impl->token.type = hz::parser::V_TRUE;
					impl->token.value = true;
				}
				else if (word == "false")
				{
					impl->token.type = hz::parser::V_FALSE;
					impl->token.value = false;
				}
				else if (word == "namespace"
						 || word == "class"
						 || word == "constructor"
						 || word == "destructor"
						 // Flow control
						 || word == "if"
						 || word == "else"
						 || word == "for"
						 || word == "while"
						 || word == "switch"
						 || word == "case"
						 || word == "default"
						 || word == "break"
						 || word == "continue"
						 || word == "return"
						 // Variable and Parameter
						 || word == "in"
						 || word == "out"
						 || word == "inout"
						 || word == "global"
						 || word == "new"
						 || word == "delete"
						 )
				{
					impl->token.type = hz::parser::S_KEYWORD;
				}
			}
			else if (impl->token.type == hz::parser::V_NUMBER)
			{
				impl->token.value = impl->fvalue;
				impl->fvalue = 0.0;
				impl->fdenOffset = 1;
				impl->fdenUsed = false;
			}

			impl->token.line = impl->line;
			impl->token.column = impl->column;
			impl->token.section = impl->currSection;
			impl->tokenList->push_back(impl->token);
		}
		else
		{
			//TODO:
			//Should warn there is an invalid Token
		}
		impl->token.type = hz::parser::S_INVALID;
		impl->token.value.reset();
		word = "";
	}

	void pushSeparatorToken(InternalDataLexical* impl, m::String& word)
	{
		//Push only a new separator token if not already the last in the list
		if (impl->tokenList->size() > 0 && impl->tokenList->back().type != hz::parser::S_SEPARATOR)
		{
			impl->token.type = hz::parser::S_SEPARATOR;
			pushToken(impl, word);
		}
		else
		{
			//Manually revert current token
			impl->token.type = hz::parser::S_INVALID;
			impl->token.value.reset();
			word = "";
		}
	}
}

namespace hz
{
	namespace parser
	{
		bool Compiler::lexical(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;

			InternalDataLexical impl;

			m_tokenList->clear();
			impl.tokenList = m_tokenList;
			impl.token.value.reset();
			impl.token.type = hz::parser::S_INVALID;
			impl.lineComment = false;
			impl.multiLineComment = false;
			impl.stream << m_loadBuffer.cStr();
			impl.fvalue = 0.0;
			impl.fdenOffset = 1;
			impl.fdenUsed = false;
			impl.line = 1;
			impl.column = 1;
			impl.columnCount = 1;
			// Section stuff
			impl.sections = m_sections;
			impl.charCount = 0;
			impl.currSection = 0;
			impl.nextEndSection = m_sections->at(0)._end;

			m::String word;

			while (!impl.stream.eof())
			{
				++impl.columnCount;
				char c = get(impl);

				// *** PARSE INFO ***
				// **************
				if (isNewLine(c))
				{
					// If m::String, update word, line & column count
					if (impl.token.type == parser::V_STRING)
					{
						word += c;
						++impl.line;
						impl.column = 1;
						impl.columnCount = 1;
					}
					else
					{
						// Push stored token before increasing line count;
						pushToken(&impl, word);
						// Update line and column
						++impl.line;
						impl.column = 1;
						impl.columnCount = 1;
					}
				}

				// *** COMMENTS ***
				// **************
				else if (impl.lineComment)
				{
					char nc = peek(impl);
					if (isNewLine(nc))
					{
						impl.lineComment = false;
					}
				}
				else if (impl.multiLineComment)
				{
					char nc = peek(impl);
					if (c == '*' && nc == '/')
					{
						impl.multiLineComment = false;
						get(impl);
					}
				}

				// *** STRING ***
				// **************
				else if (impl.token.type == parser::V_STRING)
				{
					word += c;
					if (isLitteral(c) && word[0] == c)
					{
						pushToken(&impl, word);
					}
				}
				// *** LITTERAL ***
				// **************
				else if (isLitteral(c))
				{
					pushToken(&impl, word);
					impl.token.type = parser::V_STRING;
					word += c;
				}

				// *** NUMBER ***
				// **************
				else if (isNumber(c))
				{
					if (impl.token.type != parser::V_NUMBER && impl.token.type != parser::V_IDENTIFIER)
					{
						pushToken(&impl, word);
						impl.token.type = parser::V_NUMBER;
					}

					if (impl.token.type == parser::V_IDENTIFIER)
					{
						word += c;
					}
					else
					{
						if (impl.fdenUsed)
						{
							impl.fdenOffset *= 10;
							m::f32 n = m::f32(c - '0') / impl.fdenOffset;
							impl.fvalue += n;
						}
						else
						{
							impl.fvalue *= 10;
							impl.fvalue += (c - '0');
						}
					}
				}

				// *** LETTER ***
				// **************
				else if (isLetter(c))
				{
					if (impl.token.type != parser::V_NUMBER && impl.token.type != parser::V_IDENTIFIER)
					{
						pushToken(&impl, word);
						impl.token.type = parser::V_IDENTIFIER;
					}
					if (impl.token.type == parser::V_IDENTIFIER && (isNumber(c) || isLetter(c)))
					{
						word += c;
					}
				}

				// *** LOGIC ***
				// **************
				else if (isLogicSymbol(c))
				{
					pushToken(&impl, word);
					word += c;
					switch (c)
					{
						case '&':
							impl.token.type = parser::BITWISE_AND;
							break;
						case '|':
							impl.token.type = parser::BITWISE_OR;
							break;
						case '!':
							impl.token.type = parser::LOGIC_NOT;
							break;
						case '~':
							impl.token.type = parser::BITWISE_NOT;
							break;
						case '^':
							impl.token.type = parser::BITWISE_XOR;
							break;
						case '<':
							impl.token.type = parser::LOGIC_LT;
							break;
						case '>':
							impl.token.type = parser::LOGIC_GT;
							break;
					}
					char nc = peek(impl);

					//(c == '&' || c == '|' || c == '!' || c == '~' || c == '^' || c == '<' || c == '>')
					if (isMathSymbol(nc) || isLogicSymbol(nc))
					{
						if (matchCombinedChar(c, nc, '&', '&'))
						{
							word += get(impl);
							impl.token.type = parser::LOGIC_AND;
						}
						else if (matchCombinedChar(c, nc, '|', '|'))
						{
							word += get(impl);
							impl.token.type = parser::LOGIC_OR;
						}
						else if (matchCombinedChar(c, nc, '!', '='))
						{
							word += get(impl);
							impl.token.type = parser::LOGIC_NEQ;
						}
						else if (matchCombinedChar(c, nc, '~', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_NOT;
						}
						else if (matchCombinedChar(c, nc, '&', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_AND;
						}
						else if (matchCombinedChar(c, nc, '|', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_OR;
						}
						else if (matchCombinedChar(c, nc, '^', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_XOR;
						}
						else if (matchCombinedChar(c, nc, '<', '<'))
						{
							word += get(impl);
							impl.token.type = parser::BITWISE_LSH;
						}
						else if (matchCombinedChar(c, nc, '>', '>'))
						{
							word += get(impl);
							impl.token.type = parser::BITWISE_RSH;
						}
					}
					pushToken(&impl, word);
				}

				// *** MATH ***
				// **************
				else if (isMathSymbol(c))
				{
					pushToken(&impl, word);
					word += c;
					char nc = peek(impl);
					//(c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '=')
					switch (c)
					{
						case '+':
							impl.token.type = parser::MATH_ADD;
							break;
						case '-':
							impl.token.type = parser::MATH_SUB;
							break;
						case '/':
							impl.token.type = parser::MATH_DIV;
							break;
						case '*':
							impl.token.type = parser::MATH_MUL;
							break;
						case '%':
							impl.token.type = parser::MATH_MOD;
							break;
						case '=':
							impl.token.type = parser::MATH_ASN;
							break;
					}

					//(c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '=')
					//(c == '&' || c == '|' || c == '!' || c == '~' || c == '^' || c == '<' || c == '>')
					if (isMathSymbol(nc) || isLogicSymbol(nc))
					{
						if (matchCombinedChar(c, nc, '+', '+'))
						{
							word += get(impl);
							impl.token.type = parser::MATH_INC;
						}
						else if (matchCombinedChar(c, nc, '-', '-'))
						{
							word += get(impl);
							impl.token.type = parser::MATH_DEC;
						}
						else if (matchCombinedChar(c, nc, '+', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_ADD;
						}
						else if (matchCombinedChar(c, nc, '-', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_SUB;
						}
						else if (matchCombinedChar(c, nc, '*', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_MUL;
						}
						else if (matchCombinedChar(c, nc, '/', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_DIV;
						}
						else if (matchCombinedChar(c, nc, '%', '='))
						{
							word += get(impl);
							impl.token.type = parser::MATH_ASN_MOD;
						}
						else if (matchCombinedChar(c, nc, '=', '='))
						{
							word += get(impl);
							impl.token.type = parser::LOGIC_EQ;
						}
						else if (matchCombinedChar(c, nc, '/', '/'))
						{
							//Reset current Token
							impl.token.type = parser::S_INVALID;
							word = "";
							impl.lineComment = true;
							continue;
						}
						else if (matchCombinedChar(c, nc, '/', '*'))
						{
							//Reset current Token
							impl.token.type = parser::S_INVALID;
							word = "";
							get(impl);
							impl.multiLineComment = true;
							continue;
						}
					}
					pushToken(&impl, word);
				}

				// *** BRACKET ***
				// **************
				else if (isBracket(c))
				{
					pushToken(&impl, word);
					word += c;
					switch (c)
					{
						case '[':
							impl.token.type = parser::S_LBRACKET;
							break;
						case ']':
							impl.token.type = parser::S_RBRACKET;
							break;
					}
					pushToken(&impl, word);
				}

				// *** CURLY BRACE ***
				// **************
				else if (isBrace(c))
				{
					pushToken(&impl, word);
					word += c;
					switch (c)
					{
						case '{':
							impl.token.type = parser::S_LBRACE;
							break;
						case '}':
							impl.token.type = parser::S_RBRACE;
							break;
					}
					pushToken(&impl, word);
				}

				// *** PARENTHESIS ***
				// **************
				else if (isParenthesis(c))
				{
					pushToken(&impl, word);
					word += c;
					switch (c)
					{
						case '(':
							impl.token.type = parser::S_LPARENT;
							break;
						case ')':
							impl.token.type = parser::S_RPARENT;
							break;
					}
					pushToken(&impl, word);
				}

				// *** WHITESPACE ***
				// **************
				else if (isWhitespace(c)
						 || impl.stream.eof())
				{
					pushToken(&impl, word);
					impl.column = impl.columnCount;
				}

				// *** SERPARATOR ***
				// **************
				else if (isSeparator(c))
				{
					pushToken(&impl, word);
					pushSeparatorToken(&impl, word);
				}

				// *** DOT ***
				// **************
				else if (c == '.')
				{
					if (impl.token.type == parser::V_NUMBER)
					{
						if (!impl.fdenUsed)
						{
							impl.fdenUsed = true;
						}
						else
						{
							error.state = Error::ERROR;
							error.line = impl.line;
							error.column = impl.column;
							error.message = "Ill-formed Number: '.' should be used as decimal separator.";
							return false;
						}
					}
					else
					{
						pushToken(&impl, word);
						impl.token.type = parser::S_ACCESSOR;
						word.clear();
						word += c;
						pushToken(&impl, word);
					}
				}

				// *** COMMA ***
				// **************
				else if (c == ',')
				{
					pushToken(&impl, word);
					word += c;
					impl.token.type = parser::S_COMMA;
					pushToken(&impl, word);
				}
			}

			// Push the EOF token
			m::String eof = "#EOF#";
			impl.token.type = parser::S_EOF;
			pushToken(&impl, eof);

			error.state = Error::SUCCESS;
#if defined(HAIZE_DEBUG)
			m::system::Log debug("Lexical", m::LOG_DEBUG);
			debug << "Parsed " << m_tokenList->size() << " tokens" << m::endl;
#endif
			return true;
		}
	}
}
