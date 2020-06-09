//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	CIFXModifierChain.h

			The header file that defines the base implementation class of the
			CIFXModifierChain. 
*/

#ifndef CIFXModifierChain_H
#define CIFXModifierChain_H

#include "IFXModifierChainInternal.h"
#include "CIFXSubject.h"
#include "IFXModifierDataPacketInternal.h"
#include "IFXDidRegistry.h"
#include "IFXModifier.h"
#include "IFXSet.h"
#include "IFXClock.h"

class IFXModifierChainState;

/*
Notes on impl and changes,

In general the modifier chian is agressive about forwarding invalidations and lazy
about propagating validations.

ModifierDataPacket and Modifier hold weak reference to the
modifier chain. Additionally the modifier chain holds hard references to both
the DataPackets and the Modifiers. The modifier chain hanging on to the data packets
provides a significant opportunity for performance.

Proxy data packet is used to manage Dependencies between the prepended chain and this
chain, the proxy datapacket handles the Input Invalidation Sequence for the
prepended modifier chain.

Invalidation: Invalidation are currently propagated down the entire chain when they
happen, however the propagation detects previous invalidation and does not
re invalidate items. This decision was made to speed the checking that must be done 
during GetDataElement, so that the entire dep chain does not need to be checked 
every time and element is retrieved.

NOTE: Validations: only data elements that have been requested directly or indirectly
are validated and the validatation is propagated to the requesting data packet,
which may leave several intermedate data packets invalid.

TBD: DataElements Do not Yet have the flags set correctly for supporting Unknown and
Rendererable

TBD: Consumption: Is not yet detected or proceessed.

ISSUE: what to do in the case of DeleteModelResource when a mod chain really needs
to go away but a model is appended to it, and requires inputs not provided by
the default model resource?

ISSUE: Add modifier to a previous Model Resource Chain may make the chain invalid
for as a input for appended chain. For example Adding SDS to a Model Resource
may cause the consumption of vertex weights generated by a previous Model resource
modifier. If the appended Model has a skin modifier, this model resource may
no longer be a valid input for the Model. In Any case the dependent modifier
chain should be notified of the change. SOLUTION: Don't allow the removal

ISSUE: Validate inputs for calculations, to get the proper values into the Data
packet for a given modifier may over write still valid outputs from the modifier
-- this can be solved by giving the modifier both the it's data packet and it's
input dependience, which has the nice property of reducing the amount of logic
enecessary in process  Dependencies. All input dependencies will be up dated
when the Get data element  call is made. This solution has been implementied.

Data Element Attributes: Because the validatitiy (or posible validity) of the
inputs required by a modifier must be able to be detrmined when a modifier is
added. If we allow data elements to have invalidations overridin then we can
have situations where the inputs to a modifier may be intermitently valid.
This would be problematic in that we would modifiers that could not ever
have there inputs validated. So to solve this problem we have decided to
*/

