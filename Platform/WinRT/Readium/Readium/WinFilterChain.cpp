//
//  WinFilterChain.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#include "WinFilterChain.h"
#include "WinManifest.h"
#include "PluginMaps.h"

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::FilterChainPtr, FilterChain)

FilterChain::FilterChain(::ePub3::FilterChainPtr native) : _native(native)
{
	_native->SetBridge(this);
}
FilterChain::FilterChain(IVectorView<IContentFilter^>^ filters)
{
	::ePub3::FilterChain::FilterList list;
	list.reserve(filters->Size);

	for (IContentFilter^ filter : filters)
	{
		::ePub3::ContentFilterPtr native = GetNativeContentFilter(filter);
		if (native == nullptr)
			native = std::make_shared<WinRTContentFilter>(filter);
		list.push_back(native);
	}

	_native = ::ePub3::FilterChain::New(list);
}
IClosableStream^ FilterChain::ByteStreamForManifestItem(ManifestItem^ item)
{
	return ref new Stream(_native->GetFilteredOutputStreamForManifestItem(item->NativeObject));
}

END_READIUM_API
