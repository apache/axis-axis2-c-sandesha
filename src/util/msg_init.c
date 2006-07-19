/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <sandesha2_msg_init.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_spec_specific_consts.h>
#include <sandesha2_utils.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_create_seq.h>
#include <sandesha2_create_seq_res.h>
#include <sandesha2_acks_to.h>
#include <sandesha2_address.h>
#include <sandesha2_seq_offer.h>
#include <sandesha2_accept.h>
#include "../client/sandesha2_client_constants.h"

#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_msg_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>
#include <axis2_uuid_gen.h>
#include <axis2_addr.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <axiom_node.h>


/**
 * Called to create a rmMessageContext out of an message context. Finds out things like rm version and message type
 * as well.
 * 
 * @param ctx
 * @param assumedRMNamespace
 * this is used for validation (to find out weather the rmNamespace of the current message
 * is equal to the regietered rmNamespace of the sequence). 
 * If null validation will not happen.
 * 
 * @return
 * @throws SandeshaException
 */
public static RMMsgContext initializeMessage(MessageContext ctx)
        throws SandeshaException {
    RMMsgContext rmMsgCtx = new RMMsgContext(ctx);
    
    populateRMMsgContext(ctx, rmMsgCtx);
    validateMessage(rmMsgCtx);
    return rmMsgCtx;
}

	/**
	 * Adds the message parts the the RMMessageContext.
	 * 
	 * @param msgCtx
	 * @param rmMsgContext
	 */
	private static void populateRMMsgContext(MessageContext msgCtx,
			RMMsgContext rmMsgContext) throws SandeshaException {

		//if client side and the addressing version is not set. assuming the default addressing version
		String addressingNamespace = (String) msgCtx.getProperty(AddressingConstants.WS_ADDRESSING_VERSION);
		if (addressingNamespace==null && !msgCtx.isServerSide())
			addressingNamespace = AddressingConstants.Final.WSA_NAMESPACE;
		
		RMElements elements = new RMElements(addressingNamespace);
		elements.fromSOAPEnvelope(msgCtx.getEnvelope(), msgCtx.getWSAAction());

		String rmNamespace = null;
		
		if (elements.getCreateSequence() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.CREATE_SEQ,
					elements.getCreateSequence());
			rmNamespace = elements.getCreateSequence().getNamespaceValue();
		}

		if (elements.getCreateSequenceResponse() != null) {
			rmMsgContext.setMessagePart(
					Sandesha2Constants.MessageParts.CREATE_SEQ_RESPONSE, elements
							.getCreateSequenceResponse());
			rmNamespace = elements.getCreateSequenceResponse().getNamespaceValue();
		}

		if (elements.getSequence() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.SEQUENCE,
					elements.getSequence());
			rmNamespace = elements.getSequence().getNamespaceValue();	
		}

		if (elements.getSequenceAcknowledgement() != null) {
			rmMsgContext.setMessagePart(
					Sandesha2Constants.MessageParts.SEQ_ACKNOWLEDGEMENT, elements
							.getSequenceAcknowledgement());
			rmNamespace = elements.getSequenceAcknowledgement().getNamespaceValue();
		}

		if (elements.getTerminateSequence() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.TERMINATE_SEQ,
					elements.getTerminateSequence());
			rmNamespace = elements.getTerminateSequence().getNamespaceValue();
		}
		
		if (elements.getTerminateSequenceResponse() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.TERMINATE_SEQ_RESPONSE,
					elements.getTerminateSequenceResponse());
			rmNamespace = elements.getTerminateSequenceResponse().getNamespaceValue();
		}

		if (elements.getAckRequested() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.ACK_REQUEST,
					elements.getAckRequested());
			rmNamespace = elements.getAckRequested().getNamespaceValue();
		}
		
		if (elements.getCloseSequence() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.CLOSE_SEQUENCE,
					elements.getCloseSequence());
			rmNamespace = elements.getCloseSequence().getNamespaceValue();
		}
		
		if (elements.getCloseSequenceResponse() != null) {
			rmMsgContext.setMessagePart(Sandesha2Constants.MessageParts.CLOSE_SEQUENCE_RESPONSE,
					elements.getCloseSequenceResponse());
			rmNamespace = elements.getCloseSequenceResponse().getNamespaceValue();
		}
		
		rmMsgContext.setRMNamespaceValue(rmNamespace);
		
		String addressingNamespaceValue = elements.getAddressingNamespaceValue();
		if (addressingNamespaceValue!=null)
			rmMsgContext.setAddressingNamespaceValue(addressingNamespaceValue);
	}

	/**
	 * This is used to validate the message.
	 * Also set an Message type. Possible types are given in the Sandesha2Constants.MessageTypes interface.
	 * 
	 * @param rmMsgCtx
	 * @return
	 * @throws SandeshaException
	 */
	private static boolean validateMessage(RMMsgContext rmMsgCtx)
			throws SandeshaException {

		ConfigurationContext configContext = rmMsgCtx.getMessageContext().getConfigurationContext();
		AxisConfiguration axisConfiguration = configContext.getAxisConfiguration();
		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configContext,axisConfiguration);
		SequencePropertyBeanMgr sequencePropertyBeanMgr = storageManager.getSequencePropertyBeanMgr();
		
		String sequenceID = null;
		
		CreateSequence createSequence = (CreateSequence) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.CREATE_SEQ);
		CreateSequenceResponse createSequenceResponse = (CreateSequenceResponse) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.CREATE_SEQ_RESPONSE);
		TerminateSequence terminateSequence = (TerminateSequence) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.TERMINATE_SEQ);
		TerminateSequenceResponse terminateSequenceResponse = (TerminateSequenceResponse) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.TERMINATE_SEQ_RESPONSE);
		SequenceAcknowledgement sequenceAcknowledgement = (SequenceAcknowledgement) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.SEQ_ACKNOWLEDGEMENT);
		Sequence sequence = (Sequence) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.SEQUENCE);
		AckRequested ackRequest = (AckRequested) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.ACK_REQUEST);
		CloseSequence closeSequence = (CloseSequence) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.CLOSE_SEQUENCE);
		CloseSequenceResponse closeSequenceResponse = (CloseSequenceResponse) rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.CLOSE_SEQUENCE_RESPONSE);
		
		//Setting message type.
		if (createSequence != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.CREATE_SEQ);
		}else if (createSequenceResponse != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.CREATE_SEQ_RESPONSE);
			sequenceID = createSequenceResponse.getIdentifier().getIdentifier();
		}else if (terminateSequence != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.TERMINATE_SEQ);
			sequenceID = terminateSequence.getIdentifier().getIdentifier();
		}else if (terminateSequenceResponse != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.TERMINATE_SEQ_RESPONSE);
			sequenceID = terminateSequenceResponse.getIdentifier().getIdentifier();
		}else if (rmMsgCtx.getMessagePart(Sandesha2Constants.MessageParts.SEQUENCE) != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.APPLICATION);
			sequenceID = sequence.getIdentifier().getIdentifier();
		} else if (sequenceAcknowledgement != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.ACK);
			sequenceID = sequenceAcknowledgement.getIdentifier().getIdentifier();
		} else if (ackRequest != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.ACK_REQUEST);
			sequenceID = ackRequest.getIdentifier().getIdentifier(); 
		} else if (closeSequence != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.CLOSE_SEQUENCE);
			sequenceID = closeSequence.getIdentifier().getIdentifier(); 
		} else if (closeSequenceResponse != null) {
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.CLOSE_SEQUENCE_RESPONSE);
			sequenceID = closeSequenceResponse.getIdentifier().getIdentifier(); 
		} else
			rmMsgCtx.setMessageType(Sandesha2Constants.MessageTypes.UNKNOWN);

		String propertyKey = null;
		if (rmMsgCtx.getMessageContext().getFLOW()==MessageContext.IN_FLOW) {
			propertyKey = sequenceID;
		} else {
			SequencePropertyBean internalSequenceIDBean = sequencePropertyBeanMgr.retrieve(sequenceID,Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);
			if (internalSequenceIDBean!=null) {
				propertyKey = internalSequenceIDBean.getValue();
			}
		}
		
        String rmNamespace = rmMsgCtx.getRMNamespaceValue();
        if (sequenceID!=null) {
        	String specVersion = SandeshaUtil.getRMVersion(propertyKey,storageManager);
    		
        	String sequenceRMNamespace = null;
        	if (specVersion!=null)
    			sequenceRMNamespace = SpecSpecificConstants.getRMNamespaceValue(specVersion);
    		if (sequenceRMNamespace!=null && rmNamespace!=null) {
    			if (!sequenceRMNamespace.equals(rmNamespace)) {
    				throw new SandeshaException ("Given message has rmNamespace value, which is different from the " +
    						"reqistered namespace for the sequence");
    			}
    		}
        }
		
		return true;
	}

}
