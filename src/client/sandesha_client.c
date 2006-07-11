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
 
#include <sandesha2_client.h>
#include <sandesha2_sequence_report.h>
#include <sandesha2_client_constants.h>
#include <axis2_svc_client.h>
#include <axis2_svc_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_log.h>

typedef struct sandesha2_client_impl sandesha2_client_impl_t;

/** 
 * @brief Schema Struct Impl
 *   Axis2 Schema  
 */ 
struct sandesha2_client_impl
{
    sandesha2_client_t schema;

};

#define SANDESHA2_INTF_TO_IMPL(schema) ((sandesha2_client_impl_t *) schema)

axis2_status_t AXIS2_CALL 
sandesha2_client_free(
        void *schema,
        const axis2_env_t *envv);

AXIS2_EXTERN sandesha2_client_t * AXIS2_CALL
sandesha2_client_create(
        const axis2_env_t *env)
{
    sandesha2_client_impl_t *schema_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    schema_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_client_impl_t));

    schema_impl->f_ = ;

    schema_impl->schema.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_client_ops_t)); 
    
    schema_impl->super = axis2_hash_make(env);
    if(!schema_impl->super) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    schema_impl->schema.ops->free = sandesha2_client_free;

    return &(schema_impl->schema);
}

axis2_status_t AXIS2_CALL
sandesha2_client_free(
        void *schema,
        const axis2_env_t *env)
{
    sandesha2_client_impl_t *schema_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    schema_impl = SANDESHA2_INTF_TO_IMPL(schema);

    if(schema_impl->f_namespc)
    {
        AXIS2_URI_FREE(schema_impl->f_namespc, env);
        schema_impl->f_namespc = NULL;
    }
    
    if((&(schema_impl->schema))->ops)
    {
        AXIS2_FREE(env->allocator, (&(schema_impl->schema))->ops);
        (&(schema_impl->schema))->ops = NULL;
    }

    if(schema_impl)
    {
        AXIS2_FREE(env->allocator, schema_impl);
        schema_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

/**
 * Users can get a sequence_report of the sequence defined by the information
 * given from the passed svc_client object.
 * 
 * @param service_client
 * @return
 */
sandesha2_sequence_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_sequence_report_with_svc_client(
        sandesha2_client_t *sandesha2_client,
        const axis2_env_t *env,
        axis2_svc_client_t *svc_client)
{
    sandesha2_client_impl_t *schema_impl = NULL;
    axis2_options_t *options = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_char_t *to = NULL;
    axis2_char_t *sequence_key = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, svc_client, NULL);
    schema_impl = SANDESHA2_INTF_TO_IMPL(schema);

    options = AXIS2_SVC_CLIENT_GET_OPTIONS(svc_client, env);
    if(options == NULL)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_OPTIONS_OBJECT_NOT_SET, 
                AXIS2_FAILURE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "Options object of the service client not set.");
        return NULL;
    }
    to_epr = AXIS2_OPTIONS_GET_TO(options, env);
    if(to_epr == NULL)
    {
        AXIS2_SANDESHA2_ERROR_SET(env->error, 
                AXIS2_SANDESHA2_ERROR_TO_ADDRESS_IS_NOT_SET, AXIS2_FAILURE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "To address is not set.");
    }
    to = AXIS2_ENDPOINT_REF_GET_ADDRESS(to_epr, env);
    sequence_key = (axis2_char_t *) AXIS2_OPTIONS_GET_PROPERTY(options, env, 
            SANDESHA2_CLIENT_SEQUENCE_KEY);
    svc_ctx = AXIS2_SVC_CLIET_GET_SVC_CTX(svc_client, env);
    if(svc_ctx == NULL)
    {
        AXIS2_SANDESHA2_ERROR_SET(env->error, SANDESHA2_ERROR_SVC_CTX_IS_NULL, 
                AXIS2_FAILURE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "Service Context is NULL.");
        
    }
    conf_ctx = AXIS2_SVC_CTX_GET_CONF_CTX(svc_ctx, env);    
    internal_sequence_id = sandesha2_client_get_internal_sequence_id(
            sandesha2_client, env, to, sequence_key);

    return 
        sandesha2_client_get_outgoing_sequence_report_with_internal_sequence_id(
            sandesha2_client, env, internal_sequence_id, conf_ctx); 

}

