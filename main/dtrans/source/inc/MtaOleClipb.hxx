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



#ifndef _MTAOLECLIPB_HXX_
#define _MTAOLECLIPB_HXX_

#include <sal/types.h>
#include <osl/mutex.hxx>

#if defined _MSC_VER
#pragma warning(push,1)
#endif
#include <objidl.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

//--------------------------------------------------------
// the Mta-Ole clipboard class is for internal use only!
// only one instance of this class should be created, the
// user has to ensure this!
// the class is not thread-safe because it will be used 
// only from within the clipboard service and the methods
// of the clipboard service are already synchronized
//--------------------------------------------------------

class CMtaOleClipboard
{
public:
	typedef void ( WINAPI *LPFNC_CLIPVIEWER_CALLBACK_t )( void );

public:
	CMtaOleClipboard( );
	~CMtaOleClipboard( );

	// clipboard functions
	HRESULT setClipboard( IDataObject* pIDataObject );
	HRESULT getClipboard( IDataObject** ppIDataObject );	
	HRESULT flushClipboard( );
	
	// register/unregister a clipboard viewer; there can only
	// be one at a time; parameter NULL means unregister
	// a clipboard viewer
	// returns true on success else false; use GetLastError( ) in
	// false case
	sal_Bool registerClipViewer( LPFNC_CLIPVIEWER_CALLBACK_t pfncClipViewerCallback );

private:
	unsigned int run( );
	
	// create a hidden windows which serves as an request 
	// target; so we guarantee synchronization
	void createMtaOleReqWnd( );

	// message support
	sal_Bool postMessage( UINT msg, WPARAM wParam = 0, LPARAM lParam = 0 );
	LRESULT  sendMessage( UINT msg, WPARAM wParam = 0, LPARAM lParam = 0 );

	//---------------------------------------------------------------
	// message handler functions; remember these functions are called
	// from a different thread context!
	//---------------------------------------------------------------

	LRESULT  onSetClipboard( IDataObject* pIDataObject );
	LRESULT  onGetClipboard( LPSTREAM* ppStream );	
	LRESULT  onFlushClipboard( );
	sal_Bool onRegisterClipViewer( LPFNC_CLIPVIEWER_CALLBACK_t pfncClipViewerCallback );

	// win32 clipboard-viewer support
	LRESULT onChangeCBChain( HWND hWndRemove, HWND hWndNext );
	LRESULT onDrawClipboard( );

	static LRESULT CALLBACK mtaOleReqWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
	static unsigned int WINAPI oleThreadProc( LPVOID pParam );

    static unsigned int WINAPI clipboardChangedNotifierThreadProc( LPVOID pParam );

	sal_Bool WaitForThreadReady( ) const;	

private:
	HANDLE						m_hOleThread;
	unsigned					m_uOleThreadId;
	HANDLE						m_hEvtThrdReady;	
	HWND						m_hwndMtaOleReqWnd;	
	ATOM						m_MtaOleReqWndClassAtom;
	HWND						m_hwndNextClipViewer;
	LPFNC_CLIPVIEWER_CALLBACK_t	m_pfncClipViewerCallback;	
	sal_Bool					m_bInRegisterClipViewer;

    sal_Bool                    m_bRunClipboardNotifierThread;
    HANDLE                      m_hClipboardChangedNotifierThread;
    HANDLE                      m_hClipboardChangedNotifierEvents[2];
    HANDLE&                     m_hClipboardChangedEvent;
    HANDLE&                     m_hTerminateClipboardChangedNotifierEvent;
    osl::Mutex                  m_ClipboardChangedEventCountMutex;
    sal_Int32                   m_ClipboardChangedEventCount;

    osl::Mutex                  m_pfncClipViewerCallbackMutex;

	static CMtaOleClipboard*    s_theMtaOleClipboardInst;

// not allowed
private:
	CMtaOleClipboard( const CMtaOleClipboard& );
	CMtaOleClipboard& operator=( const CMtaOleClipboard& );

	friend LRESULT CALLBACK mtaOleReqWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif
