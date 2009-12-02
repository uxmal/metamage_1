// Nitrogen/KeychainCore.h
// -----------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_KEYCHAINCORE_H
#define	NITROGEN_KEYCHAINCORE_H

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __KEYCHAINCORE__
#include <SecurityCore/KeychainCore.h>
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_ALIASES_H
#include "Nitrogen/Aliases.h"
#endif

namespace Nitrogen {

	class KeychainErrorsRegistrationDependency
	{
		public:
			KeychainErrorsRegistrationDependency();
	};
	
    using ::KCRef;
    using ::KCItemRef;
    using ::KCSearchRef;
  }

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::KCRef >: public std::unary_function< Nitrogen::KCRef, void >
     {
      void operator()( Nitrogen::KCRef kc ) const
        {
         (void) ::KCReleaseKeychain ( &kc );
        }
     };

   template <> struct Disposer< Nitrogen::KCItemRef >: public std::unary_function< Nitrogen::KCItemRef, void >
     {
      void operator()( Nitrogen::KCItemRef kci ) const
        {
         (void) ::KCReleaseItem ( &kci );
        }
     };

   template <> struct Disposer< Nitrogen::KCSearchRef >: public std::unary_function< Nitrogen::KCSearchRef, void >
     {
      void operator()( Nitrogen::KCSearchRef kcs ) const
        {
         (void) ::KCReleaseSearch ( &kcs );
        }
     };
  }

namespace Nitrogen
  {
	inline void KCSetInteractionAllowed ( Boolean state ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCSetInteractionAllowed ( state ));
		}
		
