#include "scheduler.hpp"

void printServiceConfigLocal(const ServiceConfigLocal *config) {
    cout << "  " << config->serviceName << ": " << to_string(config->count) << endl;
}

void printClusterConfigLocal(const ClusterConfigLocal *config) {
    cout << " " << config->clusterName << ":" << endl;
    for (const auto &service_ptr : config->services) {
        printServiceConfigLocal(service_ptr);
    }
}

void printRegionConfigLocal(const RegionConfigLocal *config) {
    cout << config->regionName << ":" << endl;
    for (const auto &cluster_ptr : config->clusters) {
        printClusterConfigLocal(cluster_ptr);
    }
}

void printGlobalConfigLocal(const GlobalConfigLocal *config) {
    for (const auto &region_ptr : config->regions) {
        printRegionConfigLocal(region_ptr);
    }
}

// create new service
ServiceConfigLocal *createService(string serviceName, int count) {
    ServiceConfigLocal *s = new ServiceConfigLocal;
    s->serviceName = serviceName;
    s->count = count;
    return s;
}

// create new cluster
ClusterConfigLocal *createCluster(string clusterName, vector<ServiceConfigLocal *> services) {
    ClusterConfigLocal *c = new ClusterConfigLocal;
    c->clusterName = clusterName;
    c->services = services;
    return c;
}

// create new region
RegionConfigLocal *createRegion(string regionName, vector<ClusterConfigLocal *> clusters) {
    RegionConfigLocal *r = new RegionConfigLocal;
    r->regionName = regionName;
    r->clusters = clusters;
    return r;
}

// create new global configuration
GlobalConfigLocal *createGlobal(vector<RegionConfigLocal *> regions) {
    GlobalConfigLocal *g = new GlobalConfigLocal;
    g->regions = regions;
    return g;
}

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
    for (const auto &service_ptr : cluster->services) {  // loop
        if (service_ptr->serviceName == target_service) {
            if (op == "add") {
                service_ptr->count++;  // increment number of services
            } else if (op == "sub") {
                service_ptr->count--;  // decrement number of services
            } else {
                throw invalid_argument("Invalid op value");
            }

            services_left--;
            if (services_left == 0) return;  // all counts updated
        }
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

