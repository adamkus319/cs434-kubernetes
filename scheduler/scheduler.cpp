#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

struct ServiceConfig {
    string serviceName;
    int count;
};

struct ClusterConfig {
    vector<ServiceConfig *> services;
    string clusterName;
};

struct RegionConfig {
    vector<ClusterConfig *> clusters;
    string regionName;
};

struct GlobalConfig {
    vector<RegionConfig *> regions;
};

// ServiceConfig equality
bool operator==(const ServiceConfig &lhs, const ServiceConfig &rhs) {
    return lhs.serviceName == rhs.serviceName && lhs.count == rhs.count;
}

void printServiceConfig(ServiceConfig config) {
    cout << "  " << config.serviceName << ": " << to_string(config.count) << endl;
}

void printClusterConfig(ClusterConfig config) {
    cout << " " << config.clusterName << ":" << endl;
    for (const auto &service_ptr : config.services) {
        printServiceConfig(*service_ptr);
    }
}

void printRegionConfig(RegionConfig config) {
    cout << config.regionName << ":" << endl;
    for (const auto &cluster_ptr : config.clusters) {
        printClusterConfig(*cluster_ptr);
    }
}

void printGlobalConfig(GlobalConfig config) {
    for (const auto &region_ptr : config.regions) {
        printRegionConfig(*region_ptr);
    }
}

// create new service
ServiceConfig *createService(string serviceName, int count) {
    ServiceConfig *s = new ServiceConfig;
    s->serviceName = serviceName;
    s->count = count;
    return s;
}

// create new cluster
ClusterConfig *createCluster(string clusterName, vector<ServiceConfig *> services) {
    ClusterConfig *c = new ClusterConfig;
    c->clusterName = clusterName;
    c->services = services;
    return c;
}

// create new region
RegionConfig *createRegion(string regionName, vector<ClusterConfig *> clusters) {
    RegionConfig *r = new RegionConfig;
    r->regionName = regionName;
    r->clusters = clusters;
    return r;
}

// create new global configuration
GlobalConfig *createGlobal(vector<RegionConfig *> regions) {
    GlobalConfig *g = new GlobalConfig;
    g->regions = regions;
    return g;
}

// get count of target service instances running on cluster
int clusterServices(string target_service, ClusterConfig cluster) {
    for (const auto &service_ptr : cluster.services) {  // loop over services
        if (service_ptr->serviceName == target_service) {
            return service_ptr->count;  // return number of target services in cluster
        }
    }

    return 0;  // could not find service within cluster
}

// update number of servics within a cluster by adding or subtracting num
void updateCluster(string target_service, string op, int &services_left, ClusterConfig *cluster) {
    int init_services_left = services_left;              // initial number of services left to change upon
    for (const auto &service_ptr : cluster->services) {  // loop
        if (service_ptr->serviceName == target_service) {
            if (op == "add") {
                service_ptr->count++;  // increment number of services
            } else if (op == "sub") {
                service_ptr->count--;                                                                                                       // decrement number of services
                if (service_ptr->count == 0) {                                                                                              // no copies of service left in cluster
                    ServiceConfig *s = createService(target_service, 0);                                                                    // dummy service for search
                    auto it = find_if(cluster->services.begin(), cluster->services.end(), [s](ServiceConfig *ptr) { return *ptr == *s; });  // find service entry in cluster (search values)
                    // auto it = find(cluster->services.begin(), cluster->services.end(), s);
                    if (it != cluster->services.end()) {  // found service
                        cluster->services.erase(it);      // remove service from cluster
                        delete s;                         // de-allocate space
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
            break;                           // exit for loop, no service should only appear once in cluster
        }
    }

    if (init_services_left == services_left && op == "add") {  // service not found but could be added
        ServiceConfig *s = createService(target_service, 1);   // create new service entry
        cluster->services.push_back(s);                        // add copy of service to cluster
        services_left--;
    }
}

// update number of services within a region by calling updateCluster for each cluster
void updateRegion(string target_service, string op, int &services_left, RegionConfig *region) {
    while (services_left) {                                 // keep looping over clusters until no services left
        for (const auto &cluster_ptr : region->clusters) {  // loop over clusters
            updateCluster(target_service, op, services_left, cluster_ptr);
            if (services_left == 0) return;  // all counts updated
        }
    }
}

// input current configuration and pair <regionName, unordered_map<serviceName, count>> with update
void scheduler(GlobalConfig *currGlobalConfig, pair<string, unordered_map<string, int>> update) {
    for (const auto &region_ptr : currGlobalConfig->regions) {           // iterate over regions
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

int main() {
    // US region
    ServiceConfig *s1 = createService("nginx", 1);
    ServiceConfig *s2 = createService("apache", 3);
    ServiceConfig *s3 = createService("webapp", 1);
    ServiceConfig *s4 = createService("loadbalancer", 1);

    vector<ServiceConfig *> ss1 = {s1, s2};
    vector<ServiceConfig *> ss2 = {s3, s4};
    ClusterConfig *c1 = createCluster("us-east", ss1);
    ClusterConfig *c2 = createCluster("us-west", ss2);

    vector<ClusterConfig *> cs1 = {c1, c2};
    RegionConfig *r1 = createRegion("us", cs1);

    // EUR region
    ServiceConfig *s5 = createService("jetty", 4);
    ServiceConfig *s6 = createService("nginx", 3);

    vector<ServiceConfig *> ss3 = {s5, s6};
    vector<ServiceConfig *> ss4 = {s4};
    ClusterConfig *c3 = createCluster("france", ss3);
    ClusterConfig *c4 = createCluster("uk", ss4);

    vector<ClusterConfig *> cs2 = {c3, c4};
    RegionConfig *r2 = createRegion("eur", cs2);

    // global config
    vector<RegionConfig *> rs = {r1, r2};
    GlobalConfig *currGlobalConfig = createGlobal(rs);

    cout << "Before updates:" << endl;
    printGlobalConfig(*currGlobalConfig);
    cout << endl;

    pair<string, unordered_map<string, int>> update;
    update.first = "eur";

    unordered_map<string, int> service_map;
    service_map["ngrok"] = 2;
    service_map["nginx"] = 3;
    update.second = service_map;

    scheduler(currGlobalConfig, update);

    cout << "After updates:" << endl;
    printGlobalConfig(*currGlobalConfig);

    return 0;
}