//	Boolean KCIsInteractionAllowed ( void );
	using ::KCIsInteractionAllowed;
	
	inline Nucleus::Owned<KCRef> KCMakeKCRefFromFSSpec ( const FSSpec &spec ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCMakeKCRefFromFSSpec ( const_cast <FSSpec *> ( &spec ), &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}
	
	inline Nucleus::Owned<KCRef> KCMakeKCRefFromAlias ( AliasHandle keychainAlias ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCMakeKCRefFromAlias ( keychainAlias, &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}
	
	inline Nucleus::Owned<AliasHandle> KCMakeAliasFromKCRef ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		AliasHandle	result;
		ThrowOSStatus ( ::KCMakeAliasFromKCRef ( keychain, &result ));
  	    return Nucleus::Owned<AliasHandle>::Seize( result );
		}

//	extern OSStatus KCReleaseKeychain(KCRef * keychain);
	inline Nucleus::Owned<KCRef> KCGetDefaultKeychain () {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCGetDefaultKeychain ( &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}
	
	inline void KCSetDefaultKeychain ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCSetDefaultKeychain ( keychain ));
		}
	
	inline UInt32 KCGetStatus ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		UInt32	result;
		ThrowOSStatus ( ::KCGetStatus ( keychain, &result ));
  	    return result;
		}
	
	inline Nucleus::Owned<KCRef> KCGetKeychain ( KCItemRef item ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCGetKeychain ( item, &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}


#if 0
/*
 *  KCGetKeychainName()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 */
extern OSStatus 
KCGetKeychainName(
  KCRef       keychain,
  StringPtr   keychainName)                                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

//	extern UInt16 KCCountKeychains(void)
	using ::KCCountKeychains;

	inline Nucleus::Owned<KCRef> KCGetIndKeychain ( UInt16 idx ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCGetIndKeychain ( idx, &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}


#if 0
typedef CALLBACK_API( OSStatus , KCCallbackProcPtr )(KCEvent keychainEvent, KCCallbackInfo *info, void *userContext);
typedef STACK_UPP_TYPE(KCCallbackProcPtr)                       KCCallbackUPP;
/*
 *  NewKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern KCCallbackUPP
NewKCCallbackUPP(KCCallbackProcPtr userRoutine)               AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeKCCallbackUPP(KCCallbackUPP userUPP)                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern OSStatus
InvokeKCCallbackUPP(
  KCEvent           keychainEvent,
  KCCallbackInfo *  info,
  void *            userContext,
  KCCallbackUPP     userUPP)                                  AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

#endif

#if 0
/* High-level interface for retrieving passwords */
/*
 *  KCFindAppleSharePassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindAppleSharePassword(
  AFPServerSignature *  serverSignature,       /* can be NULL */
  StringPtr             serverAddress,         /* can be NULL */
  StringPtr             serverName,            /* can be NULL */
  StringPtr             volumeName,            /* can be NULL */
  StringPtr             accountName,           /* can be NULL */
  UInt32                maxLength,
  void *                passwordData,
  UInt32 *              actualLength,
  KCItemRef *           item)                  /* can be NULL */ AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindInternetPassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindInternetPassword(
  StringPtr    serverName,           /* can be NULL */
  StringPtr    securityDomain,       /* can be NULL */
  StringPtr    accountName,          /* can be NULL */
  UInt16       port,
  OSType       protocol,
  OSType       authType,
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)                 /* can be NULL */        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindInternetPasswordWithPath()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 */
extern OSStatus 
KCFindInternetPasswordWithPath(
  StringPtr    serverName,           /* can be NULL */
  StringPtr    securityDomain,       /* can be NULL */
  StringPtr    accountName,          /* can be NULL */
  StringPtr    path,                 /* can be NULL */
  UInt16       port,
  OSType       protocol,
  OSType       authType,
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)                 /* can be NULL */        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindGenericPassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindGenericPassword(
  StringPtr    serviceName,        /* can be NULL */
  StringPtr    accountName,        /* can be NULL */
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)               /* can be NULL */          AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


#if 0
/* Keychain Manager callbacks */
/*
 *  KCAddCallback()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCAddCallback(
  KCCallbackUPP   callbackProc,
  KCEventMask     eventMask,
  void *          userContext)                                AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCRemoveCallback()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCRemoveCallback(KCCallbackUPP callbackProc)                  AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

	inline Nucleus::Owned<KCItemRef> KCNewItem ( KCItemClass itemClass, OSType itemCreator,
								UInt32 length, const void *data ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCNewItem ( itemClass, itemCreator, length, data, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}

	inline void KCSetAttribute ( KCItemRef item, SecKeychainAttrType tag, UInt32 length, const void *data ) {
		(void) KeychainErrorsRegistrationDependency();

		SecKeychainAttribute attr;
		attr.tag	= tag;
		attr.length = length;
		attr.data	= const_cast<void *> ( data );
		ThrowOSStatus ( ::KCSetAttribute ( item, &attr ));
		}
	

#if 0
<http://developer.apple.com/documentation/Carbon/Reference/Keychain_Manager/keychain_manager/function_group_7.html#//apple_ref/c/func/KCSetAttribute>
/*
 *  KCGetAttribute()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCGetAttribute(
  KCItemRef      item,
  KCAttribute *  attr,
  UInt32 *       actualLength)                                AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


	inline void KCSetData ( KCItemRef item, UInt32 length, const void *data ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCSetData ( item, length, data ));
		}

	inline UInt32 KCGetData ( KCItemRef item, UInt32 maxLength, void *data ) {
		(void) KeychainErrorsRegistrationDependency();
		UInt32 result;
		ThrowOSStatus ( ::KCGetData ( item, maxLength, data, &result ));
		return result;
		}

	inline void KCUpdateItem ( KCItemRef item ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCUpdateItem ( item ));
		}

//	extern OSStatus KCReleaseItem(KCItemRef * item);

	inline Nucleus::Owned<KCItemRef> KCCopyItem ( KCItemRef item, KCRef destKeychain ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCCopyItem ( item, destKeychain, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}


#if 0
/* Searching and enumerating keychain items */
/*
 *  KCFindFirstItem()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindFirstItem(
  KCRef                    keychain,       /* can be NULL */
  const KCAttributeList *  attrList,       /* can be NULL */
  KCSearchRef *            search,
  KCItemRef *              item)                              AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

Marshall sez: this is icky because it returns two things that need to be owned.
#endif

	inline Nucleus::Owned<KCItemRef> KCFindNextItem ( KCSearchRef search ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCFindNextItem ( search, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}

//	extern OSStatus KCReleaseSearch(KCSearchRef * search);

//	!!! Should this be an Nucleus::Owned<KCItemRef> ???
	inline void KCDeleteItem ( KCItemRef item ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCDeleteItem ( item ));
		}

	inline void KCLock ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCLock ( keychain ));
		}

//	KCUnlock is in KeychainHI.h

	}

#endif