sandesha2_sequence_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_sequence_report_with_sequence_key(
        sandesha2_client_t *sandesha2_client,
        const axis2_env_t *env,
        axis2_char_t *to,
        axis2_char_t *sequence_key,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_client_impl_t *schema_impl = NULL;
    axis2_char_t *internal_sequence_id = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, to, NULL);
    AXIS2_PARAM_CHECK(env->error, sequence_key, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    schema_impl = SANDESHA2_INTF_TO_IMPL(schema);

    internal_sequence_id = SANDESHA2_UTIL_GET_INTERNAL_SEQUENCE_ID(to, 
            sequence_key);
    return sandesha2_client_get_outgoing_sequence_report(sandesha2_client, env, 
            internal_sequence_id, conf_ctx);
}

sandesha2_sequence_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_sequence_report_with_sequence_key(
        sandesha2_client_t *sandesha2_client,
        const axis2_env_t *env,
        axis2_char_t *internal_sequence_id,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_client_impl_t *schema_impl = NULL;
    sandesha2_sequence_report_t *sequence_report = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, to, NULL);
    AXIS2_PARAM_CHECK(env->error, sequence_key, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);
    schema_impl = SANDESHA2_INTF_TO_IMPL(schema);

    sequence_report = sandesha2_sequence_report_create(env);
    SANDESHSA2_SEQUENCE_REPORT_SET_SEQUENCE_DIRECTION(sequence_report, env, 
            SANDESHA2_SEQUENCE_DIRECTION_OUT);

    
    
}


	public static SequenceReport getOutgoingSequenceReport(String internalSequenceID,
			ConfigurationContext configurationContext) throws SandeshaException {

		SequenceReport sequenceReport = new SequenceReport();
		sequenceReport.setSequenceDirection(SequenceReport.SEQUENCE_DIRECTION_OUT);

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		CreateSeqBeanMgr createSeqMgr = storageManager.getCreateSeqBeanMgr();

		String withinTransactionStr = (String) configurationContext.getProperty(Sandesha2Constants.WITHIN_TRANSACTION);
		boolean withinTransaction = false;
		if (withinTransactionStr != null && Sandesha2Constants.VALUE_TRUE.equals(withinTransactionStr))
			withinTransaction = true;

		Transaction reportTransaction = null;
		if (!withinTransaction)
			reportTransaction = storageManager.getTransaction();

		boolean rolebacked = false;

		try {

			sequenceReport.setInternalSequenceID(internalSequenceID);

			CreateSeqBean createSeqFindBean = new CreateSeqBean();
			createSeqFindBean.setInternalSequenceID(internalSequenceID);

			CreateSeqBean createSeqBean = createSeqMgr.findUnique(createSeqFindBean);

			// if data not is available sequence has to be terminated or
			// timedOut.
			if (createSeqBean == null) {

				// check weather this is an terminated sequence.
				if (isSequenceTerminated(internalSequenceID, seqPropMgr)) {
					fillTerminatedOutgoingSequenceInfo(sequenceReport, internalSequenceID, seqPropMgr);

					return sequenceReport;
				}

				if (isSequenceTimedout(internalSequenceID, seqPropMgr)) {
					fillTimedoutOutgoingSequenceInfo(sequenceReport, internalSequenceID, seqPropMgr);

					return sequenceReport;
				}

				// sequence must hv been timed out before establiching. No other
				// posibility I can think of.
				// this does not get recorded since there is no key (which is
				// normally the sequenceID) to store it.
				// (properties with key as the internalSequenceID get deleted in
				// timing out)

				// so, setting the sequence status to INITIAL
				sequenceReport.setSequenceStatus(SequenceReport.SEQUENCE_STATUS_INITIAL);

				// returning the current sequence report.
				return sequenceReport;
			}

			String outSequenceID = createSeqBean.getSequenceID();
			if (outSequenceID == null) {
				sequenceReport.setInternalSequenceID(internalSequenceID);
				sequenceReport.setSequenceStatus(SequenceReport.SEQUENCE_STATUS_INITIAL);
				sequenceReport.setSequenceDirection(SequenceReport.SEQUENCE_DIRECTION_OUT);

				return sequenceReport;
			}

			sequenceReport.setSequenceStatus(SequenceReport.SEQUENCE_STATUS_ESTABLISHED);
			fillOutgoingSequenceInfo(sequenceReport, outSequenceID, seqPropMgr);

		} catch (Exception e) {
			if (!withinTransaction && reportTransaction!=null) {
				reportTransaction.rollback();
				configurationContext.setProperty(Sandesha2Constants.WITHIN_TRANSACTION, Sandesha2Constants.VALUE_FALSE);
				rolebacked = true;
			}
		} finally {
			if (!withinTransaction && !rolebacked && reportTransaction!=null) {
				reportTransaction.commit();
				configurationContext.setProperty(Sandesha2Constants.WITHIN_TRANSACTION, Sandesha2Constants.VALUE_FALSE);
			}
		}

		return sequenceReport;
	}

	/**
	 * Users can get a list of sequenceReports each describing a incoming
	 * sequence, which are the sequences the client work as a RMD.
	 * 
	 * @param configCtx
	 * @return
	 * @throws SandeshaException
	 */
	public static ArrayList getIncomingSequenceReports(ConfigurationContext configCtx) throws SandeshaException {

		SandeshaReport report = getSandeshaReport(configCtx);
		ArrayList incomingSequenceIDs = report.getIncomingSequenceList();
		Iterator incomingSequenceIDIter = incomingSequenceIDs.iterator();

		ArrayList incomingSequenceReports = new ArrayList();

		while (incomingSequenceIDIter.hasNext()) {
			String sequnceID = (String) incomingSequenceIDIter.next();
			SequenceReport incomingSequenceReport = getIncomingSequenceReport(sequnceID, configCtx);
			if (incomingSequenceReport == null) {
				throw new SandeshaException("An incoming sequence report is not present for the given sequenceID");
			}
			incomingSequenceReports.add(incomingSequenceReport);
		}

		return incomingSequenceReports;
	}

	/**
	 * SandeshaReport gives the details of all incoming and outgoing sequences.
	 * The outgoing sequence have to pass the initial state (CS/CSR exchange) to
	 * be included in a SandeshaReport
	 * 
	 * @param configurationContext
	 * @return
	 * @throws SandeshaException
	 */
	public static SandeshaReport getSandeshaReport(ConfigurationContext configurationContext) throws SandeshaException {

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		SandeshaReport sandesha2Report = new SandeshaReport();
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();

		String withinTransactionStr = (String) configurationContext.getProperty(Sandesha2Constants.WITHIN_TRANSACTION);
		boolean withinTransaction = false;
		if (withinTransactionStr != null && Sandesha2Constants.VALUE_TRUE.equals(withinTransactionStr))
			withinTransaction = true;

		Transaction reportTransaction = null;
		if (!withinTransaction)
			reportTransaction = storageManager.getTransaction();

		boolean rolebacked = false;

		try {

			internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);
			Collection collection = seqPropMgr.find(internalSequenceFindBean);
			Iterator iterator = collection.iterator();
			while (iterator.hasNext()) {
				SequencePropertyBean bean = (SequencePropertyBean) iterator.next();
				String sequenceID = bean.getSequenceID();
				sandesha2Report.addToOutgoingSequenceList(sequenceID);
				sandesha2Report.addToOutgoingInternalSequenceMap(sequenceID, bean.getValue());

				SequenceReport report = getOutgoingSequenceReport(bean.getValue(), configurationContext);

				sandesha2Report.addToNoOfCompletedMessagesMap(sequenceID, report.getCompletedMessages().size());
				sandesha2Report.addToSequenceStatusMap(sequenceID, report.getSequenceStatus());
			}

			// incoming sequences
			SequencePropertyBean serverCompletedMsgsFindBean = new SequencePropertyBean();
			serverCompletedMsgsFindBean.setName(Sandesha2Constants.SequenceProperties.SERVER_COMPLETED_MESSAGES);

			Collection serverCompletedMsgsBeans = seqPropMgr.find(serverCompletedMsgsFindBean);
			Iterator iter = serverCompletedMsgsBeans.iterator();
			while (iter.hasNext()) {
				SequencePropertyBean serverCompletedMsgsBean = (SequencePropertyBean) iter.next();
				String sequenceID = serverCompletedMsgsBean.getSequenceID();
				sandesha2Report.addToIncomingSequenceList(sequenceID);

				SequenceReport sequenceReport = getIncomingSequenceReport(sequenceID, configurationContext);

				sandesha2Report.addToNoOfCompletedMessagesMap(sequenceID, sequenceReport.getCompletedMessages().size());
				sandesha2Report.addToSequenceStatusMap(sequenceID, sequenceReport.getSequenceStatus());
			}

		} catch (Exception e) {
			if (!withinTransaction && reportTransaction!=null) {
				reportTransaction.rollback();
				rolebacked = true;
			}
		} finally {
			if (!withinTransaction && !rolebacked && reportTransaction!=null) {
				reportTransaction.commit();
			}
		}

		return sandesha2Report;
	}

	public static void createSequence(ServiceClient serviceClient, boolean offer) throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		EndpointReference toEPR = serviceClient.getOptions().getTo();
		if (toEPR == null)
			throw new SandeshaException("ToEPR is not set");

		String to = toEPR.getAddress();
		if (to == null)
			throw new SandeshaException("To EPR is not set");

		if (offer) {
			String offeredSequenceID = SandeshaUtil.getUUID();
			options.setProperty(SandeshaClientConstants.OFFERED_SEQUENCE_ID, offeredSequenceID);
		}

		// setting a new squenceKey if not already set.
		String sequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		if (sequenceKey == null) {
			sequenceKey = SandeshaUtil.getUUID();
			options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		}

		options.setProperty(SandeshaClientConstants.DUMMY_MESSAGE, Sandesha2Constants.VALUE_TRUE);

		try {
			serviceClient.fireAndForget(null);
		} catch (AxisFault e) {
			throw new SandeshaException(e);
		}

		options.setProperty(SandeshaClientConstants.DUMMY_MESSAGE, Sandesha2Constants.VALUE_FALSE);

	}

	public static void createSequence(ServiceClient serviceClient, boolean offer, String sequenceKey)
			throws SandeshaException {

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);

		createSequence(serviceClient, offer);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}
	
	/**
	 * User can terminate the sequence defined by the passed serviceClient.
	 * 
	 * @deprecated
	 */
	public static void createSequnce(ServiceClient serviceClient, boolean offer, String sequenceKey)
		throws SandeshaException {
		createSequence(serviceClient,offer,sequenceKey);
	}

	/**
	 * User can terminate the sequence defined by the passed serviceClient.
	 * 
	 * @param serviceClient
	 * @throws SandeshaException
	 */
	public static void terminateSequence(ServiceClient serviceClient) throws SandeshaException {
		ServiceContext serviceContext = serviceClient.getServiceContext();
		if (serviceContext == null)
			throw new SandeshaException("ServiceContext is null");

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);

		if (rmSpecVersion == null)
			rmSpecVersion = SpecSpecificConstants.getDefaultSpecVersion();

		String rmNamespaceValue = SpecSpecificConstants.getRMNamespaceValue(rmSpecVersion);

		SOAPEnvelope terminateEnvelope = configureTerminateSequence(options, serviceContext.getConfigurationContext());
		OMElement terminateBody = terminateEnvelope.getBody().getFirstChildWithName(
				new QName(rmNamespaceValue, Sandesha2Constants.WSRM_COMMON.TERMINATE_SEQUENCE));

		String oldAction = options.getAction();
		options.setAction(SpecSpecificConstants.getTerminateSequenceAction(rmSpecVersion));

		try {
			serviceClient.fireAndForget(terminateBody);
		} catch (AxisFault e) {
			String message = "Could not send the terminate message";
			throw new SandeshaException(message, e);
		} finally {
			options.setAction(oldAction);
		}
	}

	public static void terminateSequence(ServiceClient serviceClient, String sequenceKey) throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		terminateSequence(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}

	/**
	 * User can close the sequence defined by the passed serviceClient.
	 * 
	 * @param serviceClient
	 * @throws SandeshaException
	 */
	public static void closeSequence(ServiceClient serviceClient) throws SandeshaException {
		ServiceContext serviceContext = serviceClient.getServiceContext();
		if (serviceContext == null)
			throw new SandeshaException("ServiceContext is null");

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);

		if (rmSpecVersion == null)
			rmSpecVersion = SpecSpecificConstants.getDefaultSpecVersion();

		String rmNamespaceValue = SpecSpecificConstants.getRMNamespaceValue(rmSpecVersion);

		SOAPEnvelope closeSequnceEnvelope = configureCloseSequence(options, serviceContext.getConfigurationContext());
		OMElement closeSequenceBody = closeSequnceEnvelope.getBody().getFirstChildWithName(
				new QName(rmNamespaceValue, Sandesha2Constants.WSRM_COMMON.CLOSE_SEQUENCE));

		String oldAction = options.getAction();
		options.setAction(SpecSpecificConstants.getCloseSequenceAction(rmSpecVersion));
		try {
			serviceClient.fireAndForget(closeSequenceBody);
		} catch (AxisFault e) {
			String message = "Could not send the close sequence message";
			throw new SandeshaException(message, e);
		} finally {
			options.setAction(oldAction);
		}
	}

	public static void closeSequence(ServiceClient serviceClient, String sequenceKey) throws SandeshaException {
		// TODO test

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String specVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);
		if (!Sandesha2Constants.SPEC_VERSIONS.v1_1.equals(specVersion)) {
			String message = "Close Sequence feature is only available for WSRM 1.1";
			throw new SandeshaException (message);
		}
		
		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		closeSequence(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}

	/**
	 * This blocks the system until the messages u have sent hv been completed.
	 * 
	 * @param serviceClient
	 */
	public static void waitUntilSequenceCompleted(ServiceClient serviceClient) throws SandeshaException {
		waitUntilSequenceCompleted(serviceClient, -1);
	}

	public static void waitUntilSequenceCompleted(ServiceClient serviceClient, String sequenceKey)
			throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		waitUntilSequenceCompleted(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}

	/**
	 * This blocks the system until the messages u have sent hv been completed
	 * or until the given time interval exceeds. (the time is taken in seconds)
	 * 
	 * @param serviceClient
	 * @param maxWaitingTime
	 */
	public static void waitUntilSequenceCompleted(ServiceClient serviceClient, long maxWaitingTime)
			throws SandeshaException {

		long startTime = System.currentTimeMillis();

		SequenceReport sequenceReport = getOutgoingSequenceReport(serviceClient);
		if (sequenceReport == null) {
			throw new SandeshaException("Cannnot find a sequence report for the given data");
		}

		boolean done = false;
		while (!done) {
			sequenceReport = getOutgoingSequenceReport(serviceClient);
			int status = sequenceReport.getSequenceStatus();
			if (status == SequenceReport.SEQUENCE_STATUS_TERMINATED)
				done = true;
			if (status == SequenceReport.SEQUENCE_STATUS_TIMED_OUT)
				done = true;

			if (maxWaitingTime >= 0) {
				long timeNow = System.currentTimeMillis();
				if (timeNow > (startTime + maxWaitingTime))
					done = true;
			}
		}
	}

	public static void waitUntilSequenceCompleted(ServiceClient serviceClient, long maxWaitingTime, String sequenceKey)
			throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		waitUntilSequenceCompleted(serviceClient, maxWaitingTime);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}

	// gives the out sequenceID if CS/CSR exchange is done. Otherwise a
	// SandeshaException
	public static String getSequenceID(ServiceClient serviceClient) throws SandeshaException {

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		EndpointReference toEPR = options.getTo();
		if (toEPR == null)
			throw new SandeshaException("To EPR is not set");

		String to = toEPR.getAddress();
		String sequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);

		ServiceContext serviceContext = serviceClient.getServiceContext();
		if (serviceContext == null)
			throw new SandeshaException("Service context is not set");

		ConfigurationContext configurationContext = serviceContext.getConfigurationContext();

		String internalSequenceID = generateInternalSequenceIDForTheClientSide(to, sequenceKey);

		SequenceReport sequenceReport = SandeshaClient.getOutgoingSequenceReport(serviceClient);
		if (sequenceReport == null)
			throw new SandeshaException("Cannot get a sequence report from the given data");

		if (sequenceReport.getSequenceStatus() != SequenceReport.SEQUENCE_STATUS_ESTABLISHED) {
			throw new SandeshaException(
					"Sequence is not in a active state. Either create sequence response has not being received or sequence has been terminated,"
							+ " cannot get sequenceID");
		}

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();

		SequencePropertyBean sequenceIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQUENCE_ID);
		if (sequenceIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String sequenceID = sequenceIDBean.getValue();
		return sequenceID;
	}

	public static void sendAckRequest(ServiceClient serviceClient) throws SandeshaException {

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		ServiceContext serviceContext = serviceClient.getServiceContext();
		if (serviceContext == null)
			throw new SandeshaException("ServiceContext is null");

		ConfigurationContext configContext = serviceContext.getConfigurationContext();

		EndpointReference toEPR = options.getTo();
		if (toEPR == null)
			throw new SandeshaException("'To' address is not set is not set");

		String to = toEPR.getAddress();

		String sequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);
		if (rmSpecVersion == null)
			rmSpecVersion = Sandesha2Constants.SPEC_VERSIONS.v1_0;

		if (Sandesha2Constants.SPEC_VERSIONS.v1_0.equals(rmSpecVersion)) {
			throw new SandeshaException("Empty AckRequest messages can only be sent with the v1_1 spec");
		}

		String internalSequenceID = getInternalSequenceID(to, sequenceKey);

		SequenceReport sequenceReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID, configContext);
		if (sequenceReport == null)
			throw new SandeshaException("Cannot generate the sequence report for the given internalSequenceID");
		if (sequenceReport.getSequenceStatus() != SequenceReport.SEQUENCE_STATUS_ESTABLISHED)
			throw new SandeshaException("Canot send the ackRequest message since it is not active");

		String outSequenceID = getSequenceID(serviceClient);

		String soapNamespaceURI = options.getSoapVersionURI();
		SOAPFactory factory = null;
		SOAPEnvelope dummyEnvelope = null;
		if (SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI.equals(soapNamespaceURI)) {
			factory = new SOAP11Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		} else {
			factory = new SOAP12Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		}

		String rmNamespaceValue = SpecSpecificConstants.getRMNamespaceValue(rmSpecVersion);

		AckRequested ackRequested = new AckRequested(factory, rmNamespaceValue);
		Identifier identifier = new Identifier(factory, rmNamespaceValue);
		identifier.setIndentifer(outSequenceID);
		ackRequested.setIdentifier(identifier);

		ackRequested.toSOAPEnvelope(dummyEnvelope);

		OMElement ackRequestedHeaderBlock = dummyEnvelope.getHeader().getFirstChildWithName(
				new QName(rmNamespaceValue, Sandesha2Constants.WSRM_COMMON.ACK_REQUESTED));

		String oldAction = options.getAction();

		options.setAction(SpecSpecificConstants.getAckRequestAction(rmSpecVersion));

		serviceClient.addHeader(ackRequestedHeaderBlock);

		try {
			serviceClient.fireAndForget(null);
		} catch (AxisFault e) {
			String message = "Could not send the ack request";
			throw new SandeshaException(message, e);
		}

		serviceClient.removeHeaders();
		options.setAction(oldAction);
	}

	public static void sendAckRequest(ServiceClient serviceClient, String sequenceKey) throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, sequenceKey);
		sendAckRequest(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQUENCE_KEY, oldSequenceKey);
	}

	private static String getInternalSequenceID(String to, String sequenceKey) {
		return SandeshaUtil.getInternalSequenceID(to, sequenceKey);
	}

	private static SOAPEnvelope configureCloseSequence(Options options, ConfigurationContext configurationContext)
			throws SandeshaException {

		if (options == null)
			throw new SandeshaException("You must set the Options object before calling this method");

		EndpointReference epr = options.getTo();
		if (epr == null)
			throw new SandeshaException("You must set the toEPR before calling this method");

		String to = epr.getAddress();
		String sequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);

		String internalSequenceID = SandeshaUtil.getInternalSequenceID(to, sequenceKey);

		SequenceReport sequenceReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID,
				configurationContext);
		if (sequenceReport == null)
			throw new SandeshaException("Cannot generate the sequence report for the given internalSequenceID");
		if (sequenceReport.getSequenceStatus() != SequenceReport.SEQUENCE_STATUS_ESTABLISHED)
			throw new SandeshaException("Canot close the sequence since it is not active");

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		SequencePropertyBean sequenceIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQUENCE_ID);
		if (sequenceIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String sequenceID = sequenceIDBean.getValue();

		if (sequenceID == null)
			throw new SandeshaException("Cannot find the sequenceID");

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);

		if (rmSpecVersion == null)
			rmSpecVersion = SpecSpecificConstants.getDefaultSpecVersion();

		if (!SpecSpecificConstants.isSequenceClosingAllowed(rmSpecVersion))
			throw new SandeshaException("This rm version does not allow sequence closing");

		SOAPEnvelope dummyEnvelope = null;
		SOAPFactory factory = null;
		String soapNamespaceURI = options.getSoapVersionURI();
		if (SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI.equals(soapNamespaceURI)) {
			factory = new SOAP12Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		} else {
			factory = new SOAP11Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		}

		String rmNamespaceValue = SpecSpecificConstants.getRMNamespaceValue(rmSpecVersion);

		CloseSequence closeSequence = new CloseSequence(factory, rmNamespaceValue);
		Identifier identifier = new Identifier(factory, rmNamespaceValue);
		identifier.setIndentifer(sequenceID);
		closeSequence.setIdentifier(identifier);

		closeSequence.toSOAPEnvelope(dummyEnvelope);

		return dummyEnvelope;
	}

	private static boolean isSequenceTerminated(String internalSequenceID, SequencePropertyBeanMgr seqPropMgr)
			throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Internal sequence Bean is not available for the given sequence";
			log.debug(message);

			return false;
		}

		String outSequenceID = internalSequenceBean.getSequenceID();

		SequencePropertyBean sequenceTerminatedBean = seqPropMgr.retrieve(outSequenceID,
				Sandesha2Constants.SequenceProperties.SEQUENCE_TERMINATED);
		if (sequenceTerminatedBean != null && Sandesha2Constants.VALUE_TRUE.equals(sequenceTerminatedBean.getValue())) {
			return true;
		}

		return false;
	}

	private static boolean isSequenceTimedout(String internalSequenceID, SequencePropertyBeanMgr seqPropMgr)
			throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Internal sequence Bean is not available for the given sequence";
			log.debug(message);

			return false;
		}

		String outSequenceID = internalSequenceBean.getSequenceID();
		SequencePropertyBean sequenceTerminatedBean = seqPropMgr.retrieve(outSequenceID,
				Sandesha2Constants.SequenceProperties.SEQUENCE_TIMED_OUT);
		if (sequenceTerminatedBean != null && Sandesha2Constants.VALUE_TRUE.equals(sequenceTerminatedBean.getValue())) {
			return true;
		}

		return false;
	}

	private static void fillTerminatedOutgoingSequenceInfo(SequenceReport report, String internalSequenceID,
			SequencePropertyBeanMgr seqPropMgr) throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Not a valid terminated sequence. Internal sequence Bean is not available for the given sequence";
			log.debug(message);

			throw new SandeshaException(message);
		}

		report.setSequenceStatus(SequenceReport.SEQUENCE_STATUS_TERMINATED);

		String outSequenceID = internalSequenceBean.getSequenceID();
		fillOutgoingSequenceInfo(report, outSequenceID, seqPropMgr);
	}

	private static void fillTimedoutOutgoingSequenceInfo(SequenceReport report, String internalSequenceID,
			SequencePropertyBeanMgr seqPropMgr) throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQUENCE_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Not a valid timedOut sequence. Internal sequence Bean is not available for the given sequence";
			log.debug(message);

			throw new SandeshaException(message);
		}

		report.setSequenceStatus(SequenceReport.SEQUENCE_STATUS_TIMED_OUT);
		String outSequenceID = internalSequenceBean.getSequenceID();
		fillOutgoingSequenceInfo(report, outSequenceID, seqPropMgr);
	}

	private static void fillOutgoingSequenceInfo(SequenceReport report, String outSequenceID,
			SequencePropertyBeanMgr seqPropMgr) throws SandeshaException {
		report.setSequenceID(outSequenceID);

		ArrayList completedMessageList = AcknowledgementManager.getClientCompletedMessagesList(outSequenceID,
				seqPropMgr);

		Iterator iter = completedMessageList.iterator();
		while (iter.hasNext()) {
			Long lng = new Long(Long.parseLong((String) iter.next()));
			report.addCompletedMessage(lng);
		}
	}

	private static byte getServerSequenceStatus(String sequenceID, StorageManager storageManager)
			throws SandeshaException {

		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();

		SequencePropertyBean terminatedBean = seqPropMgr.retrieve(sequenceID,
				Sandesha2Constants.SequenceProperties.SEQUENCE_TERMINATED);
		if (terminatedBean != null) {
			return SequenceReport.SEQUENCE_STATUS_TERMINATED;
		}

		SequencePropertyBean timedOutBean = seqPropMgr.retrieve(sequenceID,
				Sandesha2Constants.SequenceProperties.SEQUENCE_TIMED_OUT);
		if (timedOutBean != null) {
			return SequenceReport.SEQUENCE_STATUS_TIMED_OUT;
		}

		NextMsgBeanMgr nextMsgMgr = storageManager.getNextMsgBeanMgr();
		NextMsgBean nextMsgBean = nextMsgMgr.retrieve(sequenceID);

		if (nextMsgBean != null) {
			return SequenceReport.SEQUENCE_STATUS_ESTABLISHED;
		}

		throw new SandeshaException("Unrecorded sequenceID");
	}

	private class DummyCallback extends Callback {

		public void onComplete(AsyncResult result) {
			// TODO Auto-generated method stub
			System.out.println("Error: dummy callback was called");
		}

		public void onError(Exception e) {
			// TODO Auto-generated method stub
			System.out.println("Error: dummy callback received an error");

		}

	}

	private static String generateInternalSequenceIDForTheClientSide(String toEPR, String sequenceKey) {
		return SandeshaUtil.getInternalSequenceID(toEPR, sequenceKey);
	}

	private static SequenceReport getIncomingSequenceReport(String sequenceID, ConfigurationContext configCtx)
			throws SandeshaException {

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configCtx,configCtx.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();

		String withinTransactionStr = (String) configCtx.getProperty(Sandesha2Constants.WITHIN_TRANSACTION);
		boolean withinTransaction = false;
		if (withinTransactionStr != null && Sandesha2Constants.VALUE_TRUE.equals(withinTransactionStr))
			withinTransaction = true;

		Transaction reportTransaction = null;
		if (!withinTransaction)
			reportTransaction = storageManager.getTransaction();

		boolean rolebacked = false;

		try {

			SequenceReport sequenceReport = new SequenceReport();

			ArrayList completedMessageList = AcknowledgementManager.getServerCompletedMessagesList(sequenceID,
					seqPropMgr);

			Iterator iter = completedMessageList.iterator();
			while (iter.hasNext()) {
				;
				sequenceReport.addCompletedMessage((Long) iter.next());
			}

			sequenceReport.setSequenceID(sequenceID);
			sequenceReport.setInternalSequenceID(sequenceID); // for the
																// incoming side
																// internalSequenceID=sequenceID
			sequenceReport.setSequenceDirection(SequenceReport.SEQUENCE_DIRECTION_IN);

			sequenceReport.setSequenceStatus(getServerSequenceStatus(sequenceID, storageManager));

			return sequenceReport;

		} catch (Exception e) {
			if (!withinTransaction && reportTransaction!=null) {
				reportTransaction.rollback();
				configCtx.setProperty(Sandesha2Constants.WITHIN_TRANSACTION, Sandesha2Constants.VALUE_FALSE);
				rolebacked = true;
			}
		} finally {
			if (!withinTransaction && !rolebacked && reportTransaction!=null) {
				reportTransaction.commit();
				configCtx.setProperty(Sandesha2Constants.WITHIN_TRANSACTION, Sandesha2Constants.VALUE_FALSE);
			}
		}

		return null;
	}

	private static SOAPEnvelope configureTerminateSequence(Options options, ConfigurationContext configurationContext)
			throws SandeshaException {

		if (options == null)
			throw new SandeshaException("You must set the Options object before calling this method");

		EndpointReference epr = options.getTo();
		if (epr == null)
			throw new SandeshaException("You must set the toEPR before calling this method");

		String to = epr.getAddress();
		String sequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQUENCE_KEY);
		String internalSequenceID = SandeshaUtil.getInternalSequenceID(to, sequenceKey);
		SequenceReport sequenceReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID,
				configurationContext);
		if (sequenceReport == null)
			throw new SandeshaException("Cannot generate the sequence report for the given internalSequenceID");
		if (sequenceReport.getSequenceStatus() != SequenceReport.SEQUENCE_STATUS_ESTABLISHED)
			throw new SandeshaException("Canot terminate the sequence since it is not active");

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		SequencePropertyBean sequenceIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQUENCE_ID);
		if (sequenceIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String sequenceID = sequenceIDBean.getValue();

		if (sequenceID == null)
			throw new SandeshaException("Cannot find the sequenceID");

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);
		if (rmSpecVersion == null)
			rmSpecVersion = SpecSpecificConstants.getDefaultSpecVersion();

		options.setAction(SpecSpecificConstants.getTerminateSequenceAction(rmSpecVersion));
		SOAPEnvelope dummyEnvelope = null;
		SOAPFactory factory = null;
		String soapNamespaceURI = options.getSoapVersionURI();
		if (SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI.equals(soapNamespaceURI)) {
			factory = new SOAP12Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		} else {
			factory = new SOAP11Factory();
			dummyEnvelope = factory.getDefaultEnvelope();
		}

		String rmNamespaceValue = SpecSpecificConstants.getRMNamespaceValue(rmSpecVersion);
		TerminateSequence terminateSequence = new TerminateSequence(factory, rmNamespaceValue);
		Identifier identifier = new Identifier(factory, rmNamespaceValue);
		identifier.setIndentifer(sequenceID);
		terminateSequence.setIdentifier(identifier);
		terminateSequence.toSOAPEnvelope(dummyEnvelope);

		return dummyEnvelope;
	}

}
