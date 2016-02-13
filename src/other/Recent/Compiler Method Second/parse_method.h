/*************************************************************************
  
   Copyright (C) 2015. rollrat. All Rights Reserved.

------
FILE NAME:
   
   parse_method.h - [Private]

Project:

   Compiler Method Second

------
   AUTHOR: HyunJun Jeong  2015-02-17

***************************************************************************/

// 이 파일은 bitstream vera폰트를 사용하여 작성하였음

#ifndef _PARSE_METHOD_
#define _PARSE_METHOD_

#include <string>
#include "tree.h"
#include "priority.h"
#include "second_layer_priority.h"

/*
    함수, 주소코드 등 변수가 될 수 없는 형식이 포함된 경우
    먼저 치환하시기 바랍니다.
*/

template<class priority_type = priority_second_layer> class 
	parse_rvalue
{
	typedef parse_rvalue<priority_type> _Myparse;
	typedef tree<std::string> _Mytree;

	priority_type pf;
	_Mytree ps_tree;
	std::string temp;
	const char *ptr;

public:
	_Mytree start(const char *_S);

protected:
	bool is_char();
	bool is_operator();
	void skipws();

	// Parsing Module
	void var();
	void factor();
	void variable();
    
    bool get_inside_string();
};

////////////////////////////////////////////////////

template<class priority_type> 
tree<std::string> parse_rvalue<priority_type>::start(const char *_S)
{
	ptr = _S;
	var();
	return ps_tree;
}

////////////////////////////////////////////////////

template<class priority_type> 
bool parse_rvalue<priority_type>::is_char()
{
	return (
		isalnum(*ptr) ||
		*ptr == '.'  ||   // 소숫점
		*ptr == '\"' ||   // 문자열
		*ptr == '\'' ||   // 문자
		*ptr == '\\' ||   // 주소
		*ptr == '$' ||    // 변수에서 사용가능
		*ptr == '_' ||    // 변수에서 사용가능
		*ptr == '@'       // 변수로 사용불가능, 어셈블리나 치환 용도로 사용할 시
	);
}

template<class priority_type> 
bool parse_rvalue<priority_type>::is_operator()
{
	if (!*ptr)
		return false;
	return strchr("+-*/&|^%<>!~=", *ptr);
}

template<class priority_type> 
void parse_rvalue<priority_type>::skipws()
{
	while (isspace(*ptr))
		ptr++;
}

