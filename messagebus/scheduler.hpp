#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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

void printServiceConfigLocal(const ServiceConfigLocal *config);
void printClusterConfigLocal(const ClusterConfigLocal *config);
void printRegionConfigLocal(const RegionConfigLocal *config);
void printGlobalConfigLocal(const GlobalConfigLocal *config);

ServiceConfigLocal *createService(string serviceName, int count);
ClusterConfigLocal *createCluster(string clusterName, vector<ServiceConfigLocal *> services);
RegionConfigLocal *createRegion(string regionName, vector<ClusterConfigLocal *> clusters);
GlobalConfigLocal *createGlobal(vector<RegionConfigLocal *> regions);

int clusterServices(string target_service, ClusterConfigLocal cluster);
void updateCluster(string target_service, string op, int &services_left, ClusterConfigLocal *cluster);
void updateRegion(string target_service, string op, int &services_left, RegionConfigLocal *region);
void scheduler(GlobalConfigLocal *currGlobalConfigLocal, pair<string, unordered_map<string, int>> update);
