#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct ServiceConfigLocal {
    string serviceName;
    int count;
};

struct ClusterConfigLocal {
    vector<ServiceConfigLocal *> services;
    string clusterName;
};

struct RegionConfigLocal {
    vector<ClusterConfigLocal *> clusters;
    string regionName;
};

struct GlobalConfigLocal {
    vector<RegionConfigLocal *> regions;
};


// ServiceConfigLocal equality
bool operator==(const ServiceConfigLocal &lhs, const ServiceConfigLocal &rhs) {
    return lhs.serviceName == rhs.serviceName && lhs.count == rhs.count;
}


/* PRINT FUNCTIONS */
void printServiceConfigLocal(const ServiceConfigLocal *config);
void printClusterConfigLocal(const ClusterConfigLocal *config);
void printRegionConfigLocal(const RegionConfigLocal *config);
void printGlobalConfigLocal(const GlobalConfigLocal *config);

/* CREATION FUNCTIONS */
ServiceConfigLocal *createService(string serviceName, int count);
ClusterConfigLocal *createCluster(string clusterName, vector<ServiceConfigLocal *> services);
RegionConfigLocal *createRegion(string regionName, vector<ClusterConfigLocal *> clusters);
GlobalConfigLocal *createGlobal(vector<RegionConfigLocal *> regions);

/* DELETION FUNCTIONS */
void deleteService(ServiceConfigLocal *service);
void deleteCluster(ClusterConfigLocal *cluster);
void deleteRegion(RegionConfigLocal *region);
void deleteGlobal(GlobalConfigLocal *global);