template<class priority_type> 
void parse_rvalue<priority_type>::var()
{
	factor();
    skipws();
	
	//
	//	연산자가 나오면 트리에 연산자를 집어넣어야하므로 
	//	rvalue와 lvalue를 각각 저장, 나누어 분석합니다
	//
	if (is_operator())
	{
		//
		//	연산자의 길이는 상황에 따라 늘어날 수
		//	있습니다. 이를 대비하여 연산자 전체를 가져옵니다.
		//
		std::string optr;
		for (; is_operator(); ptr++)
			optr.push_back(*ptr);
		skipws();

		//
		//	만일 현재 노드에 연산자가 없을 경우 그곳에 연산자를
		//	넣으면됩니다. 정상적인 경우 lvalue는 이미 채워진 
		//	상태여야합니다.
		//
		if (!ps_tree.is_data()) {
			*ps_tree = optr;
        } else {
        RETRY:

            /*
               
            현재 노드의 연산자와 넣으려고 하는 연산자를 서로 비교하여
            우선순위를 가져옵니다.

            x ::= 현재 노드의 연산자
            y ::= 넣으려고 하는 연산자
            라고 정의하면,

            (y > x) ->  1
            (y = x) ->  0
            (y < x) -> -1
            
            을 각각 출력합니다.

            */
            int save = pf(optr, *ps_tree);

			//
			//	현재 트리 노드위에 있는 연산자와 현재
			//	연산자를 비교하여 순위를 정합니다.
			//	그 후 두 순서를 적절히 바꾸어 원활히
			//	후위표기법으로 변환될 수 있도록 합니다.
			//

			if(save > 0)
			{
				/*

				이 구문은 현재 트리 노드위에 있는 연산자가
				넣으려하는 연산자보다 낮은 우선 순위를 가질 때 실행됩니다.

				가령, 3+y*z라는 구문을 만들기 위해선

				완성된 구문

				     +
				    / \
				   3   y

				에서

				         +
				        / \
				       3   *
				          / \
				         y   z

				로 바뀌여야합니다. (y의 데이터가 *의 밑 왼쪽으로 이동함)

				위 방법은 다음과 같은 순서로 실행됩니다.

                오른쪽에 데이터가 없는 경우
				1. 현재 노드가 y를 가리키게 합니다.
				2. 현재 노드의 오른쪽에 해당 연산자를 넣습니다.
				3. 현재 노드의 값과 오른쪽 노드의 값을 서로 바꿉니다.

                오른쪽에 데이터가 있는 경우(단일 데이터도 포함됨)
                1. 오른쪽 노드를 임시저장합니다.
                2. 오른쪽 노드에 해당 연산자를 넣습니다.
                3. 현재 노드를 오른쪽 노드로 이동시킵니다.
                4. 현재 노드의 왼쪽에 임시저장한 노드를 넣습니다.

				*/
                if(!ps_tree.is_right()) {
				    ps_tree.into_right();
				    ps_tree.push_left(optr);

				    //
				    //	현재 트리 노드의 데이터와 왼쪽 노드의 데이터를 바꿉니다.
				    //
				    ps_tree.dswap_left();
                } else {
                    ps_tree.detach_right();
                    ps_tree.push_right(optr);
                    ps_tree.into_right();
                    ps_tree.attach_left();
                }

			} else if (save < 0) {
                
                /*

                이 구문은 현재 트리 노드위에 있는 연산자가
                넣으려하는 연산자보다 높은 우선 순위를 가질 때 실행됩니다.

                만일, 현재 노드가 ground에 있다면, 
                다음과 같은 형식을 만듭니다.

                가령, x*y+i의 경우 + < * 이므로,

                    *
                   / \
                  x   y

                에서

                        +
                       / \
                      *   i
                     / \
                    x   y

                으로 변환합니다.

                만일 ground에 있지 않으면
                다음 두 가지의 형태를 가지므로,

                     ...    ...
                    /          \
                   *            *
                  / \          / \
                 x   y        x   y

                정상적인 연산을 위해 escape를 수행합니다.

                */
                if(ps_tree.is_ground()) {
                    ps_tree.make_head_left();
                    *ps_tree = optr;
                } else {
                    ps_tree.escape();
                    goto RETRY;
                }

			} else {

                /*

                이 구문은 현재 트리 노드위에 있는 연산자와
                넣으려하는 연산자가 같은 우선 순위를 가질 때 실행됩니다.

                가령, x+y-i연산시 수학적인 표현은 (x+y)-i가 되므로

                     +
                    / \
                   x   y

                에서

                       -
                      / \
                     +   i
                    / \
                   x   y

                가 되어야 합니다. (-가 더 위에 있으므로 i-(x+y)로
                생각할 수 있으나 이는 잘못된 표현입니다.)

                위 방법은 아래와 같은 방법으로 실행됩니다.

                escape연산 실행 후 

                i) 현재 노드가 ground일 경우
                1. 헤드를 임시저장하고 새 헤드를 만듭니다.
                2. 헤드에 연산자를 넣습니다.
                3. 헤드의 왼쪽에 임시저장한 노드를 삽입합니다.

                ii) escape를 왼쪽으로 실행하였다면(자세한 사항은 escape함수 참고)
                1. 왼쪽 노드를 임시저장합니다.
                2. 왼쪽 노드에 연산자를 넣습니다.
                3. 현재 노드를 왼쪽 노드를 가리키게 합니다.
                4. 왼쪽에 임시저장한 노드를 넣습니다.

                iii) escape를 오른쪽으로 실행하였다면
                1. 오른쪽 노드를 임시저장합니다.
                2. 오른쪽 노드에 연산자를 넣습니다.
                3. 현재 노드를 오른쪽 노드를 가리키게 합니다.
                4. 왼쪽에 임시저장한 노드를 넣습니다.

                */

                ps_tree.detach_this();
                if(ps_tree.last_detach() == 0) {
                    *ps_tree = optr;
                    ps_tree.attach_left();
                } else if(ps_tree.last_detach() == -1) { //} else if(!ps_tree.is_left()) {
                    ps_tree.push_left(optr);
                    ps_tree.into_left();
                    ps_tree.attach_left();
                } else if(ps_tree.last_detach() == 1) { //} else if(!ps_tree.is_right()) {
                    ps_tree.push_right(optr);
                    ps_tree.into_right();
                    ps_tree.attach_left();
                }

            }
        }
		var();
	}

}

