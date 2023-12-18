#include "scheduler.hpp"

// get count of target service instances running on cluster
int clusterServices(string target_service, ClusterConfigLocal cluster) {
    for (const auto &service_ptr : cluster.services) {  // loop over services
        if (service_ptr->serviceName == target_service) {
            return service_ptr->count;  // return number of target services in cluster
        }
    }

    return 0;  // could not find service within cluster
}

// update number of servics within a cluster by adding or subtracting num
void updateCluster(string target_service, string op, int &services_left, ClusterConfigLocal *cluster) {
	int init_services_left = services_left;  // initial number of services left to change upon
    for (const auto &service_ptr : cluster->services) {  // loop
        if (service_ptr->serviceName == target_service) {
            if (op == "add") {
                service_ptr->count++;  // increment number of services
            } else if (op == "sub") {
                service_ptr->count--;  // decrement number of services
                if (service_ptr->count == 0) {  // no copies of service left in cluster
                    ServiceConfigLocal *s = createService(target_service, 0);  // dummy service for search
                    auto it = find_if(cluster->services.begin(), cluster->services.end(), [s](ServiceConfigLocal *ptr) { return *ptr == *s; });  // find service entry in cluster (search values)
                    if (it != cluster->services.end()) {  // found service
                        cluster->services.erase(it);  // remove service from cluster
                        delete s;  // de-allocate space
                    } else {
                        delete s;  // de-allocate space
                        throw logic_error("Should find service");
                    }
                }
            } else {
                throw invalid_argument("Invalid op value");
            }

            services_left--;
            if (services_left == 0) return;  // all counts updated
            break;  // exit for loop, no service should only appear once in cluster
        }
    }

    if (init_services_left == services_left && op == "add") {  // service not found but could be added
        ServiceConfigLocal *s = createService(target_service, 1);  // create new service entry
        cluster->services.push_back(s);  // add copy of service to cluster
        services_left--;
    }
}

// update number of services within a region by calling updateCluster for each cluster
void updateRegion(string target_service, string op, int &services_left, RegionConfigLocal *region) {
    while (services_left) {                                 // keep looping over clusters until no services left
        for (const auto &cluster_ptr : region->clusters) {  // loop over clusters
            updateCluster(target_service, op, services_left, cluster_ptr);
            if (services_left == 0) return;  // all counts updated
        }
    }
}

// input current configuration and pair <regionName, unordered_map<serviceName, count>> with update
void scheduler(GlobalConfigLocal *currGlobalConfigLocal, pair<string, unordered_map<string, int>> update) {
    for (const auto &region_ptr : currGlobalConfigLocal->regions) {      // iterate over regions
        if (region_ptr->regionName == update.first) {                    // check if region is region being updated
            unordered_map<string, int> service_updates = update.second;  // services to update

            for (const auto &service_pair : service_updates) {  // iterate over services to update
                string target_service = service_pair.first;
                int target_num = service_pair.second;

                int num_services = 0;                                   // counter for number of services
                for (const auto &cluster_ptr : region_ptr->clusters) {  // iterate over clusters within region to figure out current number of services
                    num_services += clusterServices(target_service, *cluster_ptr);
                }

                int services_left;
                if (num_services < target_num) {  // need to add more services to region
                    services_left = target_num - num_services;
                    updateRegion(target_service, "add", services_left, region_ptr);
                } else if (num_services > target_num) {  // need to take away services from region
                    services_left = num_services - target_num;
                    updateRegion(target_service, "sub", services_left, region_ptr);
                } else {       // target number equal to current number, no change
                    continue;  // move on to next service
                }
            }

            break;  // finished updating region, break loop (only single regional update per scheduler call)
        }
    }
}

