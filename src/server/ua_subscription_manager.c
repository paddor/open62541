#ifdef ENABLESUBSCRIPTIONS
#include "ua_types.h"
#include "ua_server_internal.h"
#include "ua_subscription_manager.h"

#include <stdio.h> // Remove later, debugging only

void SubscriptionManager_init(UA_Server *server) {
    UA_SubscriptionManager *manager = &(server->subscriptionManager);

    /* FIXME: These init values are empirical. Maybe they should be part
     *        of the server config? */
    manager->GlobalPublishingInterval      = (UA_Int32_BoundedValue)  { .maxValue = 100,   .minValue = 0, .currentValue=0 };
    manager->GlobalLifeTimeCount           = (UA_UInt32_BoundedValue) { .maxValue = 15000, .minValue = 0, .currentValue=0 };
    manager->GlobalKeepAliveCount          = (UA_UInt32_BoundedValue) { .maxValue = 100,   .minValue = 0, .currentValue=0 };
    manager->GlobalNotificationsPerPublish = (UA_Int32_BoundedValue)  { .maxValue = 1000,  .minValue = 1, .currentValue=0 };
    manager->GlobalSamplingInterval        = (UA_UInt32_BoundedValue) { .maxValue = 100,   .minValue = 0, .currentValue=0 };
    manager->GlobalQueueSize               = (UA_UInt32_BoundedValue) { .maxValue = 100,   .minValue = 0, .currentValue=0 };
    
    manager->ServerSubscriptions = (UA_ListOfUASubscriptions *) malloc (sizeof(UA_ListOfUASubscriptions));
    LIST_INIT(manager->ServerSubscriptions);
    
    // Do this after the malloc to provide some basic degree of entropy
    manager->LastSessionID = (UA_UInt32) (server->random_seed + (UA_UInt32)UA_DateTime_now());
    return;
}

UA_Subscription *UA_Subscription_new(UA_Int32 SubscriptionID) {
    UA_Subscription *new = (UA_Subscription *) malloc(sizeof(UA_Subscription));
    
    new->SubscriptionID = SubscriptionID;
    new->MonitoredItems = (UA_ListOfUAMonitoredItems *) malloc (sizeof(UA_ListOfUAMonitoredItems));
    LIST_INIT(new->MonitoredItems);
    
    return new;
}

UA_MonitoredItem *UA_MonitoredItem_new() {
    UA_MonitoredItem *new = (UA_MonitoredItem *) malloc(sizeof(UA_MonitoredItem));
    
    return new;
}

void SubscriptionManager_addSubscription(UA_SubscriptionManager *manager, UA_Subscription *newSubscription) {    
    LIST_INSERT_HEAD(manager->ServerSubscriptions, newSubscription, listEntry);

    return;
}

UA_Subscription *SubscriptionManager_getSubscriptionByID(UA_SubscriptionManager *manager, UA_Int32 SubscriptionID) {
    UA_Subscription *retsub, *sub;
    retsub = (UA_Subscription *) NULL;
    
    for (sub = (manager->ServerSubscriptions)->lh_first; sub != NULL; sub = sub->listEntry.le_next) {
        if (sub->SubscriptionID == SubscriptionID) {
            retsub = sub;
            break;
        }
    }
    return retsub;
}

void SubscriptionManager_deleteSubscription(UA_SubscriptionManager *manager, UA_Int32 SubscriptionID) {
    UA_Subscription *sub;
    
    sub = SubscriptionManager_getSubscriptionByID(manager, SubscriptionID);
    if (sub != NULL) LIST_REMOVE(sub, listEntry);
    
    return;
} 

#endif //#ifdef ENABLESUBSCRIPTIONS