template<class priority_type> 
void parse_rvalue<priority_type>::factor()
{
	skipws();
	if (*ptr == '(')
	{
		ptr++;
		_Myparse rvtemp;

		/*

		이 구문은 다음 두 가지 갈래로 나뉩니다.

			1. 왼쪽 노드에 데이터가 존재하고, 오른쪽엔 존재하지 않을 경우.
			2. 왼쪽 노드에 데이터가 존재하지 않을 경우

		*/
		if (ps_tree.is_left() && !ps_tree.is_right())

			//
			//	새로 만든 트리 노드를 오른쪽에다 붙입니다.
			//
			ps_tree.attach_right_tree(rvtemp.start(ptr).travel());
		else if (!ps_tree.is_left())
				
			//
			//	새로 만든 트리 노드를 왼쪽에다 붙입니다.
			//
			ps_tree.attach_left_tree(rvtemp.start(ptr).travel());
		else
			;	// error

		//
		//	새로 만든 트리안에서 돌았을 포인터를 가져옵니다.
		//
		ptr = rvtemp.ptr;

		//
		//	괄호가 정상적으로 닫혀있는지 확인합니다.
		//	이 작업이 정상적으로 이루어지지 않았다면 프로그램에 오류가 생길
		//	가능성이 매우 큽니다.
		//
		if (*ptr == ')')
			ptr++;
		else
			;	// error
	}
	else if(is_char()) 
	{
        //
        //  이 구문은 사실상 공백으로 나누어진 두 값을 서로 다른 값으로 보고
        //  두 값을 모두 처리시킵니다. 이런 오류를 막기위해 정상적인 식인지를 검사
        //  하여야합니다.
        //
		variable();
		factor();
	}
}

template<class priority_type> 
void parse_rvalue<priority_type>::variable()
{
	for (; is_char();)
		temp.push_back(*ptr++);

    //
    //  Reference, Function 부분을 확인합니다.
    //
RETRY:
    skipws();
    if (*ptr == '(' || *ptr == '[') {
        get_inside_string();
        goto RETRY;
    }

	//
	//	맨 먼저 왼쪽에 넣어보고 이미 데이터가 존재하면
	//	오른쪽에 넣습니다. 
	//	후위 표기법으로 변환하기 위해 왼쪽->오른쪽 순서대로 삽입합니다.
	//
	if (!ps_tree.is_left())
		ps_tree.push_left(temp);
	else if (!ps_tree.is_right())
		ps_tree.push_right(temp);
	temp.clear();
}

template<class priority_type> 
bool parse_rvalue<priority_type>::get_inside_string()
{
    //
    // 괄호의 종류는 관여하지 않음
    //

    int indicate_inside = 0;

    for(; *ptr; (ptr)++)
    {
        temp.push_back(*ptr);

        switch(*ptr)
        {
        case '[':
        case '(':
        case '{':
            indicate_inside++;
            break;

        case ']':
        case ')':
        case '}':
            indicate_inside--;
            break;

        default:
            continue;
        }

        if (indicate_inside == 0) {
            ptr++;
            return true;
        }
    }

    return false;
}


#endif