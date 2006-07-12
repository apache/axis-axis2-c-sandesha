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
#include <sandesha2_seq_report.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_transaction.h>
#include <sandesha2_client_constants.h>
#include <axis2_svc_client.h>
#include <axis2_svc_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_ctx.h>
#include <axis2_property.h>
#include <axis2_log.h>

typedef struct sandesha2_client_impl sandesha2_client_impl_t;

/** 
 * @brief Schema Struct Impl
 *   Axis2 Schema  
 */ 
struct sandesha2_client_impl
{
    sandesha2_client_t client;

};

#define SANDESHA2_INTF_TO_IMPL(client) ((sandesha2_client_impl_t *) client)

axis2_status_t AXIS2_CALL 
sandesha2_client_free(
        void *client,
        const axis2_env_t *envv);

AXIS2_EXTERN sandesha2_client_t * AXIS2_CALL
sandesha2_client_create(
        const axis2_env_t *env)
{
    sandesha2_client_impl_t *client_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    client_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_client_impl_t));

    client_impl->f_ = ;

    client_impl->client.ops = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_client_ops_t)); 
    
    client_impl->super = axis2_hash_make(env);
    if(!client_impl->super) 
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    client_impl->client.ops->free = sandesha2_client_free;

    return &(client_impl->client);
}

axis2_status_t AXIS2_CALL
sandesha2_client_free(
        void *client,
        const axis2_env_t *env)
{
    sandesha2_client_impl_t *client_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    client_impl = SANDESHA2_INTF_TO_IMPL(client);

    if(client_impl->f_namespc)
    {
        AXIS2_URI_FREE(client_impl->f_namespc, env);
        client_impl->f_namespc = NULL;
    }
    
    if((&(client_impl->client))->ops)
    {
        AXIS2_FREE(env->allocator, (&(client_impl->client))->ops);
        (&(client_impl->client))->ops = NULL;
    }

    if(client_impl)
    {
        AXIS2_FREE(env->allocator, client_impl);
        client_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

/**
 * Users can get a seq_report of the seq defined by the information
 * given from the passed svc_client object.
 * 
 * @param service_client
 * @return
 */
sandesha2_seq_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_seq_report_with_svc_client(
        const axis2_env_t *env,
        axis2_svc_client_t *svc_client)
{
    axis2_options_t *options = NULL;
    axis2_endpoint_ref_t *to_epr = NULL;
    axis2_char_t *to = NULL;
    axis2_char_t *seq_key = NULL;
    axis2_svc_ctx_t *svc_ctx = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, svc_client, NULL);

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
    seq_key = (axis2_char_t *) AXIS2_OPTIONS_GET_PROPERTY(options, env, 
            SANDESHA2_CLIENT_SEQ_KEY);
    svc_ctx = AXIS2_SVC_CLIET_GET_SVC_CTX(svc_client, env);
    if(svc_ctx == NULL)
    {
        AXIS2_SANDESHA2_ERROR_SET(env->error, SANDESHA2_ERROR_SVC_CTX_IS_NULL, 
                AXIS2_FAILURE);
        AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, 
                "Service Context is NULL.");
        
    }
    conf_ctx = AXIS2_SVC_CTX_GET_CONF_CTX(svc_ctx, env);    
    internal_seq_id = sandesha2_client_get_internal_seq_id(
            env, to, seq_key);

    return 
        sandesha2_client_get_outgoing_seq_report_with_internal_seq_id(
            env, internal_seq_id, conf_ctx); 

}

sandesha2_seq_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_seq_report_with_seq_key(
        const axis2_env_t *env,
        axis2_char_t *to,
        axis2_char_t *seq_key,
        axis2_conf_ctx_t *conf_ctx)
{
    axis2_char_t *internal_seq_id = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, to, NULL);
    AXIS2_PARAM_CHECK(env->error, seq_key, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);

    internal_seq_id = SANDESHA2_UTIL_GET_INTERNAL_SEQ_ID(to, 
            seq_key);
    return sandesha2_client_get_outgoing_seq_report(env, internal_seq_id, 
            conf_ctx);
}

