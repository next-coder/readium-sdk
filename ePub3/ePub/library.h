//
//  library.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
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

#ifndef __ePub3__library__
#define __ePub3__library__

#include <ePub3/epub3.h>
#include <ePub3/container.h>
#include <ePub3/package.h>
#include <ePub3/cfi.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/byte_stream.h>
#include <map>

EPUB3_BEGIN_NAMESPACE

// Note that this is a library in the smallest sense: it keeps track of ePub files
//  by their unique-identifier, storing the path to that ePub file *as given*. Its
//  primary presence here is to allow for inter-publication linking. It is also
//  designed to be subclassed in order to serve as the basis of a complete library
//  in an application, with all the additional functionality that would involve.
//
// Note that each application would have a single library, accessible only through
//  the MainLibrary() static method. A library can optionally be written out to
//  disk and then loaded back in by calling MainLibrary() with a Locator instance
//  at application startup. Once the singleton instance has been created,
//  MainLibrary() will ignore its argument and always return that instance.
//
// Thoughts: OCF allows for multiple packages to be specified, but I don't see any
//  handling of that in ePub3 CFI?

/**
 @ingroup utilities
 */
class Library
#if EPUB_PLATFORM(WINRT)
	: public NativeBridge
#endif
{
public:
    typedef string      EPubIdentifier;
    
protected:
                        Library() : _containers(), _packages() {}
                        Library(const Library& o) : _containers(o._containers), _packages(o._packages) {}
                        Library(Library&& o) : _containers(std::move(o._containers)), _packages(std::move(o._packages)) {}
    
    // load a library from a file generated using WriteToFile()
    EPUB3_EXPORT        Library(const string& path);
    EPUB3_EXPORT bool   Load(const string& path);
    
public:
    // access a singleton instance managed by the class
    EPUB3_EXPORT
    static Library*     MainLibrary(const string& path = "");
	EPUB3_EXPORT
    virtual             ~Library();

    EPUB3_EXPORT
    string              PathForEPubWithUniqueID(const string& uniqueID)     const;
    EPUB3_EXPORT
    string              PathForEPubWithPackageID(const string& packageID)   const;

    EPUB3_EXPORT
    void                AddPublicationsInContainer(shared_ptr<Container> container, const string& path);
    EPUB3_EXPORT
    void                AddPublicationsInContainerAtPath(const string& path);
    
    // returns an epub3:// url for the package with a given identifier
    EPUB3_EXPORT
    IRI                 EPubURLForPublication(shared_ptr<Package> package)       const;
    EPUB3_EXPORT
    IRI                 EPubURLForPublicationID(const string& identifier)   const;
    
    // may load a container/package, so non-const
    EPUB3_EXPORT
    shared_ptr<Package> PackageForEPubURL(const IRI& url, bool allowLoad=true);

    EPUB3_EXPORT
    IRI                 EPubCFIURLForManifestItem(shared_ptr<ManifestItem> item) const;
    
    // may instantiate a Container & store it, so non-const
    EPUB3_EXPORT
    shared_ptr<ManifestItem>    ManifestItemForCFI(const IRI& urlWithCFI, CFI* pRemainingCFI);

    EPUB3_EXPORT
    unique_ptr<ByteStream>  ReadStreamForEPubURL(const IRI& url, CFI* pRemainingCFI);
    
    // file format is sort-of CSV
    // each line starts with a container locator's string representation followed by a
    //  comma-separated list of package identifiers
    EPUB3_EXPORT
    bool                WriteToFile(const string& path)                     const;
    
protected:
    // list of known (but not necessarily loaded) containers
    typedef std::map<string, shared_ptr<Container>>     ContainerLookup;
    
    // if container is loaded, LookupEntry will contain a Package
    // otherwise, the locator is used to load the Container
    typedef std::pair<string, shared_ptr<Package>>      LookupEntry;
    typedef std::map<EPubIdentifier, LookupEntry>       PackageLookup;
    
    ContainerLookup                 _containers;
    PackageLookup                   _packages;
    
    static unique_ptr<Library>      _singleton;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__library__) */
