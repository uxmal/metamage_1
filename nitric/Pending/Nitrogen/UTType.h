// UTType.h

#ifndef	NITROGEN_UTTYPE_H
#define	NITROGEN_UTTYPE_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __UTTYPE__
#include "LaunchServices/UTType.h"
#endif

// #ifndef __CORESERVICES__
// #include <CoreServices/CoreServices.h>
// #endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_CFURL_H
#include "Nitrogen/CFURL.h"
#endif

namespace Nitrogen {

	struct UTTypeCreatePreferredIdentifierForTag_Failed {};

	inline Owned<CFStringRef> UTTypeCreatePreferredIdentifierForTag ( CFStringRef inTagClass, 
				CFStringRef inTag, CFStringRef inConformingToTypeIdentifier = NULL ) {
		CFStringRef result = ::UTTypeCreatePreferredIdentifierForTag ( inTagClass, inTag, inConformingToTypeIdentifier );
//  Result:
//    a new CFStringRef containing the type identifier, or NULL if
//    inTagClass is not a known tag class
		if ( result == NULL )
			throw UTTypeCreatePreferredIdentifierForTag_Failed ();
		return Owned<CFStringRef>::Seize ( result );
		}
	
	struct UTTypeCreateAllIdentifiersForTag_Failed {};
	
	inline Owned<CFArrayRef> UTTypeCreateAllIdentifiersForTag ( CFStringRef inTagClass, 
				CFStringRef inTag, CFStringRef inConformingToTypeIdentifier = NULL ) {
		CFArrayRef result = ::UTTypeCreateAllIdentifiersForTag ( inTagClass, inTag, inConformingToTypeIdentifier );
//  Result:
//    An array of uniform type identifiers, or NULL if inTagClass is
//    not a known tag class
		if ( result == NULL )
			throw UTTypeCreateAllIdentifiersForTag_Failed ();
		return Owned<CFArrayRef>::Seize ( result );
		}

	struct UTTypeCopyPreferredTagWithClass_Failed {};
	
	inline Owned<CFStringRef> UTTypeCopyPreferredTagWithClass ( CFStringRef inTypeIdentifier, CFStringRef inTagClass ) {
		CFStringRef result = ::UTTypeCopyPreferredTagWithClass ( inTypeIdentifier, inTagClass );
//  Result:
//    An array of uniform type identifiers, or NULL if inTagClass is
//    not a known tag class
		if ( result == NULL )
			throw UTTypeCopyPreferredTagWithClass_Failed ();
		return Owned<CFStringRef>::Seize ( result );
		}

//	extern Boolean UTTypeEqual ( CFStringRef inTypeIdentifier1, CFStringRef inTypeIdentifier2 );
	using ::UTTypeEqual;

//	extern Boolean UTTypeConformsTo ( CFStringRef inTypeIdentifier, CFStringRef inConformsToTypeIdentifier );
	using ::UTTypeConformsTo;


	struct UTTypeCopyDescription_Failed {};
	
	inline Owned<CFStringRef> UTTypeCopyDescription ( CFStringRef inTypeIdentifier ) {
		CFStringRef result = ::UTTypeCopyDescription ( inTypeIdentifier );
//  Result:
//    a localized string, or NULL of no type description is available
		if ( result == NULL )
			throw UTTypeCopyDescription_Failed ();
		return Owned<CFStringRef>::Seize ( result );
		}

	struct UTTypeCopyDeclaration_Failed {};
	
	inline Owned<CFDictionaryRef> UTTypeCopyDeclaration ( CFStringRef inTypeIdentifier ) {
		CFDictionaryRef result = ::UTTypeCopyDeclaration ( inTypeIdentifier );
//  Result:
//    a tag declaration dictionary, or NULL if the type is not declared
		if ( result == NULL )
			throw UTTypeCopyDeclaration_Failed ();
		return Owned<CFDictionaryRef>::Seize ( result );
		}

	struct UTTypeCopyDeclaringBundleURL_Failed {};
	
	inline Owned<CFURLRef> UTTypeCopyDeclaringBundleURL ( CFStringRef inTypeIdentifier ) {
		CFURLRef result = ::UTTypeCopyDeclaringBundleURL ( inTypeIdentifier );
//  Result:
//    a URL, or NULL if the bundle cannot be located.
		if ( result == NULL )
			throw UTTypeCopyDeclaringBundleURL_Failed ();
		return Owned<CFURLRef>::Seize ( result );
		}


	struct UTCreateStringForOSType_Failed {};
	
	inline Owned<CFStringRef> UTCreateStringForOSType ( OSType inOSType ) {
		CFStringRef result = ::UTCreateStringForOSType ( inOSType );
//  Result:
//    a new CFString representing the OSType, or NULL if the argument
//    is 0 or '????'
		if ( result == NULL )
			throw UTCreateStringForOSType_Failed ();
		return Owned<CFStringRef>::Seize ( result );
		}

	struct UTGetOSTypeFromString_Failed {};

	inline OSType UTGetOSTypeFromString ( CFStringRef inString ) {
		OSType retVal = ::UTGetOSTypeFromString ( inString );
//  Result:
//    the OSType value encoded in the string, or 0 if the string is not
//    a valid encoding of an OSType
		if ( retVal == 0 )
			throw UTGetOSTypeFromString_Failed ();
		return retVal;
		}
	
	}
	
#endif

