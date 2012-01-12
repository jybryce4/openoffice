/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <precomp.h>
#include <ary_i/ci_text2.hxx>


// NOT FULLY DEFINED SERVICES
#include <ary_i/disdocum.hxx>
#include <ary_i/d_token.hxx>


namespace ary
{
namespace inf
{

DocuTex2::DocuTex2()
{
}

DocuTex2::~DocuTex2()
{
	for ( TokenList::iterator iter = aTokens.begin();
		  iter != aTokens.end();
		  ++iter )
	{
		delete (*iter);
	}
}

void
DocuTex2::DisplayAt( DocumentationDisplay & o_rDisplay ) const
{
	for ( ary::inf::DocuTex2::TokenList::const_iterator
				iter = aTokens.begin();
		  iter != aTokens.end();
		  ++iter )
	{
		(*iter)->DisplayAt(o_rDisplay);
	}
}

void
DocuTex2::AddToken( DYN DocuToken &	let_drToken )
{
    if (aTokens.empty())
    {
        if (let_drToken.IsWhiteOnly())
            return;
    }
    aTokens.push_back(&let_drToken);
}

bool
DocuTex2::IsEmpty() const
{
	for ( ary::inf::DocuTex2::TokenList::const_iterator
				iter = aTokens.begin();
		  iter != aTokens.end();
		  ++iter )
	{
    	return false;
	}
    return true;
}

using csi::dsapi::DT_TextToken;

const String &
DocuTex2::TextOfFirstToken() const
{
    if (NOT aTokens.empty())
    {
        const DT_TextToken *
            pTok = dynamic_cast< const DT_TextToken* >(*aTokens.begin());

        if (pTok != 0)
            return pTok->GetTextStr();
    }
    return String::Null_();
}

String &
DocuTex2::Access_TextOfFirstToken()
{
    if (NOT aTokens.empty())
    {
        DT_TextToken *
            pTok = dynamic_cast< DT_TextToken* >(*aTokens.begin());

        if (pTok != 0)
            return pTok->Access_Text();
    }

    static String sDummy_;
    return sDummy_;
}



void    DocuText_Display::Display_StdAtTag(
							const csi::dsapi::DT_StdAtTag & ) {}
void    DocuText_Display::Display_SeeAlsoAtTag(
							const csi::dsapi::DT_SeeAlsoAtTag & ) {}
void    DocuText_Display::Display_ParameterAtTag(
							const csi::dsapi::DT_ParameterAtTag & ) {}
void    DocuText_Display::Display_SinceAtTag(
							const csi::dsapi::DT_SinceAtTag & ) {}



}   // namespace inf
}   // namespace ary