sandesha2_seq_report_t *AXIS2_CALL
sandesha2_client_get_outgoing_seq_report_with_internal_seq_id(
        const axis2_env_t *env,
        axis2_char_t *internal_seq_id,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_seq_report_t *seq_report = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
    sandesha2_create_seq_mgr_t *create_seq_mgr = NULL;
    sandesha2_create_seq_bean_t *create_seq_find_bean = NULL;
    sandesha2_create_seq_bean_t *create_seq_bean = NULL;
    axis2_conf_t *conf = NULL;
    axis2_ctx_t *ctx = NULL;
    axis2_property_t *property = NULL;
    axis2_bool_t within_transaction = AXIS2_FALSE;
    axis2_bool_t rolled_back = AXIS2_FALSE;
    sandesha2_transaction_t *report_transaction = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, internal_seq_id, NULL);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, NULL);

    seq_report = sandesha2_seq_report_create(env);
    SANDESHSA2_SEQ_REPORT_SET_SEQ_DIRECTION(seq_report, env, 
            SANDESHA2_SEQ_DIRECTION_OUT);

    conf = AXIS2_CONF_CTX_GET_CONF(conf_ctx, env);
    storage_mgr = sandesha2_utils_get_storage_mgr(env, conf_ctx, conf); 
    seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(storage_mgr, env); 
    create_seq_mgr = SANDESHA2_STORAGE_MGR_GET_CREATE_SEQ_MGR(storage_mgr, env); 
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    property = AXIS2_CTX_GET_PROPERTY(ctx, env);
    within_transaction_str = (axis2_char_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(within_transaction_str && 0 == AXIS2_STRCMP(within_transaction_str, 
                SANDESHA2_VALUE_TRUE))
    {
        within_transaction = AXIS2_TRUE;
    }
    if(AXIS2_TRUE != within_transaction)
        report_transaction = SANDESHA2_STORAGE_MGR_GET_TRANSACTION(storage_mgr, 
                env);
    SANDESHA2_SEQ_REPORT_SET_INTERNAL_SEQ_ID(seq_report, env, internal_seq_id);
    create_seq_find_bean = sandesha2_create_seq_bean_create(env);
    SANDESHA2_CREATE_SEQ_BEAN_SET_INTERNAL_SEQ_ID(create_seq_find_bean, 
            env, internal_seq_id);
    create_seq_bean = SANDESHA2_CREATE_SEQ_MGR_FIND_UNIQUE(create_seq_mgr, env, 
            create_seq_find_bean);
	/* if data not is available seq has to be terminated or timedOut.*/
    if(create_seq_bean == NULL)
    {
	    /* check weather this is an terminated seq.*/
        if(AXIS2_TRUE == sandesha2_client_is_seq_terminated(env, internal_seq_id, 
                    seq_prop_mgr))
        {
            sandesha2_client_fill_terminated_outgoing_seq_info(env, seq_report, 
                    internal_seq_id, seq_prop_mgr);
            return seq_report;
        }
        if(AXIS2_TRUE == sandesha2_client_is_seq_timeout(env, internal_seq_id, 
                    seq_prop_mgr))
        {
            sandesha2_client_fill_timeout_outgoing_seq_info(env, seq_report, 
                    internal_seq_id, seq_prop_mgr);
            return seq_report;
        }

        /* seq must have been timed out before establishing. No other
         * posibility I can think of.
         * this does not get recorded since there is no key (which is
         * normally the seqID) to store it.
         * (properties with key as the internalSequenceID get deleted in
         * timing out)
         */

        /* so, setting the seq status to INITIAL */
        SANDESHA2_SEQ_REPORT_SET_SEQ_STATUS(seq_report, env, 
                SANDESHA2_SEQ_STATUS_INITIAL);
		/* returning the current seq report.*/
        return seq_report;
    }
    out_seq_id = SANDESHA2_CREATE_SEQ_BEAN_GET_SEQ_ID(create_seq_bean, env);
    if(!out_seq_id)
    {
        SANDESHA2_SEQ_REPORT_SET_INTERNAL_SEQ_ID(seq_report, nev, 
                internal_seq_id);
        SANDESHA2_SEQ_REPORT_SET_SEQ_STATUS(seq_report, env, 
                SANDESHA2_SEQ_STATUS_INITIAL);
        SANDESHA2_SEQ_REPORT_SET_SEQ_DIRECTION(seq_report, env, 
                SANDESHA2_SEQ_DIRECTION_OUT);
        return seq_report;
    }
    SANDESHA2_SEQ_REPORT_SET_SEQ_STATUS(seq_report, env, 
                SANDESHA2_SEQ_STATUS_ESTABLISHED);
    sandesha2_client_fill_outgoing_seq_info(env, seq_report, out_seq_id, 
            seq_prop_mgr);
   
    return seq_report;
}

