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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"
#include "resource/sharedresources.hxx"

#include <comphelper/processfactory.hxx>
#include <comphelper/officeresourcebundle.hxx>

/** === begin UNO includes === **/
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>
#include <osl/diagnose.h>

//........................................................................
namespace connectivity
{
//........................................................................

	/** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::Exception;
	/** === end UNO using === **/

	//====================================================================
	//= SharedResources_Impl
	//====================================================================
    class SharedResources_Impl
    {
    private:
        static  SharedResources_Impl*   s_pInstance;
        static  oslInterlockedCount     s_nClients;

    private:
        ::std::auto_ptr< ::comphelper::OfficeResourceBundle >
                                        m_pResourceBundle;

    public:
        static void     registerClient();
        static void     revokeClient();

        static SharedResources_Impl&
                        getInstance();

        ::rtl::OUString getResourceString( ResourceId _nId );

    private:
        SharedResources_Impl();

        static ::osl::Mutex& getMutex()
        {
            static ::osl::Mutex s_aMutex;
            return s_aMutex;
        }
    };

	//--------------------------------------------------------------------
    SharedResources_Impl*   SharedResources_Impl::s_pInstance( NULL );
    oslInterlockedCount     SharedResources_Impl::s_nClients( 0 );

	//--------------------------------------------------------------------
    SharedResources_Impl::SharedResources_Impl()
    {
        try
        {
            Reference< XPropertySet > xFactoryProps(
                ::comphelper::getProcessServiceFactory(), UNO_QUERY_THROW );
            Reference< XComponentContext > xContext(
                xFactoryProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ) ) ),
                UNO_QUERY_THROW
            );
            m_pResourceBundle.reset( new ::comphelper::OfficeResourceBundle( xContext, "cnr" ) );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SharedResources_Impl::getResourceString( ResourceId _nId )
    {
        if ( m_pResourceBundle.get() == NULL )
            // this should never happen, but we gracefully ignore it. It has been reported
            // in the constructor in non-product builds.
            return ::rtl::OUString();

        return m_pResourceBundle->loadString( _nId );
    }

	//--------------------------------------------------------------------
    void SharedResources_Impl::registerClient()
    {
        osl_incrementInterlockedCount( &s_nClients );
    }

	//--------------------------------------------------------------------
    void SharedResources_Impl::revokeClient()
    {
        ::osl::MutexGuard aGuard( getMutex() );
        if ( 0 == osl_decrementInterlockedCount( &s_nClients ) )
        {
            delete s_pInstance;
            s_pInstance = NULL;
        }
    }

	//--------------------------------------------------------------------
    SharedResources_Impl& SharedResources_Impl::getInstance()
    {
        ::osl::MutexGuard aGuard( getMutex() );
        OSL_ENSURE( s_nClients > 0, "SharedResources_Impl::getInstance: no active clients!" );

        if ( !s_pInstance )
            s_pInstance = new SharedResources_Impl;

        return *s_pInstance;
    }

    //====================================================================
	//= helpers
	//====================================================================
    namespace
    {
        size_t lcl_substitute( ::rtl::OUString& _inout_rString,
            const sal_Char* _pAsciiPattern, const ::rtl::OUString& _rReplace )
        {
            size_t nOccurences = 0;

            ::rtl::OUString sPattern( ::rtl::OUString::createFromAscii( _pAsciiPattern ) );
            sal_Int32 nIndex = 0;
            while ( ( nIndex = _inout_rString.indexOf( sPattern ) ) > -1 )
            {
                ++nOccurences;
                _inout_rString = _inout_rString.replaceAt( nIndex, sPattern.getLength(), _rReplace );
            }

            return nOccurences;
        }
    }

    //====================================================================
	//= SharedResources
	//====================================================================
	//--------------------------------------------------------------------
    SharedResources::SharedResources()
    {
        SharedResources_Impl::registerClient();
    }

	//--------------------------------------------------------------------
    SharedResources::~SharedResources()
    {
        SharedResources_Impl::revokeClient();
    }

	//--------------------------------------------------------------------
    ::rtl::OUString SharedResources::getResourceString( ResourceId _nResId ) const
    {
        return SharedResources_Impl::getInstance().getResourceString( _nResId );
    }

	//--------------------------------------------------------------------
    ::rtl::OUString SharedResources::getResourceStringWithSubstitution( ResourceId _nResId,
                const sal_Char* _pAsciiPatternToReplace, const ::rtl::OUString& _rStringToSubstitute ) const
    {
        ::rtl::OUString sString( SharedResources_Impl::getInstance().getResourceString( _nResId ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace, _rStringToSubstitute ) );
        return sString;
    }

	//--------------------------------------------------------------------
    ::rtl::OUString SharedResources::getResourceStringWithSubstitution( ResourceId _nResId,
                const sal_Char* _pAsciiPatternToReplace1, const ::rtl::OUString& _rStringToSubstitute1,
                const sal_Char* _pAsciiPatternToReplace2, const ::rtl::OUString& _rStringToSubstitute2 ) const
    {
        ::rtl::OUString sString( SharedResources_Impl::getInstance().getResourceString( _nResId ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace1, _rStringToSubstitute1 ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace2, _rStringToSubstitute2 ) );
        return sString;
    }

	//--------------------------------------------------------------------
    ::rtl::OUString SharedResources::getResourceStringWithSubstitution( ResourceId _nResId,
                const sal_Char* _pAsciiPatternToReplace1, const ::rtl::OUString& _rStringToSubstitute1,
                const sal_Char* _pAsciiPatternToReplace2, const ::rtl::OUString& _rStringToSubstitute2,
                const sal_Char* _pAsciiPatternToReplace3, const ::rtl::OUString& _rStringToSubstitute3 ) const
    {
        ::rtl::OUString sString( SharedResources_Impl::getInstance().getResourceString( _nResId ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace1, _rStringToSubstitute1 ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace2, _rStringToSubstitute2 ) );
        OSL_VERIFY( lcl_substitute( sString, _pAsciiPatternToReplace3, _rStringToSubstitute3 ) );
        return sString;
    }
    //--------------------------------------------------------------------
    ::rtl::OUString SharedResources::getResourceStringWithSubstitution( ResourceId _nResId,
                    const ::std::list< ::std::pair<const sal_Char* , ::rtl::OUString > > _aStringToSubstitutes) const
    {
        ::rtl::OUString sString( SharedResources_Impl::getInstance().getResourceString( _nResId ) );
        ::std::list< ::std::pair<const sal_Char* , ::rtl::OUString > >::const_iterator aIter = _aStringToSubstitutes.begin();
        ::std::list< ::std::pair<const sal_Char* , ::rtl::OUString > >::const_iterator aEnd  = _aStringToSubstitutes.end();
        for(;aIter != aEnd; ++aIter)
            OSL_VERIFY( lcl_substitute( sString, aIter->first, aIter->second ) );        
        
        return sString;
    }

//........................................................................
} // namespace connectivity
//........................................................................