/**
	The CIFXModifierChain component supports the IFXModifierChainInternal, 
	IFXObserver interfaces and CIFXSubject implementation. 
*/
class CIFXModifierChain : private CIFXSubject,
                  virtual public     IFXModifierChainInternal,
				  virtual public	 IFXObserver
{
public:
	// IFXUnknown
	U32		  IFXAPI AddRef ();
	U32		  IFXAPI Release ();
	IFXRESULT IFXAPI QueryInterface (IFXREFIID riid, void **ppv);

	// IFXModifierChain
	IFXRESULT IFXAPI Initialize();
	IFXRESULT IFXAPI PrependModifierChain( IFXModifierChain* pInModifierChain );
	IFXRESULT IFXAPI AddModifier(IFXModifier& rInModifier, U32 index, BOOL isReqValidation);

	IFXRESULT IFXAPI SetModifier(IFXModifier& rInModifier, U32 index, BOOL isReqValidation);

	IFXRESULT IFXAPI RemoveModifier(U32 index);

	IFXRESULT IFXAPI GetDataPacket( IFXModifierDataPacket*& rpOutDataPacket );

	IFXRESULT IFXAPI GetModifierCount( U32& rOutModifierCount );
	IFXRESULT IFXAPI GetModifier( U32 index, IFXModifier*& rpOutModifier );

	IFXRESULT IFXAPI SetClock( IFXSubject* pInClockSubject );

	// IFXModifierChainInternal
	IFXRESULT IFXAPI Invalidate( 
							U32 invalidDataElementIndex,
							U32 modifierIndex );
	IFXRESULT IFXAPI ProcessDependencies( 
							U32 dataElementIndex,
							U32 modifierIndex );
	IFXRESULT IFXAPI AddAppendedModifierChain( 
							IFXModifierChainInternal* pModChain);
	IFXRESULT IFXAPI RemoveAppendedModifierChain( 
							IFXModifierChainInternal* pModChain);
	IFXRESULT IFXAPI RebuildDataPackets(BOOL isReqValidation);
	IFXRESULT IFXAPI BuildCachedState( 
							IFXModifierDataPacketInternal* pDP, 
							BOOL isReqValidation );

	IFXRESULT IFXAPI RestoreOldState();
	IFXRESULT IFXAPI ClearOldState();

	BOOL	  IFXAPI NeedTime();
	void	  IFXAPI RecheckNeedTime();

	IFXRESULT IFXAPI GetDEState(U32, IFXDataElementState**);
	IFXRESULT IFXAPI GetIntraDeps(IFXIntraDependencies**);
	IFXRESULT IFXAPI NotifyActive();

	// IFXObserver (for time mgmt )
	IFXRESULT IFXAPI Update(
						IFXSubject* pInSubject, 
						U32 changeBits,
						IFXREFIID rIType );
private:
	// IFXUnknown
	U32 m_refCount;

	// IFXModifierChain
	IFXModifierChainState* m_pModChainState;///< real state, if success
	IFXModifierChainState* m_pCachedState;///< populated in rebuild:  potential new state
	IFXModifierChainState* m_pOldState;   ///< rollback state

	BOOL m_bNeedTime; ///< Does this mod chain or an appended chain need the time
	BOOL m_bInApplyState; ///< Are we currently in an apply state call

	U32             m_Time;
	IFXClock*       m_pClockNR; ///< serves as signal if we are attached to the clock
	IFXSubject*     m_pClockSubjectNR;

	IFXDidRegistry* m_pDidRegistry;

	/**
	An unrefed list of modifier chains that have had this chain prepended to them.
	*/
	IFXSet<IFXModifierChainInternal*> m_appendedChains;

	/**
	Destroy and clean up the modifier chain
	*/
	void Destruct();

	/**
	Builds a new state for the modifier chain. May fail if
	input requirements are violated.

	@param	pBaseChain

	@param	pInOverrideDP

	@param	modIdx	
			May be 0 to NumModifers, or INVALID_MODIFIER_INDEX
			to indicate no modifier to add or remove.

	@param	pInMod	
			The address of the modifier to add, or if is Null and
			modIdx != INVALID_MODIFIER_INDEX then modIdx = the index
			of the modifier to remove.

	@param	ppOutModChainState

	@param	bReplace

	@param	isReqValidation

	@return	IFXRESULT code.
	*/
	IFXRESULT BuildNewModifierState(
						IFXModifierChainInternal* pBaseChain,
						IFXModifierDataPacketInternal* pInOverrideDP,
						U32 modIdx,
						IFXModifier* pInMod,
						IFXModifierChainState** ppOutModChainState,
						BOOL bReplace,
						BOOL isReqValidation);


	/** 
	Takes a New Modifier Chain State and attempts to set it as
	the active modifier chain state. Side effects will delete @e pInNewState
	if application fails, if application succeeds @e m_pOldState will hold the
	last valid state of the mod chain. Which must either be directly deleted
	via call to ClearOldState or RestoreOldState.  This call whether it succeeds
	or fails clears the old and cached states off of the appended chain hier.
	*/
	IFXRESULT ApplyNewModifierState( IFXModifierChainState* pInNewState );

			CIFXModifierChain();
	virtual ~CIFXModifierChain();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXModifierChain_Factory(IFXREFIID iid, void** ppv);
};

#endif