/**
 * Users can get a list of seq_reports each describing a incoming
 * sequence, which are the sequences the client work as a RMD.
 * 
 * @param config_ctx
 * @return
 */
axis2_array_list_t *AXIS2_CALL
sandesha2_client_get_incoming_seq_reports(
        axis2_env_t *env,
        axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_
}

	public static ArrayList getIncomingSequenceReports(ConfigurationContext configCtx) throws SandeshaException {

		SandeshaReport report = getSandeshaReport(configCtx);
		ArrayList incomingSequenceIDs = report.getIncomingSequenceList();
		Iterator incomingSequenceIDIter = incomingSequenceIDs.iterator();

		ArrayList incomingSequenceReports = new ArrayList();

		while (incomingSequenceIDIter.hasNext()) {
			String sequnceID = (String) incomingSequenceIDIter.next();
			SequenceReport incomingSequenceReport = getIncomingSequenceReport(sequnceID, configCtx);
			if (incomingSequenceReport == null) {
				throw new SandeshaException("An incoming seq report is not present for the given seqID");
			}
			incomingSequenceReports.add(incomingSequenceReport);
		}

		return incomingSequenceReports;
	}

	/**
	 * SandeshaReport gives the details of all incoming and outgoing seqs.
	 * The outgoing seq have to pass the initial state (CS/CSR exchange) to
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

			internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQ_ID);
			Collection collection = seqPropMgr.find(internalSequenceFindBean);
			Iterator iterator = collection.iterator();
			while (iterator.hasNext()) {
				SequencePropertyBean bean = (SequencePropertyBean) iterator.next();
				String seqID = bean.getSequenceID();
				sandesha2Report.addToOutgoingSequenceList(seqID);
				sandesha2Report.addToOutgoingInternalSequenceMap(seqID, bean.getValue());

				SequenceReport report = getOutgoingSequenceReport(bean.getValue(), configurationContext);

				sandesha2Report.addToNoOfCompletedMessagesMap(seqID, report.getCompletedMessages().size());
				sandesha2Report.addToSequenceStatusMap(seqID, report.getSequenceStatus());
			}

			// incoming seqs
			SequencePropertyBean serverCompletedMsgsFindBean = new SequencePropertyBean();
			serverCompletedMsgsFindBean.setName(Sandesha2Constants.SequenceProperties.SERVER_COMPLETED_MESSAGES);

			Collection serverCompletedMsgsBeans = seqPropMgr.find(serverCompletedMsgsFindBean);
			Iterator iter = serverCompletedMsgsBeans.iterator();
			while (iter.hasNext()) {
				SequencePropertyBean serverCompletedMsgsBean = (SequencePropertyBean) iter.next();
				String seqID = serverCompletedMsgsBean.getSequenceID();
				sandesha2Report.addToIncomingSequenceList(seqID);

				SequenceReport seqReport = getIncomingSequenceReport(seqID, configurationContext);

				sandesha2Report.addToNoOfCompletedMessagesMap(seqID, seqReport.getCompletedMessages().size());
				sandesha2Report.addToSequenceStatusMap(seqID, seqReport.getSequenceStatus());
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
			options.setProperty(SandeshaClientConstants.OFFERED_SEQ_ID, offeredSequenceID);
		}

		// setting a new squenceKey if not already set.
		String seqKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		if (seqKey == null) {
			seqKey = SandeshaUtil.getUUID();
			options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		}

		options.setProperty(SandeshaClientConstants.DUMMY_MESSAGE, Sandesha2Constants.VALUE_TRUE);

		try {
			serviceClient.fireAndForget(null);
		} catch (AxisFault e) {
			throw new SandeshaException(e);
		}

		options.setProperty(SandeshaClientConstants.DUMMY_MESSAGE, Sandesha2Constants.VALUE_FALSE);

	}

	public static void createSequence(ServiceClient serviceClient, boolean offer, String seqKey)
			throws SandeshaException {

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);

		createSequence(serviceClient, offer);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
	}
	
	/**
	 * User can terminate the seq defined by the passed serviceClient.
	 * 
	 * @deprecated
	 */
	public static void createSequnce(ServiceClient serviceClient, boolean offer, String seqKey)
		throws SandeshaException {
		createSequence(serviceClient,offer,seqKey);
	}

	/**
	 * User can terminate the seq defined by the passed serviceClient.
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
				new QName(rmNamespaceValue, Sandesha2Constants.WSRM_COMMON.TERMINATE_SEQ));

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

	public static void terminateSequence(ServiceClient serviceClient, String seqKey) throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		terminateSequence(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
	}

	/**
	 * User can close the seq defined by the passed serviceClient.
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
				new QName(rmNamespaceValue, Sandesha2Constants.WSRM_COMMON.CLOSE_SEQ));

		String oldAction = options.getAction();
		options.setAction(SpecSpecificConstants.getCloseSequenceAction(rmSpecVersion));
		try {
			serviceClient.fireAndForget(closeSequenceBody);
		} catch (AxisFault e) {
			String message = "Could not send the close seq message";
			throw new SandeshaException(message, e);
		} finally {
			options.setAction(oldAction);
		}
	}

	public static void closeSequence(ServiceClient serviceClient, String seqKey) throws SandeshaException {
		// TODO test

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String specVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);
		if (!Sandesha2Constants.SPEC_VERSIONS.v1_1.equals(specVersion)) {
			String message = "Close Sequence feature is only available for WSRM 1.1";
			throw new SandeshaException (message);
		}
		
		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		closeSequence(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
	}

	/**
	 * This blocks the system until the messages u have sent hv been completed.
	 * 
	 * @param serviceClient
	 */
	public static void waitUntilSequenceCompleted(ServiceClient serviceClient) throws SandeshaException {
		waitUntilSequenceCompleted(serviceClient, -1);
	}

	public static void waitUntilSequenceCompleted(ServiceClient serviceClient, String seqKey)
			throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		waitUntilSequenceCompleted(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
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

		SequenceReport seqReport = getOutgoingSequenceReport(serviceClient);
		if (seqReport == null) {
			throw new SandeshaException("Cannnot find a seq report for the given data");
		}

		boolean done = false;
		while (!done) {
			seqReport = getOutgoingSequenceReport(serviceClient);
			int status = seqReport.getSequenceStatus();
			if (status == SequenceReport.SEQ_STATUS_TERMINATED)
				done = true;
			if (status == SequenceReport.SEQ_STATUS_TIMED_OUT)
				done = true;

			if (maxWaitingTime >= 0) {
				long timeNow = System.currentTimeMillis();
				if (timeNow > (startTime + maxWaitingTime))
					done = true;
			}
		}
	}

	public static void waitUntilSequenceCompleted(ServiceClient serviceClient, long maxWaitingTime, String seqKey)
			throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		waitUntilSequenceCompleted(serviceClient, maxWaitingTime);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
	}

	// gives the out seqID if CS/CSR exchange is done. Otherwise a
	// SandeshaException
	public static String getSequenceID(ServiceClient serviceClient) throws SandeshaException {

		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		EndpointReference toEPR = options.getTo();
		if (toEPR == null)
			throw new SandeshaException("To EPR is not set");

		String to = toEPR.getAddress();
		String seqKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);

		ServiceContext serviceContext = serviceClient.getServiceContext();
		if (serviceContext == null)
			throw new SandeshaException("Service context is not set");

		ConfigurationContext configurationContext = serviceContext.getConfigurationContext();

		String internalSequenceID = generateInternalSequenceIDForTheClientSide(to, seqKey);

		SequenceReport seqReport = SandeshaClient.getOutgoingSequenceReport(serviceClient);
		if (seqReport == null)
			throw new SandeshaException("Cannot get a seq report from the given data");

		if (seqReport.getSequenceStatus() != SequenceReport.SEQ_STATUS_ESTABLISHED) {
			throw new SandeshaException(
					"Sequence is not in a active state. Either create seq response has not being received or seq has been terminated,"
							+ " cannot get seqID");
		}

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();

		SequencePropertyBean seqIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQ_ID);
		if (seqIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String seqID = seqIDBean.getValue();
		return seqID;
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

		String seqKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);
		if (rmSpecVersion == null)
			rmSpecVersion = Sandesha2Constants.SPEC_VERSIONS.v1_0;

		if (Sandesha2Constants.SPEC_VERSIONS.v1_0.equals(rmSpecVersion)) {
			throw new SandeshaException("Empty AckRequest messages can only be sent with the v1_1 spec");
		}

		String internalSequenceID = getInternalSequenceID(to, seqKey);

		SequenceReport seqReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID, configContext);
		if (seqReport == null)
			throw new SandeshaException("Cannot generate the seq report for the given internalSequenceID");
		if (seqReport.getSequenceStatus() != SequenceReport.SEQ_STATUS_ESTABLISHED)
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

	public static void sendAckRequest(ServiceClient serviceClient, String seqKey) throws SandeshaException {
		Options options = serviceClient.getOptions();
		if (options == null)
			throw new SandeshaException("Options object is not set");

		String oldSequenceKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		options.setProperty(SandeshaClientConstants.SEQ_KEY, seqKey);
		sendAckRequest(serviceClient);

		options.setProperty(SandeshaClientConstants.SEQ_KEY, oldSequenceKey);
	}

	private static String getInternalSequenceID(String to, String seqKey) {
		return SandeshaUtil.getInternalSequenceID(to, seqKey);
	}

	private static SOAPEnvelope configureCloseSequence(Options options, ConfigurationContext configurationContext)
			throws SandeshaException {

		if (options == null)
			throw new SandeshaException("You must set the Options object before calling this method");

		EndpointReference epr = options.getTo();
		if (epr == null)
			throw new SandeshaException("You must set the toEPR before calling this method");

		String to = epr.getAddress();
		String seqKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);

		String internalSequenceID = SandeshaUtil.getInternalSequenceID(to, seqKey);

		SequenceReport seqReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID,
				configurationContext);
		if (seqReport == null)
			throw new SandeshaException("Cannot generate the seq report for the given internalSequenceID");
		if (seqReport.getSequenceStatus() != SequenceReport.SEQ_STATUS_ESTABLISHED)
			throw new SandeshaException("Canot close the seq since it is not active");

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		SequencePropertyBean seqIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQ_ID);
		if (seqIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String seqID = seqIDBean.getValue();

		if (seqID == null)
			throw new SandeshaException("Cannot find the seqID");

		String rmSpecVersion = (String) options.getProperty(SandeshaClientConstants.RM_SPEC_VERSION);

		if (rmSpecVersion == null)
			rmSpecVersion = SpecSpecificConstants.getDefaultSpecVersion();

		if (!SpecSpecificConstants.isSequenceClosingAllowed(rmSpecVersion))
			throw new SandeshaException("This rm version does not allow seq closing");

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
		identifier.setIndentifer(seqID);
		closeSequence.setIdentifier(identifier);

		closeSequence.toSOAPEnvelope(dummyEnvelope);

		return dummyEnvelope;
	}

	private static boolean isSequenceTerminated(String internalSequenceID, SequencePropertyBeanMgr seqPropMgr)
			throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQ_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Internal seq Bean is not available for the given seq";
			log.debug(message);

			return false;
		}

		String outSequenceID = internalSequenceBean.getSequenceID();

		SequencePropertyBean seqTerminatedBean = seqPropMgr.retrieve(outSequenceID,
				Sandesha2Constants.SequenceProperties.SEQ_TERMINATED);
		if (seqTerminatedBean != null && Sandesha2Constants.VALUE_TRUE.equals(seqTerminatedBean.getValue())) {
			return true;
		}

		return false;
	}

	private static boolean isSequenceTimedout(String internalSequenceID, SequencePropertyBeanMgr seqPropMgr)
			throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQ_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Internal seq Bean is not available for the given seq";
			log.debug(message);

			return false;
		}

		String outSequenceID = internalSequenceBean.getSequenceID();
		SequencePropertyBean seqTerminatedBean = seqPropMgr.retrieve(outSequenceID,
				Sandesha2Constants.SequenceProperties.SEQ_TIMED_OUT);
		if (seqTerminatedBean != null && Sandesha2Constants.VALUE_TRUE.equals(seqTerminatedBean.getValue())) {
			return true;
		}

		return false;
	}

	private static void fillTerminatedOutgoingSequenceInfo(SequenceReport report, String internalSequenceID,
			SequencePropertyBeanMgr seqPropMgr) throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQ_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Not a valid terminated seq. Internal seq Bean is not available for the given seq";
			log.debug(message);

			throw new SandeshaException(message);
		}

		report.setSequenceStatus(SequenceReport.SEQ_STATUS_TERMINATED);

		String outSequenceID = internalSequenceBean.getSequenceID();
		fillOutgoingSequenceInfo(report, outSequenceID, seqPropMgr);
	}

	private static void fillTimedoutOutgoingSequenceInfo(SequenceReport report, String internalSequenceID,
			SequencePropertyBeanMgr seqPropMgr) throws SandeshaException {
		SequencePropertyBean internalSequenceFindBean = new SequencePropertyBean();
		internalSequenceFindBean.setValue(internalSequenceID);
		internalSequenceFindBean.setName(Sandesha2Constants.SequenceProperties.INTERNAL_SEQ_ID);

		SequencePropertyBean internalSequenceBean = seqPropMgr.findUnique(internalSequenceFindBean);
		if (internalSequenceBean == null) {
			String message = "Not a valid timedOut seq. Internal seq Bean is not available for the given seq";
			log.debug(message);

			throw new SandeshaException(message);
		}

		report.setSequenceStatus(SequenceReport.SEQ_STATUS_TIMED_OUT);
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

	private static byte getServerSequenceStatus(String seqID, StorageManager storageManager)
			throws SandeshaException {

		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();

		SequencePropertyBean terminatedBean = seqPropMgr.retrieve(seqID,
				Sandesha2Constants.SequenceProperties.SEQ_TERMINATED);
		if (terminatedBean != null) {
			return SequenceReport.SEQ_STATUS_TERMINATED;
		}

		SequencePropertyBean timedOutBean = seqPropMgr.retrieve(seqID,
				Sandesha2Constants.SequenceProperties.SEQ_TIMED_OUT);
		if (timedOutBean != null) {
			return SequenceReport.SEQ_STATUS_TIMED_OUT;
		}

		NextMsgBeanMgr nextMsgMgr = storageManager.getNextMsgBeanMgr();
		NextMsgBean nextMsgBean = nextMsgMgr.retrieve(seqID);

		if (nextMsgBean != null) {
			return SequenceReport.SEQ_STATUS_ESTABLISHED;
		}

		throw new SandeshaException("Unrecorded seqID");
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

	private static String generateInternalSequenceIDForTheClientSide(String toEPR, String seqKey) {
		return SandeshaUtil.getInternalSequenceID(toEPR, seqKey);
	}

	private static SequenceReport getIncomingSequenceReport(String seqID, ConfigurationContext configCtx)
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

			SequenceReport seqReport = new SequenceReport();

			ArrayList completedMessageList = AcknowledgementManager.getServerCompletedMessagesList(seqID,
					seqPropMgr);

			Iterator iter = completedMessageList.iterator();
			while (iter.hasNext()) {
				;
				seqReport.addCompletedMessage((Long) iter.next());
			}

			seqReport.setSequenceID(seqID);
			seqReport.setInternalSequenceID(seqID); // for the
																// incoming side
																// internalSequenceID=seqID
			seqReport.setSequenceDirection(SequenceReport.SEQ_DIRECTION_IN);

			seqReport.setSequenceStatus(getServerSequenceStatus(seqID, storageManager));

			return seqReport;

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
		String seqKey = (String) options.getProperty(SandeshaClientConstants.SEQ_KEY);
		String internalSequenceID = SandeshaUtil.getInternalSequenceID(to, seqKey);
		SequenceReport seqReport = SandeshaClient.getOutgoingSequenceReport(internalSequenceID,
				configurationContext);
		if (seqReport == null)
			throw new SandeshaException("Cannot generate the seq report for the given internalSequenceID");
		if (seqReport.getSequenceStatus() != SequenceReport.SEQ_STATUS_ESTABLISHED)
			throw new SandeshaException("Canot terminate the seq since it is not active");

		StorageManager storageManager = SandeshaUtil.getSandeshaStorageManager(configurationContext,configurationContext.getAxisConfiguration());
		SequencePropertyBeanMgr seqPropMgr = storageManager.getSequencePropertyBeanMgr();
		SequencePropertyBean seqIDBean = seqPropMgr.retrieve(internalSequenceID,
				Sandesha2Constants.SequenceProperties.OUT_SEQ_ID);
		if (seqIDBean == null)
			throw new SandeshaException("SequenceIdBean is not set");

		String seqID = seqIDBean.getValue();

		if (seqID == null)
			throw new SandeshaException("Cannot find the seqID");

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
		identifier.setIndentifer(seqID);
		terminateSequence.setIdentifier(identifier);
		terminateSequence.toSOAPEnvelope(dummyEnvelope);

		return dummyEnvelope;
	}

}
