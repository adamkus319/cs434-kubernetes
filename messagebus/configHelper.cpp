#include "configHelper.hpp"

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

// delete service
void deleteService(ServiceConfigLocal *service){
	delete service;
}

// delete cluster
void deleteCluster(ClusterConfigLocal *cluster){
	for(const auto &service_ptr : cluster->services){
		deleteService(service_ptr);
	}	
	delete cluster;
}

// delete region
void deleteRegion(RegionConfigLocal *region){
	for(const auto &cluster_ptr : region->clusters){
		deleteCluster(cluster_ptr);
	}
	delete region;
}

// delete global
void deleteGlobal(GlobalConfigLocal *global){
	for(const auto &region_ptr : global->regions){
		deleteRegion(region_ptr);
	}
	delete global;
}

